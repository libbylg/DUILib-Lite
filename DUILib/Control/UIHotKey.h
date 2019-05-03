#ifndef __UIHOTKEY_H__
#define __UIHOTKEY_H__

#include "Control/UILabel.h"

namespace DUI
{
    class CHotKeyWnd;

    class DUILIB_API CHotKeyUI : public CLabelUI
    {
        UI_DECLARE_CONTROL(CHotKeyUI)

            ////
            friend class CHotKeyWnd;

    public:
        CHotKeyUI();
        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);
        UINT GetControlFlags() const;
        void SetEnabled(BOOL bEnable = TRUE);
        void SetText(LPCTSTR pstrText);
        LPCTSTR GetNormalImage();
        void SetNormalImage(LPCTSTR pStrImage);
        LPCTSTR GetHotImage();
        void SetHotImage(LPCTSTR pStrImage);
        LPCTSTR GetFocusedImage();
        void SetFocusedImage(LPCTSTR pStrImage);
        LPCTSTR GetDisabledImage();
        void SetDisabledImage(LPCTSTR pStrImage);
        void SetNativeBkColor(DWORD dwBkColor);
        DWORD GetNativeBkColor() const;

        void SetPos(RECT rc);
        void SetVisible(BOOL bVisible = TRUE);
        void SetInternVisible(BOOL bVisible = TRUE);
        SIZE EstimateSize(SIZE szAvailable);
        void DoEvent(struct TEVENT_UI& event);
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        void PaintStatusImage(HDC hDC);
        void PaintText(HDC hDC);
    public:
        void GetHotKey(WORD& wVirtualKeyCode, WORD& wModifiers) const;
        DWORD GetHotKey(void) const;
        void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);

    protected:
        CHotKeyWnd* m_pWindow;
        UINT m_uButtonState;
        CStringUI m_sNormalImage;
        CStringUI m_sHotImage;
        CStringUI m_sFocusedImage;
        CStringUI m_sDisabledImage;
        DWORD m_dwHotKeybkColor;

    protected:
        WORD m_wVirtualKeyCode;
        WORD m_wModifiers;
    };
};


#endif