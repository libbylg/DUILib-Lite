#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#include "Core/UIContainer.h"
#include "Control/UIList.h"

namespace DUI
{

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class CComboWnd;

    class DUILIB_API CComboUI : public CContainerUI, public IListOwnerUI
    {
        UI_DECLARE_CONTROL(CComboUI)
            friend class CComboWnd;
    public:
        CComboUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        void DoInit();
        UINT GetControlFlags() const;

        CStringUI GetText() const;
        void SetEnabled(BOOL bEnable = TRUE);

        void SetTextStyle(UINT uStyle);
        UINT GetTextStyle() const;
        void SetTextColor(DWORD dwTextColor);
        DWORD GetTextColor() const;
        void SetDisabledTextColor(DWORD dwTextColor);
        DWORD GetDisabledTextColor() const;
        void SetFont(int index);
        int GetFont() const;
        RECT GetTextPadding() const;
        void SetTextPadding(RECT rc);
        BOOL IsShowHtml();
        void SetShowHtml(BOOL bShowHtml = TRUE);
        BOOL IsShowShadow();
        void SetShowShadow(BOOL bShow = TRUE);

        CStringUI GetDropBoxAttributeList();
        void SetDropBoxAttributeList(LPCTSTR pstrList);
        SIZE GetDropBoxSize() const;
        void SetDropBoxSize(SIZE szDropBox);
        RECT GetDropBoxInset() const;
        void SetDropBoxInset(RECT szDropBox);

        UINT GetListType();
        TLISTINFO_UI* GetListInfo();
        int GetCurSel() const;
        BOOL SelectItem(int iIndex, BOOL bTakeFocus = FALSE);
        BOOL SelectMultiItem(int iIndex, BOOL bTakeFocus = FALSE);
        BOOL UnSelectItem(int iIndex, BOOL bOthers = FALSE);
        BOOL SetItemIndex(CControlUI* pControl, int iIndex);

        BOOL Add(CControlUI* pControl);
        BOOL AddAt(CControlUI* pControl, int iIndex);
        BOOL Remove(CControlUI* pControl);
        BOOL RemoveAt(int iIndex);
        void RemoveAll();

        BOOL Activate();

        LPCTSTR GetNormalImage() const;
        void SetNormalImage(LPCTSTR pStrImage);
        LPCTSTR GetHotImage() const;
        void SetHotImage(LPCTSTR pStrImage);
        LPCTSTR GetPushedImage() const;
        void SetPushedImage(LPCTSTR pStrImage);
        LPCTSTR GetFocusedImage() const;
        void SetFocusedImage(LPCTSTR pStrImage);
        LPCTSTR GetDisabledImage() const;
        void SetDisabledImage(LPCTSTR pStrImage);

        BOOL GetScrollSelect();
        void SetScrollSelect(BOOL bScrollSelect);

        void SetItemFont(int index);
        void SetItemTextStyle(UINT uStyle);
        RECT GetItemTextPadding() const;
        void SetItemTextPadding(RECT rc);
        DWORD GetItemTextColor() const;
        void SetItemTextColor(DWORD dwTextColor);
        DWORD GetItemBkColor() const;
        void SetItemBkColor(DWORD dwBkColor);
        LPCTSTR GetItemBkImage() const;
        void SetItemBkImage(LPCTSTR pStrImage);
        BOOL IsAlternateBk() const;
        void SetAlternateBk(BOOL bAlternateBk);
        DWORD GetSelectedItemTextColor() const;
        void SetSelectedItemTextColor(DWORD dwTextColor);
        DWORD GetSelectedItemBkColor() const;
        void SetSelectedItemBkColor(DWORD dwBkColor);
        LPCTSTR GetSelectedItemImage() const;
        void SetSelectedItemImage(LPCTSTR pStrImage);
        DWORD GetHotItemTextColor() const;
        void SetHotItemTextColor(DWORD dwTextColor);
        DWORD GetHotItemBkColor() const;
        void SetHotItemBkColor(DWORD dwBkColor);
        LPCTSTR GetHotItemImage() const;
        void SetHotItemImage(LPCTSTR pStrImage);
        DWORD GetDisabledItemTextColor() const;
        void SetDisabledItemTextColor(DWORD dwTextColor);
        DWORD GetDisabledItemBkColor() const;
        void SetDisabledItemBkColor(DWORD dwBkColor);
        LPCTSTR GetDisabledItemImage() const;
        void SetDisabledItemImage(LPCTSTR pStrImage);
        DWORD GetItemLineColor() const;
        void SetItemLineColor(DWORD dwLineColor);
        BOOL IsItemShowHtml();
        void SetItemShowHtml(BOOL bShowHtml = TRUE);

        SIZE EstimateSize(SIZE szAvailable);
        void SetPos(RECT rc, BOOL bNeedInvalidate = TRUE);
        void Move(SIZE szOffset, BOOL bNeedInvalidate = TRUE);
        void DoEvent(struct TEVENT_UI& event);
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        BOOL DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
        void PaintText(HDC hDC);
        void PaintStatusImage(HDC hDC);

    protected:
        CComboWnd* m_pWindow;

        int m_iCurSel;
        DWORD	m_dwTextColor;
        DWORD	m_dwDisabledTextColor;
        int		m_iFont;
        UINT	m_uTextStyle;
        RECT	m_rcTextPadding;
        BOOL	m_bShowHtml;
        BOOL	m_bShowShadow;
        CStringUI m_sDropBoxAttributes;
        SIZE m_szDropBox;
        RECT m_rcDropBox;
        UINT m_uButtonState;

        CStringUI m_sNormalImage;
        CStringUI m_sHotImage;
        CStringUI m_sPushedImage;
        CStringUI m_sFocusedImage;
        CStringUI m_sDisabledImage;

        BOOL m_bScrollSelect;
        TLISTINFO_UI m_ListInfo;
    };

} // namespace DUI

#endif // __UICOMBO_H__
