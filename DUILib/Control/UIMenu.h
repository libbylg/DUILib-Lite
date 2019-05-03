#ifndef __UIMENU_H__
#define __UIMENU_H__

#include "Control/UIList.h"
#include "Core/UIWindow.h"

namespace DUI
{

    struct ContextMenuParam
    {
        // 1: remove all
        // 2: remove the sub menu
        WPARAM wParam;
        HWND hWnd;
    };

    struct MenuItemInfo
    {
        TCHAR szName[256];
        BOOL bChecked;
    };

    struct MenuCmd
    {
        TCHAR szName[256];
        TCHAR szUserData[1024];
        TCHAR szText[1024];
        BOOL bChecked;
    };

    enum MenuAlignment
    {
        eMenuAlignment_Left = 1 << 1,
        eMenuAlignment_Top = 1 << 2,
        eMenuAlignment_Right = 1 << 3,
        eMenuAlignment_Bottom = 1 << 4,
    };


    enum MenuItemDefaultInfo
    {
        ITEM_DEFAULT_HEIGHT = 30,		//每一个item的默认高度（只在竖状排列时自定义）
        ITEM_DEFAULT_WIDTH = 150,		//窗口的默认宽度

        ITEM_DEFAULT_ICON_WIDTH = 26,	//默认图标所占宽度
        ITEM_DEFAULT_ICON_SIZE = 16,	//默认图标的大小

        ITEM_DEFAULT_EXPLAND_ICON_WIDTH = 20,	//默认下级菜单扩展图标所占宽度
        ITEM_DEFAULT_EXPLAND_ICON_SIZE = 9,		//默认下级菜单扩展图标的大小

        DEFAULT_LINE_LEFT_INSET = ITEM_DEFAULT_ICON_WIDTH + 3,	//默认分隔线的左边距
        DEFAULT_LINE_RIGHT_INSET = 7,	//默认分隔线的右边距
        DEFAULT_LINE_HEIGHT = 6,		//默认分隔线所占高度
        DEFAULT_LINE_COLOR = 0xFFBCBFC4	//默认分隔线颜色
    };


    ///////////////////////////////////////////////
    class MenuMenuReceiverImplBase;
    class MenuMenuObserverImplBase
    {
    public:
        virtual void AddReceiver(MenuMenuReceiverImplBase* receiver) = 0;
        virtual void RemoveReceiver(MenuMenuReceiverImplBase* receiver) = 0;
        virtual BOOL RBroadcast(ContextMenuParam param) = 0;
    };
    /////////////////////////////////////////////////
    class MenuMenuReceiverImplBase
    {
    public:
        virtual void AddObserver(MenuMenuObserverImplBase* observer) = 0;
        virtual void RemoveObserver() = 0;
        virtual BOOL Receive(ContextMenuParam param) = 0;
    };
    /////////////////////////////////////////////////

    class MenuReceiverImpl;
    class DUILIB_API MenuObserverImpl : public MenuMenuObserverImplBase
    {
        friend class Iterator;
    public:
        MenuObserverImpl() :
            m_pMainWndPaintManager(NULL),
            m_pMenuCheckInfo(NULL)
        {
            pReceivers_ = new ReceiversVector;
        }

        ~MenuObserverImpl()
        {
            if (pReceivers_ != NULL) {
                delete pReceivers_;
                pReceivers_ = NULL;
            }

        }

        virtual void AddReceiver(MenuMenuReceiverImplBase* receiver)
        {
            if (receiver == NULL)
                return;

            pReceivers_->push_back(receiver);
            receiver->AddObserver(this);
        }

        virtual void RemoveReceiver(MenuMenuReceiverImplBase * receiver)
        {
            if (receiver == NULL)
                return;

            ReceiversVector::iterator it = pReceivers_->begin();
            for (; it != pReceivers_->end(); ++it) {
                if (*it == receiver) {
                    pReceivers_->erase(it);
                    break;
                }
            }
        }

        virtual BOOL RBroadcast(ContextMenuParam param)
        {
            ReceiversVector::reverse_iterator it = pReceivers_->rbegin();
            for (; it != pReceivers_->rend(); ++it) {
                (*it)->Receive(param);
            }

            return BOOL();
        }


        class Iterator
        {
            MenuObserverImpl& _tbl;
            DWORD index;
            MenuMenuReceiverImplBase* ptr;
        public:
            Iterator(MenuObserverImpl& table)
                : _tbl(table), index(0), ptr(NULL)
            {
            }

            Iterator(const Iterator& v)
                : _tbl(v._tbl), index(v.index), ptr(v.ptr)
            {
            }

            MenuMenuReceiverImplBase* next()
            {
                if (index >= _tbl.pReceivers_->size())
                    return NULL;

                for (; index < _tbl.pReceivers_->size(); ) {
                    ptr = (*(_tbl.pReceivers_))[index++];
                    if (ptr)
                        return ptr;
                }
                return NULL;
            }
        };

        virtual void SetManger(CManagerUI * pManager)
        {
            if (pManager != NULL)
                m_pMainWndPaintManager = pManager;
        }

        virtual CManagerUI* GetManager() const
        {
            return m_pMainWndPaintManager;
        }

        virtual void SetMenuCheckInfo(CStringMapUI * pInfo)
        {
            if (pInfo != NULL)
                m_pMenuCheckInfo = pInfo;
            else
                m_pMenuCheckInfo = NULL;
        }

        virtual CStringMapUI* GetMenuCheckInfo() const
        {
            return m_pMenuCheckInfo;
        }

    protected:
        typedef std::vector<MenuMenuReceiverImplBase*> ReceiversVector;
        ReceiversVector* pReceivers_;
        CManagerUI* m_pMainWndPaintManager;
        CStringMapUI* m_pMenuCheckInfo;
    };

    ////////////////////////////////////////////////////
    class DUILIB_API MenuReceiverImpl : public MenuMenuReceiverImplBase
    {
    public:
        MenuReceiverImpl()
        {
            pObservers_ = new ObserversVector;
        }

        ~MenuReceiverImpl()
        {
            if (pObservers_ != NULL) {
                delete pObservers_;
                pObservers_ = NULL;
            }
        }

        virtual void AddObserver(MenuMenuObserverImplBase* observer)
        {
            pObservers_->push_back(observer);
        }

        virtual void RemoveObserver()
        {
            ObserversVector::iterator it = pObservers_->begin();
            for (; it != pObservers_->end(); ++it) {
                (*it)->RemoveReceiver(this);
            }
        }

        virtual BOOL Receive(ContextMenuParam param)
        {
            return BOOL();
        }

    protected:
        typedef std::vector<MenuMenuObserverImplBase*> ObserversVector;
        ObserversVector* pObservers_;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class CListUI;
    class CMenuWnd;
    class DUILIB_API CMenuUI : public CListUI
    {
        UI_DECLARE_CONTROL(CMenuUI)
    public:
        CMenuUI();
        virtual ~CMenuUI();
        CMenuWnd* m_pWindow;
        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);
        UINT GetListType();

        virtual void DoEvent(struct TEVENT_UI& event);

        virtual BOOL Add(CControlUI* pControl);
        virtual BOOL AddAt(CControlUI* pControl, int iIndex);

        virtual int GetItemIndex(CControlUI* pControl) const;
        virtual BOOL SetItemIndex(CControlUI* pControl, int iIndex);
        virtual BOOL Remove(CControlUI* pControl);

        SIZE EstimateSize(SIZE szAvailable);

        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    };


} // namespace DUI

#endif // __UIMENU_H__
