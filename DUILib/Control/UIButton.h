#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#include "Control/UILabel.h"

namespace DUI
{
    class DUILIB_API CButtonUI : public CLabelUI
    {
        UI_DECLARE_CONTROL(CButtonUI)

    public:
        CButtonUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);
        UINT GetControlFlags() const;

        BOOL Activate();
        void SetEnabled(BOOL bEnable = true);
        void DoEvent(TEVENT_UI& event);

        virtual LPCTSTR GetNormalImage();
        virtual void SetNormalImage(LPCTSTR pStrImage);
        virtual LPCTSTR GetHotImage();
        virtual void SetHotImage(LPCTSTR pStrImage);
        virtual LPCTSTR GetPushedImage();
        virtual void SetPushedImage(LPCTSTR pStrImage);
        virtual LPCTSTR GetFocusedImage();
        virtual void SetFocusedImage(LPCTSTR pStrImage);
        virtual LPCTSTR GetDisabledImage();
        virtual void SetDisabledImage(LPCTSTR pStrImage);
        virtual LPCTSTR GetHotForeImage();
        virtual void SetHotForeImage(LPCTSTR pStrImage);
        void SetStateCount(int nCount);
        int GetStateCount() const;
        virtual LPCTSTR GetStateImage();
        virtual void SetStateImage(LPCTSTR pStrImage);

        void BindTabIndex(int _BindTabIndex);
        void BindTabLayoutName(LPCTSTR _TabLayoutName);
        void BindTriggerTabSel(int _SetSelectIndex = -1);
        void RemoveBindTabIndex();
        int GetBindTabLayoutIndex();
        LPCTSTR GetBindTabLayoutName();

        void SetHotFont(int index);
        int GetHotFont() const;
        void SetPushedFont(int index);
        int GetPushedFont() const;
        void SetFocusedFont(int index);
        int GetFocusedFont() const;

        void SetHotBkColor(DWORD dwColor);
        DWORD GetHotBkColor() const;
        void SetPushedBkColor(DWORD dwColor);
        DWORD GetPushedBkColor() const;
        void SetDisabledBkColor(DWORD dwColor);
        DWORD GetDisabledBkColor() const;
        void SetHotTextColor(DWORD dwColor);
        DWORD GetHotTextColor() const;
        void SetPushedTextColor(DWORD dwColor);
        DWORD GetPushedTextColor() const;
        void SetFocusedTextColor(DWORD dwColor);
        DWORD GetFocusedTextColor() const;
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        void PaintText(HDC hDC);

        void PaintBkColor(HDC hDC);
        void PaintStatusImage(HDC hDC);
        void PaintForeImage(HDC hDC);

    protected:
        UINT m_uButtonState;

        int m_iHotFont;
        int m_iPushedFont;
        int m_iFocusedFont;

        DWORD m_dwHotBkColor;
        DWORD m_dwPushedBkColor;
        DWORD m_dwDisabledBkColor;
        DWORD m_dwHotTextColor;
        DWORD m_dwPushedTextColor;
        DWORD m_dwFocusedTextColor;

        CStringUI m_sNormalImage;
        CStringUI m_sHotImage;
        CStringUI m_sHotForeImage;
        CStringUI m_sPushedImage;
        CStringUI m_sPushedForeImage;
        CStringUI m_sFocusedImage;
        CStringUI m_sDisabledImage;
        int m_nStateCount;
        CStringUI m_sStateImage;

        int m_iBindTabIndex;
        CStringUI m_sBindTabLayoutName;
    };

} // namespace DUI

#endif // __UIBUTTON_H__