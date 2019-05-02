#ifndef __UIRENDER_H__
#define __UIRENDER_H__

#include "Core/UIDefine.h"
#include "Utils/Utils.h"

#ifdef USE_XIMAGE_EFFECT
class CxImage;
#endif
namespace DUI
{
    /////////////////////////////////////////////////////////////////////////////////////
    //

    class CManagerUI;
    struct TIMAGEINFO_UI;

    class DUILIB_API CRenderClipUI
    {
    public:
        ~CRenderClipUI();
        RECT rcItem;
        HDC hDC;
        HRGN hRgn;
        HRGN hOldRgn;

        static void GenerateClip(HDC hDC, RECT rc, CRenderClipUI& clip);
        static void GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClipUI& clip);
        static void UseOldClipBegin(HDC hDC, CRenderClipUI& clip);
        static void UseOldClipEnd(HDC hDC, CRenderClipUI& clip);
    };

    class CDPI;
    typedef struct DUILIB_API TDRAWINFO_UI
    {
        TDRAWINFO_UI();
        void Parse(LPCTSTR pStrImage, LPCTSTR pStrModify, CDPI* pDPI);
        void Clear();

        CStringUI sDrawString;
        CStringUI sDrawModify;
        CStringUI sImageName;
        CStringUI sResType;
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
        CStringUI sIconAlign;
    } TDRAWINFO_UI;


    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CRenderUI
    {
    public:
        static DWORD AdjustColor(DWORD dwColor, short H, short S, short L);
        static HBITMAP CreateARGB32Bitmap(HDC hDC, int cx, int cy, BYTE** pBits);
        static void AdjustImage(BOOL bUseHSL, TIMAGEINFO_UI* imageInfo, short H, short S, short L);
        static TIMAGEINFO_UI* LoadImage(TSTRID_UI bitmap, LPCTSTR type = NULL, DWORD mask = 0, HINSTANCE instance = NULL);
#ifdef USE_XIMAGE_EFFECT
        static CxImage* LoadGifImageX(STRINGorID bitmap, LPCTSTR type = NULL, DWORD mask = 0);
#endif
        static void FreeImage(TIMAGEINFO_UI* bitmap, BOOL bDelete = TRUE);
        static TIMAGEINFO_UI* LoadImage(LPCTSTR pStrImage, LPCTSTR type = NULL, DWORD mask = 0, HINSTANCE instance = NULL);
        static TIMAGEINFO_UI* LoadImage(UINT nID, LPCTSTR type = NULL, DWORD mask = 0, HINSTANCE instance = NULL);

        static Gdiplus::Image* GdiplusLoadImage(LPCTSTR pstrPath);
        static Gdiplus::Image* GdiplusLoadImage(LPVOID pBuf, size_t dwSize);

        static BOOL DrawIconImageString(HDC hDC, CManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, \
            LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);
        static BOOL MakeFitIconDest(const RECT& rcControl, const CSizeUI& szIcon, const CStringUI& sAlign, RECT& rcDest);

        static void DrawText(HDC hDC, CManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, \
            int iFont, UINT uStyle, DWORD dwTextBKColor);

        static void DrawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint, \
            const RECT& rcBmpPart, const RECT& rcCorners, BOOL bAlpha, BYTE uFade = 255,
            BOOL hole = false, BOOL xtiled = false, BOOL ytiled = false);

        static BOOL DrawImageInfo(HDC hDC, CManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, const TDRAWINFO_UI* pDrawInfo, HINSTANCE instance = NULL);
        static BOOL DrawImageString(HDC hDC, CManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, LPCTSTR pStrImage, LPCTSTR pStrModify = NULL, HINSTANCE instance = NULL);

        static void DrawColor(HDC hDC, const RECT& rc, DWORD color);
        static void DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, BOOL bVertical, int nSteps);

        // 以下函数中的颜色参数alpha值无效
        static void DrawLine(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor, int nStyle = PS_SOLID);
        static void DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor, int nStyle = PS_SOLID);
        static void DrawRoundRect(HDC hDC, const RECT& rc, int width, int height, int nSize, DWORD dwPenColor, int nStyle = PS_SOLID);
        static void DrawText(HDC hDC, CManagerUI* pManager, RECT& rc, LPCTSTR pstrText, \
            DWORD dwTextColor, int iFont, UINT uStyle);
        static void DrawHtmlText(HDC hDC, CManagerUI* pManager, RECT& rc, LPCTSTR pstrText,
            DWORD dwTextColor, RECT* pLinks, CStringUI* sLinks, int& nLinkRects, int iFont, UINT uStyle);
        static HBITMAP GenerateBitmap(CManagerUI* pManager, RECT rc, CControlUI* pStopControl = NULL, DWORD dwFilterColor = 0);
        static HBITMAP GenerateBitmap(CManagerUI* pManager, CControlUI* pControl, RECT rc, DWORD dwFilterColor = 0);
        static SIZE GetTextSize(HDC hDC, CManagerUI* pManager, LPCTSTR pstrText, int iFont, UINT uStyle);

        //alpha utilities
        static void CheckAlphaColor(DWORD& dwColor);
    };

} // namespace DUI

#endif // __UIRENDER_H__
