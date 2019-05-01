#ifndef __UILABEL_H__
#define __UILABEL_H__


//#include "Utils/Utils.h"
//#include "Core/UIDefine.h"
////#include "Core/UIControl.h"



namespace DuiLib
{
	class UILIB_API CLabelUI : public CControlUI
	{
		DECLARE_DUICONTROL(CLabelUI)
	public:
		CLabelUI();
		~CLabelUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

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

		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintText(HDC hDC);

		virtual BOOL GetAutoCalcWidth() const;
		virtual void SetAutoCalcWidth(BOOL bAutoCalcWidth);
		virtual BOOL GetAutoCalcHeight() const;
		virtual void SetAutoCalcHeight(BOOL bAutoCalcHeight);
		virtual void SetText(LPCTSTR pstrText);
		
	protected:
		DWORD	m_dwTextColor;
		DWORD	m_dwDisabledTextColor;
		int		m_iFont;
		UINT	m_uTextStyle;
		RECT	m_rcTextPadding;
		BOOL	m_bShowHtml;
		BOOL	m_bAutoCalcWidth;
		BOOL	m_bAutoCalcHeight;

		SIZE    m_szAvailableLast;
		SIZE    m_cxyFixedLast;
		BOOL    m_bNeedEstimateSize;
	};
}

#endif // __UILABEL_H__