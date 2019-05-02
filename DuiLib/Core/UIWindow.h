#ifndef __UIWindow_H__
#define __UIWindow_H__

#include "Core/UIDefine.h"
#include "Utils/Utils.h"

namespace DUI
{
    /////////////////////////////////////////////////////////////////////////////////////
    //

#define UI_WNDSTYLE_CONTAINER  (0)
#define UI_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define UI_WNDSTYLE_CHILD      (WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define UI_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define UI_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)




#ifdef _DEBUG
    #ifndef UITRACE
    #define UITRACE    UI__Trace
    #endif
    #define UITRACEMSG UI__TraceMsg
#else
    #ifndef UITRACE
    #define UITRACE
    #endif
    #define UITRACEMSG _T("")
#endif

    void    DUILIB_API UI__Trace(LPCTSTR pstrFormat, ...);
    LPCTSTR DUILIB_API UI__TraceMsg(UINT uMsg);

    /////////////////////////////////////////////////////////////////////////////////////
    //
    
    class CControlUI;
    
    // Structure for notifications to the outside world
    typedef struct TNOTIFY_UI
    {
        CStringUI sType;
        CStringUI sVirtualWnd;
        CControlUI* pSender;
        DWORD dwTimestamp;
        POINT ptMouse;
        WPARAM wParam;
        LPARAM lParam;
    }TNOTIFY_UI;

    class DUILIB_API CNotifyPumpUI
    {
    public:
        BOOL AddVirtualWnd(CStringUI strName, CNotifyPumpUI* pObject);
        BOOL RemoveVirtualWnd(CStringUI strName);
        void NotifyPump(TNOTIFY_UI& msg);
        BOOL LoopDispatch(TNOTIFY_UI& msg);
        UI_DECLARE_MESSAGE_MAP()
    private:
        CStdStringPtrMap m_VirtualWndMap;
    };

    class DUILIB_API CWindowUI
    {
    public:
        CWindowUI();

        HWND GetHWND() const;
        operator HWND() const;

        BOOL RegisterWindowClass();
        BOOL RegisterSuperclass();

        HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
        HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
        HWND CreateDuiWindow(HWND hwndParent, LPCTSTR pstrWindowName, DWORD dwStyle = 0, DWORD dwExStyle = 0);
        HWND Subclass(HWND hWnd);
        void Unsubclass();
        void ShowWindow(BOOL bShow = true, BOOL bTakeFocus = true);
        UINT ShowModal();
        void Close(UINT nRet = IDOK);
        void CenterWindow();	// 居中，支持扩展屏幕
        void SetIcon(UINT nRes);

        LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
        LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
        void ResizeClient(int cx = -1, int cy = -1);

    protected:
        virtual LPCTSTR GetWindowClassName() const = 0;
        virtual LPCTSTR GetSuperClassName() const;
        virtual UINT GetClassStyle() const;

        virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        virtual void OnFinalMessage(HWND hWnd);

        static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    protected:
        HWND m_hWnd;
        WNDPROC m_OldWndProc;
        BOOL m_bSubclassed;
    };

} // namespace DUI

#endif // __UIWindow_H__
