#ifndef __UIROTATE_H__
#define __UIROTATE_H__

#include "Control/UILabel.h"

namespace DUI
{
	class CRingUI : public CLabelUI
	{
		enum
		{ 
			RING_TIMERID = 100,
		};
		UI_DECLARE_CONTROL(CRingUI)
	public:
		CRingUI();
		~CRingUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetBkImage(LPCTSTR pStrImage);	
		virtual void DoEvent(struct TEVENT_UI& event);
		virtual void PaintBkImage(HDC hDC);	

	private:
		void InitImage();
		void DeleteImage();

	public:
		float m_fCurAngle;
		Gdiplus::Image* m_pBkimage;
	};
}

#endif // __UIROTATE_H__