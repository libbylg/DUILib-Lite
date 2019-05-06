#include "Control/UIMenu.h"
#include "Control/UIDialogBuilder.h"
#include "Core/UIScrollBar.h"

namespace DUI
{

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class CMenuElementUI;
    class DUILIB_API CMenuWnd : public CWindowUI, public MenuReceiverImpl, public INotifyUI, public IFactoryUI
    {
    public:
        static MenuObserverImpl& GetGlobalContextMenuObserver()
        {
            static MenuObserverImpl s_context_menu_observer;
            return s_context_menu_observer;
        }
        static CMenuWnd* CreateMenu(CMenuElementUI* pOwner, TSTRID_UI xml, POINT point,
            CManagerUI* pMainPaintManager, CStringMapUI* pMenuCheckInfo = NULL,
            DWORD dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top);
        static void DestroyMenu();
        static MenuItemInfo* SetMenuItemInfo(LPCTSTR pstrName, BOOL bChecked);

    public:
        CMenuWnd();
        ~CMenuWnd();
        void Close(UINT nRet = IDOK);
        BOOL isClosing;
        /*
         *	@pOwner 一级菜单不要指定这个参数，这是菜单内部使用的
         *	@xml	菜单的布局文件
         *	@point	菜单的左上角坐标
         *	@pMainPaintManager	菜单的父窗体管理器指针
         *	@pMenuCheckInfo	保存菜单的单选和复选信息结构指针
         *	@dwAlignment		菜单的出现位置，默认出现在鼠标的右下侧。
         */

        void Init(CMenuElementUI* pOwner, TSTRID_UI xml, POINT point,
            CManagerUI* pMainPaintManager, CStringMapUI* pMenuCheckInfo = NULL,
            DWORD dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top);
        LPCTSTR GetWindowClassName() const;
        void OnFinalMessage(HWND hWnd);
        void Notify(struct TNOTIFY_UI& msg);
        CControlUI* CreateControl(LPCTSTR pstrClassName);

        LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

        BOOL Receive(ContextMenuParam param);

        // 获取根菜单控件，用于动态添加子菜单
        CMenuUI* GetMenuUI();

        // 重新调整菜单的大小
        void ResizeMenu();

        // 重新调整子菜单的大小
        void ResizeSubMenu();
        void setDPI(int DPI);

    public:

        POINT			m_BasedPoint;
        TSTRID_UI		m_xml;
        CManagerUI      m_pm;
        CMenuElementUI* m_pOwner;
        CMenuUI* m_pLayout;
        DWORD		    m_dwAlignment;	//菜单对齐方式
    };

    class CListContainerElementUI;
    class DUILIB_API CMenuElementUI : public CListContainerElementUI
    {
        UI_DECLARE_CONTROL(CMenuElementUI)

            ///
            friend class CMenuWnd;

    public:
        CMenuElementUI();
        ~CMenuElementUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);
        BOOL DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
        void DrawItemText(HDC hDC, const RECT& rcItem);
        SIZE EstimateSize(SIZE szAvailable);

        void DoEvent(struct TEVENT_UI& event);

        CMenuWnd* GetMenuWnd();
        void CreateMenuWnd();

        void SetLineType();
        void SetLineColor(DWORD color);
        DWORD GetLineColor() const;
        void SetLinePadding(RECT rcInset);
        RECT GetLinePadding() const;
        void SetIcon(LPCTSTR strIcon);
        void SetIconSize(LONG cx, LONG cy);
        void DrawItemIcon(HDC hDC, const RECT& rcItem);
        void SetChecked(BOOL bCheck = TRUE);
        BOOL GetChecked() const;
        void SetCheckItem(BOOL bCheckItem = FALSE);
        BOOL GetCheckItem() const;

        void SetShowExplandIcon(BOOL bShow);
        void DrawItemExpland(HDC hDC, const RECT& rcItem);

        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        MenuItemInfo* GetItemInfo(LPCTSTR pstrName);
        MenuItemInfo* SetItemInfo(LPCTSTR pstrName, BOOL bChecked);
    protected:
        CMenuWnd* m_pWindow;

        BOOL		m_bDrawLine;	//画分隔线
        DWORD		m_dwLineColor;  //分隔线颜色
        RECT		m_rcLinePadding;	//分割线的左右边距

        SIZE		m_szIconSize; 	//画图标
        CStringUI	m_strIcon;
        BOOL		m_bCheckItem;

        BOOL		m_bShowExplandIcon;
    };



    /////////////////////////////////////////////////////////////////////////////////////
    //
    UI_IMPLEMENT_CONTROL(CMenuUI)


        /////////////////////////////////////////////////////////////////////////////////////
        ////
        CMenuUI::CMenuUI() :
        m_pWindow(NULL)
    {
        if (GetHeader() != NULL)
            GetHeader()->SetVisible(FALSE);
    }

    CMenuUI::~CMenuUI()
    {

    }

    LPCTSTR CMenuUI::GetClass() const
    {
        return _T("MenuUI");
    }

    LPVOID CMenuUI::GetInterface(LPCTSTR pstrName)
    {
        if (_tcsicmp(pstrName, _T("Menu")) == 0) return static_cast<CMenuUI*>(this);
        return CListUI::GetInterface(pstrName);
    }

    UINT CMenuUI::GetListType()
    {
        return UILIST_MENU;
    }

    void CMenuUI::DoEvent(struct TEVENT_UI& event)
    {
        return __super::DoEvent(event);
    }

    BOOL CMenuUI::Add(CControlUI * pControl)
    {
        CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(_T("MenuElement")));
        if (pMenuItem == NULL)
            return FALSE;

        for (int i = 0; i < pMenuItem->GetCount(); ++i) {
            if (pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL) {
                (static_cast<CMenuElementUI*>(pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(FALSE);
            }
        }
        return CListUI::Add(pControl);
    }

    BOOL CMenuUI::AddAt(CControlUI * pControl, int iIndex)
    {
        CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(_T("MenuElement")));
        if (pMenuItem == NULL)
            return FALSE;

        for (int i = 0; i < pMenuItem->GetCount(); ++i) {
            if (pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL) {
                (static_cast<CMenuElementUI*>(pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(FALSE);
            }
        }
        return CListUI::AddAt(pControl, iIndex);
    }

    int CMenuUI::GetItemIndex(CControlUI * pControl) const
    {
        CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(_T("MenuElement")));
        if (pMenuItem == NULL)
            return -1;

        return __super::GetItemIndex(pControl);
    }

    BOOL CMenuUI::SetItemIndex(CControlUI * pControl, int iIndex)
    {
        CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(_T("MenuElement")));
        if (pMenuItem == NULL)
            return FALSE;

        return __super::SetItemIndex(pControl, iIndex);
    }

    BOOL CMenuUI::Remove(CControlUI * pControl)
    {
        CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(_T("MenuElement")));
        if (pMenuItem == NULL)
            return FALSE;

        return __super::Remove(pControl);
    }

    SIZE CMenuUI::EstimateSize(SIZE szAvailable)
    {
        int cxFixed = 0;
        int cyFixed = 0;
        for (int it = 0; it < GetCount(); it++) {
            CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
            if (!pControl->IsVisible()) continue;
            SIZE sz = pControl->EstimateSize(szAvailable);
            cyFixed += sz.cy;
            if (cxFixed < sz.cx)
                cxFixed = sz.cx;
        }

        for (int it = 0; it < GetCount(); it++) {
            CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
            if (!pControl->IsVisible()) continue;

            pControl->SetFixedWidth(MulDiv(cxFixed, 100, GetManager()->GetDPIObj()->GetScale()));
        }

        return CSizeUI(cxFixed, cyFixed);
    }

    void CMenuUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        CListUI::SetAttribute(pstrName, pstrValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //

    CMenuWnd::CMenuWnd() :
        m_pOwner(NULL),
        m_pLayout(),
        m_xml(_T("")),
        isClosing(FALSE)
    {
        m_dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top;
    }

    CMenuWnd::~CMenuWnd()
    {

    }

    void CMenuWnd::Close(UINT nRet)
    {
        ASSERT(::IsWindow(m_hWnd));
        if (!::IsWindow(m_hWnd)) return;
        PostMessage(WM_CLOSE, (WPARAM)nRet, 0L);
        isClosing = TRUE;
    }


    BOOL CMenuWnd::Receive(ContextMenuParam param)
    {
        switch (param.wParam) {
            case 1:
                Close();
                break;
            case 2:
            {
                HWND hParent = GetParent(m_hWnd);
                while (hParent != NULL) {
                    if (hParent == param.hWnd) {
                        Close();
                        break;
                    }
                    hParent = GetParent(hParent);
                }
            }
            break;
            default:
                break;
        }

        return TRUE;
    }

    CMenuWnd* CMenuWnd::CreateMenu(CMenuElementUI * pOwner, TSTRID_UI xml, POINT point, CManagerUI * pMainPaintManager, CStringMapUI * pMenuCheckInfo /*= NULL*/, DWORD dwAlignment /*= eMenuAlignment_Left | eMenuAlignment_Top*/)
    {
        CMenuWnd* pMenu = new CMenuWnd;
        pMenu->Init(pOwner, xml, point, pMainPaintManager, pMenuCheckInfo, dwAlignment);
        return pMenu;
    }

    void CMenuWnd::DestroyMenu()
    {
        CStringMapUI* mCheckInfos = CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo();
        if (mCheckInfos != NULL) {
            for (int i = 0; i < mCheckInfos->GetSize(); i++) {
                MenuItemInfo* pItemInfo = (MenuItemInfo*)mCheckInfos->Find(mCheckInfos->GetAt(i));
                if (pItemInfo != NULL) {
                    delete pItemInfo;
                    pItemInfo = NULL;
                }
            }
            mCheckInfos->Resize(0);
        }
    }

    MenuItemInfo* CMenuWnd::SetMenuItemInfo(LPCTSTR pstrName, BOOL bChecked)
    {
        if (pstrName == NULL || lstrlen(pstrName) <= 0) return NULL;

        CStringMapUI * mCheckInfos = CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo();
        if (mCheckInfos != NULL) {
            MenuItemInfo* pItemInfo = (MenuItemInfo*)mCheckInfos->Find(pstrName);
            if (pItemInfo == NULL) {
                pItemInfo = new MenuItemInfo;
                lstrcpy(pItemInfo->szName, pstrName);
                pItemInfo->bChecked = bChecked;
                mCheckInfos->Insert(pstrName, pItemInfo);
            } else {
                pItemInfo->bChecked = bChecked;
            }

            return pItemInfo;
        }
        return NULL;
    }

    void CMenuWnd::Init(CMenuElementUI * pOwner, TSTRID_UI xml, POINT point,
        CManagerUI * pMainPaintManager, CStringMapUI * pMenuCheckInfo/* = NULL*/,
        DWORD dwAlignment/* = eMenuAlignment_Left | eMenuAlignment_Top*/)
    {

        m_BasedPoint = point;
        m_pOwner = pOwner;
        m_pLayout = NULL;
        m_xml = xml;
        m_dwAlignment = dwAlignment;

        // 如果是一级菜单的创建
        if (pOwner == NULL) {
            ASSERT(pMainPaintManager != NULL);
            CMenuWnd::GetGlobalContextMenuObserver().SetManger(pMainPaintManager);
            if (pMenuCheckInfo != NULL)
                CMenuWnd::GetGlobalContextMenuObserver().SetMenuCheckInfo(pMenuCheckInfo);
        }

        CMenuWnd::GetGlobalContextMenuObserver().AddReceiver(this);

        Create((m_pOwner == NULL)?pMainPaintManager->GetPaintWindow():m_pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, CRectUI());

        // HACK: Don't deselect the parent's caption
        HWND hWndParent = m_hWnd;
        while (::GetParent(hWndParent) != NULL) hWndParent = ::GetParent(hWndParent);

        ::ShowWindow(m_hWnd, SW_SHOW);
        ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
    }

    LPCTSTR CMenuWnd::GetWindowClassName() const
    {
        return _T("DuiMenuWnd");
    }


    void CMenuWnd::Notify(struct TNOTIFY_UI& msg)
    {
        if (CMenuWnd::GetGlobalContextMenuObserver().GetManager() != NULL) {
            if (msg.sType == _T("click") || msg.sType == _T("valuechanged")) {
                CMenuWnd::GetGlobalContextMenuObserver().GetManager()->SendNotify(msg, FALSE);
            }
        }

    }

    CControlUI* CMenuWnd::CreateControl(LPCTSTR pstrClassName)
    {
        if (_tcsicmp(pstrClassName, _T("Menu")) == 0) {
            return new CMenuUI();
        } else if (_tcsicmp(pstrClassName, _T("MenuElement")) == 0) {
            return new CMenuElementUI();
        }
        return NULL;
    }


    void CMenuWnd::OnFinalMessage(HWND hWnd)
    {
        RemoveObserver();
        if (m_pOwner != NULL) {
            for (int i = 0; i < m_pOwner->GetCount(); i++) {
                if (static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)->GetInterface(_T("MenuElement"))) != NULL) {
                    (static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)))->SetOwner(m_pOwner->GetParent());
                    (static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)))->SetVisible(FALSE);
                    (static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(FALSE);
                }
            }
            m_pOwner->m_pWindow = NULL;
            m_pOwner->m_uButtonState &= ~UISTATE_PUSHED;
            m_pOwner->Invalidate();

            // 内部创建的内部删除
            delete this;
        }
    }

    LRESULT CMenuWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
    {
        BOOL bShowShadow = FALSE;
        if (m_pOwner != NULL) {
            LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
            styleValue &= ~WS_CAPTION;
            ::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
            RECT rcClient;
            ::GetClientRect(*this, &rcClient);
            ::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
                rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

            m_pm.Init(m_hWnd);
            m_pm.GetDPIObj()->SetScale(m_pOwner->GetManager()->GetDPIObj()->GetDPI());
            // The trick is to add the items to the new container. Their owner gets
            // reassigned by this operation - which is why it is important to reassign
            // the items back to the righfull owner/manager when the window closes.
            m_pLayout = new CMenuUI();
            m_pm.SetForceUseSharedRes(TRUE);
            m_pLayout->SetManager(&m_pm, NULL, TRUE);
            LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("Menu"));
            if (pDefaultAttributes) {
                m_pLayout->ApplyAttributeList(pDefaultAttributes);
            }
            m_pLayout->GetList()->SetAutoDestroy(FALSE);

            for (int i = 0; i < m_pOwner->GetCount(); i++) {
                if (m_pOwner->GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL) {
                    (static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)))->SetOwner(m_pLayout);
                    m_pLayout->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
                }
            }

            CShadowUI* pShadow = m_pOwner->GetManager()->GetShadow();
            pShadow->CopyShadow(m_pm.GetShadow());
            bShowShadow = m_pm.GetShadow()->IsShowShadow();
            m_pm.GetShadow()->ShowShadow(FALSE);
            m_pm.SetLayered(m_pOwner->GetManager()->IsLayered());
            m_pm.AttachDialog(m_pLayout);
            m_pm.AddNotifier(this);

            ResizeSubMenu();
        } else {
            m_pm.Init(m_hWnd);
            m_pm.GetDPIObj()->SetScale(CMenuWnd::GetGlobalContextMenuObserver().GetManager()->GetDPIObj()->GetDPI());
            CDialogBuilderUI builder;

            CControlUI* pRoot = builder.Create(m_xml, NULL, this, &m_pm);
            bShowShadow = m_pm.GetShadow()->IsShowShadow();
            m_pm.GetShadow()->ShowShadow(FALSE);
            m_pm.AttachDialog(pRoot);
            m_pm.AddNotifier(this);

            ResizeMenu();
        }
        GetMenuUI()->m_pWindow = this;
        m_pm.GetShadow()->ShowShadow(bShowShadow);
        m_pm.GetShadow()->Create(&m_pm);
        return 0;
    }

    CMenuUI* CMenuWnd::GetMenuUI()
    {
        return static_cast<CMenuUI*>(m_pm.GetRoot());
    }

    void CMenuWnd::ResizeMenu()
    {
        CControlUI* pRoot = m_pm.GetRoot();

#if defined(WIN32) && !defined(UNDER_CE)
        MONITORINFO oMonitor = {};
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
        CRectUI rcWork = oMonitor.rcWork;
#else
        CRectUI rcWork;
        GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
        SIZE szAvailable = {rcWork.right - rcWork.left, rcWork.bottom - rcWork.top};
        szAvailable = pRoot->EstimateSize(szAvailable);
        m_pm.SetInitSize(szAvailable.cx, szAvailable.cy);

        //必须是Menu标签作为xml的根节点
        CMenuUI* pMenuRoot = static_cast<CMenuUI*>(pRoot);
        ASSERT(pMenuRoot);

        SIZE szInit = m_pm.GetInitSize();
        CRectUI rc;
        CPointUI point = m_BasedPoint;
        rc.left = point.x;
        rc.top = point.y;
        rc.right = rc.left + szInit.cx;
        rc.bottom = rc.top + szInit.cy;

        int nWidth = rc.GetWidth();
        int nHeight = rc.GetHeight();

        if (m_dwAlignment & eMenuAlignment_Right) {
            rc.right = point.x;
            rc.left = rc.right - nWidth;
        }

        if (m_dwAlignment & eMenuAlignment_Bottom) {
            rc.bottom = point.y;
            rc.top = rc.bottom - nHeight;
        }

        SetForegroundWindow(m_hWnd);
        MoveWindow(m_hWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE);
        SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.GetWidth(), rc.GetHeight() + pMenuRoot->GetInset().bottom + pMenuRoot->GetInset().top, SWP_SHOWWINDOW);
    }

    void CMenuWnd::ResizeSubMenu()
    {
        // Position the popup window in absolute space
        RECT rcOwner = m_pOwner->GetPos();
        RECT rc = rcOwner;

        int cxFixed = 0;
        int cyFixed = 0;

#if defined(WIN32) && !defined(UNDER_CE)
        MONITORINFO oMonitor = {};
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
        CRectUI rcWork = oMonitor.rcWork;
#else
        CRectUI rcWork;
        GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
        SIZE szAvailable = {rcWork.right - rcWork.left, rcWork.bottom - rcWork.top};

        for (int it = 0; it < m_pOwner->GetCount(); it++) {
            if (m_pOwner->GetItemAt(it)->GetInterface(_T("MenuElement")) != NULL) {
                CControlUI* pControl = static_cast<CControlUI*>(m_pOwner->GetItemAt(it));
                SIZE sz = pControl->EstimateSize(szAvailable);
                cyFixed += sz.cy;
                if (cxFixed < sz.cx) cxFixed = sz.cx;
            }
        }

        RECT rcWindow;
        GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWindow);

        rc.top = rcOwner.top;
        rc.bottom = rc.top + cyFixed;
        ::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
        rc.left = rcWindow.right;
        rc.right = rc.left + cxFixed;
        rc.right += 2;

        BOOL bReachBottom = FALSE;
        BOOL bReachRight = FALSE;
        LONG chRightAlgin = 0;
        LONG chBottomAlgin = 0;

        RECT rcPreWindow = {0};
        MenuObserverImpl::Iterator iterator(CMenuWnd::GetGlobalContextMenuObserver());
        MenuMenuReceiverImplBase* pReceiver = iterator.next();
        while (pReceiver != NULL) {
            CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
            if (pContextMenu != NULL) {
                GetWindowRect(pContextMenu->GetHWND(), &rcPreWindow);

                bReachRight = rcPreWindow.left >= rcWindow.right;
                bReachBottom = rcPreWindow.top >= rcWindow.bottom;
                if (pContextMenu->GetHWND() == m_pOwner->GetManager()->GetPaintWindow() || bReachBottom || bReachRight)
                    break;
            }
            pReceiver = iterator.next();
        }

        if (bReachBottom) {
            rc.bottom = rcWindow.top;
            rc.top = rc.bottom - cyFixed;
        }

        if (bReachRight) {
            rc.right = rcWindow.left;
            rc.left = rc.right - cxFixed;
        }

        if (rc.bottom > rcWork.bottom) {
            rc.bottom = rc.top;
            rc.top = rc.bottom - cyFixed;
        }

        if (rc.right > rcWork.right) {
            rc.right = rcWindow.left;
            rc.left = rc.right - cxFixed;
        }

        if (rc.top < rcWork.top) {
            rc.top = rcOwner.top;
            rc.bottom = rc.top + cyFixed;
        }

        if (rc.left < rcWork.left) {
            rc.left = rcWindow.right;
            rc.right = rc.left + cxFixed;
        }

        MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + m_pLayout->GetInset().top + m_pLayout->GetInset().bottom, FALSE);
    }

    void CMenuWnd::setDPI(int DPI)
    {
        m_pm.SetDPI(DPI);
    }


    LRESULT CMenuWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
    {
        HWND hFocusWnd = (HWND)wParam;

        BOOL bInMenuWindowList = FALSE;
        ContextMenuParam param;
        param.hWnd = GetHWND();

        MenuObserverImpl::Iterator iterator(CMenuWnd::GetGlobalContextMenuObserver());
        MenuMenuReceiverImplBase* pReceiver = iterator.next();
        while (pReceiver != NULL) {
            CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
            if (pContextMenu != NULL && pContextMenu->GetHWND() == hFocusWnd) {
                bInMenuWindowList = TRUE;
                break;
            }
            pReceiver = iterator.next();
        }

        if (!bInMenuWindowList) {
            param.wParam = 1;
            CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
            return 0;
        }
        return 0;
    }
    LRESULT CMenuWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
    {
        SIZE szRoundCorner = m_pm.GetRoundCorner();
        if (!::IsIconic(*this)) {
            CRectUI rcWnd;
            ::GetWindowRect(*this, &rcWnd);
            rcWnd.Offset(-rcWnd.left, -rcWnd.top);
            rcWnd.right++; rcWnd.bottom++;
            HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
            ::SetWindowRgn(*this, hRgn, TRUE);
            ::DeleteObject(hRgn);
        }
        bHandled = FALSE;
        return 0;
    }

    LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRes = 0;
        BOOL bHandled = TRUE;
        switch (uMsg) {
            case WM_CREATE:
                lRes = OnCreate(uMsg, wParam, lParam, bHandled);
                break;
            case WM_KILLFOCUS:
                lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
                break;
            case WM_KEYDOWN:
                if (wParam == VK_ESCAPE || wParam == VK_LEFT)
                    Close();
                break;
            case WM_SIZE:
                lRes = OnSize(uMsg, wParam, lParam, bHandled);
                break;
            case WM_CLOSE:
                if (m_pOwner != NULL) {
                    m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), FALSE);
                    m_pOwner->SetPos(m_pOwner->GetPos());
                    m_pOwner->SetFocus();
                }
                break;
            case WM_RBUTTONDOWN:
            case WM_CONTEXTMENU:
            case WM_RBUTTONUP:
            case WM_RBUTTONDBLCLK:
                return 0L;
                break;
            default:
                bHandled = FALSE;
                break;
        }

        if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
        return CWindowUI::HandleMessage(uMsg, wParam, lParam);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    UI_IMPLEMENT_CONTROL(CMenuElementUI)

        ///
        CMenuElementUI::CMenuElementUI() :
        m_pWindow(NULL),
        m_bDrawLine(FALSE),
        m_dwLineColor(DEFAULT_LINE_COLOR),
        m_bCheckItem(FALSE),
        m_bShowExplandIcon(FALSE)
    {
        m_cxyFixed.cy = ITEM_DEFAULT_HEIGHT;
        m_cxyFixed.cx = ITEM_DEFAULT_WIDTH;
        m_szIconSize.cy = ITEM_DEFAULT_ICON_SIZE;
        m_szIconSize.cx = ITEM_DEFAULT_ICON_SIZE;

        m_rcLinePadding.top = m_rcLinePadding.bottom = 0;
        m_rcLinePadding.left = DEFAULT_LINE_LEFT_INSET;
        m_rcLinePadding.right = DEFAULT_LINE_RIGHT_INSET;
    }

    CMenuElementUI::~CMenuElementUI()
    {
    }

    LPCTSTR CMenuElementUI::GetClass() const
    {
        return _T("MenuElementUI");
    }

    LPVOID CMenuElementUI::GetInterface(LPCTSTR pstrName)
    {
        if (_tcsicmp(pstrName, _T("MenuElement")) == 0) return static_cast<CMenuElementUI*>(this);
        return CListContainerElementUI::GetInterface(pstrName);
    }

    BOOL CMenuElementUI::DoPaint(HDC hDC, const RECT & rcPaint, CControlUI * pStopControl)
    {
        SIZE m_cxyFixed = CMenuElementUI::m_cxyFixed;
        m_cxyFixed.cx = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cx);
        m_cxyFixed.cy = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cy);
        RECT m_rcLinePadding = CMenuElementUI::m_rcLinePadding;
        GetManager()->GetDPIObj()->Scale(&m_rcLinePadding);

        RECT rcTemp = {0};
        if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) return TRUE;

        if (m_bDrawLine) {
            RECT rcLine = {m_rcItem.left + m_rcLinePadding.left, m_rcItem.top + m_cxyFixed.cy / 2, m_rcItem.right - m_rcLinePadding.right, m_rcItem.top + m_cxyFixed.cy / 2};
            CRenderUI::DrawLine(hDC, rcLine, 1, m_dwLineColor);
        } else {
            //CMenuElementUI::DrawItemBk(hDC, m_rcItem);
            //DrawItemText(hDC, m_rcItem);
            //DrawItemIcon(hDC, m_rcItem);
            //DrawItemExpland(hDC, m_rcItem);
            //for (int i = 0; i < GetCount(); ++i)
            //{
            //	if (GetItemAt(i)->GetInterface(_T("MenuElement")) == NULL) {
            //		GetItemAt(i)->DoPaint(hDC, rcPaint);
            //	}
            //}

            CRenderClipUI clip;
            CRenderClipUI::GenerateClip(hDC, rcTemp, clip);
            CMenuElementUI::DrawItemBk(hDC, m_rcItem);
            DrawItemText(hDC, m_rcItem);
            DrawItemIcon(hDC, m_rcItem);
            DrawItemExpland(hDC, m_rcItem);

            if (m_items.GetSize() > 0) {
                RECT rc = m_rcItem;
                rc.left += m_rcInset.left;
                rc.top += m_rcInset.top;
                rc.right -= m_rcInset.right;
                rc.bottom -= m_rcInset.bottom;
                if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
                if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

                if (!::IntersectRect(&rcTemp, &rcPaint, &rc)) {
                    for (int it = 0; it < m_items.GetSize(); it++) {
                        CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
                        if (pControl == pStopControl) return FALSE;
                        if (!pControl->IsVisible()) continue;
                        if (pControl->GetInterface(_T("MenuElement")) != NULL) continue;
                        if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) continue;
                        if (pControl->IsFloat()) {
                            if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) continue;
                            if (!pControl->Paint(hDC, rcPaint, pStopControl)) return FALSE;
                        }
                    }
                } else {
                    CRenderClipUI childClip;
                    CRenderClipUI::GenerateClip(hDC, rcTemp, childClip);
                    for (int it = 0; it < m_items.GetSize(); it++) {
                        CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
                        if (pControl == pStopControl) return FALSE;
                        if (!pControl->IsVisible()) continue;
                        if (pControl->GetInterface(_T("MenuElement")) != NULL) continue;
                        if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) continue;
                        if (pControl->IsFloat()) {
                            if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) continue;
                            CRenderClipUI::UseOldClipBegin(hDC, childClip);
                            if (!pControl->Paint(hDC, rcPaint, pStopControl)) return FALSE;
                            CRenderClipUI::UseOldClipEnd(hDC, childClip);
                        } else {
                            if (!::IntersectRect(&rcTemp, &rc, &pControl->GetPos())) continue;
                            if (!pControl->Paint(hDC, rcPaint, pStopControl)) return FALSE;
                        }
                    }
                }
            }
        }

        if (m_pVerticalScrollBar != NULL) {
            if (m_pVerticalScrollBar == pStopControl) return FALSE;
            if (m_pVerticalScrollBar->IsVisible()) {
                if (::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos())) {
                    if (!m_pVerticalScrollBar->Paint(hDC, rcPaint, pStopControl)) return FALSE;
                }
            }
        }

        if (m_pHorizontalScrollBar != NULL) {
            if (m_pHorizontalScrollBar == pStopControl) return FALSE;
            if (m_pHorizontalScrollBar->IsVisible()) {
                if (::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos())) {
                    if (!m_pHorizontalScrollBar->Paint(hDC, rcPaint, pStopControl)) return FALSE;
                }
            }
        }
        return TRUE;
    }

    void CMenuElementUI::DrawItemIcon(HDC hDC, const RECT & rcItem)
    {
        if (!m_strIcon.IsEmpty() && !(m_bCheckItem && !GetChecked())) {
            SIZE m_cxyFixed = CMenuElementUI::m_cxyFixed;
            m_cxyFixed.cx = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cx);
            m_cxyFixed.cy = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cy);

            SIZE m_szIconSize = CMenuElementUI::m_szIconSize;
            m_szIconSize.cx = GetManager()->GetDPIObj()->Scale(m_szIconSize.cx);
            m_szIconSize.cy = GetManager()->GetDPIObj()->Scale(m_szIconSize.cy);
            TLISTINFO_UI* pInfo = m_pOwner->GetListInfo();
            RECT rcTextPadding = pInfo->rcTextPadding;
            GetManager()->GetDPIObj()->Scale(&rcTextPadding);
            int padding = (rcTextPadding.left - m_szIconSize.cx) / 2;
            RECT rcDest =
            {
                padding,
                (m_cxyFixed.cy - m_szIconSize.cy) / 2,
                padding + m_szIconSize.cx,
                (m_cxyFixed.cy - m_szIconSize.cy) / 2 + m_szIconSize.cy
            };
            GetManager()->GetDPIObj()->ScaleBack(&rcDest);
            CStringUI pStrImage;
            pStrImage.Format(_T("dest='%d,%d,%d,%d'"), rcDest.left, rcDest.top, rcDest.right, rcDest.bottom);
            DrawImage(hDC, m_strIcon, pStrImage);
        }
    }

    void CMenuElementUI::DrawItemExpland(HDC hDC, const RECT & rcItem)
    {
        if (m_bShowExplandIcon) {
            CStringUI strExplandIcon;
            strExplandIcon = GetManager()->GetDefaultAttributeList(_T("ExplandIcon"));
            if (strExplandIcon.IsEmpty()) {
                return;
            }
            SIZE m_cxyFixed = CMenuElementUI::m_cxyFixed;
            m_cxyFixed.cx = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cx);
            m_cxyFixed.cy = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cy);
            int padding = GetManager()->GetDPIObj()->Scale(ITEM_DEFAULT_EXPLAND_ICON_WIDTH) / 3;
            const TDRAWINFO_UI* pDrawInfo = GetManager()->GetDrawInfo((LPCTSTR)strExplandIcon, NULL);
            const TIMAGEINFO_UI* pImageInfo = GetManager()->GetImageEx(pDrawInfo->sImageName, NULL, 0);
            if (!pImageInfo) {
                return;
            }
            RECT rcDest =
            {
                m_cxyFixed.cx - pImageInfo->nX - padding,
                (m_cxyFixed.cy - pImageInfo->nY) / 2,
                m_cxyFixed.cx - pImageInfo->nX - padding + pImageInfo->nX,
                (m_cxyFixed.cy - pImageInfo->nY) / 2 + pImageInfo->nY
            };
            GetManager()->GetDPIObj()->ScaleBack(&rcDest);
            CStringUI pStrImage;
            pStrImage.Format(_T("dest='%d,%d,%d,%d'"), rcDest.left, rcDest.top, rcDest.right, rcDest.bottom);
            DrawImage(hDC, strExplandIcon, pStrImage);
        }
    }


    void CMenuElementUI::DrawItemText(HDC hDC, const RECT & rcItem)
    {
        CStringUI sText = GetText();
        if (sText.IsEmpty()) return;

        if (m_pOwner == NULL) return;
        TLISTINFO_UI * pInfo = m_pOwner->GetListInfo();
        DWORD iTextColor = pInfo->dwTextColor;
        if ((m_uButtonState & UISTATE_HOT) != 0) {
            iTextColor = pInfo->dwHotTextColor;
        }
        if (IsSelected()) {
            iTextColor = pInfo->dwSelectedTextColor;
        }
        if (!IsEnabled()) {
            iTextColor = pInfo->dwDisabledTextColor;
        }
        int nLinks = 0;
        RECT rcText = rcItem;
        RECT rcTextPadding = pInfo->rcTextPadding;
        GetManager()->GetDPIObj()->Scale(&rcTextPadding);
        rcText.left += rcTextPadding.left;
        rcText.right -= rcTextPadding.right;
        rcText.top += rcTextPadding.top;
        rcText.bottom -= rcTextPadding.bottom;

        if (pInfo->bShowHtml)
            CRenderUI::DrawHtmlText(hDC, m_pManager, rcText, sText, iTextColor, \
                NULL, NULL, nLinks, pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
        else
            CRenderUI::DrawText(hDC, m_pManager, rcText, sText, iTextColor, \
                pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
    }


    SIZE CMenuElementUI::EstimateSize(SIZE szAvailable)
    {
        SIZE m_cxyFixed = CMenuElementUI::m_cxyFixed;
        m_cxyFixed.cx = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cx);
        m_cxyFixed.cy = GetManager()->GetDPIObj()->Scale(m_cxyFixed.cy);
        SIZE cXY = {0};
        for (int it = 0; it < GetCount(); it++) {
            CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
            if (!pControl->IsVisible()) continue;
            SIZE sz = pControl->EstimateSize(szAvailable);
            cXY.cy += sz.cy;
            if (cXY.cx < sz.cx)
                cXY.cx = sz.cx;
        }
        if (cXY.cy == 0) {
            TLISTINFO_UI* pInfo = m_pOwner->GetListInfo();

            DWORD iTextColor = pInfo->dwTextColor;
            if ((m_uButtonState & UISTATE_HOT) != 0) {
                iTextColor = pInfo->dwHotTextColor;
            }
            if (IsSelected()) {
                iTextColor = pInfo->dwSelectedTextColor;
            }
            if (!IsEnabled()) {
                iTextColor = pInfo->dwDisabledTextColor;
            }
            CStringUI sText = GetText();

            RECT rcText = {0, 0, MAX(szAvailable.cx, m_cxyFixed.cx), 9999};
            RECT rcTextPadding = pInfo->rcTextPadding;
            GetManager()->GetDPIObj()->Scale(&rcTextPadding);
            rcText.left += rcTextPadding.left;
            rcText.right -= rcTextPadding.right;
            if (pInfo->bShowHtml) {
                int nLinks = 0;
                CRenderUI::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, iTextColor, NULL, NULL, nLinks, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle);
            } else {
                CRenderUI::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, sText, iTextColor, pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle);
            }
            cXY.cx = rcText.right - rcText.left + rcTextPadding.left + rcTextPadding.right;
            cXY.cy = rcText.bottom - rcText.top + rcTextPadding.top + rcTextPadding.bottom;
        }

        if (m_cxyFixed.cy != 0) cXY.cy = m_cxyFixed.cy;
        if (cXY.cx < m_cxyFixed.cx)
            cXY.cx = m_cxyFixed.cx;

        CMenuElementUI::m_cxyFixed.cy = MulDiv(cXY.cy, 100, GetManager()->GetDPIObj()->GetScale());
        CMenuElementUI::m_cxyFixed.cx = MulDiv(cXY.cx, 100, GetManager()->GetDPIObj()->GetScale());
        return cXY;
    }

    void CMenuElementUI::DoEvent(struct TEVENT_UI& event)
    {
        if (event.Type == UIEVENT_MOUSEENTER) {
            CListContainerElementUI::DoEvent(event);
            if (m_pWindow) return;
            BOOL hasSubMenu = FALSE;
            for (int i = 0; i < GetCount(); ++i) {
                if (GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL) {
                    (static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(TRUE);
                    (static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(TRUE);

                    hasSubMenu = TRUE;
                }
            }
            if (hasSubMenu) {
                m_pOwner->SelectItem(GetIndex(), TRUE);
                CreateMenuWnd();
            } else {
                ContextMenuParam param;
                param.hWnd = m_pManager->GetPaintWindow();
                param.wParam = 2;
                CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
                m_pOwner->SelectItem(GetIndex(), TRUE);
            }
            return;
        }


        if (event.Type == UIEVENT_MOUSELEAVE) {

            BOOL hasSubMenu = FALSE;
            for (int i = 0; i < GetCount(); ++i) {
                if (GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL) {

                    hasSubMenu = TRUE;
                }
            }

            if (!hasSubMenu) {
                m_pOwner->SelectItem(-1, TRUE);
            }
        }

        if (event.Type == UIEVENT_BUTTONUP) {
            if (IsEnabled()) {
                CListContainerElementUI::DoEvent(event);

                if (m_pWindow) return;

                BOOL hasSubMenu = FALSE;
                for (int i = 0; i < GetCount(); ++i) {
                    if (GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL) {
                        (static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(TRUE);
                        (static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(TRUE);

                        hasSubMenu = TRUE;
                    }
                }
                if (hasSubMenu) {
                    CreateMenuWnd();
                } else {
                    SetChecked(!GetChecked());


                    BOOL isClosing = FALSE;
                    CMenuUI* menuUI = static_cast<CMenuUI*>(GetManager()->GetRoot());
                    isClosing = (menuUI->m_pWindow->isClosing);
                    if (IsWindow(GetManager()->GetPaintWindow()) && !isClosing) {
                        if (CMenuWnd::GetGlobalContextMenuObserver().GetManager() != NULL) {

                            MenuCmd* pMenuCmd = new MenuCmd();
                            lstrcpy(pMenuCmd->szName, GetName().GetData());
                            lstrcpy(pMenuCmd->szUserData, GetUserData().GetData());
                            lstrcpy(pMenuCmd->szText, GetText().GetData());
                            pMenuCmd->bChecked = GetChecked();
                            if (!PostMessage(CMenuWnd::GetGlobalContextMenuObserver().GetManager()->GetPaintWindow(), WM_MENUCLICK, (WPARAM)pMenuCmd, (LPARAM)this)) {
                                delete pMenuCmd;
                                pMenuCmd = NULL;
                            }
                        }
                    }
                    ContextMenuParam param;
                    param.hWnd = m_pManager->GetPaintWindow();
                    param.wParam = 1;
                    CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
                }
            }

            return;
        }

        if (event.Type == UIEVENT_KEYDOWN && event.chKey == VK_RIGHT) {
            if (m_pWindow) return;
            BOOL hasSubMenu = FALSE;
            for (int i = 0; i < GetCount(); ++i) {
                if (GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL) {
                    (static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(TRUE);
                    (static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(TRUE);
                    hasSubMenu = TRUE;
                }
            }
            if (hasSubMenu) {
                m_pOwner->SelectItem(GetIndex(), TRUE);
                CreateMenuWnd();
            } else {
                ContextMenuParam param;
                param.hWnd = m_pManager->GetPaintWindow();
                param.wParam = 2;
                CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
                m_pOwner->SelectItem(GetIndex(), TRUE);
            }

            return;
        }

        CListContainerElementUI::DoEvent(event);
    }

    CMenuWnd* CMenuElementUI::GetMenuWnd()
    {
        return m_pWindow;
    }

    void CMenuElementUI::CreateMenuWnd()
    {
        if (m_pWindow) return;

        m_pWindow = new CMenuWnd();
        ASSERT(m_pWindow);

        ContextMenuParam param;
        param.hWnd = m_pManager->GetPaintWindow();
        param.wParam = 2;
        CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);

        m_pWindow->Init(static_cast<CMenuElementUI*>(this), _T(""), CPointUI(), NULL);
    }

    void CMenuElementUI::SetLineType()
    {
        m_bDrawLine = TRUE;
        if (m_cxyFixed.cy == 0 || m_cxyFixed.cy == ITEM_DEFAULT_HEIGHT)
            SetFixedHeight(DEFAULT_LINE_HEIGHT);

        SetMouseChildEnabled(FALSE);
        SetMouseEnabled(FALSE);
        SetEnabled(FALSE);
    }

    void CMenuElementUI::SetLineColor(DWORD color)
    {
        m_dwLineColor = color;
    }

    DWORD CMenuElementUI::GetLineColor() const
    {
        return m_dwLineColor;
    }
    void CMenuElementUI::SetLinePadding(RECT rcInset)
    {
        m_rcLinePadding = rcInset;
    }

    RECT CMenuElementUI::GetLinePadding() const
    {
        return m_rcLinePadding;
    }

    void CMenuElementUI::SetIcon(LPCTSTR strIcon)
    {
        if (strIcon != _T("")) {
            m_strIcon = strIcon;
        }
    }

    void CMenuElementUI::SetIconSize(LONG cx, LONG cy)
    {
        m_szIconSize.cx = cx;
        m_szIconSize.cy = cy;
    }

    void CMenuElementUI::SetChecked(BOOL bCheck/* = TRUE*/)
    {
        SetItemInfo(GetName(), bCheck);
    }

    BOOL CMenuElementUI::GetChecked() const
    {
        LPCTSTR pstrName = GetName();
        if (pstrName == NULL || lstrlen(pstrName) <= 0) return FALSE;

        CStringMapUI * mCheckInfos = CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo();
        if (mCheckInfos != NULL) {
            MenuItemInfo* pItemInfo = (MenuItemInfo*)mCheckInfos->Find(pstrName);
            if (pItemInfo != NULL) {
                return pItemInfo->bChecked;
            }
        }
        return FALSE;

    }

    void CMenuElementUI::SetCheckItem(BOOL bCheckItem/* = FALSE*/)
    {
        m_bCheckItem = bCheckItem;
    }

    BOOL CMenuElementUI::GetCheckItem() const
    {
        return m_bCheckItem;
    }

    void CMenuElementUI::SetShowExplandIcon(BOOL bShow)
    {
        m_bShowExplandIcon = bShow;
    }

    void CMenuElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if (_tcsicmp(pstrName, _T("icon")) == 0) {
            SetIcon(pstrValue);
        } else if (_tcsicmp(pstrName, _T("iconsize")) == 0) {
            LPTSTR pstr = NULL;
            LONG cx = 0, cy = 0;
            cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
            cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            SetIconSize(cx, cy);
        } else if (_tcsicmp(pstrName, _T("checkitem")) == 0) {
            SetCheckItem(_tcsicmp(pstrValue, _T("TRUE")) == 0?TRUE:FALSE);
        } else if (_tcsicmp(pstrName, _T("ischeck")) == 0) {
            CStringMapUI* mCheckInfos = CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo();
            if (mCheckInfos != NULL) {
                BOOL bFind = FALSE;
                for (int i = 0; i < mCheckInfos->GetSize(); i++) {
                    MenuItemInfo* itemInfo = (MenuItemInfo*)mCheckInfos->GetAt(i);
                    if (lstrcmpi(itemInfo->szName, GetName()) == 0) {
                        bFind = TRUE;
                        break;
                    }
                }
                if (!bFind) SetChecked(_tcsicmp(pstrValue, _T("TRUE")) == 0?TRUE:FALSE);
            }
        } else if (_tcsicmp(pstrName, _T("linetype")) == 0) {
            if (_tcsicmp(pstrValue, _T("TRUE")) == 0)
                SetLineType();
        } else if (_tcsicmp(pstrName, _T("expland")) == 0) {
            SetShowExplandIcon(_tcsicmp(pstrValue, _T("TRUE")) == 0?TRUE:FALSE);
        } else if (_tcsicmp(pstrName, _T("linecolor")) == 0) {
            if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            SetLineColor(_tcstoul(pstrValue, &pstr, 16));
        } else if (_tcsicmp(pstrName, _T("linepadding")) == 0) {
            RECT rcInset = {0};
            LPTSTR pstr = NULL;
            rcInset.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
            rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            SetLinePadding(rcInset);
        } else if (_tcsicmp(pstrName, _T("height")) == 0) {
            SetFixedHeight(_ttoi(pstrValue));
        } else
            CListContainerElementUI::SetAttribute(pstrName, pstrValue);
    }


    MenuItemInfo* CMenuElementUI::GetItemInfo(LPCTSTR pstrName)
    {
        if (pstrName == NULL || lstrlen(pstrName) <= 0) return NULL;

        CStringMapUI * mCheckInfos = CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo();
        if (mCheckInfos != NULL) {
            MenuItemInfo* pItemInfo = (MenuItemInfo*)mCheckInfos->Find(pstrName);
            if (pItemInfo != NULL) {
                return pItemInfo;
            }
        }

        return NULL;
    }

    MenuItemInfo* CMenuElementUI::SetItemInfo(LPCTSTR pstrName, BOOL bChecked)
    {
        if (pstrName == NULL || lstrlen(pstrName) <= 0) return NULL;

        CStringMapUI * mCheckInfos = CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo();
        if (mCheckInfos != NULL) {
            MenuItemInfo* pItemInfo = (MenuItemInfo*)mCheckInfos->Find(pstrName);
            if (pItemInfo == NULL) {
                pItemInfo = new MenuItemInfo;
                lstrcpy(pItemInfo->szName, pstrName);
                pItemInfo->bChecked = bChecked;
                mCheckInfos->Insert(pstrName, pItemInfo);
            } else {
                pItemInfo->bChecked = bChecked;
            }

            return pItemInfo;
        }
        return NULL;
    }
} // namespace DUI
