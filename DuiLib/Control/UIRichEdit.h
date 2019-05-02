#ifndef __UIRICHEDIT_H__
#define __UIRICHEDIT_H__

#pragma once

namespace DUI {

	class CTxtWinHost;

	class DUILIB_API CRichEditUI : public CContainerUI, public IMessageFilterUI
	{
		UI_DECLARE_CONTROL(CRichEditUI)
	public:
		CRichEditUI();
		~CRichEditUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		BOOL IsMultiLine();
		void SetMultiLine(BOOL bMultiLine);
		BOOL IsWantTab();
		void SetWantTab(BOOL bWantTab = true);
		BOOL IsWantReturn();
		void SetWantReturn(BOOL bWantReturn = true);
		BOOL IsWantCtrlReturn();
		void SetWantCtrlReturn(BOOL bWantCtrlReturn = true);
		BOOL IsTransparent();
		void SetTransparent(BOOL bTransparent = true);
		BOOL IsRich();
		void SetRich(BOOL bRich = true);
		BOOL IsReadOnly();
		void SetReadOnly(BOOL bReadOnly = true);
		BOOL IsWordWrap();
		void SetWordWrap(BOOL bWordWrap = true);
		int GetFont();
		void SetFont(int index);
		void SetFont(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic);
		LONG GetWinStyle();
		void SetWinStyle(LONG lStyle);
		DWORD GetTextColor();
		void SetTextColor(DWORD dwTextColor);
		int GetLimitText();
		void SetLimitText(int iChars);
		long GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
		CDuiString GetText() const;
		void SetText(LPCTSTR pstrText);
		BOOL IsModify() const;
		void SetModify(BOOL bModified = true) const;
		void GetSel(CHARRANGE &cr) const;
		void GetSel(long& nStartChar, long& nEndChar) const;
		int SetSel(CHARRANGE &cr);
		int SetSel(long nStartChar, long nEndChar);
		void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo);
		void ReplaceSelW(LPCWSTR lpszNewText, BOOL bCanUndo = FALSE);
		CDuiString GetSelText() const;
		int SetSelAll();
		int SetSelNone();
		WORD GetSelectionType() const;
		BOOL GetZoom(int& nNum, int& nDen) const;
		BOOL SetZoom(int nNum, int nDen);
		BOOL SetZoomOff();
		BOOL GetAutoURLDetect() const;
		BOOL SetAutoURLDetect(BOOL bAutoDetect = true);
		DWORD GetEventMask() const;
		DWORD SetEventMask(DWORD dwEventMask);
		CDuiString GetTextRange(long nStartChar, long nEndChar) const;
		void HideSelection(BOOL bHide = true, BOOL bChangeStyle = FALSE);
		void ScrollCaret();
		int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, BOOL bCanUndo = FALSE);
		int AppendText(LPCTSTR lpstrText, BOOL bCanUndo = FALSE);
		DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
		BOOL SetDefaultCharFormat(CHARFORMAT2 &cf);
		DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
		BOOL SetSelectionCharFormat(CHARFORMAT2 &cf);
		BOOL SetWordCharFormat(CHARFORMAT2 &cf);
		DWORD GetParaFormat(PARAFORMAT2 &pf) const;
		BOOL SetParaFormat(PARAFORMAT2 &pf);
		BOOL CanUndo();
		BOOL CanRedo();
		BOOL CanPaste();
		BOOL Redo();
		BOOL Undo();
		void Clear();
		void Copy();
		void Cut();
		void Paste();
		int GetLineCount() const;
		CDuiString GetLine(int nIndex, int nMaxLength) const;
		int LineIndex(int nLine = -1) const;
		int LineLength(int nLine = -1) const;
		BOOL LineScroll(int nLines, int nChars = 0);
		CDuiPoint GetCharPos(long lChar) const;
		long LineFromChar(long nIndex) const;
		CDuiPoint PosFromChar(UINT nChar) const;
		int CharFromPos(CDuiPoint pt) const;
		void EmptyUndoBuffer();
		UINT SetUndoLimit(UINT nLimit);
		long StreamIn(int nFormat, EDITSTREAM &es);
		long StreamOut(int nFormat, EDITSTREAM &es);
		void SetAccumulateDBCMode(BOOL bDBCMode);
		BOOL IsAccumulateDBCMode();

		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		LPCTSTR GetNormalImage();
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage();
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage();
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage();
		void SetDisabledImage(LPCTSTR pStrImage);
		void PaintStatusImage(HDC hDC);

		void SetTipValue(LPCTSTR pStrTipValue);
		LPCTSTR GetTipValue();
		void SetTipValueColor(LPCTSTR pStrColor);
		DWORD GetTipValueColor();
		void SetTipValueAlign(UINT uAlign);
		UINT GetTipValueAlign();

		void DoInit();
		BOOL SetDropAcceptFile(BOOL bAccept);
		// 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
		// 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
		virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const; 
		IDropTarget* GetTxDropTarget();
		virtual BOOL OnTxViewChanged();
		virtual void OnTxNotify(DWORD iNotify, void *pv);

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

		SIZE EstimateSize(SIZE szAvailable);
		void SetPos(RECT rc, BOOL bNeedInvalidate = true);
		void Move(SIZE szOffset, BOOL bNeedInvalidate = true);
		void DoEvent(TEventUI& event);
		BOOL DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		enum { 
			DEFAULT_TIMERID = 20,
		};

		CTxtWinHost* m_pTwh;
		BOOL m_bVScrollBarFixing;
		BOOL m_bWantTab;
		BOOL m_bWantReturn;
		BOOL m_bWantCtrlReturn;
		BOOL m_bTransparent;
		BOOL m_bRich;
		BOOL m_bReadOnly;
		BOOL m_bWordWrap;
		DWORD m_dwTextColor;
		int m_iFont;
		int m_iLimitText;
		LONG m_lTwhStyle;
		BOOL m_bDrawCaret;
		BOOL m_bInited;

		BOOL  m_fAccumulateDBC ; // TRUE - need to cumulate ytes from 2 WM_CHAR msgs
		// we are in this mode when we receive VK_PROCESSKEY
		UINT m_chLeadByte; // use when we are in _fAccumulateDBC mode

		RECT m_rcTextPadding;
		UINT m_uButtonState;
		CDuiString m_sNormalImage;
		CDuiString m_sHotImage;
		CDuiString m_sFocusedImage;
		CDuiString m_sDisabledImage;
		CDuiString m_sTipValue;
		DWORD m_dwTipValueColor;
		UINT m_uTipValueAlign;
	};

} // namespace DUI

#endif // __UIRICHEDIT_H__
