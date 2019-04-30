#ifndef __UIPROGRESS_H__
#define __UIPROGRESS_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CProgressUI : public CLabelUI
	{
		DECLARE_DUICONTROL(CProgressUI)
	public:
		CProgressUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		BOOL IsShowText();
		void SetShowText(BOOL bShowText = true);
		BOOL IsHorizontal();
		void SetHorizontal(BOOL bHorizontal = true);
		BOOL IsStretchForeImage();
		void SetStretchForeImage(BOOL bStretchForeImage = true);
		int GetMinValue() const;
		void SetMinValue(int nMin);
		int GetMaxValue() const;
		void SetMaxValue(int nMax);
		int GetValue() const;
		void SetValue(int nValue);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintForeColor(HDC hDC);
		void PaintForeImage(HDC hDC);
		virtual void UpdateText();

	protected:
		BOOL m_bShowText;
		BOOL m_bHorizontal;
		BOOL m_bStretchForeImage;
		int m_nMax;
		int m_nMin;
		int m_nValue;

		CDuiString m_sForeImageModify;
	};

} // namespace DuiLib

#endif // __UIPROGRESS_H__
