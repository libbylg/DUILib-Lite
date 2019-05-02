#include "Core/UIResourceManager.h"
#include "Core/UIManager.h"
#include "Utils/Utils.h"
#include "Utils/Utils.h"

namespace DUI
{

    CResourceManagerUI::CResourceManagerUI(void)
    {
        m_pQuerypInterface = NULL;

    }

    CResourceManagerUI::~CResourceManagerUI(void)
    {
        ResetResourceMap();
    }

    BOOL CResourceManagerUI::LoadResource(TSTRID_UI xml, LPCTSTR type)
    {
        if (HIWORD(xml.m_lpstr) != NULL) {
            if (*(xml.m_lpstr) == _T('<')) {
                if (!m_xml.Load(xml.m_lpstr)) return NULL;
            } else {
                if (!m_xml.LoadFromFile(xml.m_lpstr)) return NULL;
            }
        } else {
            HRSRC hResource = ::FindResource(CManagerUI::GetResourceDll(), xml.m_lpstr, type);
            if (hResource == NULL) return NULL;
            HGLOBAL hGlobal = ::LoadResource(CManagerUI::GetResourceDll(), hResource);
            if (hGlobal == NULL) {
                FreeResource(hResource);
                return NULL;
            }

            if (!m_xml.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(CManagerUI::GetResourceDll(), hResource))) {
                return NULL;
            }
            ::FreeResource(hResource);
        }

        return LoadResource(m_xml.GetRoot());
    }

    BOOL CResourceManagerUI::LoadResource(CMarkupNodeUI Root)
    {
        if (!Root.IsValid()) return FALSE;

        LPCTSTR pstrClass = NULL;
        int nAttributes = 0;
        LPCTSTR pstrName = NULL;
        LPCTSTR pstrValue = NULL;
        LPTSTR pstr = NULL;

        //加载图片资源
        LPCTSTR pstrId = NULL;
        LPCTSTR pstrPath = NULL;
        for (CMarkupNodeUI node = Root.GetChild(); node.IsValid(); node = node.GetSibling()) {
            pstrClass = node.GetName();
            CMarkupNodeUI ChildNode = node.GetChild();
            if (ChildNode.IsValid()) LoadResource(node);
            else if ((_tcsicmp(pstrClass, _T("Image")) == 0) && node.HasAttributes()) {
                //加载图片资源
                nAttributes = node.GetAttributeCount();
                for (int i = 0; i < nAttributes; i++) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);

                    if (_tcsicmp(pstrName, _T("id")) == 0) {
                        pstrId = pstrValue;
                    } else if (_tcsicmp(pstrName, _T("path")) == 0) {
                        pstrPath = pstrValue;
                    }
                }
                if (pstrId == NULL || pstrPath == NULL) continue;
                CStringUI * pstrFind = static_cast<CStringUI*>(m_mImageHashMap.Find(pstrId));
                if (pstrFind != NULL) continue;
                m_mImageHashMap.Insert(pstrId, (LPVOID)new CStringUI(pstrPath));
            } else if (_tcsicmp(pstrClass, _T("Xml")) == 0 && node.HasAttributes()) {
                //加载XML配置文件
                nAttributes = node.GetAttributeCount();
                for (int i = 0; i < nAttributes; i++) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);

                    if (_tcsicmp(pstrName, _T("id")) == 0) {
                        pstrId = pstrValue;
                    } else if (_tcsicmp(pstrName, _T("path")) == 0) {
                        pstrPath = pstrValue;
                    }
                }
                if (pstrId == NULL || pstrPath == NULL) continue;
                CStringUI * pstrFind = static_cast<CStringUI*>(m_mXmlHashMap.Find(pstrId));
                if (pstrFind != NULL) continue;
                m_mXmlHashMap.Insert(pstrId, (LPVOID)new CStringUI(pstrPath));
            } else continue;
        }
        return TRUE;
    }

    LPCTSTR CResourceManagerUI::GetImagePath(LPCTSTR lpstrId)
    {
        CStringUI* lpStr = static_cast<CStringUI*>(m_mImageHashMap.Find(lpstrId));
        return lpStr == NULL?NULL:lpStr->GetData();
    }

    LPCTSTR CResourceManagerUI::GetXmlPath(LPCTSTR lpstrId)
    {
        CStringUI* lpStr = static_cast<CStringUI*>(m_mXmlHashMap.Find(lpstrId));
        return lpStr == NULL?NULL:lpStr->GetData();
    }

    void CResourceManagerUI::ResetResourceMap()
    {
        CStringUI* lpStr;
        for (int i = 0; i < m_mImageHashMap.GetSize(); i++) {
            if (LPCTSTR key = m_mImageHashMap.GetAt(i)) {
                lpStr = static_cast<CStringUI*>(m_mImageHashMap.Find(key));
                delete lpStr;
                lpStr = NULL;
            }
        }
        for (int i = 0; i < m_mXmlHashMap.GetSize(); i++) {
            if (LPCTSTR key = m_mXmlHashMap.GetAt(i)) {
                lpStr = static_cast<CStringUI*>(m_mXmlHashMap.Find(key));
                delete lpStr;
                lpStr = NULL;
            }
        }
        for (int i = 0; i < m_mTextResourceHashMap.GetSize(); i++) {
            if (LPCTSTR key = m_mTextResourceHashMap.GetAt(i)) {
                lpStr = static_cast<CStringUI*>(m_mTextResourceHashMap.Find(key));
                delete lpStr;
                lpStr = NULL;
            }
        }
    }

    BOOL CResourceManagerUI::LoadLanguage(LPCTSTR pstrXml)
    {
        CMarkupUI xml;
        if (*(pstrXml) == _T('<')) {
            if (!xml.Load(pstrXml)) return FALSE;
        } else {
            if (!xml.LoadFromFile(pstrXml)) return FALSE;
        }
        CMarkupNodeUI Root = xml.GetRoot();
        if (!Root.IsValid()) return FALSE;

        LPCTSTR pstrClass = NULL;
        int nAttributes = 0;
        LPCTSTR pstrName = NULL;
        LPCTSTR pstrValue = NULL;
        LPTSTR pstr = NULL;

        //加载图片资源
        LPCTSTR pstrId = NULL;
        LPCTSTR pstrText = NULL;
        for (CMarkupNodeUI node = Root.GetChild(); node.IsValid(); node = node.GetSibling()) {
            pstrClass = node.GetName();
            if ((_tcsicmp(pstrClass, _T("Text")) == 0) && node.HasAttributes()) {
                //加载图片资源
                nAttributes = node.GetAttributeCount();
                for (int i = 0; i < nAttributes; i++) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);

                    if (_tcsicmp(pstrName, _T("id")) == 0) {
                        pstrId = pstrValue;
                    } else if (_tcsicmp(pstrName, _T("value")) == 0) {
                        pstrText = pstrValue;
                    }
                }
                if (pstrId == NULL || pstrText == NULL) continue;

                CStringUI * lpstrFind = static_cast<CStringUI*>(m_mTextResourceHashMap.Find(pstrId));
                if (lpstrFind != NULL) {
                    lpstrFind->Assign(pstrText);
                } else {
                    m_mTextResourceHashMap.Insert(pstrId, (LPVOID)new CStringUI(pstrText));
                }
            } else continue;
        }

        return TRUE;
    }

    CStringUI CResourceManagerUI::GetText(LPCTSTR lpstrId, LPCTSTR lpstrType)
    {
        if (lpstrId == NULL) return _T("");

        CStringUI * lpstrFind = static_cast<CStringUI*>(m_mTextResourceHashMap.Find(lpstrId));
        if (lpstrFind == NULL && m_pQuerypInterface) {
            lpstrFind = new CStringUI(m_pQuerypInterface->QueryControlText(lpstrId, lpstrType));
            m_mTextResourceHashMap.Insert(lpstrId, (LPVOID)lpstrFind);
        }
        return lpstrFind == NULL?lpstrId:*lpstrFind;
    }

    void CResourceManagerUI::ReloadText()
    {
        if (m_pQuerypInterface == NULL) return;
        //重载文字描述
        LPCTSTR lpstrId = NULL;
        LPCTSTR lpstrText;
        for (int i = 0; i < m_mTextResourceHashMap.GetSize(); i++) {
            lpstrId = m_mTextResourceHashMap.GetAt(i);
            if (lpstrId == NULL) continue;
            lpstrText = m_pQuerypInterface->QueryControlText(lpstrId, NULL);
            if (lpstrText != NULL) {
                CStringUI* lpStr = static_cast<CStringUI*>(m_mTextResourceHashMap.Find(lpstrId));
                lpStr->Assign(lpstrText);
            }
        }
    }
    void CResourceManagerUI::ResetTextMap()
    {
        CStringUI* lpStr;
        for (int i = 0; i < m_mTextResourceHashMap.GetSize(); i++) {
            if (LPCTSTR key = m_mTextResourceHashMap.GetAt(i)) {
                lpStr = static_cast<CStringUI*>(m_mTextResourceHashMap.Find(key));
                delete lpStr;
            }
        }
    }


} // namespace DUI
