#ifndef __UILIST_H__
#define __UILIST_H__

namespace DUILIB {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	typedef int (CALLBACK *PULVCompareFunc)(UINT_PTR, UINT_PTR, UINT_PTR);

	class CListHeaderUI;

#define UILIST_MAX_COLUMNS 32

	typedef struct tagTListInfoUI
	{
		int nColumns;
		RECT rcColumn[UILIST_MAX_COLUMNS];
		int nFont;
		UINT uTextStyle;
		RECT rcTextPadding;
		DWORD dwTextColor;
		DWORD dwBkColor;
		CDuiString sBkImage;
		BOOL bAlternateBk;
		DWORD dwSelectedTextColor;
		DWORD dwSelectedBkColor;
		CDuiString sSelectedImage;
		DWORD dwHotTextColor;
		DWORD dwHotBkColor;
		CDuiString sHotImage;
		DWORD dwDisabledTextColor;
		DWORD dwDisabledBkColor;
		CDuiString sDisabledImage;
		DWORD dwLineColor;
		BOOL bShowRowLine;
		BOOL bShowColumnLine;
		BOOL bShowHtml;
		BOOL bMultiExpandable;
		BOOL bRSelected;
	} TListInfoUI;


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class IListCallbackUI
	{
	public:
		virtual LPCTSTR GetItemText(CControlUI* pList, int iItem, int iSubItem) = 0;
	};

	class IListOwnerUI
	{
	public:
		virtual UINT GetListType() = 0;
		virtual TListInfoUI* GetListInfo() = 0;
		virtual int GetCurSel() const = 0;
		virtual BOOL SelectItem(int iIndex, BOOL bTakeFocus = FALSE) = 0;
		virtual BOOL SelectMultiItem(int iIndex, BOOL bTakeFocus = FALSE) = 0;
		virtual BOOL UnSelectItem(int iIndex, BOOL bOthers = FALSE) = 0;
		virtual void DoEvent(TEventUI& event) = 0;
	};

	class IListUI : public IListOwnerUI
	{
	public:
		virtual CListHeaderUI* GetHeader() const = 0;
		virtual CContainerUI* GetList() const = 0;
		virtual IListCallbackUI* GetTextCallback() const = 0;
		virtual void SetTextCallback(IListCallbackUI* pCallback) = 0;
		virtual BOOL ExpandItem(int iIndex, BOOL bExpand = true) = 0;
		virtual int GetExpandedItem() const = 0;

		virtual void SetMultiSelect(BOOL bMultiSel) = 0;
		virtual BOOL IsMultiSelect() const = 0;
		virtual void SelectAllItems() = 0;
		virtual void UnSelectAllItems() = 0;
		virtual int GetSelectItemCount() const = 0;
		virtual int GetNextSelItem(int nItem) const = 0;
	};

	class IListItemUI
	{
	public:
		virtual int GetIndex() const = 0;
		virtual void SetIndex(int iIndex) = 0;
		virtual IListOwnerUI* GetOwner() = 0;
		virtual void SetOwner(CControlUI* pOwner) = 0;
		virtual BOOL IsSelected() const = 0;
		virtual BOOL Select(BOOL bSelect = true) = 0;
		virtual BOOL SelectMulti(BOOL bSelect = true) = 0;
		virtual BOOL IsExpanded() const = 0;
		virtual BOOL Expand(BOOL bExpand = true) = 0;
		virtual void DrawItemText(HDC hDC, const RECT& rcItem) = 0;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CListBodyUI;
	class CListHeaderUI;
	class CEditUI;
	class CComboBoxUI;
	class DUILIB_API CListUI : public CVerticalLayoutUI, public IListUI
	{
		DECLARE_DUICONTROL(CListUI)

	public:
		CListUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		BOOL GetScrollSelect();
		void SetScrollSelect(BOOL bScrollSelect);
		int GetCurSel() const;
		int GetCurSelActivate() const;
		BOOL SelectItem(int iIndex, BOOL bTakeFocus = FALSE);
		BOOL SelectItemActivate(int iIndex);    // 双击选中

		BOOL SelectMultiItem(int iIndex, BOOL bTakeFocus = FALSE);
		void SetMultiSelect(BOOL bMultiSel);
		BOOL IsMultiSelect() const;
		BOOL UnSelectItem(int iIndex, BOOL bOthers = FALSE);
		void SelectAllItems();
		void UnSelectAllItems();
		int GetSelectItemCount() const;
		int GetNextSelItem(int nItem) const;

		BOOL IsFixedScrollbar();
		void SetFixedScrollbar(BOOL bFixed);

		CListHeaderUI* GetHeader() const;  
		CContainerUI* GetList() const;
		UINT GetListType();
		TListInfoUI* GetListInfo();

		CControlUI* GetItemAt(int iIndex) const;
		int GetItemIndex(CControlUI* pControl) const;
		BOOL SetItemIndex(CControlUI* pControl, int iIndex);
		int GetCount() const;
		BOOL Add(CControlUI* pControl);
		BOOL AddAt(CControlUI* pControl, int iIndex);
		BOOL Remove(CControlUI* pControl);
		BOOL RemoveAt(int iIndex);
		void RemoveAll();

		void EnsureVisible(int iIndex);
		void Scroll(int dx, int dy);

		BOOL IsDelayedDestroy() const;
		void SetDelayedDestroy(BOOL bDelayed);
		int GetChildPadding() const;
		void SetChildPadding(int iPadding);

		void SetItemFont(int index);
		void SetItemTextStyle(UINT uStyle);
		void SetItemTextPadding(RECT rc);
		void SetItemTextColor(DWORD dwTextColor);
		void SetItemBkColor(DWORD dwBkColor);
		void SetItemBkImage(LPCTSTR pStrImage);
		void SetAlternateBk(BOOL bAlternateBk);
		void SetSelectedItemTextColor(DWORD dwTextColor);
		void SetSelectedItemBkColor(DWORD dwBkColor);
		void SetSelectedItemImage(LPCTSTR pStrImage); 
		void SetHotItemTextColor(DWORD dwTextColor);
		void SetHotItemBkColor(DWORD dwBkColor);
		void SetHotItemImage(LPCTSTR pStrImage);
		void SetDisabledItemTextColor(DWORD dwTextColor);
		void SetDisabledItemBkColor(DWORD dwBkColor);
		void SetDisabledItemImage(LPCTSTR pStrImage);
		void SetItemLineColor(DWORD dwLineColor);
		void SetItemShowRowLine(BOOL bShowLine = FALSE);
		void SetItemShowColumnLine(BOOL bShowLine = FALSE);
		BOOL IsItemShowHtml();
		void SetItemShowHtml(BOOL bShowHtml = true);
		BOOL IsItemRSelected();
		void SetItemRSelected(BOOL bSelected = true);
		RECT GetItemTextPadding() const;
		DWORD GetItemTextColor() const;
		DWORD GetItemBkColor() const;
		LPCTSTR GetItemBkImage() const;
		BOOL IsAlternateBk() const;
		DWORD GetSelectedItemTextColor() const;
		DWORD GetSelectedItemBkColor() const;
		LPCTSTR GetSelectedItemImage() const;
		DWORD GetHotItemTextColor() const;
		DWORD GetHotItemBkColor() const;
		LPCTSTR GetHotItemImage() const;
		DWORD GetDisabledItemTextColor() const;
		DWORD GetDisabledItemBkColor() const;
		LPCTSTR GetDisabledItemImage() const;
		DWORD GetItemLineColor() const;

		void SetMultiExpanding(BOOL bMultiExpandable); 
		int GetExpandedItem() const;
		BOOL ExpandItem(int iIndex, BOOL bExpand = true);

		void SetPos(RECT rc, BOOL bNeedInvalidate = true);
		void Move(SIZE szOffset, BOOL bNeedInvalidate = true);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		IListCallbackUI* GetTextCallback() const;
		void SetTextCallback(IListCallbackUI* pCallback);

		SIZE GetScrollPos() const;
		SIZE GetScrollRange() const;
		void SetScrollPos(SIZE szPos, BOOL bMsg = true);
		void LineUp();
		void LineDown();
		void PageUp();
		void PageDown();
		void HomeUp();
		void EndDown();
		void LineLeft();
		void LineRight();
		void PageLeft();
		void PageRight();
		void HomeLeft();
		void EndRight();
		void EnableScrollBar(BOOL bEnableVertical = true, BOOL bEnableHorizontal = FALSE);
		virtual CScrollBarUI* GetVerticalScrollBar() const;
		virtual CScrollBarUI* GetHorizontalScrollBar() const;
		BOOL SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);

		virtual BOOL CheckColumEditable(int nColum) { return FALSE; };
		virtual CRichEditUI* GetEditUI() { return NULL; };
		virtual BOOL CheckColumComboBoxable(int nColum) { return FALSE; };
		virtual CComboBoxUI* GetComboBoxUI() { return NULL; };

	protected:
		int GetMinSelItemIndex();
		int GetMaxSelItemIndex();

	protected:
		BOOL m_bFixedScrollbar;
		BOOL m_bScrollSelect;
		int m_iCurSel;
		BOOL m_bMultiSel;
		CStdPtrArray m_aSelItems;
		int m_iCurSelActivate;  // 双击的列
		int m_iExpandedItem;
		IListCallbackUI* m_pCallback;
		CListBodyUI* m_pList;
		CListHeaderUI* m_pHeader;
		TListInfoUI m_ListInfo;

	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CListBodyUI : public CVerticalLayoutUI
	{
	public:
		CListBodyUI(CListUI* pOwner);


		int GetScrollStepSize() const;
		void SetScrollPos(SIZE szPos, BOOL bMsg = true);
		void SetPos(RECT rc, BOOL bNeedInvalidate = true);
		void DoEvent(TEventUI& event);
		BOOL SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);
	protected:
		static int __cdecl ItemComareFunc(void *pvlocale, const void *item1, const void *item2);
		int __cdecl ItemComareFunc(const void *item1, const void *item2);
	protected:
		CListUI* m_pOwner;
		PULVCompareFunc m_pCompareFunc;
		UINT_PTR m_compareData;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CListHeaderUI : public CHorizontalLayoutUI
	{
		DECLARE_DUICONTROL(CListHeaderUI)
	public:
		CListHeaderUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		SIZE EstimateSize(SIZE szAvailable);
		void SetPos(RECT rc, BOOL bNeedInvalidate = true);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void SetScaleHeader(BOOL bIsScale);
		BOOL IsScaleHeader() const;

	private:
		BOOL m_bIsScaleHeader;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CListHeaderItemUI : public CContainerUI
	{
		DECLARE_DUICONTROL(CListHeaderItemUI)

	public:
		CListHeaderItemUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetEnabled(BOOL bEnable = true);

		BOOL IsDragable() const;
		void SetDragable(BOOL bDragable);
		DWORD GetSepWidth() const;
		void SetSepWidth(int iWidth);
		DWORD GetTextStyle() const;
		void SetTextStyle(UINT uStyle);
		DWORD GetTextColor() const;
		void SetTextColor(DWORD dwTextColor);
		void SetTextPadding(RECT rc);
		RECT GetTextPadding() const;
		void SetFont(int index);
		BOOL IsShowHtml();
		void SetShowHtml(BOOL bShowHtml = true);
		LPCTSTR GetNormalImage() const;
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage() const;
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetPushedImage() const;
		void SetPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage() const;
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetSepImage() const;
		void SetSepImage(LPCTSTR pStrImage);
		void SetScale(int nScale);
		int GetScale() const;

		void DoEvent(TEventUI& event);
		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		RECT GetThumbRect() const;

		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

	protected:
		POINT ptLastMouse;
		BOOL m_bDragable;
		UINT m_uButtonState;
		int m_iSepWidth;
		DWORD m_dwTextColor;
		int m_iFont;
		UINT m_uTextStyle;
		BOOL m_bShowHtml;
		RECT m_rcTextPadding;
		CDuiString m_sNormalImage;
		CDuiString m_sHotImage;
		CDuiString m_sPushedImage;
		CDuiString m_sFocusedImage;
		CDuiString m_sSepImage;
		CDuiString m_sSepImageModify;
		int m_nScale;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CListElementUI : public CControlUI, public IListItemUI
	{
	public:
		CListElementUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetEnabled(BOOL bEnable = true);

		int GetIndex() const;
		void SetIndex(int iIndex);

		IListOwnerUI* GetOwner();
		void SetOwner(CControlUI* pOwner);
		void SetVisible(BOOL bVisible = true);

		BOOL IsSelected() const;
		BOOL Select(BOOL bSelect = true);
		BOOL SelectMulti(BOOL bSelect = true);
		BOOL IsExpanded() const;
		BOOL Expand(BOOL bExpand = true);

		void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
		BOOL Activate();

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void DrawItemBk(HDC hDC, const RECT& rcItem);

	protected:
		int m_iIndex;
		BOOL m_bSelected;
		UINT m_uButtonState;
		IListOwnerUI* m_pOwner;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CListLabelElementUI : public CListElementUI
	{
		DECLARE_DUICONTROL(CListLabelElementUI)
	public:
		CListLabelElementUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoEvent(TEventUI& event);
		SIZE EstimateSize(SIZE szAvailable);
		BOOL DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		void DrawItemText(HDC hDC, const RECT& rcItem);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CListTextElementUI : public CListLabelElementUI
	{
		DECLARE_DUICONTROL(CListTextElementUI)
	public:
		CListTextElementUI();
		~CListTextElementUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		LPCTSTR GetText(int iIndex) const;
		void SetText(int iIndex, LPCTSTR pstrText);

		void SetOwner(CControlUI* pOwner);
		CDuiString* GetLinkContent(int iIndex);

		void DoEvent(TEventUI& event);
		SIZE EstimateSize(SIZE szAvailable);

		void DrawItemText(HDC hDC, const RECT& rcItem);

	protected:
		enum { MAX_LINK = 8 };
		int m_nLinks;
		RECT m_rcLinks[MAX_LINK];
		CDuiString m_sLinks[MAX_LINK];
		int m_nHoverLink;
		IListUI* m_pOwner;
		CStdPtrArray m_aTexts;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CListContainerElementUI : public CHorizontalLayoutUI, public IListItemUI
	{
		DECLARE_DUICONTROL(CListContainerElementUI)
	public:
		CListContainerElementUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		int GetIndex() const;
		void SetIndex(int iIndex);

		IListOwnerUI* GetOwner();
		void SetOwner(CControlUI* pOwner);
		void SetVisible(BOOL bVisible = true);
		void SetEnabled(BOOL bEnable = true);

		BOOL IsSelected() const;
		BOOL Select(BOOL bSelect = true);
		BOOL SelectMulti(BOOL bSelect = true);
		BOOL IsExpanded() const;
		BOOL Expand(BOOL bExpand = true);

		void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
		BOOL Activate();

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		BOOL DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		virtual void DrawItemText(HDC hDC, const RECT& rcItem);    
		virtual void DrawItemBk(HDC hDC, const RECT& rcItem);

		void SetPos(RECT rc, BOOL bNeedInvalidate = true);

	protected:
		int m_iIndex;
		BOOL m_bSelected;
		UINT m_uButtonState;
		IListOwnerUI* m_pOwner;
	};

} // namespace DUILIB

#endif // __UILIST_H__
