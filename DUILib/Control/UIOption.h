#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#include "Control/UIButton.h"

namespace DUI
{
    class CManagerUI;

    class DUILIB_API COptionUI : public CButtonUI
    {
        UI_DECLARE_CONTROL(COptionUI)
    public:
        COptionUI();
        ~COptionUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        void SetManager(CManagerUI* pManager, CControlUI* pParent, BOOL bInit = TRUE);

        BOOL Activate();
        void SetEnabled(BOOL bEnable = TRUE);

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
        virtual void Selected(BOOL bSelected, BOOL bMsg = TRUE);

        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        void PaintBkColor(HDC hDC);
        void PaintStatusImage(HDC hDC);
        void PaintForeImage(HDC hDC);
        void PaintText(HDC hDC);

    protected:
        BOOL			m_bSelected;
        CStringUI		m_sGroupName;

        int				m_iSelectedFont;

        DWORD			m_dwSelectedBkColor;
        DWORD			m_dwSelectedTextColor;

        CStringUI		m_sSelectedImage;
        CStringUI		m_sSelectedHotImage;
        CStringUI		m_sSelectedPushedImage;
        CStringUI		m_sSelectedForeImage;

        int m_nSelectedStateCount;
        CStringUI m_sSelectedStateImage;
    };


} // namespace DUI

#endif // __UIOPTION_H__