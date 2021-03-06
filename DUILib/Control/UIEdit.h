#ifndef __UIEDIT_H__
#define __UIEDIT_H__

#include "Control/UILabel.h"

namespace DUI
{
    class CEditWnd;

    class DUILIB_API CEditUI : public CLabelUI
    {
        UI_DECLARE_CONTROL(CEditUI)
            friend class CEditWnd;
    public:
        CEditUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);
        UINT GetControlFlags() const;

        void SetEnabled(BOOL bEnable = TRUE);
        void SetText(LPCTSTR pstrText);
        void SetMaxChar(UINT uMax);
        UINT GetMaxChar();
        void SetReadOnly(BOOL bReadOnly);
        BOOL IsReadOnly() const;
        void SetPasswordMode(BOOL bPasswordMode);
        BOOL IsPasswordMode() const;
        void SetPasswordChar(TCHAR cPasswordChar);
        TCHAR GetPasswordChar() const;
        void SetNumberOnly(BOOL bNumberOnly);
        BOOL IsNumberOnly() const;
        int GetWindowStyls() const;

        LPCTSTR GetNormalImage();
        void SetNormalImage(LPCTSTR pStrImage);
        LPCTSTR GetHotImage();
        void SetHotImage(LPCTSTR pStrImage);
        LPCTSTR GetFocusedImage();
        void SetFocusedImage(LPCTSTR pStrImage);
        LPCTSTR GetDisabledImage();
        void SetDisabledImage(LPCTSTR pStrImage);
        void SetNativeEditBkColor(DWORD dwBkColor);
        DWORD GetNativeEditBkColor() const;
        void SetNativeEditTextColor(LPCTSTR pStrColor);
        DWORD GetNativeEditTextColor() const;

        BOOL IsAutoSelAll();
        void SetAutoSelAll(BOOL bAutoSelAll);
        void SetSel(long nStartChar, long nEndChar);
        void SetSelAll();
        void SetReplaceSel(LPCTSTR lpszReplace);

        void SetTipValue(LPCTSTR pStrTipValue);
        LPCTSTR GetTipValue();
        void SetTipValueColor(LPCTSTR pStrColor);
        DWORD GetTipValueColor();

        void SetPos(RECT rc, BOOL bNeedInvalidate = TRUE);
        void Move(SIZE szOffset, BOOL bNeedInvalidate = TRUE);
        void SetVisible(BOOL bVisible = TRUE);
        void SetInternVisible(BOOL bVisible = TRUE);
        SIZE EstimateSize(SIZE szAvailable);
        void DoEvent(struct TEVENT_UI& event);
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        void PaintStatusImage(HDC hDC);
        void PaintText(HDC hDC);

    protected:
        CEditWnd* m_pWindow;

        UINT m_uMaxChar;
        BOOL m_bReadOnly;
        BOOL m_bPasswordMode;
        BOOL m_bAutoSelAll;
        TCHAR m_cPasswordChar;
        UINT m_uButtonState;
        CStringUI m_sNormalImage;
        CStringUI m_sHotImage;
        CStringUI m_sFocusedImage;
        CStringUI m_sDisabledImage;
        CStringUI m_sTipValue;
        DWORD m_dwTipValueColor;
        DWORD m_dwEditbkColor;
        DWORD m_dwEditTextColor;
        int m_iWindowStyls;
    };
}
#endif // __UIEDIT_H__