#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#pragma once

namespace DUI
{
	class DUILIB_API COptionUI : public CButtonUI
	{
		UI_DECLARE_CONTROL(COptionUI)
	public:
		COptionUI();
		~COptionUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, BOOL bInit = true);

		BOOL Activate();
		void SetEnabled(BOOL bEnable = true);

		LPCTSTR GetSelectedImage();
		void SetSelectedImage(LPCTSTR pStrImage);

		LPCTSTR GetSelectedHotImage();
		void SetSelectedHotImage(LPCTSTR pStrImage);

		LPCTSTR GetSelectedPushedImage();
		void SetSelectedPushedImage(LPCTSTR pStrImage);

		void SetSelectedTextColor(DWORD dwTextColor);
		DWORD GetSelectedTextColor();

		void SetSelectedBkColor(DWORD dwBkColor);
		DWORD GetSelectBkColor();

		LPCTSTR GetSelectedForedImage();
		void SetSelectedForedImage(LPCTSTR pStrImage);

		void SetSelectedStateCount(int nCount);
		int GetSelectedStateCount() const;
		virtual LPCTSTR GetSelectedStateImage();
		virtual void SetSelectedStateImage(LPCTSTR pStrImage);

		void SetSelectedFont(int index);
		int GetSelectedFont() const;

		LPCTSTR GetGroup() const;
		void SetGroup(LPCTSTR pStrGroupName = NULL);
		BOOL IsSelected() const;
		virtual void Selected(BOOL bSelected, BOOL bMsg = true);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintBkColor(HDC hDC);
		void PaintStatusImage(HDC hDC);
		void PaintForeImage(HDC hDC);
		void PaintText(HDC hDC);

	protected:
		BOOL			m_bSelected;
		CDuiString		m_sGroupName;

		int				m_iSelectedFont;

		DWORD			m_dwSelectedBkColor;
		DWORD			m_dwSelectedTextColor;

		CDuiString		m_sSelectedImage;
		CDuiString		m_sSelectedHotImage;
		CDuiString		m_sSelectedPushedImage;
		CDuiString		m_sSelectedForeImage;

		int m_nSelectedStateCount;
		CDuiString m_sSelectedStateImage;
	};

	class DUILIB_API CCheckBoxUI : public COptionUI
	{
		UI_DECLARE_CONTROL(CCheckBoxUI)
	public:
		CCheckBoxUI();

	public:
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);

		void SetCheck(BOOL bCheck);
		BOOL GetCheck() const;

	public:
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetAutoCheck(BOOL bEnable);
		virtual void DoEvent(TEventUI& event);
		virtual void Selected(BOOL bSelected, BOOL bMsg = true);

	protected:
		BOOL m_bAutoCheck; 
	};
} // namespace DUI

#endif // __UIOPTION_H__