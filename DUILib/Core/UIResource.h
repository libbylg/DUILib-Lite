#ifndef __UIRESOURCE_H__
#define __UIRESOURCE_H__

#include "Core/UIDefine.h"
#include "Core/UIMarkup.h"

namespace DUI
{

    struct DUILIB_API TFONTINFO_UI
    {
        HFONT       hFont;
        CStringUI   sFontName;
        int         iSize;
        BOOL        bBold;
        BOOL        bUnderline;
        BOOL        bItalic;
        TEXTMETRIC  tm;
    };

    struct DUILIB_API TIMAGEINFO_UI
    {
        HBITMAP     hBitmap;
        LPBYTE      pBits;
        LPBYTE      pSrcBits;
        int         nX;
        int         nY;
        BOOL        bAlpha;
        BOOL        bUseHSL;
        CStringUI   sResType;
        DWORD       dwMask;

    };

    struct DUILIB_API TRESINFO_UI
    {
        DWORD           m_dwDefaultDisabledColor;
        DWORD           m_dwDefaultFontColor;
        DWORD           m_dwDefaultLinkFontColor;
        DWORD           m_dwDefaultLinkHoverFontColor;
        DWORD           m_dwDefaultSelectedBkColor;
        TFONTINFO_UI    m_DefaultFontInfo;
        CStringMapUI    m_CustomFonts;
        CStringMapUI    m_ImageHash;
        CStringMapUI    m_AttrHash;
        CStringMapUI    m_StyleHash;
        CStringMapUI    m_DrawInfoHash;
    };


    // 控件文字查询接口
    struct DUILIB_API IQueryControlTextUI
    {
        virtual LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType) = 0;
    };

    //typedef BOOL(*PHANDLERESOURCE_UI)(void* ctx, BYTE* pByte, DWORD dwSize);

    class DUILIB_API CResourceUI
    {
    private:
        CResourceUI(void);
        ~CResourceUI(void);

    public:
        //static BOOL LoadResource(LPCTSTR pstrFilename, void* ctx, PHANDLERESOURCE_UI handle);
        static CResourceUI* GetInstance();
        void Release(void);

    public:
        BOOL LoadResource(TSTRID_UI xml, LPCTSTR type = NULL);
        BOOL LoadResource(CMarkupNodeUI Root);
        
        void    ResetResourceMap();
        LPCTSTR GetImagePath(LPCTSTR lpstrId);
        LPCTSTR GetXmlPath(LPCTSTR lpstrId);

    public:
        void    SetLanguage(LPCTSTR pstrLanguage) { m_sLauguage = pstrLanguage; }
        LPCTSTR GetLanguage() { return m_sLauguage; }
        BOOL    LoadLanguage(LPCTSTR pstrXml);

    public:
        void        SetTextQueryInterface(IQueryControlTextUI* pInterface) { m_pQuerypInterface = pInterface; }
        CStringUI   GetText(LPCTSTR lpstrId, LPCTSTR lpstrType = NULL);
        void        ReloadText();
        void        ResetTextMap();

        HINSTANCE           GetResourceDll();
        const CStringUI&    GetResourcePath();
        const CStringUI&    GetResourceZip();
        const CStringUI&    GetResourceZipPwd();
        BOOL                IsCachedResourceZip();
        HANDLE              GetResourceZipHandle();
        //void                SetInstance(HINSTANCE hInst);
        //void                SetCurrentPath(LPCTSTR pStrPath);
        void                SetResourceDll(HINSTANCE hInst);
        void                SetResourcePath(LPCTSTR pStrPath);
        void                SetResourceZip(LPVOID pVoid, unsigned int len, LPCTSTR password = NULL);
        void                SetResourceZip(LPCTSTR pstrZip, BOOL bCachedResourceZip = FALSE, LPCTSTR password = NULL);
        void                SetResourceType(RESTYPE_UI nType);
        RESTYPE_UI          GetResourceType();

        const TRESINFO_UI&  GetSharedResInfo()const;
        TRESINFO_UI&        GetSharedResInfo();

    private:
        CStringMapUI            m_mTextResourceHashMap;
        IQueryControlTextUI*    m_pQuerypInterface;
        CStringMapUI            m_mImageHashMap;
        CStringMapUI            m_mXmlHashMap;
        CMarkupUI               m_xml;
        CStringUI               m_sLauguage;
        CStringMapUI            m_mTextHashMap;

        //  资源类
        HINSTANCE               m_hResourceInstance;
        CStringUI               m_pStrResourcePath;
        CStringUI               m_pStrResourceZip;
        CStringUI               m_pStrResourceZipPwd;
        HANDLE                  m_hResourceZip;
        BOOL                    m_bCachedResourceZip;
        RESTYPE_UI              m_nResType;
        TRESINFO_UI             m_SharedResInfo;
    };

} // namespace DUI

#endif // __UIRESOURCE_H__