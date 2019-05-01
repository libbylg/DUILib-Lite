#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace DUILIB {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CComboWnd;

	class DUILIB_API CComboUI : public CContainerUI, public IListOwnerUI
	{
		DECLARE_DUICONTROL(CComboUI)
		friend class CComboWnd;
	public:
		CComboUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoInit();
		UINT GetControlFlags() const;

		CDuiString GetText() const;
		void SetEnabled(BOOL bEnable = true);

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
		void SetShowHtml(BOOL bShowHtml = true);
		BOOL IsShowShadow();
		void SetShowShadow(BOOL bShow = true);

		CDuiString GetDropBoxAttributeList();
		void SetDropBoxAttributeList(LPCTSTR pstrList);
		SIZE GetDropBoxSize() const;
		void SetDropBoxSize(SIZE szDropBox);
		RECT GetDropBoxInset() const;
		void SetDropBoxInset(RECT szDropBox);

		UINT GetListType();
		TListInfoUI* GetListInfo();
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
		void SetItemShowHtml(BOOL bShowHtml = true);

		SIZE EstimateSize(SIZE szAvailable);
		void SetPos(RECT rc, BOOL bNeedInvalidate = true);
		void Move(SIZE szOffset, BOOL bNeedInvalidate = true);
		void DoEvent(struct TEventUI& event);
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
		CDuiString m_sDropBoxAttributes;
		SIZE m_szDropBox;
		RECT m_rcDropBox;
		UINT m_uButtonState;

		CDuiString m_sNormalImage;
		CDuiString m_sHotImage;
		CDuiString m_sPushedImage;
		CDuiString m_sFocusedImage;
		CDuiString m_sDisabledImage;

		BOOL m_bScrollSelect;
		TListInfoUI m_ListInfo;
	};

} // namespace DUILIB

#endif // __UICOMBO_H__
