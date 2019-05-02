#ifndef __UIMARKUP_H__
#define __UIMARKUP_H__

#include "Core/UIDefine.h"

namespace DUI
{

    enum XMLFILE_ENCODING_TYPE_UI
    {
        UIXMLFILE_ENCODING_UTF8 = 0,
        UIXMLFILE_ENCODING_UNICODE = 1,
        UIXMLFILE_ENCODING_ASNI = 2,
    };

    class CMarkupUI;
    class CMarkupNodeUI;


    class DUILIB_API CMarkupUI
    {
        friend class CMarkupNodeUI;
    public:
        CMarkupUI(LPCTSTR pstrXML = NULL);
        ~CMarkupUI();

        BOOL Load(LPCTSTR pstrXML);
        BOOL LoadFromMem(BYTE* pByte, DWORD dwSize, int encoding = UIXMLFILE_ENCODING_UTF8);
        BOOL LoadFromFile(LPCTSTR pstrFilename, int encoding = UIXMLFILE_ENCODING_UTF8);
        void Release();
        BOOL IsValid() const;

        void SetPreserveWhitespace(BOOL bPreserve = true);
        void GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const;
        void GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const;

        CMarkupNodeUI GetRoot();

    private:
        typedef struct TXMLELEMENT_UI
        {
            ULONG iStart;
            ULONG iChild;
            ULONG iNext;
            ULONG iParent;
            ULONG iData;
        } TXMLELEMENT_UI;

        LPTSTR m_pstrXML;
        TXMLELEMENT_UI* m_pElements;
        ULONG m_nElements;
        ULONG m_nReservedElements;
        TCHAR m_szErrorMsg[100];
        TCHAR m_szErrorXML[50];
        BOOL m_bPreserveWhitespace;

    private:
        BOOL _Parse();
        BOOL _Parse(LPTSTR& pstrText, ULONG iParent);
        TXMLELEMENT_UI* _ReserveElement();
        inline void _SkipWhitespace(LPTSTR& pstr) const;
        inline void _SkipWhitespace(LPCTSTR& pstr) const;
        inline void _SkipIdentifier(LPTSTR& pstr) const;
        inline void _SkipIdentifier(LPCTSTR& pstr) const;
        BOOL _ParseData(LPTSTR& pstrText, LPTSTR& pstrData, char cEnd);
        void _ParseMetaChar(LPTSTR& pstrText, LPTSTR& pstrDest);
        BOOL _ParseAttributes(LPTSTR& pstrText);
        BOOL _Failed(LPCTSTR pstrError, LPCTSTR pstrLocation = NULL);
    };


    class DUILIB_API CMarkupNodeUI
    {
        friend class CMarkupUI;
    private:
        CMarkupNodeUI();
        CMarkupNodeUI(CMarkupUI* pOwner, int iPos);

    public:
        BOOL IsValid() const;

        CMarkupNodeUI GetParent();
        CMarkupNodeUI GetSibling();
        CMarkupNodeUI GetChild();
        CMarkupNodeUI GetChild(LPCTSTR pstrName);

        BOOL HasSiblings() const;
        BOOL HasChildren() const;
        LPCTSTR GetName() const;
        LPCTSTR GetValue() const;

        BOOL HasAttributes();
        BOOL HasAttribute(LPCTSTR pstrName);
        int GetAttributeCount();
        LPCTSTR GetAttributeName(int iIndex);
        LPCTSTR GetAttributeValue(int iIndex);
        LPCTSTR GetAttributeValue(LPCTSTR pstrName);
        BOOL GetAttributeValue(int iIndex, LPTSTR pstrValue, SIZE_T cchMax);
        BOOL GetAttributeValue(LPCTSTR pstrName, LPTSTR pstrValue, SIZE_T cchMax);

    private:
        void _MapAttributes();

        enum { MAX_XML_ATTRIBUTES = 64 };

        typedef struct
        {
            ULONG iName;
            ULONG iValue;
        } TXMLATTRIBUTE_UI;

        int m_iPos;
        int m_nAttributes;
        TXMLATTRIBUTE_UI m_aAttributes[MAX_XML_ATTRIBUTES];
        CMarkupUI* m_pOwner;
    };

} // namespace DUI

#endif // __UIMARKUP_H__
