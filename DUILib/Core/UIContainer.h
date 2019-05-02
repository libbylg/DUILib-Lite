#ifndef __UICONTAINER_H__
#define __UICONTAINER_H__

#include "Core/UIControl.h"
#include "Core/UIScrollBar.h"

namespace DUI
{
    /////////////////////////////////////////////////////////////////////////////////////
    //

    class IContainerUI
    {
    public:
        virtual CControlUI* GetItemAt(int iIndex) const = 0;
        virtual int GetItemIndex(CControlUI* pControl) const = 0;
        virtual BOOL SetItemIndex(CControlUI* pControl, int iIndex) = 0;
        virtual int GetCount() const = 0;
        virtual BOOL Add(CControlUI* pControl) = 0;
        virtual BOOL AddAt(CControlUI* pControl, int iIndex) = 0;
        virtual BOOL Remove(CControlUI* pControl) = 0;
        virtual BOOL RemoveAt(int iIndex) = 0;
        virtual void RemoveAll() = 0;
    };


    /////////////////////////////////////////////////////////////////////////////////////
    //
    class CScrollBarUI;

    class DUILIB_API CContainerUI : public CControlUI, public IContainerUI
    {
        UI_DECLARE_CONTROL(CContainerUI)

    public:
        CContainerUI();
        virtual ~CContainerUI();

    public:
        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        CControlUI* GetItemAt(int iIndex) const;
        int GetItemIndex(CControlUI* pControl) const;
        BOOL SetItemIndex(CControlUI* pControl, int iIndex);
        int GetCount() const;
        BOOL Add(CControlUI* pControl);
        BOOL AddAt(CControlUI* pControl, int iIndex);
        BOOL Remove(CControlUI* pControl);
        BOOL RemoveAt(int iIndex);
        void RemoveAll();

        void DoEvent(TEVENT_UI& event);
        void SetVisible(BOOL bVisible = true);
        void SetInternVisible(BOOL bVisible = true);
        void SetEnabled(BOOL bEnabled);
        void SetMouseEnabled(BOOL bEnable = true);

        virtual RECT GetInset() const;
        virtual void SetInset(RECT rcInset); // 设置内边距，相当于设置客户区
        virtual int GetChildPadding() const;
        virtual void SetChildPadding(int iPadding);
        virtual UINT GetChildAlign() const;
        virtual void SetChildAlign(UINT iAlign);
        virtual UINT GetChildVAlign() const;
        virtual void SetChildVAlign(UINT iVAlign);
        virtual BOOL IsAutoDestroy() const;
        virtual void SetAutoDestroy(BOOL bAuto);
        virtual BOOL IsDelayedDestroy() const;
        virtual void SetDelayedDestroy(BOOL bDelayed);
        virtual BOOL IsMouseChildEnabled() const;
        virtual void SetMouseChildEnabled(BOOL bEnable = true);

        virtual int FindSelectable(int iIndex, BOOL bForward = true) const;

        RECT GetClientPos() const;
        void SetPos(RECT rc, BOOL bNeedInvalidate = true);
        void Move(SIZE szOffset, BOOL bNeedInvalidate = true);
        BOOL DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        void SetManager(CManagerUI* pManager, CControlUI* pParent, BOOL bInit = true);
        CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

        BOOL SetSubControlText(LPCTSTR pstrSubControlName, LPCTSTR pstrText);
        BOOL SetSubControlFixedHeight(LPCTSTR pstrSubControlName, int cy);
        BOOL SetSubControlFixedWdith(LPCTSTR pstrSubControlName, int cx);
        BOOL SetSubControlUserData(LPCTSTR pstrSubControlName, LPCTSTR pstrText);

        CStringUI GetSubControlText(LPCTSTR pstrSubControlName);
        int GetSubControlFixedHeight(LPCTSTR pstrSubControlName);
        int GetSubControlFixedWdith(LPCTSTR pstrSubControlName);
        const CStringUI GetSubControlUserData(LPCTSTR pstrSubControlName);
        CControlUI* FindSubControl(LPCTSTR pstrSubControlName);

        virtual SIZE GetScrollPos() const;
        virtual SIZE GetScrollRange() const;
        virtual void SetScrollPos(SIZE szPos, BOOL bMsg = true);
        virtual void SetScrollStepSize(int nSize);
        virtual int GetScrollStepSize() const;
        virtual void LineUp();
        virtual void LineDown();
        virtual void PageUp();
        virtual void PageDown();
        virtual void HomeUp();
        virtual void EndDown();
        virtual void LineLeft();
        virtual void LineRight();
        virtual void PageLeft();
        virtual void PageRight();
        virtual void HomeLeft();
        virtual void EndRight();
        virtual void EnableScrollBar(BOOL bEnableVertical = true, BOOL bEnableHorizontal = false);
        virtual CScrollBarUI* GetVerticalScrollBar() const;
        virtual CScrollBarUI* GetHorizontalScrollBar() const;

    protected:
        virtual void SetFloatPos(int iIndex);
        virtual void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);

    protected:
        CStdPtrArray m_items;
        RECT m_rcInset;
        int m_iChildPadding;
        UINT m_iChildAlign;
        UINT m_iChildVAlign;
        BOOL m_bAutoDestroy;
        BOOL m_bDelayedDestroy;
        BOOL m_bMouseChildEnabled;
        int	 m_nScrollStepSize;

        CScrollBarUI* m_pVerticalScrollBar;
        CScrollBarUI* m_pHorizontalScrollBar;
        CStringUI	m_sVerticalScrollBarStyle;
        CStringUI	m_sHorizontalScrollBarStyle;
    };

} // namespace DUI

#endif // __UICONTAINER_H__
