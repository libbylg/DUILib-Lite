#ifndef __UIRESOURCEMANAGER_H__
#define __UIRESOURCEMANAGER_H__

#include "Utils/Utils.h"
#include "Core/UIMarkup.h"

namespace DUI
{
    // �ؼ����ֲ�ѯ�ӿ�
    class DUILIB_API IQueryControlTextUI
    {
    public:
        virtual LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType) = 0;
    };

    class DUILIB_API CResourceManagerUI
    {
    private:
        CResourceManagerUI(void);
        ~CResourceManagerUI(void);

    public:
        static CResourceManagerUI* GetInstance()
        {
            static CResourceManagerUI* p = new CResourceManagerUI;
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
        CStdStringPtrMap m_mTextResourceHashMap;
        IQueryControlTextUI* m_pQuerypInterface;
        CStdStringPtrMap m_mImageHashMap;
        CStdStringPtrMap m_mXmlHashMap;
        CMarkupUI m_xml;
        CStringUI m_sLauguage;
        CStdStringPtrMap m_mTextHashMap;
    };

} // namespace DUI

#endif // __UIRESOURCEMANAGER_H__