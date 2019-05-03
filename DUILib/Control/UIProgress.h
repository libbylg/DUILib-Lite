#ifndef __UIPROGRESS_H__
#define __UIPROGRESS_H__

#include "Control/UILabel.h"

namespace DUI
{
    class DUILIB_API CProgressUI : public CLabelUI
    {
        UI_DECLARE_CONTROL(CProgressUI)
    public:
        CProgressUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        BOOL IsShowText();
        void SetShowText(BOOL bShowText = TRUE);
        BOOL IsHorizontal();
        void SetHorizontal(BOOL bHorizontal = TRUE);
        BOOL IsStretchForeImage();
        void SetStretchForeImage(BOOL bStretchForeImage = TRUE);
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

        CStringUI m_sForeImageModify;
    };

} // namespace DUI

#endif // __UIPROGRESS_H__
