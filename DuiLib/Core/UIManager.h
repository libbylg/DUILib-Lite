#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__

#include "Utils/Utils.h"
//#include "Core/DragDropImpl.h"
#include "Utils/DPI.h"

#include "Core/UIDefine.h"
#include "Core/UIShadow.h"

namespace DuiLib
{
    /////////////////////////////////////////////////////////////////////////////////////
    //

    class CControlUI;
    class CRichEditUI;
    class CIDropTarget;
    class CShadowUI;
    class CPaintManagerUI;
    class CDPI;


    /////////////////////////////////////////////////////////////////////////////////////
    //
    enum UILIB_RESTYPE
    {
        UILIB_FILE = 1,		// 来自磁盘文件
        UILIB_ZIP,			// 来自磁盘zip压缩包
        UILIB_RESOURCE,		// 来自资源
        UILIB_ZIPRESOURCE,	// 来自资源的zip压缩包
    };
    /////////////////////////////////////////////////////////////////////////////////////
    //

    typedef enum EVENTTYPE_UI
    {
        UIEVENT__FIRST = 1,
        UIEVENT__KEYBEGIN,
        UIEVENT_KEYDOWN,
        UIEVENT_KEYUP,
        UIEVENT_CHAR,
        UIEVENT_SYSKEY,
        UIEVENT__KEYEND,
        UIEVENT__MOUSEBEGIN,
        UIEVENT_MOUSEMOVE,
        UIEVENT_MOUSELEAVE,
        UIEVENT_MOUSEENTER,
        UIEVENT_MOUSEHOVER,
        UIEVENT_BUTTONDOWN,
        UIEVENT_BUTTONUP,
        UIEVENT_RBUTTONDOWN,
        UIEVENT_RBUTTONUP,
        UIEVENT_MBUTTONDOWN,
        UIEVENT_MBUTTONUP,
        UIEVENT_DBLCLICK,
        UIEVENT_CONTEXTMENU,
        UIEVENT_SCROLLWHEEL,
        UIEVENT__MOUSEEND,
        UIEVENT_KILLFOCUS,
        UIEVENT_SETFOCUS,
        UIEVENT_WINDOWSIZE,
        UIEVENT_SETCURSOR,
        UIEVENT_TIMER,
        UIEVENT__LAST,
    }EVENTTYPE_UI;


    /////////////////////////////////////////////////////////////////////////////////////
    //
    // 内部保留的消息
    typedef enum MSGTYPE_UI
    {
        UIMSG_TRAYICON = WM_USER + 1,// 托盘消息
        UIMSG_SET_DPI,				 // DPI
        WM_MENUCLICK,				 // 菜单消息
        UIMSG_USER = WM_USER + 100,	 // 程序自定义消息
    }MSGTYPE_UI;

    /////////////////////////////////////////////////////////////////////////////////////
    //

    // Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

    // Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_UPDATETEST    0x00000008
#define UIFIND_TOP_FIRST     0x00000010
#define UIFIND_ME_FIRST      0x80000000

    // Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040



    /////////////////////////////////////////////////////////////////////////////////////
    //

    struct DUILIB_API TFontInfo
    {
        HFONT hFont;
        CDuiString sFontName;
        int iSize;
        BOOL bBold;
        BOOL bUnderline;
        BOOL bItalic;
        TEXTMETRIC tm;
    };

    struct DUILIB_API TImageInfo
    {
        HBITMAP hBitmap;
        LPBYTE pBits;
        LPBYTE pSrcBits;
        int nX;
        int nY;
        BOOL bAlpha;
        BOOL bUseHSL;
        CDuiString sResType;
        DWORD dwMask;

    };

    typedef struct DUILIB_API TDrawInfo
    {
        TDrawInfo();
        void Parse(LPCTSTR pStrImage, LPCTSTR pStrModify, CPaintManagerUI* pManager);
        void Clear();

        CDuiString sDrawString;
        CDuiString sDrawModify;
        CDuiString sImageName;
        CDuiString sResType;
        RECT rcDest;
        RECT rcSource;
        RECT rcCorner;
        DWORD dwMask;
        BYTE uFade;
        BOOL bHole;
        BOOL bTiledX;
        BOOL bTiledY;
        BOOL bHSL;
        SIZE szIcon;
        CDuiString sIconAlign;
    } TDrawInfo;

    typedef struct DUILIB_API TPercentInfo
    {
        double left;
        double top;
        double right;
        double bottom;
    } TPercentInfo;

    struct DUILIB_API TResInfo
    {
        DWORD m_dwDefaultDisabledColor;
        DWORD m_dwDefaultFontColor;
        DWORD m_dwDefaultLinkFontColor;
        DWORD m_dwDefaultLinkHoverFontColor;
        DWORD m_dwDefaultSelectedBkColor;
        TFontInfo m_DefaultFontInfo;
        CStdStringPtrMap m_CustomFonts;
        CStdStringPtrMap m_ImageHash;
        CStdStringPtrMap m_AttrHash;
        CStdStringPtrMap m_StyleHash;
        CStdStringPtrMap m_DrawInfoHash;
    };

    // Structure for notifications from the system
    // to the control implementation.
    typedef struct DUILIB_API TEventUI
    {
        int Type;
        CControlUI* pSender;
        DWORD dwTimestamp;
        POINT ptMouse;
        TCHAR chKey;
        WORD wKeyState;
        WPARAM wParam;
        LPARAM lParam;
    } TEventUI;

    // Drag&Drop control
    const TCHAR* const CF_MOVECONTROL = _T("CF_MOVECONTROL");

    struct DUILIB_API TCFMoveUI
    {
        CControlUI* pControl;
    };

    // Listener interface
    class INotifyUI
    {
    public:
        virtual void Notify(TNotifyUI& msg) = 0;
    };

    // MessageFilter interface
    class IMessageFilterUI
    {
    public:
        virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) = 0;
    };

    class ITranslateAccelerator
    {
    public:
        virtual LRESULT TranslateAccelerator(MSG* pMsg) = 0;
    };


    /////////////////////////////////////////////////////////////////////////////////////
    //
    typedef CControlUI* (*LPCREATECONTROL)(LPCTSTR pstrType);

    class DUILIB_API CPaintManagerUI : public CIDropTarget
    {
    public:
        CPaintManagerUI();
        ~CPaintManagerUI();

    public:
        void Init(HWND hWnd, LPCTSTR pstrName = NULL);
        BOOL IsUpdateNeeded() const;
        void NeedUpdate();
        void Invalidate();
        void Invalidate(RECT& rcItem);

        LPCTSTR GetName() const;
        HDC GetPaintDC() const;
        HWND GetPaintWindow() const;
        HWND GetTooltipWindow() const;
        int GetHoverTime() const;
        void SetHoverTime(int iTime);

        POINT GetMousePos() const;
        SIZE GetClientSize() const;
        SIZE GetInitSize();
        void SetInitSize(int cx, int cy);
        RECT& GetSizeBox();
        void SetSizeBox(RECT& rcSizeBox);
        RECT& GetCaptionRect();
        void SetCaptionRect(RECT& rcCaption);
        SIZE GetRoundCorner() const;
        void SetRoundCorner(int cx, int cy);
        SIZE GetMinInfo() const;
        void SetMinInfo(int cx, int cy);
        SIZE GetMaxInfo() const;
        void SetMaxInfo(int cx, int cy);
        BOOL IsShowUpdateRect() const;
        void SetShowUpdateRect(BOOL show);
        BOOL IsNoActivate();
        void SetNoActivate(BOOL bNoActivate);

        BYTE GetOpacity() const;
        void SetOpacity(BYTE nOpacity);

        BOOL IsLayered();
        void SetLayered(BOOL bLayered);
        RECT& GetLayeredInset();
        void SetLayeredInset(RECT& rcLayeredInset);
        BYTE GetLayeredOpacity();
        void SetLayeredOpacity(BYTE nOpacity);
        LPCTSTR GetLayeredImage();
        void SetLayeredImage(LPCTSTR pstrImage);

        CShadowUI* GetShadow();

        void SetUseGdiplusText(BOOL bUse);
        BOOL IsUseGdiplusText() const;
        void SetGdiplusTextRenderingHint(int trh);
        int GetGdiplusTextRenderingHint() const;

        static HINSTANCE GetInstance();
        static CDuiString GetInstancePath();
        static CDuiString GetCurrentPath();
        static HINSTANCE GetResourceDll();
        static const CDuiString& GetResourcePath();
        static const CDuiString& GetResourceZip();
        static const CDuiString& GetResourceZipPwd();
        static BOOL IsCachedResourceZip();
        static HANDLE GetResourceZipHandle();
        static void SetInstance(HINSTANCE hInst);
        static void SetCurrentPath(LPCTSTR pStrPath);
        static void SetResourceDll(HINSTANCE hInst);
        static void SetResourcePath(LPCTSTR pStrPath);
        static void SetResourceZip(LPVOID pVoid, unsigned int len, LPCTSTR password = NULL);
        static void SetResourceZip(LPCTSTR pstrZip, BOOL bCachedResourceZip = false, LPCTSTR password = NULL);
        static void SetResourceType(int nType);
        static int GetResourceType();
        static BOOL GetHSL(short* H, short* S, short* L);
        static void SetHSL(BOOL bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200 
        static void ReloadSkin();
        static CPaintManagerUI* GetPaintManager(LPCTSTR pstrName);
        static CStdPtrArray* GetPaintManagers();
        static BOOL LoadPlugin(LPCTSTR pstrModuleName);
        static CStdPtrArray* GetPlugins();

        BOOL IsForceUseSharedRes() const;
        void SetForceUseSharedRes(BOOL bForce);
        // 注意：只支持简单类型指针，因为只释放内存，不会调用类对象的析构函数
        void DeletePtr(void* ptr);

        DWORD GetDefaultDisabledColor() const;
        void SetDefaultDisabledColor(DWORD dwColor, BOOL bShared = false);
        DWORD GetDefaultFontColor() const;
        void SetDefaultFontColor(DWORD dwColor, BOOL bShared = false);
        DWORD GetDefaultLinkFontColor() const;
        void SetDefaultLinkFontColor(DWORD dwColor, BOOL bShared = false);
        DWORD GetDefaultLinkHoverFontColor() const;
        void SetDefaultLinkHoverFontColor(DWORD dwColor, BOOL bShared = false);
        DWORD GetDefaultSelectedBkColor() const;
        void SetDefaultSelectedBkColor(DWORD dwColor, BOOL bShared = false);
        TFontInfo* GetDefaultFontInfo();
        void SetDefaultFont(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic, BOOL bShared = false);
        DWORD GetCustomFontCount(BOOL bShared = false) const;
        void AddFontArray(LPCTSTR pstrPath);
        HFONT AddFont(int id, LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic, BOOL bShared = false);
        HFONT GetFont(int id);
        HFONT GetFont(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic);
        int GetFontIndex(HFONT hFont, BOOL bShared = false);
        int GetFontIndex(LPCTSTR pStrFontName, int nSize, BOOL bBold, BOOL bUnderline, BOOL bItalic, BOOL bShared = false);
        void RemoveFont(HFONT hFont, BOOL bShared = false);
        void RemoveFont(int id, BOOL bShared = false);
        void RemoveAllFonts(BOOL bShared = false);
        TFontInfo* GetFontInfo(int id);
        TFontInfo* GetFontInfo(HFONT hFont);

        const TImageInfo* GetImage(LPCTSTR bitmap);
        const TImageInfo* GetImageEx(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0, BOOL bUseHSL = false, HINSTANCE instance = NULL);
        const TImageInfo* AddImage(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0, BOOL bUseHSL = false, BOOL bShared = false, HINSTANCE instance = NULL);
        const TImageInfo* AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, BOOL bAlpha, BOOL bShared = false);
        void RemoveImage(LPCTSTR bitmap, BOOL bShared = false);
        void RemoveAllImages(BOOL bShared = false);
        static void ReloadSharedImages();
        void ReloadImages();

        const TDrawInfo* GetDrawInfo(LPCTSTR pStrImage, LPCTSTR pStrModify);
        void RemoveDrawInfo(LPCTSTR pStrImage, LPCTSTR pStrModify);
        void RemoveAllDrawInfos();

        void AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList, BOOL bShared = false);
        LPCTSTR GetDefaultAttributeList(LPCTSTR pStrControlName) const;
        BOOL RemoveDefaultAttributeList(LPCTSTR pStrControlName, BOOL bShared = false);
        void RemoveAllDefaultAttributeList(BOOL bShared = false);

        void AddWindowCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr);
        LPCTSTR GetWindowCustomAttribute(LPCTSTR pstrName) const;
        BOOL RemoveWindowCustomAttribute(LPCTSTR pstrName);
        void RemoveAllWindowCustomAttribute();

        // 样式管理
        void AddStyle(LPCTSTR pName, LPCTSTR pStyle, BOOL bShared = false);
        LPCTSTR GetStyle(LPCTSTR pName) const;
        BOOL RemoveStyle(LPCTSTR pName, BOOL bShared = false);
        const CStdStringPtrMap& GetStyles(BOOL bShared = false) const;
        void RemoveAllStyle(BOOL bShared = false);

        const TImageInfo* GetImageString(LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);

        // 初始化拖拽
        BOOL InitDragDrop();
        virtual BOOL OnDrop(FORMATETC* pFmtEtc, STGMEDIUM& medium, DWORD* pdwEffect);

        BOOL AttachDialog(CControlUI* pControl);
        BOOL InitControls(CControlUI* pControl, CControlUI* pParent = NULL);
        void ReapObjects(CControlUI* pControl);

        BOOL AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
        CStdPtrArray* GetOptionGroup(LPCTSTR pStrGroupName);
        void RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
        void RemoveAllOptionGroups();

        CControlUI* GetFocus() const;
        void SetFocus(CControlUI* pControl);
        void SetFocusNeeded(CControlUI* pControl);

        BOOL SetNextTabControl(BOOL bForward = true);

        BOOL SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse);
        BOOL KillTimer(CControlUI* pControl, UINT nTimerID);
        void KillTimer(CControlUI* pControl);
        void RemoveAllTimers();

        void SetCapture();
        void ReleaseCapture();
        BOOL IsCaptured();

        BOOL IsPainting();
        void SetPainting(BOOL bIsPainting);

        BOOL AddNotifier(INotifyUI* pControl);
        BOOL RemoveNotifier(INotifyUI* pControl);
        void SendNotify(TNotifyUI& Msg, BOOL bAsync = false);
        void SendNotify(CControlUI* pControl, LPCTSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, BOOL bAsync = false);

        BOOL AddPreMessageFilter(IMessageFilterUI* pFilter);
        BOOL RemovePreMessageFilter(IMessageFilterUI* pFilter);

        BOOL AddMessageFilter(IMessageFilterUI* pFilter);
        BOOL RemoveMessageFilter(IMessageFilterUI* pFilter);

        int GetPostPaintCount() const;
        BOOL IsPostPaint(CControlUI* pControl);
        BOOL AddPostPaint(CControlUI* pControl);
        BOOL RemovePostPaint(CControlUI* pControl);
        BOOL SetPostPaintIndex(CControlUI* pControl, int iIndex);

        int GetNativeWindowCount() const;
        RECT GetNativeWindowRect(HWND hChildWnd);
        BOOL AddNativeWindow(CControlUI* pControl, HWND hChildWnd);
        BOOL RemoveNativeWindow(HWND hChildWnd);

        void AddDelayedCleanup(CControlUI* pControl);
        void AddMouseLeaveNeeded(CControlUI* pControl);
        BOOL RemoveMouseLeaveNeeded(CControlUI* pControl);

        BOOL AddTranslateAccelerator(ITranslateAccelerator* pTranslateAccelerator);
        BOOL RemoveTranslateAccelerator(ITranslateAccelerator* pTranslateAccelerator);
        BOOL TranslateAccelerator(LPMSG pMsg);

        CControlUI* GetRoot() const;
        CControlUI* FindControl(POINT pt) const;
        CControlUI* FindControl(LPCTSTR pstrName) const;
        CControlUI* FindSubControlByPoint(CControlUI* pParent, POINT pt) const;
        CControlUI* FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const;
        CControlUI* FindSubControlByClass(CControlUI* pParent, LPCTSTR pstrClass, int iIndex = 0);
        CStdPtrArray* FindSubControlsByClass(CControlUI* pParent, LPCTSTR pstrClass);

        static void MessageLoop();
        static BOOL TranslateMessage(const LPMSG pMsg);
        static void Term();

        CDPI* GetDPIObj();
        void ResetDPIAssets();
        void RebuildFont(TFontInfo* pFontInfo);
        void SetDPI(int iDPI);
        static void SetAllDPI(int iDPI);

        BOOL MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
        BOOL PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
        void UsedVirtualWnd(BOOL bUsed);

    private:
        CStdPtrArray* GetFoundControls();
        static CControlUI* CALLBACK __FindControlFromNameHash(CControlUI* pThis, LPVOID pData);
        static CControlUI* CALLBACK __FindControlFromCount(CControlUI* pThis, LPVOID pData);
        static CControlUI* CALLBACK __FindControlFromPoint(CControlUI* pThis, LPVOID pData);
        static CControlUI* CALLBACK __FindControlFromTab(CControlUI* pThis, LPVOID pData);
        static CControlUI* CALLBACK __FindControlFromShortcut(CControlUI* pThis, LPVOID pData);
        static CControlUI* CALLBACK __FindControlFromName(CControlUI* pThis, LPVOID pData);
        static CControlUI* CALLBACK __FindControlFromClass(CControlUI* pThis, LPVOID pData);
        static CControlUI* CALLBACK __FindControlsFromClass(CControlUI* pThis, LPVOID pData);
        static CControlUI* CALLBACK __FindControlsFromUpdate(CControlUI* pThis, LPVOID pData);

        static void AdjustSharedImagesHSL();
        void AdjustImagesHSL();
        void PostAsyncNotify();

    private:
        CDuiString m_sName;
        HWND m_hWndPaint;	//所附加的窗体的句柄
        HDC m_hDcPaint;
        HDC m_hDcOffscreen;
        HDC m_hDcBackground;
        HBITMAP m_hbmpOffscreen;
        BYTE* m_pOffscreenBits;
        HBITMAP m_hbmpBackground;
        COLORREF* m_pBackgroundBits;

        // 提示信息
        HWND m_hwndTooltip;
        TOOLINFO m_ToolTip;
        int m_iHoverTime;
        BOOL m_bNoActivate;
        BOOL m_bShowUpdateRect;

        //
        CControlUI* m_pRoot;
        CControlUI* m_pFocus;
        CControlUI* m_pEventHover;
        CControlUI* m_pEventClick;
        CControlUI* m_pEventKey;
        CControlUI* m_pLastToolTip;
        //
        POINT m_ptLastMousePos;
        SIZE m_szMinWindow;
        SIZE m_szMaxWindow;
        SIZE m_szInitWindowSize;
        RECT m_rcSizeBox;
        SIZE m_szRoundCorner;
        RECT m_rcCaption;
        UINT m_uTimerID;
        BOOL m_bFirstLayout;
        BOOL m_bUpdateNeeded;
        BOOL m_bFocusNeeded;
        BOOL m_bOffscreenPaint;

        BYTE m_nOpacity;
        BOOL m_bLayered;
        RECT m_rcLayeredInset;
        BOOL m_bLayeredChanged;
        RECT m_rcLayeredUpdate;
        TDrawInfo m_diLayered;

        BOOL m_bMouseTracking;
        BOOL m_bMouseCapture;
        BOOL m_bIsPainting;
        BOOL m_bUsedVirtualWnd;
        BOOL m_bAsyncNotifyPosted;

        //
        CStdPtrArray m_aNotifiers;
        CStdPtrArray m_aTimers;
        CStdPtrArray m_aTranslateAccelerator;
        CStdPtrArray m_aPreMessageFilters;
        CStdPtrArray m_aMessageFilters;
        CStdPtrArray m_aPostPaintControls;
        CStdPtrArray m_aNativeWindow;
        CStdPtrArray m_aNativeWindowControl;
        CStdPtrArray m_aDelayedCleanup;
        CStdPtrArray m_aAsyncNotify;
        CStdPtrArray m_aFoundControls;
        CStdPtrArray m_aFonts;
        CStdPtrArray m_aNeedMouseLeaveNeeded;
        CStdStringPtrMap m_mNameHash;
        CStdStringPtrMap m_mWindowCustomAttrHash;
        CStdStringPtrMap m_mOptionGroup;

        BOOL m_bForceUseSharedRes;
        TResInfo m_ResInfo;

        // 窗口阴影
        CShadowUI m_shadow;

        // DPI管理器
        CDPI* m_pDPI;
        // 是否开启Gdiplus
        BOOL m_bUseGdiplusText;
        int m_trh;
        ULONG_PTR m_gdiplusToken;
        Gdiplus::GdiplusStartupInput* m_pGdiplusStartupInput;

        // 拖拽
        BOOL m_bDragMode;
        HBITMAP m_hDragBitmap;

        //
        static HINSTANCE m_hInstance;
        static HINSTANCE m_hResourceInstance;
        static CDuiString m_pStrResourcePath;
        static CDuiString m_pStrResourceZip;
        static CDuiString m_pStrResourceZipPwd;
        static HANDLE m_hResourceZip;
        static BOOL m_bCachedResourceZip;
        static int m_nResType;
        static TResInfo m_SharedResInfo;
        static BOOL m_bUseHSL;
        static short m_H;
        static short m_S;
        static short m_L;
        static CStdPtrArray m_aPreMessages;
        static CStdPtrArray m_aPlugins;
    };

} // namespace DuiLib

#endif // __UIMANAGER_H__
