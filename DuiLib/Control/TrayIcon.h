#ifndef __UITRAICON_H__
#define __UITRAICON_H__

#include "UILIB.h"
#include "Utils/Utils.h"


namespace DuiLib
{
    class DUILIB_API CTrayIcon
    {
    public:
        CTrayIcon(void);
        ~CTrayIcon(void);

    public:
        void CreateTrayIcon(HWND _RecvHwnd, UINT _IconIDResource, LPCTSTR _ToolTipText = NULL, UINT _Message = NULL);
        void DeleteTrayIcon();
        BOOL SetTooltipText(LPCTSTR _ToolTipText);
        BOOL SetTooltipText(UINT _IDResource);
        CDuiString GetTooltipText() const;

        BOOL SetIcon(HICON _Hicon);
        BOOL SetIcon(LPCTSTR _IconFile);
        BOOL SetIcon(UINT _IDResource);
        HICON GetIcon() const;
        void SetHideIcon();
        void SetShowIcon();
        void RemoveIcon();
        BOOL Enabled() { return m_bEnabled; };
        BOOL IsVisible() { return !m_bVisible; };

    private:
        BOOL m_bEnabled;
        BOOL m_bVisible;
        HWND m_hWnd;
        UINT m_uMessage;
        HICON m_hIcon;
        NOTIFYICONDATA	m_trayData;
    };
}
#endif // 

