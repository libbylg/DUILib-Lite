#ifndef __UIRESOURCE_H__
#define __UIRESOURCE_H__

#include "Core/UIDefine.h"
#include "Core/UIMarkup.h"

namespace DUI
{
    // 控件文字查询接口
    class DUILIB_API IQueryControlTextUI
    {
    public:
        virtual LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType) = 0;
    };

    class DUILIB_API CResourceUI
    {
    private:
        CResourceUI(void);
        ~CResourceUI(void);

    public:
        static CResourceUI* GetInstance()
        {
            static CResourceUI* p = new CResourceUI;
            return p;
        };
        void Release(void) { delete this; }

    public:
        BOOL LoadResource(TSTRID_UI xml, LPCTSTR type = NULL);
        BOOL LoadResource(CMarkupNodeUI Root);
        void ResetResourceMap();
        LPCTSTR GetImagePath(LPCTSTR lpstrId);
        LPCTSTR GetXmlPath(LPCTSTR lpstrId);

    public:
        void SetLanguage(LPCTSTR pstrLanguage) { m_sLauguage = pstrLanguage; }
        LPCTSTR GetLanguage() { return m_sLauguage; }
        BOOL LoadLanguage(LPCTSTR pstrXml);

    public:
        void SetTextQueryInterface(IQueryControlTextUI* pInterface) { m_pQuerypInterface = pInterface; }
        CStringUI GetText(LPCTSTR lpstrId, LPCTSTR lpstrType = NULL);
        void ReloadText();
        void ResetTextMap();

    private:
        CStringMapUI m_mTextResourceHashMap;
        IQueryControlTextUI* m_pQuerypInterface;
        CStringMapUI m_mImageHashMap;
        CStringMapUI m_mXmlHashMap;
        CMarkupUI m_xml;
        CStringUI m_sLauguage;
        CStringMapUI m_mTextHashMap;
    };

} // namespace DUI

#endif // __UIRESOURCE_H__