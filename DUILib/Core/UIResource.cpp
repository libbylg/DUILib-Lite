#include "Core/UIResource.h"
#include "Core/UIManager.h"
#include "Utils/unzip.h"

namespace DUI
{
    CResourceUI* CResourceUI::GetInstance()
    {
        static CResourceUI* p = new CResourceUI;
        return p;
    };

    CResourceUI::CResourceUI(void)
    {
        m_pQuerypInterface = NULL;
        m_hResourceInstance = NULL;
        m_pStrResourcePath;
        m_pStrResourceZip;
        m_pStrResourceZipPwd; //Garfield 20160325 带密码zip包解密
        m_hResourceZip = NULL;
        m_bCachedResourceZip = true;
        m_nResType = UIRES_FILE;
        m_SharedResInfo;
    }

    CResourceUI::~CResourceUI(void)
    {
        ResetResourceMap();
    }


    BOOL CResourceUI::LoadResource(TSTRID_UI xml, LPCTSTR type)
    {
        if (HIWORD(xml.m_lpstr) != NULL) {
            if (*(xml.m_lpstr) == _T('<')) {
                if (!m_xml.LoadFromString(xml.m_lpstr)) {
                    return NULL;
                }
            } else {
                CStringUI sFileName = xml.m_lpstr;
                CFileReaderUI reader(sFileName);
                if (!m_xml.LoadFromReader(&reader, NULL, UIXMLFILE_ENCODING_UTF8)) {
                    return NULL;
                }
            }
        } else {
            HRSRC hResource = ::FindResource(CResourceUI::GetResourceDll(), xml.m_lpstr, type);
            if (hResource == NULL) {
                return NULL;
            }
            HGLOBAL hGlobal = ::LoadResource(CResourceUI::GetResourceDll(), hResource);
            if (hGlobal == NULL) {
                FreeResource(hResource);
                return NULL;
            }
            if (!m_xml.LoadFromMemory((BYTE*) ::LockResource(hGlobal), ::SizeofResource(CResourceUI::GetResourceDll(), hResource))) {
                return NULL;
            }
            ::FreeResource(hResource);
        }

        return LoadResource(m_xml.GetRoot());
    }

    enum
    {
        TYPE_FILE,
        TYPE_ZIP,
        TYPE_RES,
        TYPE_RAW
    };

    BOOL CResourceUI::LoadResource(TDATA_UI& tData, const CStringUI& uri, void* ctx = NULL, LPLOADRESOURCE_CALLBACK_UI pCallback = NULL);
    {
        int iType = TYPE_RAW;



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
    }

//    BOOL CResourceUI::LoadResource(LPCTSTR pstrFilename, void* ctx, PHANDLERESOURCE_UI handle)
//    {
//        //Release();
//        CStringUI sFile = CManagerUI::GetResourcePath();
//        if (CManagerUI::GetResourceZip().IsEmpty()) {
//            sFile += pstrFilename;
//            HANDLE hFile = ::CreateFile(sFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//            if (hFile == INVALID_HANDLE_VALUE) {
//                return FALSE;// _Failed(_T("Error opening file"));
//            }
//
//            DWORD dwSize = ::GetFileSize(hFile, NULL);
//            if (dwSize == 0) {
//                return FALSE;//_Failed(_T("File is empty"));
//            }
//
//            if (dwSize > 4096 * 1024) {
//                return FALSE;//_Failed(_T("File too large"));
//            }
//
//            DWORD dwRead = 0;
//            BYTE* pByte = new BYTE[dwSize];
//            ::ReadFile(hFile, pByte, dwSize, &dwRead, NULL);
//            ::CloseHandle(hFile);
//            if (dwRead != dwSize) {
//                delete[] pByte;
//                pByte = NULL;
//                //Release();
//                return FALSE;//_Failed(_T("Could not read file"));
//            }
//
//            BOOL ret = handle(ctx, pByte, dwSize);
//
//            //BOOL ret = xml.LoadFromMemory(pByte, dwSize, encoding);
//            delete[] pByte;
//            pByte = NULL;
//
//            return ret;
//        } else {
//            sFile += CManagerUI::GetResourceZip();
//            HZIP hz = NULL;
//            if (CManagerUI::IsCachedResourceZip()) {
//                hz = (HZIP)CManagerUI::GetResourceZipHandle();
//            } else {
//                CStringUI sFilePwd = CManagerUI::GetResourceZipPwd();
//#ifdef UNICODE
//                char* pwd = w2a((wchar_t*)sFilePwd.GetData());
//                hz = OpenZip(sFile.GetData(), pwd);
//                if (pwd) delete[] pwd;
//#else
//                hz = OpenZip(sFile.GetData(), sFilePwd.GetData());
//#endif
//            }
//            if (hz == NULL) {
//                return FALSE;// _Failed(_T("Error opening zip file"));
//            }
//            ZIPENTRY ze;
//            int i = 0;
//            CStringUI key = pstrFilename;
//            key.Replace(_T("\\"), _T("/"));
//            if (FindZipItem(hz, key, TRUE, &i, &ze) != 0) {
//                return FALSE;// _Failed(_T("Could not find ziped file"));
//            }
//            DWORD dwSize = ze.unc_size;
//            if (dwSize == 0) {
//                return FALSE;//_Failed(_T("File is empty"));
//            }
//            if (dwSize > 4096 * 1024) {
//                return FALSE;//_Failed(_T("File too large"));
//            }
//            BYTE * pByte = new BYTE[dwSize];
//            int res = UnzipItem(hz, i, pByte, dwSize);
//            if (res != 0x00000000 && res != 0x00000600) {
//                delete[] pByte;
//                if (!CManagerUI::IsCachedResourceZip()) {
//                    CloseZip(hz);
//                }
//                return FALSE;//_Failed(_T("Could not unzip file"));
//            }
//            if (!CManagerUI::IsCachedResourceZip()) {
//                CloseZip(hz);
//            }
//            //BOOL ret = xml.LoadFromMemory(pByte, dwSize, encoding);
//            BOOL ret = handle(ctx, pByte, dwSize);
//            delete[] pByte;
//            pByte = NULL;
//            return ret;
//        }
//    }

    BOOL CResourceUI::LoadResource(CMarkupNodeUI Root)
    {
        if (!Root.IsValid()) {
            return FALSE;
        }

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
            if (ChildNode.IsValid()) {
                LoadResource(node);
            } else if ((_tcsicmp(pstrClass, _T("Image")) == 0) && node.HasAttributes()) {
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
                if (pstrId == NULL || pstrPath == NULL) {
                    continue;
                }
                CStringUI * pstrFind = static_cast<CStringUI*>(m_mImageHashMap.Find(pstrId));
                if (pstrFind != NULL) {
                    continue;
                }
                m_mImageHashMap.Insert(pstrId, (LPVOID) new CStringUI(pstrPath));
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
                m_mXmlHashMap.Insert(pstrId, (LPVOID) new CStringUI(pstrPath));
            }
        }
        return TRUE;
    }

    LPCTSTR CResourceUI::GetImagePath(LPCTSTR lpstrId)
    {
        CStringUI* lpStr = static_cast<CStringUI*>(m_mImageHashMap.Find(lpstrId));
        return lpStr == NULL?NULL:lpStr->GetData();
    }

    LPCTSTR CResourceUI::GetXmlPath(LPCTSTR lpstrId)
    {
        CStringUI* lpStr = static_cast<CStringUI*>(m_mXmlHashMap.Find(lpstrId));
        return lpStr == NULL?NULL:lpStr->GetData();
    }

    CStringUI   CResourceUI::ResolveResourcePath(LPCTSTR lpResName)
    {
        CStringUI sFile = CResourceUI::GetInstance()->GetResourcePath();
        if (CResourceUI::GetInstance()->GetResourceZip().IsEmpty()) {
            sFile += "/";
            sFile += lpResName;
            return sFile;
        }
        
        sFile += CResourceUI::GetInstance()->GetResourceZip();
        HZIP hz = NULL;
        if (CResourceUI::GetInstance()->IsCachedResourceZip())
            hz = (HZIP)CResourceUI::GetInstance()->GetResourceZipHandle();
        else {
            CStringUI sFilePwd = CResourceUI::GetInstance()->GetResourceZipPwd();
        }
    }

    void CResourceUI::ResetResourceMap()
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

    BOOL CResourceUI::LoadLanguage(LPCTSTR pstrXml)
    {
        CMarkupUI xml;
        if (*(pstrXml) == _T('<')) {
            if (!xml.LoadFromString(pstrXml)) {
                return FALSE;
            }
        } else {
            CStringUI sFileName = pstrXml;
            CFileReaderUI reader(sFileName);
            if (!xml.LoadFromReader(&reader, NULL, UIXMLFILE_ENCODING_UTF8)) {
                return FALSE;
            }
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
                    m_mTextResourceHashMap.Insert(pstrId, (LPVOID) new CStringUI(pstrText));
                }
            } else
                continue;
        }

        return TRUE;
    }

    CStringUI CResourceUI::GetText(LPCTSTR lpstrId, LPCTSTR lpstrType)
    {
        if (lpstrId == NULL) return _T("");

        CStringUI * lpstrFind = static_cast<CStringUI*>(m_mTextResourceHashMap.Find(lpstrId));
        if (lpstrFind == NULL && m_pQuerypInterface) {
            lpstrFind = new CStringUI(m_pQuerypInterface->QueryControlText(lpstrId, lpstrType));
            m_mTextResourceHashMap.Insert(lpstrId, (LPVOID)lpstrFind);
        }
        return lpstrFind == NULL?lpstrId:*lpstrFind;
    }

    void CResourceUI::ReloadText()
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
    void CResourceUI::ResetTextMap()
    {
        CStringUI* lpStr;
        for (int i = 0; i < m_mTextResourceHashMap.GetSize(); i++) {
            if (LPCTSTR key = m_mTextResourceHashMap.GetAt(i)) {
                lpStr = static_cast<CStringUI*>(m_mTextResourceHashMap.Find(key));
                delete lpStr;
            }
        }
    }


    HINSTANCE CResourceUI::GetResourceDll()
    {
        if (m_hResourceInstance == NULL) {
            return CManagerUI::GetInstance();
        }
        return m_hResourceInstance;
    }

    const CStringUI& CResourceUI::GetResourcePath()
    {
        return m_pStrResourcePath;
    }

    const CStringUI& CResourceUI::GetResourceZip()
    {
        return m_pStrResourceZip;
    }

    const CStringUI& CResourceUI::GetResourceZipPwd()
    {
        return m_pStrResourceZipPwd;
    }

    BOOL CResourceUI::IsCachedResourceZip()
    {
        return m_bCachedResourceZip;
    }

    HANDLE CResourceUI::GetResourceZipHandle()
    {
        return m_hResourceZip;
    }

    void CResourceUI::SetResourceDll(HINSTANCE hInst)
    {
        m_hResourceInstance = hInst;
    }

    void CResourceUI::SetResourcePath(LPCTSTR pStrPath)
    {
        m_pStrResourcePath = pStrPath;
        if (m_pStrResourcePath.IsEmpty()) return;
        TCHAR cEnd = m_pStrResourcePath.GetAt(m_pStrResourcePath.GetLength() - 1);
        if (cEnd != _T('\\') && cEnd != _T('/')) m_pStrResourcePath += _T('\\');
    }

    void CResourceUI::SetResourceZip(LPVOID pVoid, unsigned int len, LPCTSTR password)
    {
        if (m_pStrResourceZip == _T("membuffer")) return;
        if (m_bCachedResourceZip && m_hResourceZip != NULL) {
            CloseZip((HZIP)m_hResourceZip);
            m_hResourceZip = NULL;
        }
        m_pStrResourceZip = _T("membuffer");
        m_bCachedResourceZip = true;
        m_pStrResourceZipPwd = password; //Garfield 20160325 带密码zip包解密
        if (m_bCachedResourceZip) {
#ifdef UNICODE
            char* pwd = w2a((wchar_t*)password);
            m_hResourceZip = (HANDLE)OpenZip(pVoid, len, pwd);
            if (pwd) {
                delete[] pwd;
                pwd = NULL;
            }
#else
            m_hResourceZip = (HANDLE)OpenZip(pVoid, len, password);
#endif
        }
    }

    void CResourceUI::SetResourceZip(LPCTSTR pStrPath, BOOL bCachedResourceZip, LPCTSTR password)
    {
        if (m_pStrResourceZip == pStrPath && m_bCachedResourceZip == bCachedResourceZip) return;
        if (m_bCachedResourceZip && m_hResourceZip != NULL) {
            CloseZip((HZIP)m_hResourceZip);
            m_hResourceZip = NULL;
        }
        m_pStrResourceZip = pStrPath;
        m_bCachedResourceZip = bCachedResourceZip;
        m_pStrResourceZipPwd = password;
        if (m_bCachedResourceZip) {
            CStringUI sFile = CResourceUI::GetResourcePath();
            sFile += CResourceUI::GetResourceZip();
#ifdef UNICODE
            char* pwd = w2a((wchar_t*)password);
            m_hResourceZip = (HANDLE)OpenZip(sFile.GetData(), pwd);
            if (pwd) {
                delete[] pwd;
                pwd = NULL;
            }
#else
            m_hResourceZip = (HANDLE)OpenZip(sFile.GetData(), password);
#endif
        }
    }

    void CResourceUI::SetResourceType(RESTYPE_UI nType)
    {
        m_nResType = nType;
    }

    RESTYPE_UI CResourceUI::GetResourceType()
    {
        return m_nResType;
    }


    void CResourceUI::Release()
    {
        // 清理共享资源
        // 图片
        TIMAGEINFO_UI* data;
        for (int i = 0; i < m_SharedResInfo.m_ImageHash.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_ImageHash.GetAt(i)) {
                data = static_cast<TIMAGEINFO_UI*>(m_SharedResInfo.m_ImageHash.Find(key, FALSE));
                if (data) {
                    CRenderUI::FreeImage(data);
                    data = NULL;
                }
            }
        }
        m_SharedResInfo.m_ImageHash.RemoveAll();
        // 字体
        TFONTINFO_UI* pFontInfo;
        for (int i = 0; i < m_SharedResInfo.m_CustomFonts.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_CustomFonts.GetAt(i)) {
                pFontInfo = static_cast<TFONTINFO_UI*>(m_SharedResInfo.m_CustomFonts.Find(key, FALSE));
                if (pFontInfo) {
                    ::DeleteObject(pFontInfo->hFont);
                    delete pFontInfo;
                    pFontInfo = NULL;
                }
            }
        }
        m_SharedResInfo.m_CustomFonts.RemoveAll();
        // 默认字体
        if (m_SharedResInfo.m_DefaultFontInfo.hFont != NULL) {
            ::DeleteObject(m_SharedResInfo.m_DefaultFontInfo.hFont);
        }
        // 样式
        CStringUI* pStyle;
        for (int i = 0; i < m_SharedResInfo.m_StyleHash.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_StyleHash.GetAt(i)) {
                pStyle = static_cast<CStringUI*>(m_SharedResInfo.m_StyleHash.Find(key, FALSE));
                if (pStyle) {
                    delete pStyle;
                    pStyle = NULL;
                }
            }
        }
        m_SharedResInfo.m_StyleHash.RemoveAll();

        // 样式
        CStringUI* pAttr;
        for (int i = 0; i < m_SharedResInfo.m_AttrHash.GetSize(); i++) {
            if (LPCTSTR key = m_SharedResInfo.m_AttrHash.GetAt(i)) {
                pAttr = static_cast<CStringUI*>(m_SharedResInfo.m_AttrHash.Find(key, FALSE));
                if (pAttr) {
                    delete pAttr;
                    pAttr = NULL;
                }
            }
        }
        m_SharedResInfo.m_AttrHash.RemoveAll();

        // 关闭ZIP
        if (m_bCachedResourceZip && m_hResourceZip != NULL) {
            CloseZip((HZIP)m_hResourceZip);
            m_hResourceZip = NULL;
        }

        //  对象自身销毁
        delete this;
    }


    const TRESINFO_UI& CResourceUI::GetSharedResInfo()const
    {
        return m_SharedResInfo;
    }

    TRESINFO_UI& CResourceUI::GetSharedResInfo()
    {
        return m_SharedResInfo;
    }

} // namespace DUI
