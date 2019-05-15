#ifndef __UTILS_H__
#define __UTILS_H__

#include "DUILib.h"
#include "unzip.h"

namespace DUI
{

    typedef BOOL(*LPPARAM_PROVIDER_UI)(void* ctx, int iType, void* pData);

    /////////////////////////////////////////////////////////////////////////////////////
    //

    struct DUILIB_API   TDATA_UI
    {
        DWORD   dwCap;
        DWORD   dwLen;
        BYTE*   pRef;
        BYTE    pData[256];

        TDATA_UI();
        ~TDATA_UI();
        operator LPCVOID()  const;
        operator LPVOID();
        void Clear(BOOL bErase = FALSE);
        BOOL Write(LPCVOID* pData, DWORD dwSize);
        BOOL Reserve(DWORD dwSize);
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    struct DUILIB_API TSTRID_UI
    {
        TSTRID_UI(LPCTSTR lpString);
        TSTRID_UI(UINT nID);
        LPCTSTR m_lpstr;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    struct DUILIB_API CPointUI : public POINT
    {
        CPointUI();
        CPointUI(const POINT& src);
        CPointUI(int x, int y);
        CPointUI(LPARAM lParam);
    };


    /////////////////////////////////////////////////////////////////////////////////////
    //

    struct DUILIB_API CSizeUI : public SIZE
    {
        CSizeUI();
        CSizeUI(const SIZE& src);
        CSizeUI(const RECT rc);
        CSizeUI(int cx, int cy);
    };


    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CRectUI : public RECT
    {
    public:
        CRectUI();
        CRectUI(const RECT& src);
        CRectUI(int iLeft, int iTop, int iRight, int iBottom);

        int GetWidth() const;
        int GetHeight() const;
        void Empty();
        BOOL IsNull() const;
        void Join(const RECT& rc);
        void ResetOffset();
        void Normalize();
        void Offset(int cx, int cy);
        void Inflate(int cx, int cy);
        void Deflate(int cx, int cy);
        void Union(CRectUI& rc);
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CPtrArrayUI
    {
    public:
        CPtrArrayUI(int iPreallocSize = 0);
        CPtrArrayUI(const CPtrArrayUI& src);
        ~CPtrArrayUI();

        void    Empty();
        void    Resize(int iSize);
        BOOL    IsEmpty() const;
        int     Find(LPVOID iIndex) const;
        BOOL    Add(LPVOID pData);
        BOOL    SetAt(int iIndex, LPVOID pData);
        BOOL    InsertAt(int iIndex, LPVOID pData);
        BOOL    Remove(int iIndex);
        int     GetSize() const;
        LPVOID* GetData();

        LPVOID  GetAt(int iIndex) const;
        LPVOID  operator[] (int nIndex) const;

    protected:
        LPVOID* m_ppVoid;
        int m_nCount;
        int m_nAllocated;
    };


    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CValueArrayUI
    {
    public:
        CValueArrayUI(int iElementSize, int iPreallocSize = 0);
        ~CValueArrayUI();

        void Empty();
        BOOL IsEmpty() const;
        BOOL Add(LPCVOID pData);
        BOOL Remove(int iIndex);
        int GetSize() const;
        LPVOID GetData();

        LPVOID GetAt(int iIndex) const;
        LPVOID operator[] (int nIndex) const;

    protected:
        LPBYTE m_pVoid;
        int m_iElementSize;
        int m_nCount;
        int m_nAllocated;
    };


    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CStringUI
    {
    public:
        enum { MAX_LOCAL_STRING_LEN = 63 };

        CStringUI();
        CStringUI(const TCHAR ch);
        CStringUI(const CStringUI& src);
        CStringUI(LPCTSTR lpsz, int nLen = -1);
        ~CStringUI();

        void        Empty();
        int         GetLength() const;
        BOOL        IsEmpty() const;
        TCHAR       GetAt(int nIndex) const;
        void        Append(LPCTSTR pstr);
        void        Assign(LPCTSTR pstr, int nLength = -1);
        LPCTSTR     GetData() const;

        void        SetAt(int nIndex, TCHAR ch);
        operator    LPCTSTR() const;

        TCHAR               operator[] (int nIndex) const;
        const CStringUI&    operator=(const CStringUI& src);
        const CStringUI&    operator=(const TCHAR ch);
        const CStringUI&    operator=(LPCTSTR pstr);
#ifdef _UNICODE
        const CStringUI&    operator=(LPCSTR lpStr);
        const CStringUI&    operator+=(LPCSTR lpStr);
#else
        const CStringUI&    operator=(LPCWSTR lpwStr);
        const CStringUI&    operator+=(LPCWSTR lpwStr);
#endif
        CStringUI           operator+(const CStringUI& src) const;
        CStringUI           operator+(LPCTSTR pstr) const;
        const CStringUI&    operator+=(const CStringUI& src);
        const CStringUI&    operator+=(LPCTSTR pstr);
        const CStringUI&    operator+=(const TCHAR ch);

        BOOL operator == (LPCTSTR str) const;
        BOOL operator != (LPCTSTR str) const;
        BOOL operator <= (LPCTSTR str) const;
        BOOL operator <  (LPCTSTR str) const;
        BOOL operator >= (LPCTSTR str) const;
        BOOL operator >  (LPCTSTR str) const;

        int         Compare(LPCTSTR pstr) const;
        int         CompareNoCase(LPCTSTR pstr) const;

        void        MakeUpper();
        void        MakeLower();

        CStringUI   Left(int nLength) const;
        CStringUI   Mid(int iPos, int nLength = -1) const;
        CStringUI   Right(int nLength) const;

        int         Find(TCHAR ch, int iPos = 0) const;
        int         Find(LPCTSTR pstr, int iPos = 0) const;
        int         ReverseFind(TCHAR ch) const;
        int         Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);

        int __cdecl Format(LPCTSTR pstrFormat, ...);
        int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);

        BOOL        HasPrefix(const CStringUI& prefix) const;

    protected:
        int __cdecl InnerFormat(LPCTSTR pstrFormat, va_list Args);

    protected:
        LPTSTR  m_pstr;
        TCHAR   m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
    };

    static std::vector<CStringUI> StrSplit(CStringUI text, CStringUI sp)
    {
        std::vector<CStringUI> vResults;
        int pos = text.Find(sp, 0);
        while (pos >= 0) {
            CStringUI t = text.Left(pos);
            vResults.push_back(t);
            text = text.Right(text.GetLength() - pos - sp.GetLength());
            pos = text.Find(sp);
        }
        vResults.push_back(text);
        return vResults;
    }


    /////////////////////////////////////////////////////////////////////////////////////
    //


    class DUILIB_API CStringMapUI
    {
    public:
        CStringMapUI(int nSize = 83);
        ~CStringMapUI();

        void Resize(int nSize = 83);
        LPVOID Find(LPCTSTR key, BOOL optimize = TRUE) const;
        BOOL Insert(LPCTSTR key, LPVOID pData);
        LPVOID Set(LPCTSTR key, LPVOID pData);
        BOOL Remove(LPCTSTR key);
        void RemoveAll();
        int GetSize() const;
        LPCTSTR GetAt(int iIndex) const;
        LPCTSTR operator[] (int nIndex) const;

    protected:
        struct TITEM_UI
        {
            CStringUI Key;
            LPVOID Data;
            struct TITEM_UI* pPrev;
            struct TITEM_UI* pNext;
        };
        TITEM_UI** m_aT;
        int m_nBuckets;
        int m_nCount;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class DUILIB_API CWaitCursorUI
    {
    public:
        CWaitCursorUI();
        ~CWaitCursorUI();

    protected:
        HCURSOR m_hOrigCursor;
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //

    class CVariantUI : public VARIANT
    {
    public:
        CVariantUI()
        {
            VariantInit(this);
        }
        CVariantUI(int i)
        {
            VariantInit(this);
            this->vt = VT_I4;
            this->intVal = i;
        }
        CVariantUI(float f)
        {
            VariantInit(this);
            this->vt = VT_R4;
            this->fltVal = f;
        }
        CVariantUI(LPOLESTR s)
        {
            VariantInit(this);
            this->vt = VT_BSTR;
            this->bstrVal = s;
        }
        CVariantUI(IDispatch* disp)
        {
            VariantInit(this);
            this->vt = VT_DISPATCH;
            this->pdispVal = disp;
        }

        ~CVariantUI()
        {
            VariantClear(this);
        }
    };

    //////////////////////////////////////////////////////////////////////////////////////
    //
    static char* w2a(wchar_t* lpszSrc, UINT   CodePage = CP_ACP)
    {
        if (lpszSrc != NULL) {
            int  nANSILen = WideCharToMultiByte(CodePage, 0, lpszSrc, -1, NULL, 0, NULL, NULL);
            char* pANSI = new char[nANSILen + 1];
            if (pANSI != NULL) {
                ZeroMemory(pANSI, nANSILen + 1);
                WideCharToMultiByte(CodePage, 0, lpszSrc, -1, pANSI, nANSILen, NULL, NULL);
                return pANSI;
            }
        }
        return NULL;
    }

    static wchar_t* a2w(char* lpszSrc, UINT   CodePage = CP_ACP)
    {
        if (lpszSrc != NULL) {
            int nUnicodeLen = MultiByteToWideChar(CodePage, 0, lpszSrc, -1, NULL, 0);
            LPWSTR pUnicode = new WCHAR[nUnicodeLen + 1];
            if (pUnicode != NULL) {
                ZeroMemory((void*)pUnicode, (nUnicodeLen + 1) * sizeof(WCHAR));
                MultiByteToWideChar(CodePage, 0, lpszSrc, -1, pUnicode, nUnicodeLen);
                return pUnicode;
            }
        }
        return NULL;
    }

    //struct DUILIB_API TURI_UI
    //{
    //    CStringUI   sSchema;
    //    CStringUI   sLocation;
    //    CStringUI   sFilePath;
    //    TURI_UI(LPCTSTR uri);
    //    CStringUI AsString();
    //};



    /////////////////////////////////////////////////////////////////////////////////////
    struct DUILIB_API IReaderUI
    {
        virtual BOOL                Open(void* ctx) = 0;
        virtual BOOL                Read(BYTE* pCache, DWORD dwCacheSize, DWORD* dwReadSize) = 0;  //  0：正常；-1：异常；1：结束
        virtual void                Close() = 0;
        virtual const CStringUI&    Error() = 0;
    };


    class DUILIB_API CFileReaderUI : public IReaderUI
    {
    protected:
        CStringUI   m_sFileName;
        CStringUI   m_sError;
        HANDLE      m_hFile;

    protected:
        BOOL _Failed(LPCTSTR pstrError);

    public:
        CFileReaderUI(const CStringUI& sFileName);
        ~CFileReaderUI();
        virtual BOOL Open(void* ctx);
        virtual BOOL Read(BYTE* pCache, DWORD dwCacheSize, DWORD* dwReadSize);
        virtual void Close();
        virtual const CStringUI& Error();
    };

    class DUILIB_API CZipFileReaderUI : public CFileReaderUI
    {
    protected:
        CStringUI   m_sPassword;
        BOOL        m_bAttached;

        BYTE*       m_cCache;
        DWORD       m_dwLen;
        DWORD       m_dwPos;

    public:
        CZipFileReaderUI(const CStringUI& sFileName);
        CZipFileReaderUI(HANDLE hZipFile);
        virtual BOOL Open(void* ctx);
        virtual BOOL Read(BYTE* pCache, DWORD dwCacheSize, DWORD* dwReadSize);
        virtual void Close();
    };


    ///////////////////////////////////////////////////////////////////////////////////////
    ////
    //struct TIMAGEINFO_UI;
    //class CManagerUI;
    //class DUILIB_API CImageString
    //{
    //public:
    //	CImageString();
    //	CImageString(const CImageString&);
    //	const CImageString& operator=(const CImageString&);
    //	virtual ~CImageString();

    //	const CStringUI& GetAttributeString() const;
    //	void SetAttributeString(LPCTSTR pStrImageAttri);
    //	void ModifyAttribute(LPCTSTR pStrModify);
    //	BOOL LoadImage(CManagerUI* pManager);
    //	BOOL IsLoadSuccess();

    //	RECT GetDest() const;
    //	void SetDest(const RECT &rcDest);
    //	const TIMAGEINFO_UI* GeTIMAGEINFO_UI() const;

    //private:
    //	void Clone(const CImageString&);
    //	void Clear();
    //	void ParseAttribute(LPCTSTR pStrImageAttri);

    //protected:
    //	friend class CRenderUI;
    //	CStringUI	m_sImageAttribute;

    //	CStringUI	m_sImage;
    //	CStringUI	m_sResType;
    //	TIMAGEINFO_UI	*m_imageInfo;
    //	BOOL		m_bLoadSuccess;

    //	RECT	m_rcDest;
    //	RECT	m_rcSource;
    //	RECT	m_rcCorner;
    //	BYTE	m_bFade;
    //	DWORD	m_dwMask;
    //	BOOL	m_bHole;
    //	BOOL	m_bTiledX;
    //	BOOL	m_bTiledY;
    //};
}// namespace DUI

#endif // __UTILS_H__