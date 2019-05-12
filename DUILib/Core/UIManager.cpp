#include "Core/UIManager.h"
#include "Core/UIControl.h"
#include "Core/UIRender.h"
#include "Core/UIWindow.h"
#include "Utils/unzip.h"

namespace DUI
{

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    static void GetChildWndRect(HWND hWnd, HWND hChildWnd, RECT& rcChildWnd)
    {
        ::GetWindowRect(hChildWnd, &rcChildWnd);

        POINT pt;
        pt.x = rcChildWnd.left;
        pt.y = rcChildWnd.top;
        ::ScreenToClient(hWnd, &pt);
        rcChildWnd.left = pt.x;
        rcChildWnd.top = pt.y;

        pt.x = rcChildWnd.right;
        pt.y = rcChildWnd.bottom;
        ::ScreenToClient(hWnd, &pt);
        rcChildWnd.right = pt.x;
        rcChildWnd.bottom = pt.y;
    }

    static WORD MapKeyState()
    {
        WORD uState = 0;
        if (::GetKeyState(VK_CONTROL) < 0) uState |= MK_CONTROL;
        if (::GetKeyState(VK_LBUTTON) < 0) uState |= MK_LBUTTON;
        if (::GetKeyState(VK_RBUTTON) < 0) uState |= MK_RBUTTON;
        if (::GetKeyState(VK_SHIFT) < 0) uState |= MK_SHIFT;
        if (::GetKeyState(VK_MENU) < 0) uState |= MK_ALT;
        return uState;
    }

    class CControlUI;
    typedef struct FINDTABINFO
    {
        CControlUI* pFocus;
        CControlUI* pLast;
        BOOL bForward;
        BOOL bNextIsIt;
    } FINDTABINFO;

    typedef struct FINDSHORTCUT
    {
        TCHAR ch;
        BOOL bPickNext;
    } FINDSHORTCUT;

    typedef struct TIMERINFO
    {
        CControlUI* pSender;
        UINT nLocalID;
        HWND hWnd;
        UINT uWinTimer;
        BOOL bKilled;
    } TIMERINFO;

    /////////////////////////////////////////////////////////////////////////////////////
    typedef BOOL(__stdcall* PFUNCUPDATELAYEREDWINDOW)(HWND, HDC, POINT*, SIZE*, HDC, POINT*, COLORREF, BLENDFUNCTION*, DWORD);
    PFUNCUPDATELAYEREDWINDOW g_fUpdateLayeredWindow = NULL;

    HPEN m_hUpdateRectPen = NULL;

    //HINSTANCE CManagerUI::m_hResourceInstance = NULL;
    //CStringUI CManagerUI::m_pStrResourcePath;
    //CStringUI CManagerUI::m_pStrResourceZip;
    //CStringUI CManagerUI::m_pStrResourceZipPwd; //Garfield 20160325 带密码zip包解密
    //HANDLE CManagerUI::m_hResourceZip = NULL;
    //BOOL CManagerUI::m_bCachedResourceZip = true;
    //RESTYPE_UI CManagerUI::m_nResType = UIRES_FILE;
    //TRESINFO_UI CManagerUI::CResourceUI::GetInstance()->GetSharedResInfo();

    HINSTANCE CManagerUI::m_hInstance = NULL;
    BOOL CManagerUI::m_bUseHSL = FALSE;
    short CManagerUI::m_H = 180;
    short CManagerUI::m_S = 100;
    short CManagerUI::m_L = 100;
    CPtrArrayUI CManagerUI::m_aPreMessages;
    CPtrArrayUI CManagerUI::m_aPlugins;

    CManagerUI::CManagerUI() : m_hWndPaint(NULL),
        m_hDcPaint(NULL),
        m_hDcOffscreen(NULL),
        m_hDcBackground(NULL),
        m_bOffscreenPaint(true),
        m_hbmpOffscreen(NULL),
        m_pOffscreenBits(NULL),
        m_hbmpBackground(NULL),
        m_pBackgroundBits(NULL),
        m_hwndTooltip(NULL),
        m_uTimerID(0x1000),
        m_pRoot(NULL),
        m_pFocus(NULL),
        m_pEventHover(NULL),
        m_pEventClick(NULL),
        m_pEventKey(NULL),
        m_bFirstLayout(true),
        m_bFocusNeeded(FALSE),
        m_bUpdateNeeded(FALSE),
        m_bMouseTracking(FALSE),
        m_bMouseCapture(FALSE),
        m_bAsyncNotifyPosted(FALSE),
        m_bUsedVirtualWnd(FALSE),
        m_bForceUseSharedRes(FALSE),
        m_nOpacity(0xFF),
        m_bLayered(FALSE),
        m_bLayeredChanged(FALSE),
        m_bShowUpdateRect(FALSE),
        m_bUseGdiplusText(FALSE),
        m_trh(0),
        m_bDragMode(FALSE),
        m_hDragBitmap(NULL),
        m_pDPI(NULL),
        m_iHoverTime(400UL)
    {
        if (CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.sFontName.IsEmpty()) {
            CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultDisabledColor = 0xFFA7A6AA;
            CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultFontColor = 0xFF000000;
            CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultLinkFontColor = 0xFF0000FF;
            CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultLinkHoverFontColor = 0xFFD3215F;
            CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultSelectedBkColor = 0xFFBAE4FF;

            LOGFONT lf = {0};
            ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
            lf.lfCharSet = DEFAULT_CHARSET;
            HFONT hDefaultFont = ::CreateFontIndirect(&lf);
            CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.hFont = hDefaultFont;
            CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.sFontName = lf.lfFaceName;
            CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.iSize = -lf.lfHeight;
            CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.bBold = (lf.lfWeight >= FW_BOLD);
            CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.bUnderline = (lf.lfUnderline == TRUE);
            CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.bItalic = (lf.lfItalic == TRUE);
            ::ZeroMemory(&CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.tm, sizeof(CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.tm));
        }

        m_ResInfo.m_dwDefaultDisabledColor = CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultDisabledColor;
        m_ResInfo.m_dwDefaultFontColor = CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultFontColor;
        m_ResInfo.m_dwDefaultLinkFontColor = CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultLinkFontColor;
        m_ResInfo.m_dwDefaultLinkHoverFontColor = CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultLinkHoverFontColor;
        m_ResInfo.m_dwDefaultSelectedBkColor = CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultSelectedBkColor;

        if (m_hUpdateRectPen == NULL) {
            m_hUpdateRectPen = ::CreatePen(PS_SOLID, 1, RGB(220, 0, 0));
            // Boot Windows Common Controls (for the ToolTip control)
            ::InitCommonControls();
            ::LoadLibrary(_T("msimg32.dll"));
        }

        m_szMinWindow.cx = 0;
        m_szMinWindow.cy = 0;
        m_szMaxWindow.cx = 0;
        m_szMaxWindow.cy = 0;
        m_szInitWindowSize.cx = 0;
        m_szInitWindowSize.cy = 0;
        m_szRoundCorner.cx = m_szRoundCorner.cy = 0;
        ::ZeroMemory(&m_rcSizeBox, sizeof(m_rcSizeBox));
        ::ZeroMemory(&m_rcCaption, sizeof(m_rcCaption));
        ::ZeroMemory(&m_rcLayeredInset, sizeof(m_rcLayeredInset));
        ::ZeroMemory(&m_rcLayeredUpdate, sizeof(m_rcLayeredUpdate));
        m_ptLastMousePos.x = m_ptLastMousePos.y = -1;

        m_pGdiplusStartupInput = new Gdiplus::GdiplusStartupInput;
        Gdiplus::GdiplusStartup(&m_gdiplusToken, m_pGdiplusStartupInput, NULL); // 加载GDI接口

        CShadowUI::Initialize(m_hInstance);
    }

    CManagerUI::~CManagerUI()
    {
        // Delete the control-tree structures
        for (int i = 0; i < m_aDelayedCleanup.GetSize(); i++)
            delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
        m_aDelayedCleanup.Resize(0);
        for (int i = 0; i < m_aAsyncNotify.GetSize(); i++)
            delete static_cast<TNOTIFY_UI*>(m_aAsyncNotify[i]);
        m_aAsyncNotify.Resize(0);

        m_mNameHash.Resize(0);
        if (m_pRoot != NULL) delete m_pRoot;

        ::DeleteObject(m_ResInfo.m_DefaultFontInfo.hFont);
        RemoveAllFonts();
        RemoveAllImages();
        RemoveAllStyle();
        RemoveAllDefaultAttributeList();
        RemoveAllWindowCustomAttribute();
        RemoveAllOptionGroups();
        RemoveAllTimers();
        RemoveAllDrawInfos();

        if (m_hwndTooltip != NULL) {
            ::DestroyWindow(m_hwndTooltip);
            m_hwndTooltip = NULL;
        }
        if (!m_aFonts.IsEmpty()) {
            for (int i = 0; i < m_aFonts.GetSize(); ++i) {
                HANDLE handle = static_cast<HANDLE>(m_aFonts.GetAt(i));
                ::RemoveFontMemResourceEx(handle);
            }
        }
        if (m_hDcOffscreen != NULL) ::DeleteDC(m_hDcOffscreen);
        if (m_hDcBackground != NULL) ::DeleteDC(m_hDcBackground);
        if (m_hbmpOffscreen != NULL) ::DeleteObject(m_hbmpOffscreen);
        if (m_hbmpBackground != NULL) ::DeleteObject(m_hbmpBackground);
        if (m_hDcPaint != NULL) ::ReleaseDC(m_hWndPaint, m_hDcPaint);
        m_aPreMessages.Remove(m_aPreMessages.Find(this));
        // 销毁拖拽图片
        if (m_hDragBitmap != NULL) ::DeleteObject(m_hDragBitmap);
        //卸载GDIPlus
        Gdiplus::GdiplusShutdown(m_gdiplusToken);
        delete m_pGdiplusStartupInput;
        // DPI管理对象
        if (m_pDPI != NULL) {
            delete m_pDPI;
            m_pDPI = NULL;
        }
    }

    void CManagerUI::Init(HWND hWnd, LPCTSTR pstrName)
    {
        ASSERT(::IsWindow(hWnd));

        m_mNameHash.Resize();
        RemoveAllFonts();
        RemoveAllImages();
        RemoveAllStyle();
        RemoveAllDefaultAttributeList();
        RemoveAllWindowCustomAttribute();
        RemoveAllOptionGroups();
        RemoveAllTimers();

        m_sName.Empty();
        if (pstrName != NULL) {
            m_sName = pstrName;
        }

        if (m_hWndPaint != hWnd) {
            m_hWndPaint = hWnd;
            m_hDcPaint = ::GetDC(hWnd);
            m_aPreMessages.Add(this);
        }

        SetTargetWnd(hWnd);
        InitDragDrop();
    }

    void CManagerUI::DeletePtr(void* ptr)
    {
        if (ptr) {
            delete ptr;
            ptr = NULL;
        }
    }

    HINSTANCE CManagerUI::GetInstance()
    {
        return m_hInstance;
    }

    CStringUI CManagerUI::GetInstancePath()
    {
        if (m_hInstance == NULL) return _T('\0');

        TCHAR tszModule[MAX_PATH + 1] = {0};
        ::GetModuleFileName(m_hInstance, tszModule, MAX_PATH);
        CStringUI sInstancePath = tszModule;
        int pos = sInstancePath.ReverseFind(_T('\\'));
        if (pos >= 0) sInstancePath = sInstancePath.Left(pos + 1);
        return sInstancePath;
    }

    CStringUI CManagerUI::GetCurrentPath()
    {
        TCHAR tszModule[MAX_PATH + 1] = {0};
        ::GetCurrentDirectory(MAX_PATH, tszModule);
        return tszModule;
    }

    //HINSTANCE CManagerUI::GetResourceDll()
    //{
    //    if (m_hResourceInstance == NULL) return m_hInstance;
    //    return m_hResourceInstance;
    //}

    //const CStringUI& CManagerUI::GetResourcePath()
    //{
    //    return m_pStrResourcePath;
    //}

    //const CStringUI& CManagerUI::GetResourceZip()
    //{
    //    return m_pStrResourceZip;
    //}

    //const CStringUI& CManagerUI::GetResourceZipPwd()
    //{
    //    return m_pStrResourceZipPwd;
    //}

    //BOOL CManagerUI::IsCachedResourceZip()
    //{
    //    return m_bCachedResourceZip;
    //}

    //HANDLE CManagerUI::GetResourceZipHandle()
    //{
    //    return m_hResourceZip;
    //}

    void CManagerUI::SetInstance(HINSTANCE hInst)
    {
        m_hInstance = hInst;
    }

    void CManagerUI::SetCurrentPath(LPCTSTR pStrPath)
    {
        ::SetCurrentDirectory(pStrPath);
    }



    BOOL CManagerUI::GetHSL(short* H, short* S, short* L)
    {
        *H = m_H;
        *S = m_S;
        *L = m_L;
        return m_bUseHSL;
    }

    void CManagerUI::SetHSL(BOOL bUseHSL, short H, short S, short L)
    {
        if (m_bUseHSL || m_bUseHSL != bUseHSL) {
            m_bUseHSL = bUseHSL;
            if (H == m_H && S == m_S && L == m_L) return;
            m_H = CLAMP(H, 0, 360);
            m_S = CLAMP(S, 0, 200);
            m_L = CLAMP(L, 0, 200);
            AdjustSharedImagesHSL();
            for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
                CManagerUI* pManager = static_cast<CManagerUI*>(m_aPreMessages[i]);
                if (pManager != NULL) pManager->AdjustImagesHSL();
            }
        }
    }

    void CManagerUI::ReloadSkin()
    {
        ReloadSharedImages();
        for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
            CManagerUI* pManager = static_cast<CManagerUI*>(m_aPreMessages[i]);
            pManager->ReloadImages();
        }
    }

    CManagerUI* CManagerUI::GetPaintManager(LPCTSTR pstrName)
    {
        if (pstrName == NULL) return NULL;
        CStringUI sName = pstrName;
        if (sName.IsEmpty()) return NULL;
        for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
            CManagerUI* pManager = static_cast<CManagerUI*>(m_aPreMessages[i]);
            if (pManager != NULL && sName == pManager->GetName()) return pManager;
        }
        return NULL;
    }

    CPtrArrayUI* CManagerUI::GetPaintManagers()
    {
        return &m_aPreMessages;
    }

    BOOL CManagerUI::LoadPlugin(LPCTSTR pstrModuleName)
    {
        ASSERT(!::IsBadStringPtr(pstrModuleName, (UINT_PTR)(-1)) || (pstrModuleName == NULL));
        if (pstrModuleName == NULL) return FALSE;
        HMODULE hModule = ::LoadLibrary(pstrModuleName);
        if (hModule != NULL) {
            LPCREATECONTROL_UI lpCreateControl = (LPCREATECONTROL_UI)::GetProcAddress(hModule, "CreateControl");
            if (lpCreateControl != NULL) {
                if (m_aPlugins.Find(lpCreateControl) >= 0) return true;
                m_aPlugins.Add(lpCreateControl);
                return true;
            }
        }
        return FALSE;
    }

    CPtrArrayUI* CManagerUI::GetPlugins()
    {
        return &m_aPlugins;
    }

    HWND CManagerUI::GetPaintWindow() const
    {
        return m_hWndPaint;
    }

    HWND CManagerUI::GetTooltipWindow() const
    {
        return m_hwndTooltip;
    }
    int CManagerUI::GetHoverTime() const
    {
        return m_iHoverTime;
    }

    void CManagerUI::SetHoverTime(int iTime)
    {
        m_iHoverTime = iTime;
    }

    LPCTSTR CManagerUI::GetName() const
    {
        return m_sName;
    }

    HDC CManagerUI::GetPaintDC() const
    {
        return m_hDcPaint;
    }

    POINT CManagerUI::GetMousePos() const
    {
        return m_ptLastMousePos;
    }

    SIZE CManagerUI::GetClientSize() const
    {
        RECT rcClient = {0};
        ::GetClientRect(m_hWndPaint, &rcClient);
        return CSizeUI(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
    }

    SIZE CManagerUI::GetInitSize()
    {
        return m_szInitWindowSize;
    }

    void CManagerUI::SetInitSize(int cx, int cy)
    {
        m_szInitWindowSize.cx = cx;
        m_szInitWindowSize.cy = cy;
        if (m_pRoot == NULL && m_hWndPaint != NULL) {
            ::SetWindowPos(m_hWndPaint, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
        }
    }

    RECT& CManagerUI::GetSizeBox()
    {
        return m_rcSizeBox;
    }

    void CManagerUI::SetSizeBox(RECT & rcSizeBox)
    {
        m_rcSizeBox = rcSizeBox;
    }

    RECT& CManagerUI::GetCaptionRect()
    {
        return m_rcCaption;
    }

    void CManagerUI::SetCaptionRect(RECT & rcCaption)
    {
        m_rcCaption = rcCaption;
    }

    SIZE CManagerUI::GetRoundCorner() const
    {
        return m_szRoundCorner;
    }

    void CManagerUI::SetRoundCorner(int cx, int cy)
    {
        m_szRoundCorner.cx = cx;
        m_szRoundCorner.cy = cy;
    }

    SIZE CManagerUI::GetMinInfo() const
    {
        return m_szMinWindow;
    }

    void CManagerUI::SetMinInfo(int cx, int cy)
    {
        ASSERT(cx >= 0 && cy >= 0);
        m_szMinWindow.cx = cx;
        m_szMinWindow.cy = cy;
    }

    SIZE CManagerUI::GetMaxInfo() const
    {
        return m_szMaxWindow;
    }

    void CManagerUI::SetMaxInfo(int cx, int cy)
    {
        ASSERT(cx >= 0 && cy >= 0);
        m_szMaxWindow.cx = cx;
        m_szMaxWindow.cy = cy;
    }

    BOOL CManagerUI::IsShowUpdateRect() const
    {
        return m_bShowUpdateRect;
    }

    void CManagerUI::SetShowUpdateRect(BOOL show)
    {
        m_bShowUpdateRect = show;
    }

    BOOL CManagerUI::IsNoActivate()
    {
        return m_bNoActivate;
    }

    void CManagerUI::SetNoActivate(BOOL bNoActivate)
    {
        m_bNoActivate = bNoActivate;
    }

    BYTE CManagerUI::GetOpacity() const
    {
        return m_nOpacity;
    }

    void CManagerUI::SetOpacity(BYTE nOpacity)
    {
        m_nOpacity = nOpacity;
        if (m_hWndPaint != NULL) {
            typedef BOOL(__stdcall * PFUNCSETLAYEREDWINDOWATTR)(HWND, COLORREF, BYTE, DWORD);
            PFUNCSETLAYEREDWINDOWATTR fSetLayeredWindowAttributes = NULL;

            HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
            if (hUser32) {
                fSetLayeredWindowAttributes =
                    (PFUNCSETLAYEREDWINDOWATTR)::GetProcAddress(hUser32, "SetLayeredWindowAttributes");
                if (fSetLayeredWindowAttributes == NULL) return;
            }

            DWORD dwStyle = ::GetWindowLong(m_hWndPaint, GWL_EXSTYLE);
            DWORD dwNewStyle = dwStyle;
            if (nOpacity >= 0 && nOpacity < 256)
                dwNewStyle |= WS_EX_LAYERED;
            else
                dwNewStyle &= ~WS_EX_LAYERED;
            if (dwStyle != dwNewStyle) ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewStyle);
            fSetLayeredWindowAttributes(m_hWndPaint, 0, nOpacity, LWA_ALPHA);
        }
    }

    BOOL CManagerUI::IsLayered()
    {
        return m_bLayered;
    }

    void CManagerUI::SetLayered(BOOL bLayered)
    {
        if (m_hWndPaint != NULL && bLayered != m_bLayered) {
            UINT uStyle = GetWindowStyle(m_hWndPaint);
            if ((uStyle & WS_CHILD) != 0) return;
            if (g_fUpdateLayeredWindow == NULL) {
                HMODULE hUser32 = ::GetModuleHandle(_T("User32.dll"));
                if (hUser32) {
                    g_fUpdateLayeredWindow =
                        (PFUNCUPDATELAYEREDWINDOW)::GetProcAddress(hUser32, "UpdateLayeredWindow");
                    if (g_fUpdateLayeredWindow == NULL) return;
                }
            }
            m_bLayered = bLayered;
            if (m_pRoot != NULL) m_pRoot->NeedUpdate();
            Invalidate();
        }
    }

    RECT& CManagerUI::GetLayeredInset()
    {
        return m_rcLayeredInset;
    }

    void CManagerUI::SetLayeredInset(RECT & rcLayeredInset)
    {
        m_rcLayeredInset = rcLayeredInset;
        m_bLayeredChanged = true;
        Invalidate();
    }

    BYTE CManagerUI::GetLayeredOpacity()
    {
        return m_nOpacity;
    }

    void CManagerUI::SetLayeredOpacity(BYTE nOpacity)
    {
        m_nOpacity = nOpacity;
        m_bLayeredChanged = true;
        Invalidate();
    }

    LPCTSTR CManagerUI::GetLayeredImage()
    {
        return m_diLayered.sDrawString;
    }

    void CManagerUI::SetLayeredImage(LPCTSTR pstrImage)
    {
        m_diLayered.sDrawString = pstrImage;
        RECT rcNull = {0};
        CRenderUI::DrawImageInfo(NULL, this, rcNull, rcNull, &m_diLayered);
        m_bLayeredChanged = true;
        Invalidate();
    }

    CShadowUI* CManagerUI::GetShadow()
    {
        return &m_shadow;
    }

    void CManagerUI::SetUseGdiplusText(BOOL bUse)
    {
        m_bUseGdiplusText = bUse;
    }

    BOOL CManagerUI::IsUseGdiplusText() const
    {
        return m_bUseGdiplusText;
    }

    void CManagerUI::SetGdiplusTextRenderingHint(int trh)
    {
        m_trh = trh;
    }

    int CManagerUI::GetGdiplusTextRenderingHint() const
    {
        return m_trh;
    }

    class CRichEditUI;
    BOOL CManagerUI::PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& /*lRes*/)
    {
        for (int i = 0; i < m_aPreMessageFilters.GetSize(); i++) {
            BOOL bHandled = FALSE;
            LRESULT lResult = static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
            if (bHandled) {
                return true;
            }
        }
        switch (uMsg) {
            case WM_KEYDOWN: {
                // Tabbing between controls
                if (wParam == VK_TAB) {
                    if (m_pFocus && m_pFocus->IsVisible() && m_pFocus->IsEnabled() && _tcsstr(m_pFocus->GetClass(), _T("RichEditUI")) != NULL) {
                        /* if (static_cast<CRichEditUI*>(m_pFocus)->IsWantTab()) {
                                     return FALSE;
                                 }*/
                    }
                    if (m_pFocus && m_pFocus->IsVisible() && m_pFocus->IsEnabled() && _tcsstr(m_pFocus->GetClass(), _T("WkeWebkitUI")) != NULL) {
                        return FALSE;
                    }
                    SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
                    return true;
                }
            } break;
            case WM_SYSCHAR: {
                // Handle ALT-shortcut key-combinations
                FINDSHORTCUT fs = {0};
                fs.ch = (TCHAR)toupper((int)wParam);
                CControlUI* pControl = m_pRoot->FindControl(__FindControlFromShortcut, &fs, UIFIND_ENABLED | UIFIND_ME_FIRST | UIFIND_TOP_FIRST);
                if (pControl != NULL) {
                    pControl->SetFocus();
                    pControl->Activate();
                    return true;
                }
            } break;
            case WM_SYSKEYDOWN: {
                if (m_pFocus != NULL) {
                    TEVENT_UI event = {0};
                    event.Type = UIEVENT_SYSKEY;
                    event.chKey = (TCHAR)wParam;
                    event.ptMouse = m_ptLastMousePos;
                    event.wKeyState = (WORD)(MapKeyState());
                    event.dwTimestamp = ::GetTickCount();
                    m_pFocus->Event(event);
                }
            } break;
        }
        return FALSE;
    }

    BOOL CManagerUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & lRes)
    {
        if (m_hWndPaint == NULL) return FALSE;
        // Cycle through listeners
        for (int i = 0; i < m_aMessageFilters.GetSize(); i++) {
            BOOL bHandled = FALSE;
            LRESULT lResult = static_cast<IMessageFilterUI*>(m_aMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
            if (bHandled) {
                lRes = lResult;
                switch (uMsg) {
                    case WM_MOUSEMOVE:
                    case WM_LBUTTONDOWN:
                    case WM_LBUTTONDBLCLK:
                    case WM_LBUTTONUP: {
                        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                        m_ptLastMousePos = pt;
                    } break;
                    case WM_CONTEXTMENU:
                    case WM_MOUSEWHEEL: {
                        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                        ::ScreenToClient(m_hWndPaint, &pt);
                        m_ptLastMousePos = pt;
                    } break;
                }
                return true;
            }
        }

        if (m_bLayered) {
            switch (uMsg) {
                case WM_NCACTIVATE:
                    if (!::IsIconic(m_hWndPaint)) {
                        lRes = (wParam == 0)?TRUE:FALSE;
                        return true;
                    }
                    break;
                case WM_NCCALCSIZE:
                case WM_NCPAINT:
                    lRes = 0;
                    return true;
            }
        }
        // Custom handling of events
        switch (uMsg) {
            case WM_APP + 1: {
                for (int i = 0; i < m_aDelayedCleanup.GetSize(); i++)
                    delete static_cast<CControlUI*>(m_aDelayedCleanup[i]);
                m_aDelayedCleanup.Empty();

                m_bAsyncNotifyPosted = FALSE;

                TNOTIFY_UI * pMsg = NULL;
                while (NULL != (pMsg = static_cast<TNOTIFY_UI*>(m_aAsyncNotify.GetAt(0)))) {
                    m_aAsyncNotify.Remove(0);
                    if (pMsg->pSender != NULL) {
                        if (pMsg->pSender->OnNotify) pMsg->pSender->OnNotify(pMsg);
                    }
                    for (int j = 0; j < m_aNotifiers.GetSize(); j++) {
                        static_cast<INotifyUI*>(m_aNotifiers[j])->Notify(*pMsg);
                    }
                    delete pMsg;
                }
            } break;
            case WM_CLOSE: {
                // Make sure all matching "closing" events are sent
                TEVENT_UI event = {0};
                event.ptMouse = m_ptLastMousePos;
                event.wKeyState = (WORD)(MapKeyState());
                event.dwTimestamp = ::GetTickCount();
                if (m_pEventHover != NULL) {
                    event.Type = UIEVENT_MOUSELEAVE;
                    event.pSender = m_pEventHover;
                    m_pEventHover->Event(event);
                }
                if (m_pEventClick != NULL) {
                    event.Type = UIEVENT_BUTTONUP;
                    event.pSender = m_pEventClick;
                    m_pEventClick->Event(event);
                }

                SetFocus(NULL);

                if (::GetActiveWindow() == m_hWndPaint) {
                    HWND hwndParent = GetWindowOwner(m_hWndPaint);
                    if (hwndParent != NULL) ::SetFocus(hwndParent);
                }

                if (m_hwndTooltip != NULL) {
                    ::DestroyWindow(m_hwndTooltip);
                    m_hwndTooltip = NULL;
                }
            } break;
            case WM_ERASEBKGND: {
                // We'll do the painting here...
                lRes = 1;
            }
                                return true;
            case WM_PAINT: {
                if (m_pRoot == NULL) {
                    PAINTSTRUCT ps = {0};
                    ::BeginPaint(m_hWndPaint, &ps);
                    CRenderUI::DrawColor(m_hDcPaint, ps.rcPaint, 0xFF000000);
                    ::EndPaint(m_hWndPaint, &ps);
                    return true;
                }

                RECT rcClient = {0};
                ::GetClientRect(m_hWndPaint, &rcClient);

                RECT rcPaint = {0};
                if (!::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE)) return true;

                //if( m_bLayered ) {
                //	m_bOffscreenPaint = true;
                //	rcPaint = m_rcLayeredUpdate;
                //	if( ::IsRectEmpty(&m_rcLayeredUpdate) ) {
                //		PAINTSTRUCT ps = { 0 };
                //		::BeginPaint(m_hWndPaint, &ps);
                //		::EndPaint(m_hWndPaint, &ps);
                //		return true;
                //	}
                //	if( rcPaint.right > rcClient.right ) rcPaint.right = rcClient.right;
                //	if( rcPaint.bottom > rcClient.bottom ) rcPaint.bottom = rcClient.bottom;
                //	::ZeroMemory(&m_rcLayeredUpdate, sizeof(m_rcLayeredUpdate));
                //}
                //else {
                //	if( !::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE) ) return true;
                //}

                // Set focus to first control?
                if (m_bFocusNeeded) {
                    SetNextTabControl();
                }

                SetPainting(true);

                BOOL bNeedSizeMsg = FALSE;
                DWORD dwWidth = rcClient.right - rcClient.left;
                DWORD dwHeight = rcClient.bottom - rcClient.top;

                SetPainting(true);
                if (m_bUpdateNeeded) {
                    m_bUpdateNeeded = FALSE;
                    if (!::IsRectEmpty(&rcClient) && !::IsIconic(m_hWndPaint)) {
                        if (m_pRoot->IsUpdateNeeded()) {
                            RECT rcRoot = rcClient;
                            if (m_hDcOffscreen != NULL) ::DeleteDC(m_hDcOffscreen);
                            if (m_hDcBackground != NULL) ::DeleteDC(m_hDcBackground);
                            if (m_hbmpOffscreen != NULL) ::DeleteObject(m_hbmpOffscreen);
                            if (m_hbmpBackground != NULL) ::DeleteObject(m_hbmpBackground);
                            m_hDcOffscreen = NULL;
                            m_hDcBackground = NULL;
                            m_hbmpOffscreen = NULL;
                            m_hbmpBackground = NULL;
                            if (m_bLayered) {
                                rcRoot.left += m_rcLayeredInset.left;
                                rcRoot.top += m_rcLayeredInset.top;
                                rcRoot.right -= m_rcLayeredInset.right;
                                rcRoot.bottom -= m_rcLayeredInset.bottom;
                            }
                            m_pRoot->SetPos(rcRoot, true);
                            bNeedSizeMsg = true;
                        } else {
                            CControlUI* pControl = NULL;
                            m_aFoundControls.Empty();
                            m_pRoot->FindControl(__FindControlsFromUpdate, NULL, UIFIND_VISIBLE | UIFIND_ME_FIRST | UIFIND_UPDATETEST);
                            for (int it = 0; it < m_aFoundControls.GetSize(); it++) {
                                pControl = static_cast<CControlUI*>(m_aFoundControls[it]);
                                if (!pControl->IsFloat())
                                    pControl->SetPos(pControl->GetPos(), true);
                                else
                                    pControl->SetPos(pControl->GetRelativePos(), true);
                            }
                            bNeedSizeMsg = true;
                        }
                        // We'll want to notify the window when it is first initialized
                        // with the correct layout. The window form would take the time
                        // to submit swipes/animations.
                        if (m_bFirstLayout) {
                            m_bFirstLayout = FALSE;
                            SendNotify(m_pRoot, UIMSG_WINDOWINIT, 0, 0, FALSE);
                            if (m_bLayered && m_bLayeredChanged) {
                                Invalidate();
                                SetPainting(FALSE);
                                return true;
                            }
                            // 更新阴影窗口显示
                            m_shadow.Update(m_hWndPaint);
                        }
                    }
                } else if (m_bLayered && m_bLayeredChanged) {
                    RECT rcRoot = rcClient;
                    if (m_pOffscreenBits) ::ZeroMemory(m_pOffscreenBits, (rcRoot.right - rcRoot.left) * (rcRoot.bottom - rcRoot.top) * 4);
                    rcRoot.left += m_rcLayeredInset.left;
                    rcRoot.top += m_rcLayeredInset.top;
                    rcRoot.right -= m_rcLayeredInset.right;
                    rcRoot.bottom -= m_rcLayeredInset.bottom;
                    m_pRoot->SetPos(rcRoot, true);
                }

                if (m_bLayered) {
                    DWORD dwExStyle = ::GetWindowLong(m_hWndPaint, GWL_EXSTYLE);
                    DWORD dwNewExStyle = dwExStyle | WS_EX_LAYERED;
                    if (dwExStyle != dwNewExStyle) ::SetWindowLong(m_hWndPaint, GWL_EXSTYLE, dwNewExStyle);
                    m_bOffscreenPaint = true;
                    UnionRect(&rcPaint, &rcPaint, &m_rcLayeredUpdate);
                    if (rcPaint.right > rcClient.right) rcPaint.right = rcClient.right;
                    if (rcPaint.bottom > rcClient.bottom) rcPaint.bottom = rcClient.bottom;
                    ::ZeroMemory(&m_rcLayeredUpdate, sizeof(m_rcLayeredUpdate));
                }

                //
                // Render screen
                //
                // Prepare offscreen bitmap
                if (m_bOffscreenPaint && m_hbmpOffscreen == NULL) {
                    m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
                    m_hbmpOffscreen = CRenderUI::CreateARGB32Bitmap(m_hDcPaint, dwWidth, dwHeight, (LPBYTE*)& m_pOffscreenBits);
                    ASSERT(m_hDcOffscreen);
                    ASSERT(m_hbmpOffscreen);
                }
                // Begin Windows paint
                PAINTSTRUCT ps = {0};
                ::BeginPaint(m_hWndPaint, &ps);
                if (m_bOffscreenPaint) {
                    HBITMAP hOldBitmap = (HBITMAP)::SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
                    int iSaveDC = ::SaveDC(m_hDcOffscreen);
                    if (m_bLayered) {
                        for (LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y) {
                            for (LONG x = rcPaint.left; x < rcPaint.right; ++x) {
                                int i = (y * dwWidth + x) * 4;
                                *(DWORD*)(&m_pOffscreenBits[i]) = 0;
                            }
                        }
                    }
                    m_pRoot->Paint(m_hDcOffscreen, rcPaint, NULL);

                    if (m_bLayered) {
                        for (int i = 0; i < m_aNativeWindow.GetSize();) {
                            HWND hChildWnd = static_cast<HWND>(m_aNativeWindow[i]);
                            if (!::IsWindow(hChildWnd)) {
                                m_aNativeWindow.Remove(i);
                                m_aNativeWindowControl.Remove(i);
                                continue;
                            }
                            ++i;
                            if (!::IsWindowVisible(hChildWnd)) continue;
                            RECT rcChildWnd = GetNativeWindowRect(hChildWnd);
                            RECT rcTemp = {0};
                            if (!::IntersectRect(&rcTemp, &rcPaint, &rcChildWnd)) continue;

                            COLORREF* pChildBitmapBits = NULL;
                            HDC hChildMemDC = ::CreateCompatibleDC(m_hDcOffscreen);
                            HBITMAP hChildBitmap = CRenderUI::CreateARGB32Bitmap(hChildMemDC, rcChildWnd.right - rcChildWnd.left, rcChildWnd.bottom - rcChildWnd.top, (BYTE * *)& pChildBitmapBits);
                            ::ZeroMemory(pChildBitmapBits, (rcChildWnd.right - rcChildWnd.left) * (rcChildWnd.bottom - rcChildWnd.top) * 4);
                            HBITMAP hOldChildBitmap = (HBITMAP)::SelectObject(hChildMemDC, hChildBitmap);
                            ::SendMessage(hChildWnd, WM_PRINT, (WPARAM)hChildMemDC, (LPARAM)(PRF_CHECKVISIBLE | PRF_CHILDREN | PRF_CLIENT | PRF_OWNED));
                            COLORREF * pChildBitmapBit;
                            for (LONG y = 0; y < rcChildWnd.bottom - rcChildWnd.top; y++) {
                                for (LONG x = 0; x < rcChildWnd.right - rcChildWnd.left; x++) {
                                    pChildBitmapBit = pChildBitmapBits + y * (rcChildWnd.right - rcChildWnd.left) + x;
                                    if (*pChildBitmapBit != 0x00000000) * pChildBitmapBit |= 0xff000000;
                                }
                            }
                            ::BitBlt(m_hDcOffscreen, rcChildWnd.left, rcChildWnd.top, rcChildWnd.right - rcChildWnd.left,
                                rcChildWnd.bottom - rcChildWnd.top, hChildMemDC, 0, 0, SRCCOPY);
                            ::SelectObject(hChildMemDC, hOldChildBitmap);
                            ::DeleteObject(hChildBitmap);
                            ::DeleteDC(hChildMemDC);
                        }
                    }

                    for (int i = 0; i < m_aPostPaintControls.GetSize(); i++) {
                        CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
                        pPostPaintControl->DoPostPaint(m_hDcOffscreen, rcPaint);
                    }

                    ::RestoreDC(m_hDcOffscreen, iSaveDC);

                    if (m_bLayered) {
                        RECT rcWnd = {0};
                        ::GetWindowRect(m_hWndPaint, &rcWnd);
                        if (!m_diLayered.sDrawString.IsEmpty()) {
                            DWORD dwWidtht = rcClient.right - rcClient.left;
                            DWORD dwHeightt = rcClient.bottom - rcClient.top;
                            RECT rcLayeredClient = rcClient;
                            rcLayeredClient.left += m_rcLayeredInset.left;
                            rcLayeredClient.top += m_rcLayeredInset.top;
                            rcLayeredClient.right -= m_rcLayeredInset.right;
                            rcLayeredClient.bottom -= m_rcLayeredInset.bottom;

                            COLORREF* pOffscreenBits = (COLORREF*)m_pOffscreenBits;
                            COLORREF* pBackgroundBits = m_pBackgroundBits;
                            BYTE A = 0;
                            BYTE R = 0;
                            BYTE G = 0;
                            BYTE B = 0;
                            if (!m_diLayered.sDrawString.IsEmpty()) {
                                if (m_hbmpBackground == NULL) {
                                    m_hDcBackground = ::CreateCompatibleDC(m_hDcPaint);
                                    m_hbmpBackground = CRenderUI::CreateARGB32Bitmap(m_hDcPaint, dwWidtht, dwHeightt, (BYTE * *)& m_pBackgroundBits);
                                    ::ZeroMemory(m_pBackgroundBits, dwWidtht * dwHeightt * 4);
                                    ::SelectObject(m_hDcBackground, m_hbmpBackground);
                                    CRenderClipUI clip;
                                    CRenderClipUI::GenerateClip(m_hDcBackground, rcLayeredClient, clip);
                                    CRenderUI::DrawImageInfo(m_hDcBackground, this, rcLayeredClient, rcLayeredClient, &m_diLayered);
                                } else if (m_bLayeredChanged) {
                                    ::ZeroMemory(m_pBackgroundBits, dwWidtht * dwHeightt * 4);
                                    CRenderClipUI clip;
                                    CRenderClipUI::GenerateClip(m_hDcBackground, rcLayeredClient, clip);
                                    CRenderUI::DrawImageInfo(m_hDcBackground, this, rcLayeredClient, rcLayeredClient, &m_diLayered);
                                }
                                for (LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y) {
                                    for (LONG x = rcPaint.left; x < rcPaint.right; ++x) {
                                        pOffscreenBits = (COLORREF*)(m_pOffscreenBits + y * dwWidtht + x);
                                        pBackgroundBits = m_pBackgroundBits + y * dwWidtht + x;
                                        A = (BYTE)((*pBackgroundBits) >> 24);
                                        R = (BYTE)((*pOffscreenBits) >> 16) * A / 255;
                                        G = (BYTE)((*pOffscreenBits) >> 8) * A / 255;
                                        B = (BYTE)(*pOffscreenBits) * A / 255;
                                        *pOffscreenBits = RGB(B, G, R) + ((DWORD)A << 24);
                                    }
                                }
                            }
                        } else {
                            for (LONG y = rcClient.bottom - rcPaint.bottom; y < rcClient.bottom - rcPaint.top; ++y) {
                                for (LONG x = rcPaint.left; x < rcPaint.right; ++x) {
                                    int i = (y * dwWidth + x) * 4;
                                    if ((m_pOffscreenBits[i + 3] == 0) && (m_pOffscreenBits[i + 0] != 0 || m_pOffscreenBits[i + 1] != 0 || m_pOffscreenBits[i + 2] != 0))
                                        m_pOffscreenBits[i + 3] = 255;
                                }
                            }
                        }

                        BLENDFUNCTION bf = {AC_SRC_OVER, 0, m_nOpacity, AC_SRC_ALPHA};
                        POINT ptPos = {rcWnd.left, rcWnd.top};
                        SIZE sizeWnd = {(LONG)dwWidth, (LONG)dwHeight};
                        POINT ptSrc = {0, 0};
                        g_fUpdateLayeredWindow(m_hWndPaint, m_hDcPaint, &ptPos, &sizeWnd, m_hDcOffscreen, &ptSrc, 0, &bf, ULW_ALPHA);
                    } else {
                        ::BitBlt(m_hDcPaint, rcPaint.left, rcPaint.top, rcPaint.right - rcPaint.left, rcPaint.bottom - rcPaint.top, m_hDcOffscreen, rcPaint.left, rcPaint.top, SRCCOPY);
                    }
                    ::SelectObject(m_hDcOffscreen, hOldBitmap);

                    if (m_bShowUpdateRect && !m_bLayered) {
                        HPEN hOldPen = (HPEN)::SelectObject(m_hDcPaint, m_hUpdateRectPen);
                        ::SelectObject(m_hDcPaint, ::GetStockObject(HOLLOW_BRUSH));
                        ::Rectangle(m_hDcPaint, rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
                        ::SelectObject(m_hDcPaint, hOldPen);
                    }
                } else {
                    // A standard paint job
                    int iSaveDC = ::SaveDC(m_hDcPaint);
                    m_pRoot->Paint(m_hDcPaint, rcPaint, NULL);
                    for (int i = 0; i < m_aPostPaintControls.GetSize(); i++) {
                        CControlUI* pPostPaintControl = static_cast<CControlUI*>(m_aPostPaintControls[i]);
                        pPostPaintControl->DoPostPaint(m_hDcPaint, rcPaint);
                    }
                    ::RestoreDC(m_hDcPaint, iSaveDC);
                }
                // All Done!
                ::EndPaint(m_hWndPaint, &ps);

                // 绘制结束
                SetPainting(FALSE);
                m_bLayeredChanged = FALSE;
                if (m_bUpdateNeeded) Invalidate();

                // 发送窗口大小改变消息
                if (bNeedSizeMsg) {
                    this->SendNotify(m_pRoot, UIMSG_WINDOWSIZE, 0, 0, true);
                }
                return true;
            }
            case WM_PRINTCLIENT: {
                if (m_pRoot == NULL) break;
                RECT rcClient;
                ::GetClientRect(m_hWndPaint, &rcClient);
                HDC hDC = (HDC)wParam;
                int save = ::SaveDC(hDC);
                m_pRoot->Paint(hDC, rcClient, NULL);
                if ((lParam & PRF_CHILDREN) != 0) {
                    HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
                    while (hWndChild != NULL) {
                        RECT rcPos = {0};
                        ::GetWindowRect(hWndChild, &rcPos);
                        ::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), 2);
                        ::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
                        ::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
                        hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
                    }
                }
                ::RestoreDC(hDC, save);
            } break;
            case WM_GETMINMAXINFO: {
                MONITORINFO Monitor = {};
                Monitor.cbSize = sizeof(Monitor);
                ::GetMonitorInfo(::MonitorFromWindow(m_hWndPaint, MONITOR_DEFAULTTOPRIMARY), &Monitor);
                RECT rcWork = Monitor.rcWork;
                if (Monitor.dwFlags != MONITORINFOF_PRIMARY) {
                    ::OffsetRect(&rcWork, -rcWork.left, -rcWork.top);
                }

                LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
                if (m_szMinWindow.cx > 0) lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
                if (m_szMinWindow.cy > 0) lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
                if (m_szMaxWindow.cx > 0) lpMMI->ptMaxTrackSize.x = m_szMaxWindow.cx;
                if (m_szMaxWindow.cy > 0) lpMMI->ptMaxTrackSize.y = m_szMaxWindow.cy;
                if (m_szMaxWindow.cx > 0) lpMMI->ptMaxSize.x = m_szMaxWindow.cx;
                if (m_szMaxWindow.cy > 0) lpMMI->ptMaxSize.y = m_szMaxWindow.cy;
            } break;
            case WM_SIZE: {
                if (m_pFocus != NULL) {
                    TEVENT_UI event = {0};
                    event.Type = UIEVENT_WINDOWSIZE;
                    event.pSender = m_pFocus;
                    event.dwTimestamp = ::GetTickCount();
                    m_pFocus->Event(event);
                }
                if (m_pRoot != NULL) m_pRoot->NeedUpdate();
            }
                          return true;
            case WM_TIMER: {
                for (int i = 0; i < m_aTimers.GetSize(); i++) {
                    const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
                    if (pTimer->hWnd == m_hWndPaint &&
                        pTimer->uWinTimer == LOWORD(wParam) &&
                        pTimer->bKilled == FALSE) {
                        TEVENT_UI event = {0};
                        event.Type = UIEVENT_TIMER;
                        event.pSender = pTimer->pSender;
                        event.dwTimestamp = ::GetTickCount();
                        event.ptMouse = m_ptLastMousePos;
                        event.wKeyState = MapKeyState();
                        event.wParam = pTimer->nLocalID;
                        event.lParam = lParam;
                        pTimer->pSender->Event(event);
                        break;
                    }
                }
            } break;
            case WM_MOUSEHOVER: {
                m_bMouseTracking = FALSE;
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                CControlUI* pHover = FindControl(pt);
                if (pHover == NULL) break;
                // Generate mouse hover event
                if (m_pEventHover != NULL) {
                    TEVENT_UI event = {0};
                    event.Type = UIEVENT_MOUSEHOVER;
                    event.pSender = m_pEventHover;
                    event.wParam = wParam;
                    event.lParam = lParam;
                    event.dwTimestamp = ::GetTickCount();
                    event.ptMouse = pt;
                    event.wKeyState = MapKeyState();
                    m_pEventHover->Event(event);
                }
                // Create tooltip information
                CStringUI sToolTip = pHover->GetToolTip();
                if (sToolTip.IsEmpty()) return true;
                ::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
                m_ToolTip.cbSize = sizeof(TOOLINFO);
                m_ToolTip.uFlags = TTF_IDISHWND;
                m_ToolTip.hwnd = m_hWndPaint;
                m_ToolTip.uId = (UINT_PTR)m_hWndPaint;
                m_ToolTip.hinst = m_hInstance;
                m_ToolTip.lpszText = const_cast<LPTSTR>((LPCTSTR)sToolTip);
                m_ToolTip.rect = pHover->GetPos();
                if (m_hwndTooltip == NULL) {
                    m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
                    ::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM)& m_ToolTip);
                    ::SendMessage(m_hwndTooltip, TTM_SETMAXTIPWIDTH, 0, pHover->GetToolTipWidth());
                }
                if (!::IsWindowVisible(m_hwndTooltip)) {
                    ::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)& m_ToolTip);
                    ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)& m_ToolTip);
                }
            }
                                return true;
            case WM_MOUSELEAVE: {
                if (m_hwndTooltip != NULL) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)& m_ToolTip);
                if (m_bMouseTracking) {
                    POINT pt = {0};
                    RECT rcWnd = {0};
                    ::GetCursorPos(&pt);
                    ::GetWindowRect(m_hWndPaint, &rcWnd);
                    if (!::IsIconic(m_hWndPaint) && ::GetActiveWindow() == m_hWndPaint && ::PtInRect(&rcWnd, pt)) {
                        if (::SendMessage(m_hWndPaint, WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y)) == HTCLIENT) {
                            ::ScreenToClient(m_hWndPaint, &pt);
                            ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
                        } else
                            ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)-1);
                    } else
                        ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)-1);
                }
                m_bMouseTracking = FALSE;
            } break;
            case WM_MOUSEMOVE: {
                // Start tracking this entire window again...
                if (!m_bMouseTracking) {
                    TRACKMOUSEEVENT tme = {0};
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_HOVER | TME_LEAVE;
                    tme.hwndTrack = m_hWndPaint;
                    tme.dwHoverTime = m_hwndTooltip == NULL?m_iHoverTime:(DWORD)::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
                    _TrackMouseEvent(&tme);
                    m_bMouseTracking = true;
                }

                // Generate the appropriate mouse messages
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                // 是否移动
                BOOL bNeedDrag = true;
                if (m_ptLastMousePos.x == pt.x && m_ptLastMousePos.y == pt.y) {
                    bNeedDrag = FALSE;
                }
                // 记录鼠标位置
                m_ptLastMousePos = pt;
                CControlUI* pNewHover = FindControl(pt);
                if (pNewHover != NULL && pNewHover->GetManager() != this) break;

                // 拖拽事件
                if (bNeedDrag && m_bDragMode && wParam == MK_LBUTTON) {
                    ::ReleaseCapture();
                    CIDropSource* pdsrc = new CIDropSource;
                    if (pdsrc == NULL) return 0;
                    pdsrc->AddRef();

                    CIDataObject * pdobj = new CIDataObject(pdsrc);
                    if (pdobj == NULL) return 0;
                    pdobj->AddRef();

                    FORMATETC fmtetc = {0};
                    STGMEDIUM medium = {0};
                    fmtetc.dwAspect = DVASPECT_CONTENT;
                    fmtetc.lindex = -1;
                    //////////////////////////////////////
                    fmtetc.cfFormat = CF_BITMAP;
                    fmtetc.tymed = TYMED_GDI;
                    medium.tymed = TYMED_GDI;
                    HBITMAP hBitmap = (HBITMAP)OleDuplicateData(m_hDragBitmap, fmtetc.cfFormat, NULL);
                    medium.hBitmap = hBitmap;
                    pdobj->SetData(&fmtetc, &medium, FALSE);
                    //////////////////////////////////////
                    BITMAP bmap;
                    GetObject(hBitmap, sizeof(BITMAP), &bmap);
                    RECT rc = {0, 0, bmap.bmWidth, bmap.bmHeight};
                    fmtetc.cfFormat = CF_ENHMETAFILE;
                    fmtetc.tymed = TYMED_ENHMF;
                    HDC hMetaDC = CreateEnhMetaFile(m_hDcPaint, NULL, NULL, NULL);
                    HDC hdcMem = CreateCompatibleDC(m_hDcPaint);
                    HGDIOBJ hOldBmp = ::SelectObject(hdcMem, hBitmap);
                    ::BitBlt(hMetaDC, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
                    ::SelectObject(hdcMem, hOldBmp);
                    medium.hEnhMetaFile = CloseEnhMetaFile(hMetaDC);
                    DeleteDC(hdcMem);
                    medium.tymed = TYMED_ENHMF;
                    pdobj->SetData(&fmtetc, &medium, TRUE);
                    //////////////////////////////////////
                    CDragSourceHelper dragSrcHelper;
                    POINT ptDrag = {0};
                    ptDrag.x = bmap.bmWidth / 2;
                    ptDrag.y = bmap.bmHeight / 2;
                    dragSrcHelper.InitializeFromBitmap(hBitmap, ptDrag, rc, pdobj); //will own the bmp
                    DWORD dwEffect;
                    HRESULT hr = ::DoDragDrop(pdobj, pdsrc, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);
                    if (dwEffect)
                        pdsrc->Release();
                    delete pdsrc;
                    pdobj->Release();
                    m_bDragMode = FALSE;
                    break;
                }
                TEVENT_UI event = {0};
                event.ptMouse = pt;
                event.wParam = wParam;
                event.lParam = lParam;
                event.dwTimestamp = ::GetTickCount();
                event.wKeyState = MapKeyState();
                if (!IsCaptured()) {
                    pNewHover = FindControl(pt);
                    if (pNewHover != NULL && pNewHover->GetManager() != this) break;
                    if (pNewHover != m_pEventHover && m_pEventHover != NULL) {
                        event.Type = UIEVENT_MOUSELEAVE;
                        event.pSender = m_pEventHover;

                        CPtrArrayUI aNeedMouseLeaveNeeded(m_aNeedMouseLeaveNeeded.GetSize());
                        aNeedMouseLeaveNeeded.Resize(m_aNeedMouseLeaveNeeded.GetSize());
                        ::CopyMemory(aNeedMouseLeaveNeeded.GetData(), m_aNeedMouseLeaveNeeded.GetData(), m_aNeedMouseLeaveNeeded.GetSize() * sizeof(LPVOID));
                        for (int i = 0; i < aNeedMouseLeaveNeeded.GetSize(); i++) {
                            static_cast<CControlUI*>(aNeedMouseLeaveNeeded[i])->Event(event);
                        }

                        m_pEventHover->Event(event);
                        m_pEventHover = NULL;
                        if (m_hwndTooltip != NULL) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)& m_ToolTip);
                    }
                    if (pNewHover != m_pEventHover && pNewHover != NULL) {
                        event.Type = UIEVENT_MOUSEENTER;
                        event.pSender = pNewHover;
                        pNewHover->Event(event);
                        m_pEventHover = pNewHover;
                    }
                }
                if (m_pEventClick != NULL) {
                    event.Type = UIEVENT_MOUSEMOVE;
                    event.pSender = m_pEventClick;
                    m_pEventClick->Event(event);
                } else if (pNewHover != NULL) {
                    event.Type = UIEVENT_MOUSEMOVE;
                    event.pSender = pNewHover;
                    pNewHover->Event(event);
                }
            } break;
            case WM_LBUTTONDOWN: {
                // We alway set focus back to our app (this helps
                // when Win32 child windows are placed on the dialog
                // and we need to remove them on focus change).
                if (!m_bNoActivate) ::SetFocus(m_hWndPaint);
                if (m_pRoot == NULL) break;
                // 查找控件
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                CControlUI * pControl = FindControl(pt);
                if (pControl == NULL) break;
                if (pControl->GetManager() != this) break;

                // 准备拖拽
                if (pControl->IsDragEnabled()) {
                    m_bDragMode = true;
                    if (m_hDragBitmap != NULL) {
                        ::DeleteObject(m_hDragBitmap);
                        m_hDragBitmap = NULL;
                    }
                    m_hDragBitmap = CRenderUI::GenerateBitmap(this, pControl, pControl->GetPos());
                }

                // 开启捕获
                SetCapture();
                // 事件处理
                m_pEventClick = pControl;
                pControl->SetFocus();

                TEVENT_UI event = {0};
                event.Type = UIEVENT_BUTTONDOWN;
                event.pSender = pControl;
                event.wParam = wParam;
                event.lParam = lParam;
                event.ptMouse = pt;
                event.wKeyState = (WORD)wParam;
                event.dwTimestamp = ::GetTickCount();
                pControl->Event(event);
            } break;
            case WM_LBUTTONDBLCLK: {
                if (!m_bNoActivate) ::SetFocus(m_hWndPaint);

                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                CControlUI* pControl = FindControl(pt);
                if (pControl == NULL) break;
                if (pControl->GetManager() != this) break;
                SetCapture();
                TEVENT_UI event = {0};
                event.Type = UIEVENT_DBLCLICK;
                event.pSender = pControl;
                event.ptMouse = pt;
                event.wParam = wParam;
                event.lParam = lParam;
                event.wKeyState = (WORD)wParam;
                event.dwTimestamp = ::GetTickCount();
                pControl->Event(event);
                m_pEventClick = pControl;
            } break;
            case WM_LBUTTONUP: {
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                if (m_pEventClick == NULL) break;
                ReleaseCapture();
                TEVENT_UI event = {0};
                event.Type = UIEVENT_BUTTONUP;
                event.pSender = m_pEventClick;
                event.wParam = wParam;
                event.lParam = lParam;
                event.ptMouse = pt;
                event.wKeyState = (WORD)wParam;
                event.dwTimestamp = ::GetTickCount();

                CControlUI * pClick = m_pEventClick;
                m_pEventClick = NULL;
                pClick->Event(event);
            } break;
            case WM_RBUTTONDOWN: {
                if (!m_bNoActivate) ::SetFocus(m_hWndPaint);
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                CControlUI* pControl = FindControl(pt);
                if (pControl == NULL) break;
                if (pControl->GetManager() != this) break;
                pControl->SetFocus();
                SetCapture();
                TEVENT_UI event = {0};
                event.Type = UIEVENT_RBUTTONDOWN;
                event.pSender = pControl;
                event.wParam = wParam;
                event.lParam = lParam;
                event.ptMouse = pt;
                event.wKeyState = (WORD)wParam;
                event.dwTimestamp = ::GetTickCount();
                pControl->Event(event);
                m_pEventClick = pControl;
            } break;
            case WM_RBUTTONUP: {
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                m_pEventClick = FindControl(pt);
                if (m_pEventClick == NULL) break;
                ReleaseCapture();
                TEVENT_UI event = {0};
                event.Type = UIEVENT_RBUTTONUP;
                event.pSender = m_pEventClick;
                event.wParam = wParam;
                event.lParam = lParam;
                event.ptMouse = pt;
                event.wKeyState = (WORD)wParam;
                event.dwTimestamp = ::GetTickCount();
                m_pEventClick->Event(event);
            } break;
            case WM_MBUTTONDOWN: {
                if (!m_bNoActivate) ::SetFocus(m_hWndPaint);
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                CControlUI* pControl = FindControl(pt);
                if (pControl == NULL) break;
                if (pControl->GetManager() != this) break;
                pControl->SetFocus();
                SetCapture();
                TEVENT_UI event = {0};
                event.Type = UIEVENT_MBUTTONDOWN;
                event.pSender = pControl;
                event.wParam = wParam;
                event.lParam = lParam;
                event.ptMouse = pt;
                event.wKeyState = (WORD)wParam;
                event.dwTimestamp = ::GetTickCount();
                pControl->Event(event);
                m_pEventClick = pControl;
            } break;
            case WM_MBUTTONUP: {
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                m_ptLastMousePos = pt;
                m_pEventClick = FindControl(pt);
                if (m_pEventClick == NULL) break;
                ReleaseCapture();

                TEVENT_UI event = {0};
                event.Type = UIEVENT_MBUTTONUP;
                event.pSender = m_pEventClick;
                event.wParam = wParam;
                event.lParam = lParam;
                event.ptMouse = pt;
                event.wKeyState = (WORD)wParam;
                event.dwTimestamp = ::GetTickCount();
                m_pEventClick->Event(event);
            } break;
            case WM_CONTEXTMENU: {
                if (m_pRoot == NULL) break;
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ::ScreenToClient(m_hWndPaint, &pt);
                m_ptLastMousePos = pt;
                if (m_pEventClick == NULL) break;
                ReleaseCapture();
                TEVENT_UI event = {0};
                event.Type = UIEVENT_CONTEXTMENU;
                event.pSender = m_pEventClick;
                event.wParam = wParam;
                event.lParam = lParam;
                event.ptMouse = pt;
                event.wKeyState = (WORD)wParam;
                event.lParam = (LPARAM)m_pEventClick;
                event.dwTimestamp = ::GetTickCount();
                m_pEventClick->Event(event);
                m_pEventClick = NULL;
            } break;
            case WM_MOUSEWHEEL: {
                if (m_pRoot == NULL) break;
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ::ScreenToClient(m_hWndPaint, &pt);
                m_ptLastMousePos = pt;
                CControlUI * pControl = FindControl(pt);
                if (pControl == NULL) break;
                if (pControl->GetManager() != this) break;
                int zDelta = (int)(short)HIWORD(wParam);
                TEVENT_UI event = {0};
                event.Type = UIEVENT_SCROLLWHEEL;
                event.pSender = pControl;
                event.wParam = MAKELPARAM(zDelta < 0?SB_LINEDOWN:SB_LINEUP, 0);
                event.lParam = lParam;
                event.ptMouse = pt;
                event.wKeyState = MapKeyState();
                event.dwTimestamp = ::GetTickCount();
                pControl->Event(event);

                // Let's make sure that the scroll item below the cursor is the same as before...
                ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM)MAKELPARAM(m_ptLastMousePos.x, m_ptLastMousePos.y));
            } break;
            case WM_CHAR: {
                if (m_pRoot == NULL) break;
                if (m_pFocus == NULL) break;
                TEVENT_UI event = {0};
                event.Type = UIEVENT_CHAR;
                event.pSender = m_pFocus;
                event.wParam = wParam;
                event.lParam = lParam;
                event.chKey = (TCHAR)wParam;
                event.ptMouse = m_ptLastMousePos;
                event.wKeyState = MapKeyState();
                event.dwTimestamp = ::GetTickCount();
                m_pFocus->Event(event);
            } break;
            case WM_KEYDOWN: {
                if (m_pRoot == NULL) break;
                if (m_pFocus == NULL) break;
                TEVENT_UI event = {0};
                event.Type = UIEVENT_KEYDOWN;
                event.pSender = m_pFocus;
                event.wParam = wParam;
                event.lParam = lParam;
                event.chKey = (TCHAR)wParam;
                event.ptMouse = m_ptLastMousePos;
                event.wKeyState = MapKeyState();
                event.dwTimestamp = ::GetTickCount();
                m_pFocus->Event(event);
                m_pEventKey = m_pFocus;
            } break;
            case WM_KEYUP: {
                if (m_pRoot == NULL) break;
                if (m_pEventKey == NULL) break;
                TEVENT_UI event = {0};
                event.Type = UIEVENT_KEYUP;
                event.pSender = m_pEventKey;
                event.wParam = wParam;
                event.lParam = lParam;
                event.chKey = (TCHAR)wParam;
                event.ptMouse = m_ptLastMousePos;
                event.wKeyState = MapKeyState();
                event.dwTimestamp = ::GetTickCount();
                m_pEventKey->Event(event);
                m_pEventKey = NULL;
            } break;
            case WM_SETCURSOR: {
                if (m_pRoot == NULL) break;
                if (LOWORD(lParam) != HTCLIENT) break;
                if (m_bMouseCapture) return true;

                POINT pt = {0};
                ::GetCursorPos(&pt);
                ::ScreenToClient(m_hWndPaint, &pt);
                CControlUI * pControl = FindControl(pt);
                if (pControl == NULL) break;
                if ((pControl->GetControlFlags() & UIFLAG_SETCURSOR) == 0) break;
                TEVENT_UI event = {0};
                event.Type = UIEVENT_SETCURSOR;
                event.pSender = pControl;
                event.wParam = wParam;
                event.lParam = lParam;
                event.ptMouse = pt;
                event.wKeyState = MapKeyState();
                event.dwTimestamp = ::GetTickCount();
                pControl->Event(event);
            }
                               return true;
            case WM_SETFOCUS: {
                if (m_pFocus != NULL) {
                    TEVENT_UI event = {0};
                    event.Type = UIEVENT_SETFOCUS;
                    event.wParam = wParam;
                    event.lParam = lParam;
                    event.pSender = m_pFocus;
                    event.dwTimestamp = ::GetTickCount();
                    m_pFocus->Event(event);
                }
                break;
            }
            case WM_KILLFOCUS: {
                if (IsCaptured()) ReleaseCapture();
                break;
            }
            case WM_NOTIFY: {
                if (lParam == 0) break;
                LPNMHDR lpNMHDR = (LPNMHDR)lParam;
                if (lpNMHDR != NULL) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
                return true;
            } break;
            case WM_COMMAND: {
                if (lParam == 0) break;
                HWND hWndChild = (HWND)lParam;
                lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
                if (lRes != 0) return true;
            } break;
            case WM_CTLCOLOREDIT:
            case WM_CTLCOLORSTATIC: {
                // Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
                // Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
                if (lParam == 0) break;
                HWND hWndChild = (HWND)lParam;
                lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
                if (lRes != 0) return true;
            } break;
            default:
                break;
        }
        return FALSE;
    }

    BOOL CManagerUI::IsUpdateNeeded() const
    {
        return m_bUpdateNeeded;
    }

    void CManagerUI::NeedUpdate()
    {
        m_bUpdateNeeded = true;
    }

    void CManagerUI::Invalidate()
    {
        RECT rcClient = {0};
        ::GetClientRect(m_hWndPaint, &rcClient);
        ::UnionRect(&m_rcLayeredUpdate, &m_rcLayeredUpdate, &rcClient);
        ::InvalidateRect(m_hWndPaint, NULL, FALSE);
    }

    void CManagerUI::Invalidate(RECT & rcItem)
    {
        if (rcItem.left < 0) rcItem.left = 0;
        if (rcItem.top < 0) rcItem.top = 0;
        if (rcItem.right < rcItem.left) rcItem.right = rcItem.left;
        if (rcItem.bottom < rcItem.top) rcItem.bottom = rcItem.top;
        ::UnionRect(&m_rcLayeredUpdate, &m_rcLayeredUpdate, &rcItem);
        ::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
    }

    BOOL CManagerUI::AttachDialog(CControlUI * pControl)
    {
        ASSERT(::IsWindow(m_hWndPaint));
        // 创建阴影窗口
        m_shadow.Create(this);

        // Reset any previous attachment
        SetFocus(NULL);
        m_pEventKey = NULL;
        m_pEventHover = NULL;
        m_pEventClick = NULL;
        // Remove the existing control-tree. We might have gotten inside this function as
        // a result of an event fired or similar, so we cannot just delete the objects and
        // pull the internal memory of the calling code. We'll delay the cleanup.
        if (m_pRoot != NULL) {
            m_aPostPaintControls.Empty();
            AddDelayedCleanup(m_pRoot);
        }
        // Set the dialog root element
        m_pRoot = pControl;
        // Go ahead...
        m_bUpdateNeeded = true;
        m_bFirstLayout = true;
        m_bFocusNeeded = true;
        // Initiate all control
        return InitControls(pControl);
    }

    BOOL CManagerUI::InitControls(CControlUI * pControl, CControlUI * pParent /*= NULL*/)
    {
        ASSERT(pControl);
        if (pControl == NULL) return FALSE;
        pControl->SetManager(this, pParent != NULL?pParent:pControl->GetParent(), true);
        pControl->FindControl(__FindControlFromNameHash, this, UIFIND_ALL);
        return true;
    }

    void CManagerUI::ReapObjects(CControlUI * pControl)
    {
        if (pControl == m_pEventKey) m_pEventKey = NULL;
        if (pControl == m_pEventHover) m_pEventHover = NULL;
        if (pControl == m_pEventClick) m_pEventClick = NULL;
        if (pControl == m_pFocus) m_pFocus = NULL;
        KillTimer(pControl);
        const CStringUI & sName = pControl->GetName();
        if (!sName.IsEmpty()) {
            if (pControl == FindControl(sName)) m_mNameHash.Remove(sName);
        }
        for (int i = 0; i < m_aAsyncNotify.GetSize(); i++) {
            TNOTIFY_UI* pMsg = static_cast<TNOTIFY_UI*>(m_aAsyncNotify[i]);
            if (pMsg->pSender == pControl) pMsg->pSender = NULL;
        }
    }

    BOOL CManagerUI::AddOptionGroup(LPCTSTR pStrGroupName, CControlUI * pControl)
    {
        LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
        if (lp) {
            CPtrArrayUI* aOptionGroup = static_cast<CPtrArrayUI*>(lp);
            for (int i = 0; i < aOptionGroup->GetSize(); i++) {
                if (static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl) {
                    return FALSE;
                }
            }
            aOptionGroup->Add(pControl);
        } else {
            CPtrArrayUI* aOptionGroup = new CPtrArrayUI(6);
            aOptionGroup->Add(pControl);
            m_mOptionGroup.Insert(pStrGroupName, aOptionGroup);
        }
        return true;
    }

    CPtrArrayUI* CManagerUI::GetOptionGroup(LPCTSTR pStrGroupName)
    {
        LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
        if (lp) return static_cast<CPtrArrayUI*>(lp);
        return NULL;
    }

    void CManagerUI::RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI * pControl)
    {
        LPVOID lp = m_mOptionGroup.Find(pStrGroupName);
        if (lp) {
            CPtrArrayUI* aOptionGroup = static_cast<CPtrArrayUI*>(lp);
            if (aOptionGroup == NULL) return;
            for (int i = 0; i < aOptionGroup->GetSize(); i++) {
                if (static_cast<CControlUI*>(aOptionGroup->GetAt(i)) == pControl) {
                    aOptionGroup->Remove(i);
                    break;
                }
            }
            if (aOptionGroup->IsEmpty()) {
                delete aOptionGroup;
                m_mOptionGroup.Remove(pStrGroupName);
            }
        }
    }

    void CManagerUI::RemoveAllOptionGroups()
    {
        CPtrArrayUI* aOptionGroup;
        for (int i = 0; i < m_mOptionGroup.GetSize(); i++) {
            if (LPCTSTR key = m_mOptionGroup.GetAt(i)) {
                aOptionGroup = static_cast<CPtrArrayUI*>(m_mOptionGroup.Find(key));
                delete aOptionGroup;
            }
        }
        m_mOptionGroup.RemoveAll();
    }

    void CManagerUI::MessageLoop()
    {
        MSG msg = {0};
        while (::GetMessage(&msg, NULL, 0, 0)) {
            if (!CManagerUI::TranslateMessage(&msg)) {
                ::TranslateMessage(&msg);
                try {
                    ::DispatchMessage(&msg);
                } catch (...) {
                    UITRACE(_T("EXCEPTION: %s(%d)\n"), __FILET__, __LINE__);
#ifdef _DEBUG
                    throw "CManagerUI::MessageLoop";
#endif
                }
            }
        }
    }

    void CManagerUI::Term()
    {
        // 销毁资源管理器
        CResourceUI::GetInstance()->Release();
        //CControlFactory::GetInstance()->Release();
        //CMenuWnd::DestroyMenu();

    }

    CDPI* CManagerUI::GetDPIObj()
    {
        if (m_pDPI == NULL) {
            m_pDPI = new CDPI;
        }
        return m_pDPI;
    }

    void CManagerUI::SetDPI(int iDPI)
    {
        int scale1 = GetDPIObj()->GetScale();
        GetDPIObj()->SetScale(iDPI);
        int scale2 = GetDPIObj()->GetScale();
        ResetDPIAssets();
        RECT rcWnd = {0};
        ::GetWindowRect(GetPaintWindow(), &rcWnd);
        RECT* prcNewWindow = &rcWnd;
        if (!::IsZoomed(GetPaintWindow())) {
            RECT rc = rcWnd;
            rc.right = rcWnd.left + (rcWnd.right - rcWnd.left) * scale2 / scale1;
            rc.bottom = rcWnd.top + (rcWnd.bottom - rcWnd.top) * scale2 / scale1;
            prcNewWindow = &rc;
        }
        SetWindowPos(GetPaintWindow(), NULL, prcNewWindow->left, prcNewWindow->top, prcNewWindow->right - prcNewWindow->left, prcNewWindow->bottom - prcNewWindow->top, SWP_NOZORDER | SWP_NOACTIVATE);
        if (GetRoot() != NULL) GetRoot()->NeedUpdate();
        ::PostMessage(GetPaintWindow(), UIMSG_SET_DPI, 0, 0);
    }

    void CManagerUI::SetAllDPI(int iDPI)
    {
        for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
            CManagerUI* pManager = static_cast<CManagerUI*>(m_aPreMessages[i]);
            pManager->SetDPI(iDPI);
        }
    }

    void CManagerUI::ResetDPIAssets()
    {
        RemoveAllDrawInfos();
        RemoveAllImages();
        ;

        for (int it = 0; it < m_ResInfo.m_CustomFonts.GetSize(); it++) {
            TFONTINFO_UI* pFontInfo = static_cast<TFONTINFO_UI*>(m_ResInfo.m_CustomFonts.Find(m_ResInfo.m_CustomFonts[it]));
            RebuildFont(pFontInfo);
        }
        RebuildFont(&m_ResInfo.m_DefaultFontInfo);

        for (int it = 0; it < CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetSize(); it++) {
            TFONTINFO_UI* pFontInfo = static_cast<TFONTINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Find(CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts[it]));
            RebuildFont(pFontInfo);
        }
        RebuildFont(&CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo);

        CPtrArrayUI* richEditList = FindSubControlsByClass(GetRoot(), _T("RichEditUI"));
        for (int i = 0; i < richEditList->GetSize(); i++) {
            //CRichEditUI* pT = static_cast<CRichEditUI*>((*richEditList)[i]);
            //pT->SetFont(pT->GetFont());
        }
    }

    void CManagerUI::RebuildFont(TFONTINFO_UI * pFontInfo)
    {
        ::DeleteObject(pFontInfo->hFont);
        LOGFONT lf = {0};
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        _tcsncpy(lf.lfFaceName, pFontInfo->sFontName, LF_FACESIZE);
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfHeight = -GetDPIObj()->Scale(pFontInfo->iSize);
        lf.lfQuality = CLEARTYPE_QUALITY;
        if (pFontInfo->bBold) lf.lfWeight += FW_BOLD;
        if (pFontInfo->bUnderline) lf.lfUnderline = TRUE;
        if (pFontInfo->bItalic) lf.lfItalic = TRUE;
        HFONT hFont = ::CreateFontIndirect(&lf);
        pFontInfo->hFont = hFont;
        ::ZeroMemory(&(pFontInfo->tm), sizeof(pFontInfo->tm));
        if (m_hDcPaint) {
            HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, hFont);
            ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
            ::SelectObject(m_hDcPaint, hOldFont);
        }
    }

    CControlUI* CManagerUI::GetFocus() const
    {
        return m_pFocus;
    }

    void CManagerUI::SetFocus(CControlUI * pControl)
    {
        // Paint manager window has focus?
        HWND hFocusWnd = ::GetFocus();
        if (hFocusWnd != m_hWndPaint && pControl != m_pFocus) ::SetFocus(m_hWndPaint);
        // Already has focus?
        if (pControl == m_pFocus) return;
        // Remove focus from old control
        if (m_pFocus != NULL) {
            TEVENT_UI event = {0};
            event.Type = UIEVENT_KILLFOCUS;
            event.pSender = pControl;
            event.dwTimestamp = ::GetTickCount();
            m_pFocus->Event(event);
            SendNotify(m_pFocus, UIMSG_KILLFOCUS);
            m_pFocus = NULL;
        }
        if (pControl == NULL) return;
        // Set focus to new control
        if (pControl != NULL && pControl->GetManager() == this && pControl->IsVisible() && pControl->IsEnabled()) {
            m_pFocus = pControl;
            TEVENT_UI event = {0};
            event.Type = UIEVENT_SETFOCUS;
            event.pSender = pControl;
            event.dwTimestamp = ::GetTickCount();
            m_pFocus->Event(event);
            SendNotify(m_pFocus, UIMSG_SETFOCUS);
        }
    }

    void CManagerUI::SetFocusNeeded(CControlUI * pControl)
    {
        ::SetFocus(m_hWndPaint);
        if (pControl == NULL) return;
        if (m_pFocus != NULL) {
            TEVENT_UI event = {0};
            event.Type = UIEVENT_KILLFOCUS;
            event.pSender = pControl;
            event.dwTimestamp = ::GetTickCount();
            m_pFocus->Event(event);
            SendNotify(m_pFocus, UIMSG_KILLFOCUS);
            m_pFocus = NULL;
        }
        FINDTABINFO info = {0};
        info.pFocus = pControl;
        info.bForward = FALSE;
        m_pFocus = m_pRoot->FindControl(__FindControlFromTab, &info, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
        m_bFocusNeeded = true;
        if (m_pRoot != NULL) m_pRoot->NeedUpdate();
    }

    BOOL CManagerUI::SetTimer(CControlUI * pControl, UINT nTimerID, UINT uElapse)
    {
        ASSERT(pControl != NULL);
        ASSERT(uElapse > 0);
        for (int i = 0; i < m_aTimers.GetSize(); i++) {
            TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
            if (pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint && pTimer->nLocalID == nTimerID) {
                if (pTimer->bKilled) {
                    if (::SetTimer(m_hWndPaint, pTimer->uWinTimer, uElapse, NULL)) {
                        pTimer->bKilled = FALSE;
                        return true;
                    }
                    return FALSE;
                }
                return FALSE;
            }
        }

        m_uTimerID = (++m_uTimerID) % 0xF0; //0xf1-0xfe特殊用途
        if (!::SetTimer(m_hWndPaint, m_uTimerID, uElapse, NULL)) return FALSE;
        TIMERINFO* pTimer = new TIMERINFO;
        if (pTimer == NULL) return FALSE;
        pTimer->hWnd = m_hWndPaint;
        pTimer->pSender = pControl;
        pTimer->nLocalID = nTimerID;
        pTimer->uWinTimer = m_uTimerID;
        pTimer->bKilled = FALSE;
        return m_aTimers.Add(pTimer);
    }

    BOOL CManagerUI::KillTimer(CControlUI * pControl, UINT nTimerID)
    {
        ASSERT(pControl != NULL);
        for (int i = 0; i < m_aTimers.GetSize(); i++) {
            TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
            if (pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint && pTimer->nLocalID == nTimerID) {
                if (pTimer->bKilled == FALSE) {
                    if (::IsWindow(m_hWndPaint)) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
                    pTimer->bKilled = true;
                    return true;
                }
            }
        }
        return FALSE;
    }

    void CManagerUI::KillTimer(CControlUI * pControl)
    {
        ASSERT(pControl != NULL);
        int count = m_aTimers.GetSize();
        for (int i = 0, j = 0; i < count; i++) {
            TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i - j]);
            if (pTimer->pSender == pControl && pTimer->hWnd == m_hWndPaint) {
                if (pTimer->bKilled == FALSE) ::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
                delete pTimer;
                m_aTimers.Remove(i - j);
                j++;
            }
        }
    }

    void CManagerUI::RemoveAllTimers()
    {
        for (int i = 0; i < m_aTimers.GetSize(); i++) {
            TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
            if (pTimer->hWnd == m_hWndPaint) {
                if (pTimer->bKilled == FALSE) {
                    if (::IsWindow(m_hWndPaint)) ::KillTimer(m_hWndPaint, pTimer->uWinTimer);
                }
                delete pTimer;
            }
        }

        m_aTimers.Empty();
    }

    void CManagerUI::SetCapture()
    {
        ::SetCapture(m_hWndPaint);
        m_bMouseCapture = true;
    }

    void CManagerUI::ReleaseCapture()
    {
        ::ReleaseCapture();
        m_bMouseCapture = FALSE;
        m_bDragMode = FALSE;
    }

    BOOL CManagerUI::IsCaptured()
    {
        return m_bMouseCapture;
    }

    BOOL CManagerUI::IsPainting()
    {
        return m_bIsPainting;
    }

    void CManagerUI::SetPainting(BOOL bIsPainting)
    {
        m_bIsPainting = bIsPainting;
    }

    BOOL CManagerUI::SetNextTabControl(BOOL bForward)
    {
        // If we're in the process of restructuring the layout we can delay the
        // focus calulation until the next repaint.
        if (m_bUpdateNeeded && bForward) {
            m_bFocusNeeded = true;
            ::InvalidateRect(m_hWndPaint, NULL, FALSE);
            return true;
        }
        // Find next/previous tabbable control
        FINDTABINFO info1 = {0};
        info1.pFocus = m_pFocus;
        info1.bForward = bForward;
        CControlUI* pControl = m_pRoot->FindControl(__FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
        if (pControl == NULL) {
            if (bForward) {
                // Wrap around
                FINDTABINFO info2 = {0};
                info2.pFocus = bForward?NULL:info1.pLast;
                info2.bForward = bForward;
                pControl = m_pRoot->FindControl(__FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
            } else {
                pControl = info1.pLast;
            }
        }
        
        if (pControl != NULL) {
            SetFocus(pControl);
        }

        m_bFocusNeeded = FALSE;
        return true;
    }

    BOOL CManagerUI::AddNotifier(INotifyUI * pNotifier)
    {
        ASSERT(m_aNotifiers.Find(pNotifier) < 0);
        return m_aNotifiers.Add(pNotifier);
    }

    BOOL CManagerUI::RemoveNotifier(INotifyUI * pNotifier)
    {
        for (int i = 0; i < m_aNotifiers.GetSize(); i++) {
            if (static_cast<INotifyUI*>(m_aNotifiers[i]) == pNotifier) {
                return m_aNotifiers.Remove(i);
            }
        }
        return FALSE;
    }

    BOOL CManagerUI::AddPreMessageFilter(IMessageFilterUI * pFilter)
    {
        ASSERT(m_aPreMessageFilters.Find(pFilter) < 0);
        return m_aPreMessageFilters.Add(pFilter);
    }

    BOOL CManagerUI::RemovePreMessageFilter(IMessageFilterUI * pFilter)
    {
        for (int i = 0; i < m_aPreMessageFilters.GetSize(); i++) {
            if (static_cast<IMessageFilterUI*>(m_aPreMessageFilters[i]) == pFilter) {
                return m_aPreMessageFilters.Remove(i);
            }
        }
        return FALSE;
    }

    BOOL CManagerUI::AddMessageFilter(IMessageFilterUI * pFilter)
    {
        ASSERT(m_aMessageFilters.Find(pFilter) < 0);
        return m_aMessageFilters.Add(pFilter);
    }

    BOOL CManagerUI::RemoveMessageFilter(IMessageFilterUI * pFilter)
    {
        for (int i = 0; i < m_aMessageFilters.GetSize(); i++) {
            if (static_cast<IMessageFilterUI*>(m_aMessageFilters[i]) == pFilter) {
                return m_aMessageFilters.Remove(i);
            }
        }
        return FALSE;
    }

    int CManagerUI::GetPostPaintCount() const
    {
        return m_aPostPaintControls.GetSize();
    }

    BOOL CManagerUI::IsPostPaint(CControlUI * pControl)
    {
        return m_aPostPaintControls.Find(pControl) >= 0;
    }

    BOOL CManagerUI::AddPostPaint(CControlUI * pControl)
    {
        ASSERT(m_aPostPaintControls.Find(pControl) < 0);
        return m_aPostPaintControls.Add(pControl);
    }

    BOOL CManagerUI::RemovePostPaint(CControlUI * pControl)
    {
        for (int i = 0; i < m_aPostPaintControls.GetSize(); i++) {
            if (static_cast<CControlUI*>(m_aPostPaintControls[i]) == pControl) {
                return m_aPostPaintControls.Remove(i);
            }
        }
        return FALSE;
    }

    BOOL CManagerUI::SetPostPaintIndex(CControlUI * pControl, int iIndex)
    {
        RemovePostPaint(pControl);
        return m_aPostPaintControls.InsertAt(iIndex, pControl);
    }

    int CManagerUI::GetNativeWindowCount() const
    {
        return m_aNativeWindow.GetSize();
    }

    BOOL CManagerUI::AddNativeWindow(CControlUI * pControl, HWND hChildWnd)
    {
        if ((pControl == NULL) || (hChildWnd == NULL)) {
            return FALSE;
        }

        RECT rcChildWnd = GetNativeWindowRect(hChildWnd);
        Invalidate(rcChildWnd);

        if (m_aNativeWindow.Find(hChildWnd) >= 0) {
            return FALSE;
        }

        if (m_aNativeWindow.Add(hChildWnd)) {
            m_aNativeWindowControl.Add(pControl);
            return true;
        }

        return FALSE;
    }

    BOOL CManagerUI::RemoveNativeWindow(HWND hChildWnd)
    {
        for (int i = 0; i < m_aNativeWindow.GetSize(); i++) {
            if (static_cast<HWND>(m_aNativeWindow[i]) == hChildWnd) {
                if (m_aNativeWindow.Remove(i)) {
                    m_aNativeWindowControl.Remove(i);
                    return true;
                }
                return FALSE;
            }
        }
        return FALSE;
    }

    RECT CManagerUI::GetNativeWindowRect(HWND hChildWnd)
    {
        RECT rcChildWnd;
        ::GetWindowRect(hChildWnd, &rcChildWnd);
        ::ScreenToClient(m_hWndPaint, (LPPOINT)(&rcChildWnd));
        ::ScreenToClient(m_hWndPaint, (LPPOINT)(&rcChildWnd) + 1);
        return rcChildWnd;
    }

    void CManagerUI::AddDelayedCleanup(CControlUI * pControl)
    {
        if (pControl == NULL) return;
        pControl->SetManager(this, NULL, FALSE);
        m_aDelayedCleanup.Add(pControl);
        PostAsyncNotify();
    }

    void CManagerUI::AddMouseLeaveNeeded(CControlUI * pControl)
    {
        if (pControl == NULL) return;
        for (int i = 0; i < m_aNeedMouseLeaveNeeded.GetSize(); i++) {
            if (static_cast<CControlUI*>(m_aNeedMouseLeaveNeeded[i]) == pControl) {
                return;
            }
        }
        m_aNeedMouseLeaveNeeded.Add(pControl);
    }

    BOOL CManagerUI::RemoveMouseLeaveNeeded(CControlUI * pControl)
    {
        if (pControl == NULL) return FALSE;
        for (int i = 0; i < m_aNeedMouseLeaveNeeded.GetSize(); i++) {
            if (static_cast<CControlUI*>(m_aNeedMouseLeaveNeeded[i]) == pControl) {
                return m_aNeedMouseLeaveNeeded.Remove(i);
            }
        }
        return FALSE;
    }

    void CManagerUI::SendNotify(CControlUI * pControl, LPCTSTR pstrMessage, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/, BOOL bAsync /*= FALSE*/)
    {
        TNOTIFY_UI Msg;
        Msg.pSender = pControl;
        Msg.sType = pstrMessage;
        Msg.wParam = wParam;
        Msg.lParam = lParam;
        SendNotify(Msg, bAsync);
    }

    void CManagerUI::SendNotify(TNOTIFY_UI & Msg, BOOL bAsync /*= FALSE*/)
    {
        Msg.ptMouse = m_ptLastMousePos;
        Msg.dwTimestamp = ::GetTickCount();
        if (m_bUsedVirtualWnd) {
            Msg.sVirtualWnd = Msg.pSender->GetVirtualWnd();
        }

        if (!bAsync) {
            // Send to all listeners
            if (Msg.pSender != NULL) {
                if (Msg.pSender->OnNotify) Msg.pSender->OnNotify(&Msg);
            }
            for (int i = 0; i < m_aNotifiers.GetSize(); i++) {
                static_cast<INotifyUI*>(m_aNotifiers[i])->Notify(Msg);
            }
        } else {
            TNOTIFY_UI* pMsg = new TNOTIFY_UI;
            pMsg->pSender = Msg.pSender;
            pMsg->sType = Msg.sType;
            pMsg->wParam = Msg.wParam;
            pMsg->lParam = Msg.lParam;
            pMsg->ptMouse = Msg.ptMouse;
            pMsg->dwTimestamp = Msg.dwTimestamp;
            m_aAsyncNotify.Add(pMsg);

            PostAsyncNotify();
        }
    }

    BOOL CManagerUI::IsForceUseSharedRes() const
    {
        return m_bForceUseSharedRes;
    }

    void CManagerUI::SetForceUseSharedRes(BOOL bForce)
    {
        m_bForceUseSharedRes = bForce;
    }

    DWORD CManagerUI::GetDefaultDisabledColor() const
    {
        return m_ResInfo.m_dwDefaultDisabledColor;
    }

    void CManagerUI::SetDefaultDisabledColor(DWORD dwColor, BOOL bShared)
    {
        if (bShared) {
            if (m_ResInfo.m_dwDefaultDisabledColor == CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultDisabledColor) {
                m_ResInfo.m_dwDefaultDisabledColor = dwColor;
            }
            CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultDisabledColor = dwColor;
        } else {
            m_ResInfo.m_dwDefaultDisabledColor = dwColor;
        }
    }

    DWORD CManagerUI::GetDefaultFontColor() const
    {
        return m_ResInfo.m_dwDefaultFontColor;
    }

    void CManagerUI::SetDefaultFontColor(DWORD dwColor, BOOL bShared)
    {
        if (bShared) {
            if (m_ResInfo.m_dwDefaultFontColor == CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultFontColor) {
                m_ResInfo.m_dwDefaultFontColor = dwColor;
            }
            CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultFontColor = dwColor;
        } else {
            m_ResInfo.m_dwDefaultFontColor = dwColor;
        }
    }

    DWORD CManagerUI::GetDefaultLinkFontColor() const
    {
        return m_ResInfo.m_dwDefaultLinkFontColor;
    }

    void CManagerUI::SetDefaultLinkFontColor(DWORD dwColor, BOOL bShared)
    {
        if (bShared) {
            if (m_ResInfo.m_dwDefaultLinkFontColor == CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultLinkFontColor) {
                m_ResInfo.m_dwDefaultLinkFontColor = dwColor;
            }
            CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultLinkFontColor = dwColor;
        } else {
            m_ResInfo.m_dwDefaultLinkFontColor = dwColor;
        }
    }

    DWORD CManagerUI::GetDefaultLinkHoverFontColor() const
    {
        return m_ResInfo.m_dwDefaultLinkHoverFontColor;
    }

    void CManagerUI::SetDefaultLinkHoverFontColor(DWORD dwColor, BOOL bShared)
    {
        if (bShared) {
            if (m_ResInfo.m_dwDefaultLinkHoverFontColor == CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultLinkHoverFontColor) {
                m_ResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
            }
            CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultLinkHoverFontColor = dwColor;
        } else {
            m_ResInfo.m_dwDefaultLinkHoverFontColor = dwColor;
        }
    }

    DWORD CManagerUI::GetDefaultSelectedBkColor() const
    {
        return m_ResInfo.m_dwDefaultSelectedBkColor;
    }

    void CManagerUI::SetDefaultSelectedBkColor(DWORD dwColor, BOOL bShared)
    {
        if (bShared) {
            if (m_ResInfo.m_dwDefaultSelectedBkColor == CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultSelectedBkColor) {
                m_ResInfo.m_dwDefaultSelectedBkColor = dwColor;
            }
            CResourceUI::GetInstance()->GetSharedResInfo().m_dwDefaultSelectedBkColor = dwColor;
        } else {
            m_ResInfo.m_dwDefaultSelectedBkColor = dwColor;
        }
    }

    TFONTINFO_UI* CManagerUI::GetDefaultFontInfo()
    {
        if (m_ResInfo.m_DefaultFontInfo.sFontName.IsEmpty()) {
            if (CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.tm.tmHeight == 0) {
                HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.hFont);
                ::GetTextMetrics(m_hDcPaint, &CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.tm);
                ::SelectObject(m_hDcPaint, hOldFont);
            }
            return &CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo;
        } else {
            if (m_ResInfo.m_DefaultFontInfo.tm.tmHeight == 0) {
                HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, m_ResInfo.m_DefaultFontInfo.hFont);
                ::GetTextMetrics(m_hDcPaint, &m_ResInfo.m_DefaultFontInfo.tm);
                ::SelectObject(m_hDcPaint, hOldFont);
            }
            return &m_ResInfo.m_DefaultFontInfo;
        }
    }

    void CManagerUI::SetDefaultFont(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic, BOOL bShared)
    {
        LOGFONT lf = {0};
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        if (lstrlen(pStrFontName) > 0) {
            TCHAR szFaceName[32] = {0}; //_T("@");
            _tcsncat(szFaceName, pStrFontName, LF_FACESIZE);
            _tcsncpy(lf.lfFaceName, szFaceName, LF_FACESIZE);
        }
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfHeight = -GetDPIObj()->Scale(nSize);
        ;
        if (bBold) lf.lfWeight += FW_BOLD;
        if (bUnderline) lf.lfUnderline = TRUE;
        if (bItalic) lf.lfItalic = TRUE;

        HFONT hFont = ::CreateFontIndirect(&lf);
        if (hFont == NULL) return;

        if (bShared) {
            ::DeleteObject(CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.hFont);
            CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.hFont = hFont;
            CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.sFontName = lf.lfFaceName;
            CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.iSize = nSize;
            CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.bBold = bBold;
            CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.bUnderline = bUnderline;
            CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.bItalic = bItalic;
            ::ZeroMemory(&CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.tm, sizeof(CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.tm));
            if (m_hDcPaint) {
                HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, hFont);
                ::GetTextMetrics(m_hDcPaint, &CResourceUI::GetInstance()->GetSharedResInfo().m_DefaultFontInfo.tm);
                ::SelectObject(m_hDcPaint, hOldFont);
            }
        } else {
            ::DeleteObject(m_ResInfo.m_DefaultFontInfo.hFont);
            m_ResInfo.m_DefaultFontInfo.hFont = hFont;
            m_ResInfo.m_DefaultFontInfo.sFontName = lf.lfFaceName;
            m_ResInfo.m_DefaultFontInfo.iSize = nSize;
            m_ResInfo.m_DefaultFontInfo.bBold = bBold;
            m_ResInfo.m_DefaultFontInfo.bUnderline = bUnderline;
            m_ResInfo.m_DefaultFontInfo.bItalic = bItalic;
            ::ZeroMemory(&m_ResInfo.m_DefaultFontInfo.tm, sizeof(m_ResInfo.m_DefaultFontInfo.tm));
            if (m_hDcPaint) {
                HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, hFont);
                ::GetTextMetrics(m_hDcPaint, &m_ResInfo.m_DefaultFontInfo.tm);
                ::SelectObject(m_hDcPaint, hOldFont);
            }
        }
    }

    DWORD CManagerUI::GetCustomFontCount(BOOL bShared) const
    {
        if (bShared)
            return CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetSize();
        else
            return m_ResInfo.m_CustomFonts.GetSize();
    }

    HFONT CManagerUI::AddFont(int id, LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic, BOOL bShared)
    {
        LOGFONT lf = {0};
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        if (lstrlen(pStrFontName) > 0) {
            TCHAR szFaceName[32] = {0}; //_T("@");
            _tcsncat(szFaceName, pStrFontName, LF_FACESIZE);
            _tcsncpy(lf.lfFaceName, szFaceName, LF_FACESIZE);
        }
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfHeight = -GetDPIObj()->Scale(nSize);
        if (bBold) lf.lfWeight = FW_BOLD;
        if (bUnderline) lf.lfUnderline = TRUE;
        if (bItalic) lf.lfItalic = TRUE;
        HFONT hFont = ::CreateFontIndirect(&lf);
        if (hFont == NULL) return NULL;

        TFONTINFO_UI * pFontInfo = new TFONTINFO_UI;
        if (!pFontInfo) return FALSE;
        ::ZeroMemory(pFontInfo, sizeof(TFONTINFO_UI));
        pFontInfo->hFont = hFont;
        pFontInfo->sFontName = lf.lfFaceName;
        pFontInfo->iSize = nSize;
        pFontInfo->bBold = bBold;
        pFontInfo->bUnderline = bUnderline;
        pFontInfo->bItalic = bItalic;
        if (m_hDcPaint) {
            HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, hFont);
            ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
            ::SelectObject(m_hDcPaint, hOldFont);
        }
        TCHAR idBuffer[16];
        ::ZeroMemory(idBuffer, sizeof(idBuffer));
        _itot(id, idBuffer, 10);
        if (bShared || m_bForceUseSharedRes) {
            TFONTINFO_UI* pOldFontInfo = static_cast<TFONTINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Find(idBuffer));
            if (pOldFontInfo) {
                ::DeleteObject(pOldFontInfo->hFont);
                delete pOldFontInfo;
                CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Remove(idBuffer);
            }

            if (!CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Insert(idBuffer, pFontInfo)) {
                ::DeleteObject(hFont);
                delete pFontInfo;
                return NULL;
            }
        } else {
            TFONTINFO_UI* pOldFontInfo = static_cast<TFONTINFO_UI*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
            if (pOldFontInfo) {
                ::DeleteObject(pOldFontInfo->hFont);
                delete pOldFontInfo;
                m_ResInfo.m_CustomFonts.Remove(idBuffer);
            }

            if (!m_ResInfo.m_CustomFonts.Insert(idBuffer, pFontInfo)) {
                ::DeleteObject(hFont);
                delete pFontInfo;
                return NULL;
            }
        }

        return hFont;
    }
    void CManagerUI::AddFontArray(LPCTSTR pstrPath)
    {
        LPBYTE pData = NULL;
        DWORD dwSize = 0;
        do {
            CStringUI sFile = CResourceUI::GetInstance()->GetResourcePath();
            if (CResourceUI::GetInstance()->GetResourceZip().IsEmpty()) {
                sFile += pstrPath;
                HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL, NULL);
                if (hFile == INVALID_HANDLE_VALUE) break;
                dwSize = ::GetFileSize(hFile, NULL);
                if (dwSize == 0) break;

                DWORD dwRead = 0;
                pData = new BYTE[dwSize];
                ::ReadFile(hFile, pData, dwSize, &dwRead, NULL);
                ::CloseHandle(hFile);

                if (dwRead != dwSize) {
                    delete[] pData;
                    pData = NULL;
                    break;
                }
            } else {
                sFile += CResourceUI::GetInstance()->GetResourceZip();
                HZIP hz = NULL;
                if (CResourceUI::GetInstance()->IsCachedResourceZip())
                    hz = (HZIP)CResourceUI::GetInstance()->GetResourceZipHandle();
                else {
                    CStringUI sFilePwd = CResourceUI::GetInstance()->GetResourceZipPwd();
#ifdef UNICODE
                    char* pwd = w2a((wchar_t*)sFilePwd.GetData());
                    hz = OpenZip(sFile.GetData(), pwd);
                    if (pwd) delete[] pwd;
#else
                    hz = OpenZip(sFile.GetData(), sFilePwd.GetData());
#endif
                }
                if (hz == NULL) break;
                ZIPENTRY ze;
                int i = 0;
                CStringUI key = pstrPath;
                key.Replace(_T("\\"), _T("/"));
                if (FindZipItem(hz, key, true, &i, &ze) != 0) break;
                dwSize = ze.unc_size;
                if (dwSize == 0) break;
                pData = new BYTE[dwSize];
                int res = UnzipItem(hz, i, pData, dwSize);
                if (res != 0x00000000 && res != 0x00000600) {
                    delete[] pData;
                    pData = NULL;
                    if (!CResourceUI::GetInstance()->IsCachedResourceZip()) CloseZip(hz);
                    break;
                }
                if (!CResourceUI::GetInstance()->IsCachedResourceZip()) CloseZip(hz);
            }

        } while (0);

        while (!pData) {
            //读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
            HANDLE hFile = ::CreateFile(pstrPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile == INVALID_HANDLE_VALUE) break;
            dwSize = ::GetFileSize(hFile, NULL);
            if (dwSize == 0) break;

            DWORD dwRead = 0;
            pData = new BYTE[dwSize];
            ::ReadFile(hFile, pData, dwSize, &dwRead, NULL);
            ::CloseHandle(hFile);

            if (dwRead != dwSize) {
                delete[] pData;
                pData = NULL;
            }
            break;
        }
        DWORD nFonts;
        HANDLE hFont = ::AddFontMemResourceEx(pData, dwSize, NULL, &nFonts);
        delete[] pData;
        pData = NULL;
        m_aFonts.Add(hFont);
    }
    HFONT CManagerUI::GetFont(int id)
    {
        if (id < 0) return GetDefaultFontInfo()->hFont;

        TCHAR idBuffer[16];
        ::ZeroMemory(idBuffer, sizeof(idBuffer));
        _itot(id, idBuffer, 10);
        TFONTINFO_UI* pFontInfo = static_cast<TFONTINFO_UI*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
        if (!pFontInfo) pFontInfo = static_cast<TFONTINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Find(idBuffer));
        if (!pFontInfo) return GetDefaultFontInfo()->hFont;
        return pFontInfo->hFont;
    }

    HFONT CManagerUI::GetFont(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic)
    {
        TFONTINFO_UI* pFontInfo = NULL;
        for (int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFONTINFO_UI*>(m_ResInfo.m_CustomFonts.Find(key));
                if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize &&
                    pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic)
                    return pFontInfo->hFont;
            }
        }
        for (int i = 0; i < CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFONTINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Find(key));
                if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize &&
                    pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic)
                    return pFontInfo->hFont;
            }
        }

        return NULL;
    }

    int CManagerUI::GetFontIndex(HFONT hFont, BOOL bShared)
    {
        TFONTINFO_UI* pFontInfo = NULL;
        if (bShared) {
            for (int i = 0; i < CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetSize(); i++) {
                if (LPCTSTR key = CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetAt(i)) {
                    pFontInfo = static_cast<TFONTINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Find(key));
                    if (pFontInfo && pFontInfo->hFont == hFont) return _ttoi(key);
                }
            }
        } else {
            for (int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++) {
                if (LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
                    pFontInfo = static_cast<TFONTINFO_UI*>(m_ResInfo.m_CustomFonts.Find(key));
                    if (pFontInfo && pFontInfo->hFont == hFont) return _ttoi(key);
                }
            }
        }

        return -1;
    }

    int CManagerUI::GetFontIndex(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic, BOOL bShared)
    {
        TFONTINFO_UI* pFontInfo = NULL;
        if (bShared) {
            for (int i = 0; i < CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetSize(); i++) {
                if (LPCTSTR key = CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetAt(i)) {
                    pFontInfo = static_cast<TFONTINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Find(key));
                    if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize &&
                        pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic)
                        return _ttoi(key);
                }
            }
        } else {
            for (int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++) {
                if (LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
                    pFontInfo = static_cast<TFONTINFO_UI*>(m_ResInfo.m_CustomFonts.Find(key));
                    if (pFontInfo && pFontInfo->sFontName == pStrFontName && pFontInfo->iSize == nSize &&
                        pFontInfo->bBold == bBold && pFontInfo->bUnderline == bUnderline && pFontInfo->bItalic == bItalic)
                        return _ttoi(key);
                }
            }
        }

        return -1;
    }

    void CManagerUI::RemoveFont(HFONT hFont, BOOL bShared)
    {
        TFONTINFO_UI* pFontInfo = NULL;
        if (bShared) {
            for (int i = 0; i < CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetSize(); i++) {
                if (LPCTSTR key = CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetAt(i)) {
                    pFontInfo = static_cast<TFONTINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Find(key));
                    if (pFontInfo && pFontInfo->hFont == hFont) {
                        ::DeleteObject(pFontInfo->hFont);
                        delete pFontInfo;
                        CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Remove(key);
                        return;
                    }
                }
            }
        } else {
            for (int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++) {
                if (LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
                    pFontInfo = static_cast<TFONTINFO_UI*>(m_ResInfo.m_CustomFonts.Find(key));
                    if (pFontInfo && pFontInfo->hFont == hFont) {
                        ::DeleteObject(pFontInfo->hFont);
                        delete pFontInfo;
                        m_ResInfo.m_CustomFonts.Remove(key);
                        return;
                    }
                }
            }
        }
    }

    void CManagerUI::RemoveFont(int id, BOOL bShared)
    {
        TCHAR idBuffer[16];
        ::ZeroMemory(idBuffer, sizeof(idBuffer));
        _itot(id, idBuffer, 10);

        TFONTINFO_UI* pFontInfo = NULL;
        if (bShared) {
            pFontInfo = static_cast<TFONTINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Find(idBuffer));
            if (pFontInfo) {
                ::DeleteObject(pFontInfo->hFont);
                delete pFontInfo;
                CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Remove(idBuffer);
            }
        } else {
            pFontInfo = static_cast<TFONTINFO_UI*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
            if (pFontInfo) {
                ::DeleteObject(pFontInfo->hFont);
                delete pFontInfo;
                m_ResInfo.m_CustomFonts.Remove(idBuffer);
            }
        }
    }

    void CManagerUI::RemoveAllFonts(BOOL bShared)
    {
        TFONTINFO_UI* pFontInfo;
        if (bShared) {
            for (int i = 0; i < CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetSize(); i++) {
                if (LPCTSTR key = CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetAt(i)) {
                    pFontInfo = static_cast<TFONTINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Find(key, FALSE));
                    if (pFontInfo) {
                        ::DeleteObject(pFontInfo->hFont);
                        delete pFontInfo;
                    }
                }
            }
            CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.RemoveAll();
        } else {
            for (int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++) {
                if (LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
                    pFontInfo = static_cast<TFONTINFO_UI*>(m_ResInfo.m_CustomFonts.Find(key, FALSE));
                    if (pFontInfo) {
                        ::DeleteObject(pFontInfo->hFont);
                        delete pFontInfo;
                    }
                }
            }
            m_ResInfo.m_CustomFonts.RemoveAll();
        }
    }

    TFONTINFO_UI* CManagerUI::GetFontInfo(int id)
    {
        if (id < 0) {
            return GetDefaultFontInfo();
        }

        TCHAR idBuffer[16];
        ::ZeroMemory(idBuffer, sizeof(idBuffer));
        _itot(id, idBuffer, 10);
        TFONTINFO_UI* pFontInfo = static_cast<TFONTINFO_UI*>(m_ResInfo.m_CustomFonts.Find(idBuffer));
        if (!pFontInfo) {
            pFontInfo = static_cast<TFONTINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Find(idBuffer));
        }
        if (!pFontInfo) {
            pFontInfo = GetDefaultFontInfo();
        }
        if (pFontInfo->tm.tmHeight == 0) {
            HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, pFontInfo->hFont);
            ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
            ::SelectObject(m_hDcPaint, hOldFont);
        }
        return pFontInfo;
    }

    TFONTINFO_UI* CManagerUI::GetFontInfo(HFONT hFont)
    {
        TFONTINFO_UI* pFontInfo = NULL;
        for (int i = 0; i < m_ResInfo.m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = m_ResInfo.m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFONTINFO_UI*>(m_ResInfo.m_CustomFonts.Find(key));
                if (pFontInfo && pFontInfo->hFont == hFont) {
                    break;
                }
            }
        }
        if (!pFontInfo) {
            for (int i = 0; i < CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetSize(); i++) {
                if (LPCTSTR key = CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.GetAt(i)) {
                    pFontInfo = static_cast<TFONTINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_CustomFonts.Find(key));
                    if (pFontInfo && pFontInfo->hFont == hFont) break;
                }
            }
        }
        if (!pFontInfo) pFontInfo = GetDefaultFontInfo();
        if (pFontInfo->tm.tmHeight == 0) {
            HFONT hOldFont = (HFONT)::SelectObject(m_hDcPaint, pFontInfo->hFont);
            ::GetTextMetrics(m_hDcPaint, &pFontInfo->tm);
            ::SelectObject(m_hDcPaint, hOldFont);
        }
        return pFontInfo;
    }

    const TIMAGEINFO_UI* CManagerUI::GetImage(LPCTSTR bitmap)const
    {
        TIMAGEINFO_UI* data = static_cast<TIMAGEINFO_UI*>(m_ResInfo.m_ImageHash.Find(bitmap));
        if (!data) data = static_cast<TIMAGEINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.Find(bitmap));
        return data;
    }

    const TIMAGEINFO_UI* CManagerUI::GetImageEx(LPCTSTR bitmap, LPCTSTR type, DWORD mask, BOOL bUseHSL, HINSTANCE instance) const
    {
        const TIMAGEINFO_UI* data = GetImage(bitmap);
        if (!data) {
            if (AddImage(bitmap, type, mask, bUseHSL, FALSE, instance)) {
                if (m_bForceUseSharedRes)
                    data = static_cast<TIMAGEINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.Find(bitmap));
                else
                    data = static_cast<TIMAGEINFO_UI*>(m_ResInfo.m_ImageHash.Find(bitmap));
            }
        }

        return data;
    }

    const TIMAGEINFO_UI* CManagerUI::AddImage(LPCTSTR bitmap, LPCTSTR type, DWORD mask, BOOL bUseHSL, BOOL bShared, HINSTANCE instance)
    {
        if (bitmap == NULL || bitmap[0] == _T('\0')) return NULL;

        TIMAGEINFO_UI * data = NULL;
        if (type != NULL && lstrlen(type) > 0) {
            if (isdigit(*bitmap)) {
                LPTSTR pstr = NULL;
                int iIndex = _tcstol(bitmap, &pstr, 10);
                data = CRenderUI::LoadImage(iIndex, type, mask, instance);
            }
        } else {
            data = CRenderUI::LoadImage(bitmap, NULL, mask, instance);
        }

        if (data == NULL) {
            return NULL;
        }
        data->bUseHSL = bUseHSL;
        if (type != NULL) data->sResType = type;
        data->dwMask = mask;
        if (data->bUseHSL) {
            data->pSrcBits = new BYTE[data->nX * data->nY * 4];
            ::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
        } else {
            data->pSrcBits = NULL;
        }

        if (m_bUseHSL) {
            CRenderUI::AdjustImage(true, data, m_H, m_S, m_L);
        }

        if (data) {
            if (bShared || m_bForceUseSharedRes) {
                TIMAGEINFO_UI* pOldImageInfo = static_cast<TIMAGEINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.Find(bitmap));
                if (pOldImageInfo) {
                    CRenderUI::FreeImage(pOldImageInfo);
                    CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.Remove(bitmap);
                }

                if (!CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.Insert(bitmap, data)) {
                    CRenderUI::FreeImage(data);
                    data = NULL;
                }
            } else {
                TIMAGEINFO_UI* pOldImageInfo = static_cast<TIMAGEINFO_UI*>(m_ResInfo.m_ImageHash.Find(bitmap));
                if (pOldImageInfo) {
                    CRenderUI::FreeImage(pOldImageInfo);
                    m_ResInfo.m_ImageHash.Remove(bitmap);
                }

                if (!m_ResInfo.m_ImageHash.Insert(bitmap, data)) {
                    CRenderUI::FreeImage(data);
                    data = NULL;
                }
            }
        }

        return data;
    }

    const TIMAGEINFO_UI* CManagerUI::AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, BOOL bAlpha, BOOL bShared)
    {
        // 因无法确定外部HBITMAP格式，不能使用hsl调整
        if (bitmap == NULL || bitmap[0] == _T('\0')) return NULL;
        if (hBitmap == NULL || iWidth <= 0 || iHeight <= 0) return NULL;

        TIMAGEINFO_UI * data = new TIMAGEINFO_UI;
        data->pBits = NULL;
        data->pSrcBits = NULL;
        data->hBitmap = hBitmap;
        data->pBits = NULL;
        data->nX = iWidth;
        data->nY = iHeight;
        data->bAlpha = bAlpha;
        data->bUseHSL = FALSE;
        data->pSrcBits = NULL;
        data->dwMask = 0;

        if (bShared || m_bForceUseSharedRes) {
            if (!CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.Insert(bitmap, data)) {
                CRenderUI::FreeImage(data);
                data = NULL;
            }
        } else {
            if (!m_ResInfo.m_ImageHash.Insert(bitmap, data)) {
                CRenderUI::FreeImage(data);
                data = NULL;
            }
        }

        return data;
    }

    void CManagerUI::RemoveImage(LPCTSTR bitmap, BOOL bShared)
    {
        TIMAGEINFO_UI* data = NULL;
        if (bShared) {
            data = static_cast<TIMAGEINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.Find(bitmap));
            if (data) {
                CRenderUI::FreeImage(data);
                CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.Remove(bitmap);
            }
        } else {
            data = static_cast<TIMAGEINFO_UI*>(m_ResInfo.m_ImageHash.Find(bitmap));
            if (data) {
                CRenderUI::FreeImage(data);
                m_ResInfo.m_ImageHash.Remove(bitmap);
            }
        }
    }

    void CManagerUI::RemoveAllImages(BOOL bShared)
    {
        if (bShared) {
            TIMAGEINFO_UI* data;
            for (int i = 0; i < CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.GetSize(); i++) {
                if (LPCTSTR key = CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.GetAt(i)) {
                    data = static_cast<TIMAGEINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.Find(key, FALSE));
                    if (data) {
                        CRenderUI::FreeImage(data);
                    }
                }
            }
            CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.RemoveAll();
        } else {
            TIMAGEINFO_UI* data;
            for (int i = 0; i < m_ResInfo.m_ImageHash.GetSize(); i++) {
                if (LPCTSTR key = m_ResInfo.m_ImageHash.GetAt(i)) {
                    data = static_cast<TIMAGEINFO_UI*>(m_ResInfo.m_ImageHash.Find(key, FALSE));
                    if (data) {
                        CRenderUI::FreeImage(data);
                    }
                }
            }
            m_ResInfo.m_ImageHash.RemoveAll();
        }
    }

    void CManagerUI::AdjustSharedImagesHSL()
    {
        TIMAGEINFO_UI* data;
        for (int i = 0; i < CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.GetSize(); i++) {
            if (LPCTSTR key = CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.GetAt(i)) {
                data = static_cast<TIMAGEINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.Find(key));
                if (data && data->bUseHSL) {
                    CRenderUI::AdjustImage(m_bUseHSL, data, m_H, m_S, m_L);
                }
            }
        }
    }

    void CManagerUI::AdjustImagesHSL()
    {
        TIMAGEINFO_UI* data;
        for (int i = 0; i < m_ResInfo.m_ImageHash.GetSize(); i++) {
            if (LPCTSTR key = m_ResInfo.m_ImageHash.GetAt(i)) {
                data = static_cast<TIMAGEINFO_UI*>(m_ResInfo.m_ImageHash.Find(key));
                if (data && data->bUseHSL) {
                    CRenderUI::AdjustImage(m_bUseHSL, data, m_H, m_S, m_L);
                }
            }
        }
        Invalidate();
    }

    void CManagerUI::PostAsyncNotify()
    {
        if (!m_bAsyncNotifyPosted) {
            ::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
            m_bAsyncNotifyPosted = true;
        }
    }
    void CManagerUI::ReloadSharedImages()
    {
        TIMAGEINFO_UI* data = NULL;
        TIMAGEINFO_UI* pNewData = NULL;
        for (int i = 0; i < CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.GetSize(); i++) {
            if (LPCTSTR bitmap = CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.GetAt(i)) {
                data = static_cast<TIMAGEINFO_UI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_ImageHash.Find(bitmap));
                if (data != NULL) {
                    if (!data->sResType.IsEmpty()) {
                        if (isdigit(*bitmap)) {
                            LPTSTR pstr = NULL;
                            int iIndex = _tcstol(bitmap, &pstr, 10);
                            pNewData = CRenderUI::LoadImage(iIndex, data->sResType.GetData(), data->dwMask);
                        }
                    } else {
                        pNewData = CRenderUI::LoadImage(bitmap, NULL, data->dwMask);
                    }
                    if (pNewData == NULL) continue;

                    CRenderUI::FreeImage(data, FALSE);
                    data->hBitmap = pNewData->hBitmap;
                    data->pBits = pNewData->pBits;
                    data->nX = pNewData->nX;
                    data->nY = pNewData->nY;
                    data->bAlpha = pNewData->bAlpha;
                    data->pSrcBits = NULL;
                    if (data->bUseHSL) {
                        data->pSrcBits = new BYTE[data->nX * data->nY * 4];
                        ::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
                    } else
                        data->pSrcBits = NULL;
                    if (m_bUseHSL) CRenderUI::AdjustImage(true, data, m_H, m_S, m_L);

                    delete pNewData;
                }
            }
        }
    }

    void CManagerUI::ReloadImages()
    {
        RemoveAllDrawInfos();

        TIMAGEINFO_UI* data = NULL;
        TIMAGEINFO_UI* pNewData = NULL;
        for (int i = 0; i < m_ResInfo.m_ImageHash.GetSize(); i++) {
            if (LPCTSTR bitmap = m_ResInfo.m_ImageHash.GetAt(i)) {
                data = static_cast<TIMAGEINFO_UI*>(m_ResInfo.m_ImageHash.Find(bitmap));
                if (data != NULL) {
                    if (!data->sResType.IsEmpty()) {
                        if (isdigit(*bitmap)) {
                            LPTSTR pstr = NULL;
                            int iIndex = _tcstol(bitmap, &pstr, 10);
                            pNewData = CRenderUI::LoadImage(iIndex, data->sResType.GetData(), data->dwMask);
                        }
                    } else {
                        pNewData = CRenderUI::LoadImage(bitmap, NULL, data->dwMask);
                    }

                    CRenderUI::FreeImage(data, FALSE);
                    if (pNewData == NULL) {
                        m_ResInfo.m_ImageHash.Remove(bitmap);
                        continue;
                    }
                    data->hBitmap = pNewData->hBitmap;
                    data->pBits = pNewData->pBits;
                    data->nX = pNewData->nX;
                    data->nY = pNewData->nY;
                    data->bAlpha = pNewData->bAlpha;
                    data->pSrcBits = NULL;
                    if (data->bUseHSL) {
                        data->pSrcBits = new BYTE[data->nX * data->nY * 4];
                        ::CopyMemory(data->pSrcBits, data->pBits, data->nX * data->nY * 4);
                    } else
                        data->pSrcBits = NULL;
                    if (m_bUseHSL) CRenderUI::AdjustImage(true, data, m_H, m_S, m_L);

                    delete pNewData;
                }
            }
        }

        if (m_pRoot) m_pRoot->Invalidate();
    }

    const TDRAWINFO_UI* CManagerUI::GetDrawInfo(LPCTSTR pStrImage, LPCTSTR pStrModify)
    {
        CStringUI sStrImage = pStrImage;
        CStringUI sStrModify = pStrModify;
        CStringUI sKey = sStrImage + sStrModify;
        TDRAWINFO_UI* pDrawInfo = static_cast<TDRAWINFO_UI*>(m_ResInfo.m_DrawInfoHash.Find(sKey));
        if (pDrawInfo == NULL && !sKey.IsEmpty()) {
            pDrawInfo = new TDRAWINFO_UI();
            pDrawInfo->Parse(pStrImage, pStrModify, this->GetDPIObj());
            m_ResInfo.m_DrawInfoHash.Insert(sKey, pDrawInfo);
        }
        return pDrawInfo;
    }

    void CManagerUI::RemoveDrawInfo(LPCTSTR pStrImage, LPCTSTR pStrModify)
    {
        CStringUI sStrImage = pStrImage;
        CStringUI sStrModify = pStrModify;
        CStringUI sKey = sStrImage + sStrModify;
        TDRAWINFO_UI* pDrawInfo = static_cast<TDRAWINFO_UI*>(m_ResInfo.m_DrawInfoHash.Find(sKey));
        if (pDrawInfo != NULL) {
            m_ResInfo.m_DrawInfoHash.Remove(sKey);
            delete pDrawInfo;
            pDrawInfo = NULL;
        }
    }

    void CManagerUI::RemoveAllDrawInfos()
    {
        TDRAWINFO_UI* pDrawInfo = NULL;
        for (int i = 0; i < m_ResInfo.m_DrawInfoHash.GetSize(); i++) {
            LPCTSTR key = m_ResInfo.m_DrawInfoHash.GetAt(i);
            if (key != NULL) {
                pDrawInfo = static_cast<TDRAWINFO_UI*>(m_ResInfo.m_DrawInfoHash.Find(key, FALSE));
                if (pDrawInfo) {
                    delete pDrawInfo;
                    pDrawInfo = NULL;
                }
            }
        }
        m_ResInfo.m_DrawInfoHash.RemoveAll();
    }

    void CManagerUI::AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList, BOOL bShared)
    {
        if (bShared || m_bForceUseSharedRes) {
            CStringUI* pDefaultAttr = new CStringUI(pStrControlAttrList);
            if (pDefaultAttr != NULL) {
                CStringUI* pOldDefaultAttr = static_cast<CStringUI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_AttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr));
                if (pOldDefaultAttr) delete pOldDefaultAttr;
            }
        } else {
            CStringUI* pDefaultAttr = new CStringUI(pStrControlAttrList);
            if (pDefaultAttr != NULL) {
                CStringUI* pOldDefaultAttr = static_cast<CStringUI*>(m_ResInfo.m_AttrHash.Set(pStrControlName, (LPVOID)pDefaultAttr));
                if (pOldDefaultAttr) delete pOldDefaultAttr;
            }
        }
    }

    LPCTSTR CManagerUI::GetDefaultAttributeList(LPCTSTR pStrControlName) const
    {
        CStringUI* pDefaultAttr = static_cast<CStringUI*>(m_ResInfo.m_AttrHash.Find(pStrControlName));
        if (!pDefaultAttr) pDefaultAttr = static_cast<CStringUI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_AttrHash.Find(pStrControlName));
        if (pDefaultAttr) return pDefaultAttr->GetData();
        return NULL;
    }

    BOOL CManagerUI::RemoveDefaultAttributeList(LPCTSTR pStrControlName, BOOL bShared)
    {
        if (bShared) {
            CStringUI* pDefaultAttr = static_cast<CStringUI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_AttrHash.Find(pStrControlName));
            if (!pDefaultAttr) return FALSE;

            delete pDefaultAttr;
            return CResourceUI::GetInstance()->GetSharedResInfo().m_AttrHash.Remove(pStrControlName);
        } else {
            CStringUI* pDefaultAttr = static_cast<CStringUI*>(m_ResInfo.m_AttrHash.Find(pStrControlName));
            if (!pDefaultAttr) return FALSE;

            delete pDefaultAttr;
            return m_ResInfo.m_AttrHash.Remove(pStrControlName);
        }
    }

    void CManagerUI::RemoveAllDefaultAttributeList(BOOL bShared)
    {
        if (bShared) {
            CStringUI* pDefaultAttr;
            for (int i = 0; i < CResourceUI::GetInstance()->GetSharedResInfo().m_AttrHash.GetSize(); i++) {
                if (LPCTSTR key = CResourceUI::GetInstance()->GetSharedResInfo().m_AttrHash.GetAt(i)) {
                    pDefaultAttr = static_cast<CStringUI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_AttrHash.Find(key));
                    if (pDefaultAttr) delete pDefaultAttr;
                }
            }
            CResourceUI::GetInstance()->GetSharedResInfo().m_AttrHash.RemoveAll();
        } else {
            CStringUI* pDefaultAttr;
            for (int i = 0; i < m_ResInfo.m_AttrHash.GetSize(); i++) {
                if (LPCTSTR key = m_ResInfo.m_AttrHash.GetAt(i)) {
                    pDefaultAttr = static_cast<CStringUI*>(m_ResInfo.m_AttrHash.Find(key));
                    if (pDefaultAttr) delete pDefaultAttr;
                }
            }
            m_ResInfo.m_AttrHash.RemoveAll();
        }
    }

    void CManagerUI::AddWindowCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr)
    {
        if (pstrName == NULL || pstrName[0] == _T('\0') || pstrAttr == NULL || pstrAttr[0] == _T('\0')) return;
        CStringUI * pCostomAttr = new CStringUI(pstrAttr);
        if (pCostomAttr != NULL) {
            if (m_mWindowCustomAttrHash.Find(pstrName) == NULL)
                m_mWindowCustomAttrHash.Set(pstrName, (LPVOID)pCostomAttr);
            else
                delete pCostomAttr;
        }
    }

    LPCTSTR CManagerUI::GetWindowCustomAttribute(LPCTSTR pstrName) const
    {
        if (pstrName == NULL || pstrName[0] == _T('\0')) return NULL;
        CStringUI * pCostomAttr = static_cast<CStringUI*>(m_mWindowCustomAttrHash.Find(pstrName));
        if (pCostomAttr) return pCostomAttr->GetData();
        return NULL;
    }

    BOOL CManagerUI::RemoveWindowCustomAttribute(LPCTSTR pstrName)
    {
        if (pstrName == NULL || pstrName[0] == _T('\0')) return NULL;
        CStringUI * pCostomAttr = static_cast<CStringUI*>(m_mWindowCustomAttrHash.Find(pstrName));
        if (!pCostomAttr) return FALSE;

        delete pCostomAttr;
        return m_mWindowCustomAttrHash.Remove(pstrName);
    }

    void CManagerUI::RemoveAllWindowCustomAttribute()
    {
        CStringUI* pCostomAttr;
        for (int i = 0; i < m_mWindowCustomAttrHash.GetSize(); i++) {
            if (LPCTSTR key = m_mWindowCustomAttrHash.GetAt(i)) {
                pCostomAttr = static_cast<CStringUI*>(m_mWindowCustomAttrHash.Find(key));
                delete pCostomAttr;
            }
        }
        m_mWindowCustomAttrHash.Resize();
    }

    CControlUI* CManagerUI::GetRoot() const
    {
        ASSERT(m_pRoot);
        return m_pRoot;
    }

    CControlUI* CManagerUI::FindControl(POINT pt) const
    {
        ASSERT(m_pRoot);
        return m_pRoot->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
    }

    CControlUI* CManagerUI::FindControl(LPCTSTR pstrName) const
    {
        ASSERT(m_pRoot);
        return static_cast<CControlUI*>(m_mNameHash.Find(pstrName));
    }

    CControlUI* CManagerUI::FindSubControlByPoint(CControlUI * pParent, POINT pt) const
    {
        if (pParent == NULL) pParent = GetRoot();
        ASSERT(pParent);
        return pParent->FindControl(__FindControlFromPoint, &pt, UIFIND_VISIBLE | UIFIND_HITTEST | UIFIND_TOP_FIRST);
    }

    CControlUI * CManagerUI::FindSubControlByName(CControlUI * pParent, LPCTSTR pstrName) const
    {
        if (pParent == NULL) pParent = GetRoot();
        ASSERT(pParent);
        return pParent->FindControl(__FindControlFromName, (LPVOID)pstrName, UIFIND_ALL);
    }

    CControlUI* CManagerUI::FindSubControlByClass(CControlUI * pParent, LPCTSTR pstrClass, int iIndex)
    {
        if (pParent == NULL) pParent = GetRoot();
        ASSERT(pParent);
        m_aFoundControls.Resize(iIndex + 1);
        return pParent->FindControl(__FindControlFromClass, (LPVOID)pstrClass, UIFIND_ALL);
    }

    CPtrArrayUI * CManagerUI::FindSubControlsByClass(CControlUI * pParent, LPCTSTR pstrClass)
    {
        if (pParent == NULL) pParent = GetRoot();
        ASSERT(pParent);
        m_aFoundControls.Empty();
        pParent->FindControl(__FindControlsFromClass, (LPVOID)pstrClass, UIFIND_ALL);
        return &m_aFoundControls;
    }

    CPtrArrayUI* CManagerUI::GetFoundControls()
    {
        return &m_aFoundControls;
    }

    CControlUI* CALLBACK CManagerUI::__FindControlFromNameHash(CControlUI * pThis, LPVOID pData)
    {
        CManagerUI* pManager = static_cast<CManagerUI*>(pData);
        const CStringUI& sName = pThis->GetName();
        if (sName.IsEmpty()) return NULL;
        // Add this control to the hash list
        pManager->m_mNameHash.Set(sName, pThis);
        return NULL; // Attempt to add all controls
    }

    CControlUI* CALLBACK CManagerUI::__FindControlFromCount(CControlUI* /*pThis*/, LPVOID pData)
    {
        int* pnCount = static_cast<int*>(pData);
        (*pnCount)++;
        return NULL; // Count all controls
    }

    CControlUI* CALLBACK CManagerUI::__FindControlFromPoint(CControlUI * pThis, LPVOID pData)
    {
        LPPOINT pPoint = static_cast<LPPOINT>(pData);
        return ::PtInRect(&pThis->GetPos(), *pPoint)?pThis:NULL;
    }

    CControlUI* CALLBACK CManagerUI::__FindControlFromTab(CControlUI * pThis, LPVOID pData)
    {
        FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
        if (pInfo->pFocus == pThis) {
            if (pInfo->bForward) pInfo->bNextIsIt = true;
            return pInfo->bForward?NULL:pInfo->pLast;
        }
        if ((pThis->GetControlFlags() & UIFLAG_TABSTOP) == 0) return NULL;
        pInfo->pLast = pThis;
        if (pInfo->bNextIsIt) return pThis;
        if (pInfo->pFocus == NULL) return pThis;
        return NULL; // Examine all controls
    }

    CControlUI * CALLBACK CManagerUI::__FindControlFromShortcut(CControlUI * pThis, LPVOID pData)
    {
        if (!pThis->IsVisible()) return NULL;
        FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(pData);
        if (pFS->ch == toupper(pThis->GetShortcut())) pFS->bPickNext = true;
        if (_tcsstr(pThis->GetClass(), _T("LabelUI")) != NULL) return NULL; // Labels never get focus!
        return pFS->bPickNext?pThis:NULL;
    }

    CControlUI * CALLBACK CManagerUI::__FindControlFromName(CControlUI * pThis, LPVOID pData)
    {
        LPCTSTR pstrName = static_cast<LPCTSTR>(pData);
        const CStringUI& sName = pThis->GetName();
        if (sName.IsEmpty()) return NULL;
        return (_tcsicmp(sName, pstrName) == 0)?pThis:NULL;
    }

    CControlUI* CALLBACK CManagerUI::__FindControlFromClass(CControlUI * pThis, LPVOID pData)
    {
        LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
        LPCTSTR pType = pThis->GetClass();
        CPtrArrayUI* pFoundControls = pThis->GetManager()->GetFoundControls();
        if (_tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0) {
            int iIndex = -1;
            while (pFoundControls->GetAt(++iIndex) != NULL)
                ;
            if (iIndex < pFoundControls->GetSize()) pFoundControls->SetAt(iIndex, pThis);
        }
        if (pFoundControls->GetAt(pFoundControls->GetSize() - 1) != NULL) return pThis;
        return NULL;
    }

    CControlUI * CALLBACK CManagerUI::__FindControlsFromClass(CControlUI * pThis, LPVOID pData)
    {
        LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
        LPCTSTR pType = pThis->GetClass();
        if (_tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0)
            pThis->GetManager()->GetFoundControls()->Add((LPVOID)pThis);
        return NULL;
    }

    CControlUI* CALLBACK CManagerUI::__FindControlsFromUpdate(CControlUI * pThis, LPVOID pData)
    {
        if (pThis->IsUpdateNeeded()) {
            pThis->GetManager()->GetFoundControls()->Add((LPVOID)pThis);
            return pThis;
        }
        return NULL;
    }

    BOOL CManagerUI::TranslateAccelerator(LPMSG pMsg)
    {
        for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++) {
            LRESULT lResult = static_cast<ITranslateAcceleratorUI*>(m_aTranslateAccelerator[i])->TranslateAccelerator(pMsg);
            if (lResult == S_OK) return true;
        }
        return FALSE;
    }

    BOOL CManagerUI::TranslateMessage(const LPMSG pMsg)
    {
        // Pretranslate Message takes care of system-wide messages, such as
        // tabbing and shortcut key-combos. We'll look for all messages for
        // each window and any child control attached.
        UINT uStyle = GetWindowStyle(pMsg->hwnd);
        UINT uChildRes = uStyle & WS_CHILD;
        LRESULT lRes = 0;
        if (uChildRes != 0) {
            HWND hWndParent = ::GetParent(pMsg->hwnd);

            for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
                CManagerUI* pT = static_cast<CManagerUI*>(m_aPreMessages[i]);
                HWND hTempParent = hWndParent;
                while (hTempParent) {
                    if (pMsg->hwnd == pT->GetPaintWindow() || hTempParent == pT->GetPaintWindow()) {
                        if (pT->TranslateAccelerator(pMsg)) {
                            return true;
                        }

                        pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes);
                    }
                    hTempParent = GetParent(hTempParent);
                }
            }
        } else {
            for (int i = 0; i < m_aPreMessages.GetSize(); i++) {
                CManagerUI* pT = static_cast<CManagerUI*>(m_aPreMessages[i]);
                if (pMsg->hwnd == pT->GetPaintWindow()) {
                    if (pT->TranslateAccelerator(pMsg))
                        return true;

                    if (pT->PreMessageHandler(pMsg->message, pMsg->wParam, pMsg->lParam, lRes))
                        return true;

                    return FALSE;
                }
            }
        }
        return FALSE;
    }

    BOOL CManagerUI::AddTranslateAccelerator(ITranslateAcceleratorUI * pTranslateAccelerator)
    {
        ASSERT(m_aTranslateAccelerator.Find(pTranslateAccelerator) < 0);
        return m_aTranslateAccelerator.Add(pTranslateAccelerator);
    }

    BOOL CManagerUI::RemoveTranslateAccelerator(ITranslateAcceleratorUI * pTranslateAccelerator)
    {
        for (int i = 0; i < m_aTranslateAccelerator.GetSize(); i++) {
            if (static_cast<ITranslateAcceleratorUI*>(m_aTranslateAccelerator[i]) == pTranslateAccelerator) {
                return m_aTranslateAccelerator.Remove(i);
            }
        }
        return FALSE;
    }

    void CManagerUI::UsedVirtualWnd(BOOL bUsed)
    {
        m_bUsedVirtualWnd = bUsed;
    }

    // 样式管理
    void CManagerUI::AddStyle(LPCTSTR pName, LPCTSTR pDeclarationList, BOOL bShared)
    {
        CStringUI* pStyle = new CStringUI(pDeclarationList);

        if (bShared || m_bForceUseSharedRes) {
            if (!CResourceUI::GetInstance()->GetSharedResInfo().m_StyleHash.Insert(pName, pStyle)) {
                delete pStyle;
            }
        } else {
            if (!m_ResInfo.m_StyleHash.Insert(pName, pStyle)) {
                delete pStyle;
            }
        }
    }

    LPCTSTR CManagerUI::GetStyle(LPCTSTR pName) const
    {
        CStringUI* pStyle = static_cast<CStringUI*>(m_ResInfo.m_StyleHash.Find(pName));
        if (!pStyle) pStyle = static_cast<CStringUI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_StyleHash.Find(pName));
        if (pStyle)
            return pStyle->GetData();
        else
            return NULL;
    }

    BOOL CManagerUI::RemoveStyle(LPCTSTR pName, BOOL bShared)
    {
        CStringUI* pStyle = NULL;
        if (bShared) {
            pStyle = static_cast<CStringUI*>(CResourceUI::GetInstance()->GetSharedResInfo().m_StyleHash.Find(pName));
            if (pStyle) {
                delete pStyle;
                CResourceUI::GetInstance()->GetSharedResInfo().m_StyleHash.Remove(pName);
            }
        } else {
            pStyle = static_cast<CStringUI*>(m_ResInfo.m_StyleHash.Find(pName));
            if (pStyle) {
                delete pStyle;
                m_ResInfo.m_StyleHash.Remove(pName);
            }
        }
        return true;
    }

    const CStringMapUI& CManagerUI::GetStyles(BOOL bShared) const
    {
        if (bShared) {
            return CResourceUI::GetInstance()->GetSharedResInfo().m_StyleHash;
        } else {
            return m_ResInfo.m_StyleHash;
        }
    }

    CStringMapUI& CManagerUI::GetStyles(BOOL bShared)
    {
        if (bShared) {
            return CResourceUI::GetInstance()->GetSharedResInfo().m_StyleHash;
        } else {
            return m_ResInfo.m_StyleHash;
        }
    }


    void CManagerUI::RemoveAllStyle(BOOL bShared)
    {
        CStringMapUI* pStyles = NULL;
        if (bShared) {
            CStringUI* pStyle;
            pStyles = &(CResourceUI::GetInstance()->GetSharedResInfo().m_StyleHash);
        } else {
            pStyles = &(m_ResInfo.m_StyleHash);
        }

        for (int i = 0; i < pStyles->GetSize(); i++) {
            if (LPCTSTR key = pStyles->GetAt(i)) {
                CStringUI* pStyleItem = static_cast<CStringUI*>(pStyles->Find(key));
                delete pStyleItem;
            }
        }
        pStyles->RemoveAll();
    }

    const TIMAGEINFO_UI* CManagerUI::GetImageString(LPCTSTR pStrImage, LPCTSTR pStrModify) const
    {
        CStringUI sImageName = pStrImage;
        CStringUI sImageResType = _T("");
        DWORD dwMask = 0;
        CStringUI sItem;
        CStringUI sValue;
        LPTSTR pstr = NULL;

        for (int i = 0; i < 2; ++i) {
            if (i == 1)
                pStrImage = pStrModify;

            if (!pStrImage) continue;

            while (*pStrImage != _T('\0')) {
                sItem.Empty();
                sValue.Empty();
                while (*pStrImage > _T('\0') && *pStrImage <= _T(' '))
                    pStrImage = ::CharNext(pStrImage);
                while (*pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ')) {
                    LPTSTR pstrTemp = ::CharNext(pStrImage);
                    while (pStrImage < pstrTemp) {
                        sItem += *pStrImage++;
                    }
                }
                while (*pStrImage > _T('\0') && *pStrImage <= _T(' '))
                    pStrImage = ::CharNext(pStrImage);
                if (*pStrImage++ != _T('=')) break;
                while (*pStrImage > _T('\0') && *pStrImage <= _T(' '))
                    pStrImage = ::CharNext(pStrImage);
                if (*pStrImage++ != _T('\'')) break;
                while (*pStrImage != _T('\0') && *pStrImage != _T('\'')) {
                    LPTSTR pstrTemp = ::CharNext(pStrImage);
                    while (pStrImage < pstrTemp) {
                        sValue += *pStrImage++;
                    }
                }
                if (*pStrImage++ != _T('\'')) break;
                if (!sValue.IsEmpty()) {
                    if (sItem == _T("file") || sItem == _T("res")) {
                        sImageName = sValue;
                    } else if (sItem == _T("restype")) {
                        sImageResType = sValue;
                    } else if (sItem == _T("mask")) {
                        if (sValue[0] == _T('#'))
                            dwMask = _tcstoul(sValue.GetData() + 1, &pstr, 16);
                        else
                            dwMask = _tcstoul(sValue.GetData(), &pstr, 16);
                    }
                }
                if (*pStrImage++ != _T(' ')) break;
            }
        }
        return GetImageEx(sImageName, sImageResType, dwMask);
    }

    BOOL CManagerUI::InitDragDrop()
    {
        AddRef();

        if (FAILED(RegisterDragDrop(m_hWndPaint, this))) //calls addref
        {
            DWORD dwError = GetLastError();
            return FALSE;
        } else
            Release(); //i decided to AddRef explicitly after new

        FORMATETC ftetc = {0};
        ftetc.cfFormat = CF_BITMAP;
        ftetc.dwAspect = DVASPECT_CONTENT;
        ftetc.lindex = -1;
        ftetc.tymed = TYMED_GDI;
        AddSuportedFormat(ftetc);
        ftetc.cfFormat = CF_DIB;
        ftetc.tymed = TYMED_HGLOBAL;
        AddSuportedFormat(ftetc);
        ftetc.cfFormat = CF_HDROP;
        ftetc.tymed = TYMED_HGLOBAL;
        AddSuportedFormat(ftetc);
        ftetc.cfFormat = CF_ENHMETAFILE;
        ftetc.tymed = TYMED_ENHMF;
        AddSuportedFormat(ftetc);
        return true;
    }
    static WORD DIBNumColors(void* pv)
    {
        int bits;
        LPBITMAPINFOHEADER lpbi;
        LPBITMAPCOREHEADER lpbc;
        lpbi = ((LPBITMAPINFOHEADER)pv);
        lpbc = ((LPBITMAPCOREHEADER)pv);
        /*  With the BITMAPINFO format headers, the size of the palette
            *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
            *  is dependent on the bits per pixel ( = 2 raised to the power of
            *  bits/pixel).
            */
        if (lpbi->biSize != sizeof(BITMAPCOREHEADER)) {
            if (lpbi->biClrUsed != 0)
                return (WORD)lpbi->biClrUsed;
            bits = lpbi->biBitCount;
        } else
            bits = lpbc->bcBitCount;
        switch (bits) {
            case 1:
                return 2;
            case 4:
                return 16;
            case 8:
                return 256;
            default:
                /* A 24 bitcount DIB has no color table */
                return 0;
        }
    }
    //code taken from SEEDIB MSDN sample
    static WORD ColorTableSize(LPVOID lpv)
    {
        LPBITMAPINFOHEADER lpbih = (LPBITMAPINFOHEADER)lpv;

        if (lpbih->biSize != sizeof(BITMAPCOREHEADER)) {
            if (((LPBITMAPINFOHEADER)(lpbih))->biCompression == BI_BITFIELDS)
                /* Remember that 16/32bpp dibs can still have a color table */
                return (sizeof(DWORD) * 3) + (DIBNumColors(lpbih) * sizeof(RGBQUAD));
            else
                return (WORD)(DIBNumColors(lpbih) * sizeof(RGBQUAD));
        } else
            return (WORD)(DIBNumColors(lpbih) * sizeof(RGBTRIPLE));
    }

    BOOL CManagerUI::OnDrop(FORMATETC * pFmtEtc, STGMEDIUM & medium, DWORD * pdwEffect)
    {
        POINT ptMouse = {0};
        GetCursorPos(&ptMouse);
        ::SendMessage(m_hTargetWnd, WM_LBUTTONUP, NULL, MAKELPARAM(ptMouse.x, ptMouse.y));

        if (pFmtEtc->cfFormat == CF_DIB && medium.tymed == TYMED_HGLOBAL) {
            if (medium.hGlobal != NULL) {
                LPBITMAPINFOHEADER lpbi = (BITMAPINFOHEADER*)GlobalLock(medium.hGlobal);
                if (lpbi != NULL) {
                    HBITMAP hbm = NULL;
                    HDC hdc = GetDC(NULL);
                    if (hdc != NULL) {
                        int i = ((BITMAPFILEHEADER*)lpbi)->bfOffBits;
                        hbm = CreateDIBitmap(hdc, (LPBITMAPINFOHEADER)lpbi,
                            (LONG)CBM_INIT,
                            (LPSTR)lpbi + lpbi->biSize + ColorTableSize(lpbi),
                            (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);

                        ::ReleaseDC(NULL, hdc);
                    }
                    GlobalUnlock(medium.hGlobal);
                    if (hbm != NULL)
                        hbm = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm);
                    if (hbm != NULL)
                        DeleteObject(hbm);
                    return true; //release the medium
                }
            }
        }
        if (pFmtEtc->cfFormat == CF_BITMAP && medium.tymed == TYMED_GDI) {
            if (medium.hBitmap != NULL) {
                HBITMAP hBmp = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)medium.hBitmap);
                if (hBmp != NULL)
                    DeleteObject(hBmp);
                return FALSE; //don't free the bitmap
            }
        }
        if (pFmtEtc->cfFormat == CF_ENHMETAFILE && medium.tymed == TYMED_ENHMF) {
            ENHMETAHEADER emh;
            GetEnhMetaFileHeader(medium.hEnhMetaFile, sizeof(ENHMETAHEADER), &emh);
            RECT rc; //={0,0,EnhMetaHdr.rclBounds.right-EnhMetaHdr.rclBounds.left, EnhMetaHdr.rclBounds.bottom-EnhMetaHdr.rclBounds.top};
            HDC hDC = GetDC(m_hTargetWnd);
            //start code: taken from ENHMETA.EXE MSDN Sample
            //*ALSO NEED to GET the pallete (select and RealizePalette it, but i was too lazy*
            // Get the characteristics of the output device
            float PixelsX = (float)GetDeviceCaps(hDC, HORZRES);
            float PixelsY = (float)GetDeviceCaps(hDC, VERTRES);
            float MMX = (float)GetDeviceCaps(hDC, HORZSIZE);
            float MMY = (float)GetDeviceCaps(hDC, VERTSIZE);
            // Calculate the rect in which to draw the metafile based on the
            // intended size and the current output device resolution
            // Remember that the intended size is given in 0.01mm units, so
            // convert those to device units on the target device
            rc.top = (int)((float)(emh.rclFrame.top) * PixelsY / (MMY * 100.0f));
            rc.left = (int)((float)(emh.rclFrame.left) * PixelsX / (MMX * 100.0f));
            rc.right = (int)((float)(emh.rclFrame.right) * PixelsX / (MMX * 100.0f));
            rc.bottom = (int)((float)(emh.rclFrame.bottom) * PixelsY / (MMY * 100.0f));
            //end code: taken from ENHMETA.EXE MSDN Sample

            HDC hdcMem = CreateCompatibleDC(hDC);
            HGDIOBJ hBmpMem = CreateCompatibleBitmap(hDC, emh.rclBounds.right, emh.rclBounds.bottom);
            HGDIOBJ hOldBmp = ::SelectObject(hdcMem, hBmpMem);
            PlayEnhMetaFile(hdcMem, medium.hEnhMetaFile, &rc);
            HBITMAP hBmp = (HBITMAP)::SelectObject(hdcMem, hOldBmp);
            DeleteDC(hdcMem);
            ReleaseDC(m_hTargetWnd, hDC);
            hBmp = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmp);
            if (hBmp != NULL)
                DeleteObject(hBmp);
            return true;
        }
        if (pFmtEtc->cfFormat == CF_HDROP && medium.tymed == TYMED_HGLOBAL) {
            HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
            if (hDrop != NULL) {
                TCHAR szFileName[MAX_PATH];
                UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
                if (cFiles > 0) {
                    DragQueryFile(hDrop, 0, szFileName, sizeof(szFileName));
                    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
                    if (hBitmap) {
                        HBITMAP hBmp = (HBITMAP)SendMessage(m_hTargetWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
                        if (hBmp != NULL)
                            DeleteObject(hBmp);
                    }
                }
                //DragFinish(hDrop); // base class calls ReleaseStgMedium
            }
            GlobalUnlock(medium.hGlobal);
        }
        return true; //let base free the medium
    }




} // namespace DUI
