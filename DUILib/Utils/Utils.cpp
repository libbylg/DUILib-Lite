#include "Utils/Utils.h"
#include "Utils/unzip.h"

namespace DUI
{

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    TSTRID_UI::TSTRID_UI(LPCTSTR lpString) : m_lpstr(lpString)
    {
    }
    TSTRID_UI::TSTRID_UI(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CPointUI::CPointUI()
    {
        x = y = 0;
    }

    CPointUI::CPointUI(const POINT& src)
    {
        x = src.x;
        y = src.y;
    }

    CPointUI::CPointUI(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    CPointUI::CPointUI(LPARAM lParam)
    {
        x = GET_X_LPARAM(lParam);
        y = GET_Y_LPARAM(lParam);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CSizeUI::CSizeUI()
    {
        cx = cy = 0;
    }

    CSizeUI::CSizeUI(const SIZE& src)
    {
        cx = src.cx;
        cy = src.cy;
    }

    CSizeUI::CSizeUI(const RECT rc)
    {
        cx = rc.right - rc.left;
        cy = rc.bottom - rc.top;
    }

    CSizeUI::CSizeUI(int _cx, int _cy)
    {
        cx = _cx;
        cy = _cy;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CRectUI::CRectUI()
    {
        left = top = right = bottom = 0;
    }

    CRectUI::CRectUI(const RECT& src)
    {
        left = src.left;
        top = src.top;
        right = src.right;
        bottom = src.bottom;
    }

    CRectUI::CRectUI(int iLeft, int iTop, int iRight, int iBottom)
    {
        left = iLeft;
        top = iTop;
        right = iRight;
        bottom = iBottom;
    }

    int CRectUI::GetWidth() const
    {
        return right - left;
    }

    int CRectUI::GetHeight() const
    {
        return bottom - top;
    }

    void CRectUI::Empty()
    {
        left = top = right = bottom = 0;
    }

    BOOL CRectUI::IsNull() const
    {
        return (left == 0 && right == 0 && top == 0 && bottom == 0);
    }

    void CRectUI::Join(const RECT & rc)
    {
        if (rc.left < left) left = rc.left;
        if (rc.top < top) top = rc.top;
        if (rc.right > right) right = rc.right;
        if (rc.bottom > bottom) bottom = rc.bottom;
    }

    void CRectUI::ResetOffset()
    {
        ::OffsetRect(this, -left, -top);
    }

    void CRectUI::Normalize()
    {
        if (left > right) {
            int iTemp = left;
            left = right;
            right = iTemp;
        }
        if (top > bottom) {
            int iTemp = top;
            top = bottom;
            bottom = iTemp;
        }
    }

    void CRectUI::Offset(int cx, int cy)
    {
        ::OffsetRect(this, cx, cy);
    }

    void CRectUI::Inflate(int cx, int cy)
    {
        ::InflateRect(this, cx, cy);
    }

    void CRectUI::Deflate(int cx, int cy)
    {
        ::InflateRect(this, -cx, -cy);
    }

    void CRectUI::Union(CRectUI & rc)
    {
        ::UnionRect(this, this, &rc);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CPtrArrayUI::CPtrArrayUI(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
    {
        ASSERT(iPreallocSize >= 0);
        if (iPreallocSize > 0) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
    }

    CPtrArrayUI::CPtrArrayUI(const CPtrArrayUI & src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
    {
        for (int i = 0; i < src.GetSize(); i++)
            Add(src.GetAt(i));
    }

    CPtrArrayUI::~CPtrArrayUI()
    {
        if (m_ppVoid != NULL) free(m_ppVoid);
    }

    void CPtrArrayUI::Empty()
    {
        if (m_ppVoid != NULL) free(m_ppVoid);
        m_ppVoid = NULL;
        m_nCount = m_nAllocated = 0;
    }

    void CPtrArrayUI::Resize(int iSize)
    {
        Empty();
        m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
        ::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
        m_nAllocated = iSize;
        m_nCount = iSize;
    }

    BOOL CPtrArrayUI::IsEmpty() const
    {
        return m_nCount == 0;
    }

    BOOL CPtrArrayUI::Add(LPVOID pData)
    {
        if (++m_nCount >= m_nAllocated) {
            int nAllocated = m_nAllocated * 2;
            if (nAllocated == 0) nAllocated = 11;
            LPVOID * ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
            if (ppVoid != NULL) {
                m_nAllocated = nAllocated;
                m_ppVoid = ppVoid;
            } else {
                --m_nCount;
                return FALSE;
            }
        }
        m_ppVoid[m_nCount - 1] = pData;
        return true;
    }

    BOOL CPtrArrayUI::InsertAt(int iIndex, LPVOID pData)
    {
        if (iIndex == m_nCount) return Add(pData);
        if (iIndex < 0 || iIndex > m_nCount) return FALSE;
        if (++m_nCount >= m_nAllocated) {
            int nAllocated = m_nAllocated * 2;
            if (nAllocated == 0) nAllocated = 11;
            LPVOID * ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
            if (ppVoid != NULL) {
                m_nAllocated = nAllocated;
                m_ppVoid = ppVoid;
            } else {
                --m_nCount;
                return FALSE;
            }
        }
        memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
        m_ppVoid[iIndex] = pData;
        return true;
    }

    BOOL CPtrArrayUI::SetAt(int iIndex, LPVOID pData)
    {
        if (iIndex < 0 || iIndex >= m_nCount) return FALSE;
        m_ppVoid[iIndex] = pData;
        return true;
    }

    BOOL CPtrArrayUI::Remove(int iIndex)
    {
        if (iIndex < 0 || iIndex >= m_nCount) return FALSE;
        if (iIndex < --m_nCount) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
        return true;
    }

    int CPtrArrayUI::Find(LPVOID pData) const
    {
        for (int i = 0; i < m_nCount; i++)
            if (m_ppVoid[i] == pData) return i;
        return -1;
    }

    int CPtrArrayUI::GetSize() const
    {
        return m_nCount;
    }

    LPVOID* CPtrArrayUI::GetData()
    {
        return m_ppVoid;
    }

    LPVOID CPtrArrayUI::GetAt(int iIndex) const
    {
        if (iIndex < 0 || iIndex >= m_nCount) return NULL;
        return m_ppVoid[iIndex];
    }

    LPVOID CPtrArrayUI::operator[](int iIndex) const
    {
        ASSERT(iIndex >= 0 && iIndex < m_nCount);
        return m_ppVoid[iIndex];
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CValueArrayUI::CValueArrayUI(int iElementSize, int iPreallocSize /*= 0*/) : m_pVoid(NULL),
        m_nCount(0),
        m_iElementSize(iElementSize),
        m_nAllocated(iPreallocSize)
    {
        ASSERT(iElementSize > 0);
        ASSERT(iPreallocSize >= 0);
        if (iPreallocSize > 0) m_pVoid = static_cast<LPBYTE>(malloc(iPreallocSize * m_iElementSize));
    }

    CValueArrayUI::~CValueArrayUI()
    {
        if (m_pVoid != NULL) free(m_pVoid);
    }

    void CValueArrayUI::Empty()
    {
        m_nCount = 0; // NOTE: We keep the memory in place
    }

    BOOL CValueArrayUI::IsEmpty() const
    {
        return m_nCount == 0;
    }

    BOOL CValueArrayUI::Add(LPCVOID pData)
    {
        if (++m_nCount >= m_nAllocated) {
            int nAllocated = m_nAllocated * 2;
            if (nAllocated == 0) nAllocated = 11;
            LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
            if (pVoid != NULL) {
                m_nAllocated = nAllocated;
                m_pVoid = pVoid;
            } else {
                --m_nCount;
                return FALSE;
            }
        }
        ::CopyMemory(m_pVoid + ((m_nCount - 1) * m_iElementSize), pData, m_iElementSize);
        return true;
    }

    BOOL CValueArrayUI::Remove(int iIndex)
    {
        if (iIndex < 0 || iIndex >= m_nCount) return FALSE;
        if (iIndex < --m_nCount) ::CopyMemory(m_pVoid + (iIndex * m_iElementSize), m_pVoid + ((iIndex + 1) * m_iElementSize), (m_nCount - iIndex) * m_iElementSize);
        return true;
    }

    int CValueArrayUI::GetSize() const
    {
        return m_nCount;
    }

    LPVOID CValueArrayUI::GetData()
    {
        return static_cast<LPVOID>(m_pVoid);
    }

    LPVOID CValueArrayUI::GetAt(int iIndex) const
    {
        if (iIndex < 0 || iIndex >= m_nCount) return NULL;
        return m_pVoid + (iIndex * m_iElementSize);
    }

    LPVOID CValueArrayUI::operator[](int iIndex) const
    {
        ASSERT(iIndex >= 0 && iIndex < m_nCount);
        return m_pVoid + (iIndex * m_iElementSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CStringUI::CStringUI() : m_pstr(m_szBuffer)
    {
        m_szBuffer[0] = '\0';
    }

    CStringUI::CStringUI(const TCHAR ch):m_pstr(m_szBuffer)
    {
        m_szBuffer[0] = ch;
        m_szBuffer[1] = '\0';
    }

    CStringUI::CStringUI(LPCTSTR lpsz, int nLen) : m_pstr(m_szBuffer)
    {
        ASSERT(!::IsBadStringPtr(lpsz, (UINT_PTR)(nLen)) || lpsz == NULL);
        m_szBuffer[0] = '\0';
        Assign(lpsz, nLen);
    }

    CStringUI::CStringUI(const CStringUI & src) : m_pstr(m_szBuffer)
    {
        m_szBuffer[0] = '\0';
        Assign(src.m_pstr);
    }

    CStringUI::~CStringUI()
    {
        if (m_pstr != m_szBuffer) free(m_pstr);
    }

    int CStringUI::GetLength() const
    {
        return (int)_tcslen(m_pstr);
    }

    CStringUI::operator LPCTSTR() const
    {
        return m_pstr;
    }

    void CStringUI::Append(LPCTSTR pstr)
    {
        int nNewLength = GetLength() + (int)_tcslen(pstr);
        if (nNewLength >= MAX_LOCAL_STRING_LEN) {
            if (m_pstr == m_szBuffer) {
                m_pstr = static_cast<LPTSTR>(malloc((nNewLength + 1) * sizeof(TCHAR)));
                _tcscpy(m_pstr, m_szBuffer);
                _tcscat(m_pstr, pstr);
            } else {
                m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (nNewLength + 1) * sizeof(TCHAR)));
                _tcscat(m_pstr, pstr);
            }
        } else {
            if (m_pstr != m_szBuffer) {
                free(m_pstr);
                m_pstr = m_szBuffer;
            }
            _tcscat(m_szBuffer, pstr);
        }
    }

    void CStringUI::Assign(LPCTSTR pstr, int cchMax)
    {
        if (pstr == NULL) pstr = _T("");
        cchMax = (cchMax < 0?(int)_tcslen(pstr):cchMax);
        if (cchMax < MAX_LOCAL_STRING_LEN) {
            if (m_pstr != m_szBuffer) {
                free(m_pstr);
                m_pstr = m_szBuffer;
            }
        } else if (cchMax > GetLength() || m_pstr == m_szBuffer) {
            if (m_pstr == m_szBuffer) m_pstr = NULL;
            m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (cchMax + 1) * sizeof(TCHAR)));
        }
        _tcsncpy(m_pstr, pstr, cchMax);
        m_pstr[cchMax] = '\0';
    }

    BOOL CStringUI::IsEmpty() const
    {
        return m_pstr[0] == '\0';
    }

    void CStringUI::Empty()
    {
        if (m_pstr != m_szBuffer) free(m_pstr);
        m_pstr = m_szBuffer;
        m_szBuffer[0] = '\0';
    }

    LPCTSTR CStringUI::GetData() const
    {
        return m_pstr;
    }

    TCHAR CStringUI::GetAt(int nIndex) const
    {
        return m_pstr[nIndex];
    }

    TCHAR CStringUI::operator[](int nIndex) const
    {
        return m_pstr[nIndex];
    }

    const CStringUI& CStringUI::operator=(const CStringUI & src)
    {
        Assign(src);
        return *this;
    }

    const CStringUI& CStringUI::operator=(LPCTSTR lpStr)
    {
        if (lpStr) {
            ASSERT(!::IsBadStringPtr(lpStr, (UINT_PTR)(-1)));
            Assign(lpStr);
        } else {
            Empty();
        }
        return *this;
    }

#ifdef _UNICODE

    const CStringUI& CStringUI::operator=(LPCSTR lpStr)
    {
        if (lpStr) {
            ASSERT(!::IsBadStringPtrA(lpStr, (UINT_PTR)(-1)));
            int cchStr = (int)strlen(lpStr) + 1;
            LPWSTR pwstr = (LPWSTR)_alloca(cchStr);
            if (pwstr != NULL) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr);
            Assign(pwstr);
        } else {
            Empty();
        }
        return *this;
    }

    const CStringUI& CStringUI::operator+=(LPCSTR lpStr)
    {
        if (lpStr) {
            ASSERT(!::IsBadStringPtrA(lpStr, (UINT_PTR)(-1)));
            int cchStr = (int)strlen(lpStr) + 1;
            LPWSTR pwstr = (LPWSTR)_alloca(cchStr);
            if (pwstr != NULL) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr);
            Append(pwstr);
        }

        return *this;
    }

#else

    const CStringUI& CStringUI::operator=(LPCWSTR lpwStr)
    {
        if (lpwStr) {
            ASSERT(!::IsBadStringPtrW(lpwStr, -1));
            int cchStr = ((int)wcslen(lpwStr) * 2) + 1;
            LPSTR pstr = (LPSTR)_alloca(cchStr);
            if (pstr != NULL) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
            Assign(pstr);
        } else {
            Empty();
        }

        return *this;
    }

    const CStringUI& CStringUI::operator+=(LPCWSTR lpwStr)
    {
        if (lpwStr) {
            ASSERT(!::IsBadStringPtrW(lpwStr, -1));
            int cchStr = ((int)wcslen(lpwStr) * 2) + 1;
            LPSTR pstr = (LPSTR)_alloca(cchStr);
            if (pstr != NULL) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
            Append(pstr);
        }

        return *this;
    }

#endif // _UNICODE

    const CStringUI& CStringUI::operator=(const TCHAR ch)
    {
        Empty();
        m_szBuffer[0] = ch;
        m_szBuffer[1] = '\0';
        return *this;
    }

    CStringUI CStringUI::operator+(const CStringUI & src) const
    {
        CStringUI sTemp = *this;
        sTemp.Append(src);
        return sTemp;
    }

    CStringUI CStringUI::operator+(LPCTSTR lpStr) const
    {
        if (lpStr) {
            ASSERT(!::IsBadStringPtr(lpStr, (UINT_PTR)(-1)));
            CStringUI sTemp = *this;
            sTemp.Append(lpStr);
            return sTemp;
        }

        return *this;
    }

    const CStringUI& CStringUI::operator+=(const CStringUI & src)
    {
        Append(src);
        return *this;
    }

    const CStringUI& CStringUI::operator+=(LPCTSTR lpStr)
    {
        if (lpStr) {
            ASSERT(!::IsBadStringPtr(lpStr, (UINT_PTR)(-1)));
            Append(lpStr);
        }

        return *this;
    }

    const CStringUI& CStringUI::operator+=(const TCHAR ch)
    {
        TCHAR str[] = {ch, '\0'};
        Append(str);
        return *this;
    }

    BOOL CStringUI::operator==(LPCTSTR str) const { return (Compare(str) == 0); };
    BOOL CStringUI::operator!=(LPCTSTR str) const { return (Compare(str) != 0); };
    BOOL CStringUI::operator<=(LPCTSTR str) const { return (Compare(str) <= 0); };
    BOOL CStringUI::operator<(LPCTSTR str) const { return (Compare(str) < 0); };
    BOOL CStringUI::operator>=(LPCTSTR str) const { return (Compare(str) >= 0); };
    BOOL CStringUI::operator>(LPCTSTR str) const { return (Compare(str) > 0); };

    void CStringUI::SetAt(int nIndex, TCHAR ch)
    {
        ASSERT(nIndex >= 0 && nIndex < GetLength());
        m_pstr[nIndex] = ch;
    }

    int CStringUI::Compare(LPCTSTR lpsz) const
    {
        return _tcscmp(m_pstr, lpsz);
    }

    int CStringUI::CompareNoCase(LPCTSTR lpsz) const
    {
        return _tcsicmp(m_pstr, lpsz);
    }

    void CStringUI::MakeUpper()
    {
        _tcsupr(m_pstr);
    }

    void CStringUI::MakeLower()
    {
        _tcslwr(m_pstr);
    }

    CStringUI CStringUI::Left(int iLength) const
    {
        if (iLength < 0) iLength = 0;
        if (iLength > GetLength()) iLength = GetLength();
        return CStringUI(m_pstr, iLength);
    }

    CStringUI CStringUI::Mid(int iPos, int iLength) const
    {
        if (iLength < 0) iLength = GetLength() - iPos;
        if (iPos + iLength > GetLength()) iLength = GetLength() - iPos;
        if (iLength <= 0) return CStringUI();
        return CStringUI(m_pstr + iPos, iLength);
    }

    CStringUI CStringUI::Right(int iLength) const
    {
        int iPos = GetLength() - iLength;
        if (iPos < 0) {
            iPos = 0;
            iLength = GetLength();
        }
        return CStringUI(m_pstr + iPos, iLength);
    }

    int CStringUI::Find(TCHAR ch, int iPos /*= 0*/) const
    {
        ASSERT(iPos >= 0 && iPos <= GetLength());
        if (iPos != 0 && (iPos < 0 || iPos >= GetLength())) return -1;
        LPCTSTR p = _tcschr(m_pstr + iPos, ch);
        if (p == NULL) return -1;
        return (int)(p - m_pstr);
    }

    int CStringUI::Find(LPCTSTR pstrSub, int iPos /*= 0*/) const
    {
        ASSERT(!::IsBadStringPtr(pstrSub, UINT_PTR(-1)));
        ASSERT(iPos >= 0 && iPos <= GetLength());
        if (iPos != 0 && (iPos < 0 || iPos > GetLength())) return -1;
        LPCTSTR p = _tcsstr(m_pstr + iPos, pstrSub);
        if (p == NULL) return -1;
        return (int)(p - m_pstr);
    }

    int CStringUI::ReverseFind(TCHAR ch) const
    {
        LPCTSTR p = _tcsrchr(m_pstr, ch);
        if (p == NULL) return -1;
        return (int)(p - m_pstr);
    }

    int CStringUI::Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo)
    {
        CStringUI sTemp;
        int nCount = 0;
        int iPos = Find(pstrFrom);
        if (iPos < 0) return 0;
        int cchFrom = (int)_tcslen(pstrFrom);
        int cchTo = (int)_tcslen(pstrTo);
        while (iPos >= 0) {
            sTemp = Left(iPos);
            sTemp += pstrTo;
            sTemp += Mid(iPos + cchFrom);
            Assign(sTemp);
            iPos = Find(pstrFrom, iPos + cchTo);
            nCount++;
        }
        return nCount;
    }

    int CStringUI::Format(LPCTSTR pstrFormat, ...)
    {
        int nRet;
        va_list Args;

        va_start(Args, pstrFormat);
        nRet = InnerFormat(pstrFormat, Args);
        va_end(Args);

        return nRet;
    }

    int CStringUI::SmallFormat(LPCTSTR pstrFormat, ...)
    {
        CStringUI sFormat = pstrFormat;
        TCHAR szBuffer[64] = {0};
        va_list argList;
        va_start(argList, pstrFormat);
        int iRet = ::_vsntprintf(szBuffer, sizeof(szBuffer), sFormat, argList);
        va_end(argList);
        Assign(szBuffer);
        return iRet;
    }

    int CStringUI::InnerFormat(LPCTSTR pstrFormat, va_list Args)
    {
#if _MSC_VER <= 1400
        TCHAR * szBuffer = NULL;
        int size = 512, nLen, counts;
        szBuffer = (TCHAR*)malloc(size);
        ZeroMemory(szBuffer, size);
        while (TRUE) {
            counts = size / sizeof(TCHAR);
            nLen = _vsntprintf(szBuffer, counts, pstrFormat, Args);
            if (nLen != -1 && nLen < counts) {
                break;
            }
            if (nLen == -1) {
                size *= 2;
            } else {
                size += 1 * sizeof(TCHAR);
            }

            if ((szBuffer = (TCHAR*)realloc(szBuffer, size)) != NULL) {
                ZeroMemory(szBuffer, size);
            } else {
                break;
            }
        }

        Assign(szBuffer);
        free(szBuffer);
        return nLen;
#else
        int nLen, totalLen;
        TCHAR* szBuffer;
        nLen = _vsntprintf(NULL, 0, pstrFormat, Args);
        totalLen = (nLen + 1) * sizeof(TCHAR);
        szBuffer = (TCHAR*)malloc(totalLen);
        ZeroMemory(szBuffer, totalLen);
        nLen = _vsntprintf(szBuffer, nLen + 1, pstrFormat, Args);
        Assign(szBuffer);
        free(szBuffer);
        return nLen;

#endif
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    //

    static UINT HashKey(LPCTSTR Key)
    {
        UINT i = 0;
        SIZE_T len = _tcslen(Key);
        while (len-- > 0)
            i = (i << 5) + i + Key[len];
        return i;
    }

    static UINT HashKey(const CStringUI & Key)
    {
        return HashKey((LPCTSTR)Key);
    };

    CStringMapUI::CStringMapUI(int nSize) : m_nCount(0)
    {
        if (nSize < 16) nSize = 16;
        m_nBuckets = nSize;
        m_aT = new TITEM_UI * [nSize];
        memset(m_aT, 0, nSize * sizeof(TITEM_UI*));
    }

    CStringMapUI::~CStringMapUI()
    {
        if (m_aT) {
            int len = m_nBuckets;
            while (len--) {
                TITEM_UI* pItem = m_aT[len];
                while (pItem) {
                    TITEM_UI* pKill = pItem;
                    pItem = pItem->pNext;
                    delete pKill;
                }
            }
            delete[] m_aT;
            m_aT = NULL;
        }
    }

    void CStringMapUI::RemoveAll()
    {
        this->Resize(m_nBuckets);
    }

    void CStringMapUI::Resize(int nSize)
    {
        if (m_aT) {
            int len = m_nBuckets;
            while (len--) {
                TITEM_UI* pItem = m_aT[len];
                while (pItem) {
                    TITEM_UI* pKill = pItem;
                    pItem = pItem->pNext;
                    delete pKill;
                }
            }
            delete[] m_aT;
            m_aT = NULL;
        }

        if (nSize < 0) nSize = 0;
        if (nSize > 0) {
            m_aT = new TITEM_UI * [nSize];
            memset(m_aT, 0, nSize * sizeof(TITEM_UI*));
        }
        m_nBuckets = nSize;
        m_nCount = 0;
    }

    LPVOID CStringMapUI::Find(LPCTSTR key, BOOL optimize) const
    {
        if (m_nBuckets == 0 || GetSize() == 0) return NULL;

        UINT slot = HashKey(key) % m_nBuckets;
        for (TITEM_UI* pItem = m_aT[slot]; pItem; pItem = pItem->pNext) {
            if (pItem->Key == key) {
                if (optimize && pItem != m_aT[slot]) {
                    if (pItem->pNext) {
                        pItem->pNext->pPrev = pItem->pPrev;
                    }
                    pItem->pPrev->pNext = pItem->pNext;
                    pItem->pPrev = NULL;
                    pItem->pNext = m_aT[slot];
                    pItem->pNext->pPrev = pItem;
                    //将item移动至链条头部
                    m_aT[slot] = pItem;
                }
                return pItem->Data;
            }
        }

        return NULL;
    }

    BOOL CStringMapUI::Insert(LPCTSTR key, LPVOID pData)
    {
        if (m_nBuckets == 0) return FALSE;
        if (Find(key)) return FALSE;

        // Add first in bucket
        UINT slot = HashKey(key) % m_nBuckets;
        TITEM_UI * pItem = new TITEM_UI;
        pItem->Key = key;
        pItem->Data = pData;
        pItem->pPrev = NULL;
        pItem->pNext = m_aT[slot];
        if (pItem->pNext)
            pItem->pNext->pPrev = pItem;
        m_aT[slot] = pItem;
        m_nCount++;
        return true;
    }

    LPVOID CStringMapUI::Set(LPCTSTR key, LPVOID pData)
    {
        if (m_nBuckets == 0) return pData;

        if (GetSize() > 0) {
            UINT slot = HashKey(key) % m_nBuckets;
            // Modify existing item
            for (TITEM_UI* pItem = m_aT[slot]; pItem; pItem = pItem->pNext) {
                if (pItem->Key == key) {
                    LPVOID pOldData = pItem->Data;
                    pItem->Data = pData;
                    return pOldData;
                }
            }
        }

        Insert(key, pData);
        return NULL;
    }

    BOOL CStringMapUI::Remove(LPCTSTR key)
    {
        if (m_nBuckets == 0 || GetSize() == 0) return FALSE;

        UINT slot = HashKey(key) % m_nBuckets;
        TITEM_UI * *ppItem = &m_aT[slot];
        while (*ppItem) {
            if ((*ppItem)->Key == key) {
                TITEM_UI* pKill = *ppItem;
                *ppItem = (*ppItem)->pNext;
                if (*ppItem)
                    (*ppItem)->pPrev = pKill->pPrev;
                delete pKill;
                m_nCount--;
                return true;
            }
            ppItem = &((*ppItem)->pNext);
        }

        return FALSE;
    }

    int CStringMapUI::GetSize() const
    {
#if 0 //def _DEBUG
        int nCount = 0;
        int len = m_nBuckets;
        while (len--) {
            for (const TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext) nCount++;
        }
        ASSERT(m_nCount == nCount);
#endif
        return m_nCount;
    }

    LPCTSTR CStringMapUI::GetAt(int iIndex) const
    {
        if (m_nBuckets == 0 || GetSize() == 0) return FALSE;

        int pos = 0;
        int len = m_nBuckets;
        while (len--) {
            for (TITEM_UI* pItem = m_aT[len]; pItem; pItem = pItem->pNext) {
                if (pos++ == iIndex) {
                    return pItem->Key.GetData();
                }
            }
        }

        return NULL;
    }

    LPCTSTR CStringMapUI::operator[](int nIndex) const
    {
        return GetAt(nIndex);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //

    CWaitCursorUI::CWaitCursorUI()
    {
        m_hOrigCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
    }

    CWaitCursorUI::~CWaitCursorUI()
    {
        ::SetCursor(m_hOrigCursor);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //
    //
    //CImageString::CImageString()
    //{
    //	Clear();
    //}

    //CImageString::CImageString(const CImageString& image)
    //{
    //	Clone(image);
    //}

    //const CImageString& CImageString::operator=(const CImageString& image)
    //{
    //	Clone(image);
    //	return *this;
    //}

    //void CImageString::Clone(const CImageString& image)
    //{
    //	m_sImageAttribute = image.m_sImageAttribute;

    //	m_sImage = image.m_sImage;
    //	m_sResType = image.m_sResType;
    //	m_imageInfo = image.m_imageInfo;
    //	m_bLoadSuccess = image.m_bLoadSuccess;

    //	m_rcDest = image.m_rcDest;
    //	m_rcSource = image.m_rcSource;
    //	m_rcCorner = image.m_rcCorner;
    //	m_bFade = image.m_bFade;
    //	m_dwMask = image.m_dwMask;
    //	m_bHole = image.m_bHole;
    //	m_bTiledX = image.m_bTiledX;
    //	m_bTiledY = image.m_bTiledY;
    //}

    //CImageString::~CImageString()
    //{

    //}

    //const CStringUI& CImageString::GetAttributeString() const
    //{
    //	return m_sImageAttribute;
    //}

    //void CImageString::SetAttributeString(LPCTSTR pStrImageAttri)
    //{
    //	if (m_sImageAttribute == pStrImageAttri) return;
    //	Clear();
    //	m_sImageAttribute = pStrImageAttri;
    //	m_sImage = m_sImageAttribute;
    //}

    //BOOL CImageString::LoadImage(CManagerUI* pManager)
    //{
    //	m_imageInfo = NULL;
    //	m_bLoadSuccess = true;
    //	ZeroMemory(&m_rcDest, sizeof(RECT));
    //	ZeroMemory(&m_rcSource, sizeof(RECT));
    //	ZeroMemory(&m_rcCorner, sizeof(RECT));
    //	m_bFade = 0xFF;
    //	m_dwMask = 0;
    //	m_bHole = FALSE;
    //	m_bTiledX = FALSE;
    //	m_bTiledY = FALSE;
    //	ParseAttribute(m_sImageAttribute,*pManager->GetDPIObj());
    //	if (!m_bLoadSuccess) return FALSE;

    //	const TIMAGEINFO_UI* data = NULL;
    //	if (m_sResType.IsEmpty())
    //	{
    //		data = pManager->GetImageEx((LPCTSTR)m_sImage, NULL, m_dwMask);
    //	}
    //	else
    //	{
    //		data = pManager->GetImageEx((LPCTSTR)m_sImage, (LPCTSTR)m_sResType, m_dwMask);
    //	}
    //	if (data == NULL)
    //	{
    //		m_bLoadSuccess = FALSE;
    //		return FALSE;
    //	}
    //	else
    //	{
    //		m_bLoadSuccess = true;
    //	}

    //	if (m_rcSource.left == 0 && m_rcSource.right == 0 && m_rcSource.top == 0 && m_rcSource.bottom == 0)
    //	{
    //		m_rcSource.right = data->nX;
    //		m_rcSource.bottom = data->nY;
    //	}
    //	if (m_rcSource.right > data->nX) m_rcSource.right = data->nX;
    //	if (m_rcSource.bottom > data->nY) m_rcSource.bottom = data->nY;
    //	m_imageInfo = const_cast<TIMAGEINFO_UI*>(data);

    //	return true;
    //}

    //BOOL CImageString::IsLoadSuccess()
    //{
    //	return !m_sImageAttribute.IsEmpty() && m_bLoadSuccess;
    //}

    //void CImageString::ModifyAttribute(LPCTSTR pStrModify)
    //{
    //	//ParseAttribute(pStrModify);
    //}

    //void CImageString::Clear()
    //{
    //	m_sImageAttribute.Empty();
    //	m_sImage.Empty();
    //	m_sResType.Empty();
    //	m_imageInfo = NULL;
    //	m_bLoadSuccess = true;
    //	ZeroMemory(&m_rcDest, sizeof(RECT));
    //	ZeroMemory(&m_rcSource, sizeof(RECT));
    //	ZeroMemory(&m_rcCorner, sizeof(RECT));
    //	m_bFade = 0xFF;
    //	m_dwMask = 0;
    //	m_bHole = FALSE;
    //	m_bTiledX = FALSE;
    //	m_bTiledY = FALSE;
    //}

    //void CImageString::ParseAttribute(LPCTSTR pStrImage)
    //{
    //	if (pStrImage == NULL)
    //		return;

    //	// 1、aaa.jpg
    //	// 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0'
    //	// mask='#FF0000' fade='255' hole='FALSE' xtiled='FALSE' ytiled='FALSE'
    //	CStringUI sItem;
    //	CStringUI sValue;
    //	LPTSTR pstr = NULL;

    //	while (*pStrImage != _T('\0'))
    //	{
    //		sItem.Empty();
    //		sValue.Empty();
    //		while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
    //		while (*pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' '))
    //		{
    //			LPTSTR pstrTemp = ::CharNext(pStrImage);
    //			while (pStrImage < pstrTemp)
    //			{
    //				sItem += *pStrImage++;
    //			}
    //		}
    //		while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
    //		if (*pStrImage++ != _T('=')) break;
    //		while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
    //		if (*pStrImage++ != _T('\'')) break;
    //		while (*pStrImage != _T('\0') && *pStrImage != _T('\''))
    //		{
    //			LPTSTR pstrTemp = ::CharNext(pStrImage);
    //			while (pStrImage < pstrTemp)
    //			{
    //				sValue += *pStrImage++;
    //			}
    //		}
    //		if (*pStrImage++ != _T('\'')) break;
    //		if (!sValue.IsEmpty())
    //		{
    //			if (sItem == _T("file") || sItem == _T("res"))
    //			{
    //				m_sImage = sValue;
    //				//if (g_Dpi.GetScale() != 100) {
    //				//	std::wstringstream wss;
    //				//	wss << L"@" << g_Dpi.GetScale() << L".";
    //				//	std::wstring suffix = wss.str();
    //				//	m_sImage.Replace(L".", suffix.c_str());
    //				//}
    //			}
    //			else if (sItem == _T("restype"))
    //			{
    //				m_sResType = sValue;
    //			}
    //			else if (sItem == _T("dest"))
    //			{
    //				m_rcDest.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
    //				m_rcDest.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
    //				m_rcDest.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
    //				m_rcDest.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);

    //				//g_Dpi.ScaleRect(&m_rcDest);
    //			}
    //			else if (sItem == _T("source"))
    //			{
    //				m_rcSource.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
    //				m_rcSource.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
    //				m_rcSource.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
    //				m_rcSource.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
    //				//g_Dpi.ScaleRect(&m_rcSource);
    //			}
    //			else if (sItem == _T("corner"))
    //			{
    //				m_rcCorner.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);
    //				m_rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
    //				m_rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
    //				m_rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
    //				//g_Dpi.ScaleRect(&m_rcCorner);
    //			}
    //			else if (sItem == _T("mask"))
    //			{
    //				if (sValue[0] == _T('#')) m_dwMask = _tcstoul(sValue.GetData() + 1, &pstr, 16);
    //				else m_dwMask = _tcstoul(sValue.GetData(), &pstr, 16);
    //			}
    //			else if (sItem == _T("fade"))
    //			{
    //				m_bFade = (BYTE)_tcstoul(sValue.GetData(), &pstr, 10);
    //			}
    //			else if (sItem == _T("hole"))
    //			{
    //				m_bHole = (_tcscmp(sValue.GetData(), _T("true")) == 0);
    //			}
    //			else if (sItem == _T("xtiled"))
    //			{
    //				m_bTiledX = (_tcscmp(sValue.GetData(), _T("true")) == 0);
    //			}
    //			else if (sItem == _T("ytiled"))
    //			{
    //				m_bTiledY = (_tcscmp(sValue.GetData(), _T("true")) == 0);
    //			}
    //		}
    //		if (*pStrImage++ != _T(' ')) break;
    //	}
    //}

    //void CImageString::SetDest(const RECT &rcDest)
    //{
    //	m_rcDest = rcDest;
    //}

    //RECT CImageString::GetDest() const
    //{
    //	return m_rcDest;
    //}

    //const TIMAGEINFO_UI* CImageString::GeTIMAGEINFO_UI() const
    //{
    //	return m_imageInfo;
    //}


    //TURI_UI::TURI_UI(LPCTSTR uri)
    //{
    //
    //}
    //
    //CStringUI TURI_UI::AsString()
    //{
    //
    //}


    CFileReaderUI::CFileReaderUI(const CStringUI& sFileURI)
    {
        this->m_sFileName = sFileURI;
        this->m_hFile = INVALID_HANDLE_VALUE;
    }

    CFileReaderUI::~CFileReaderUI()
    {
        if (INVALID_HANDLE_VALUE != this->m_hFile) {
            ::CloseHandle(this->m_hFile);
            this->m_hFile = INVALID_HANDLE_VALUE;
        }
    }

    BOOL CFileReaderUI::_Failed(LPCTSTR pstrError)
    {
        this->m_sError = ((NULL == pstrError)?_T(""):pstrError);
        return FALSE; // Always return 'FALSE'
    }

    const CStringUI & CFileReaderUI::Error()
    {
        return m_sError;
    }

    BOOL CFileReaderUI::Open(void* ctx)
    {
        this->m_hFile = ::CreateFile(this->m_sFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (this->m_hFile == INVALID_HANDLE_VALUE) {
            return _Failed(_T("Error opening file"));
        }

        return TRUE;
    }

    BOOL  CFileReaderUI::Read(BYTE * pCache, DWORD dwCacheSize, DWORD * pdwReadSize)
    {
        BOOL bResult = ::ReadFile(this->m_hFile, pCache, dwCacheSize, pdwReadSize, NULL);
        if (FALSE != bResult) {
            _Failed(_T("Could not read file"));
            return -1;
        }

        return (bResult);
    }

    void CFileReaderUI::Close()
    {
        ::CloseHandle(this->m_hFile);
        this->m_hFile = INVALID_HANDLE_VALUE;
    }

    CZipFileReaderUI::CZipFileReaderUI(const CStringUI& sFilePath)
        : CFileReaderUI(sFilePath)
    {
        //  file://d:/xxx/yyy/zzz
        //  zip://d:/xxx/yyy/zzz
        m_sFileName = sFilePath;
        m_sPassword = "";   // TODO
        m_hFile = INVALID_HANDLE_VALUE;
        m_bAttached = FALSE;
    }

    CZipFileReaderUI::CZipFileReaderUI(HANDLE hZipFile)
        : CFileReaderUI(_T(""))
    {
        m_hFile = hZipFile;
        m_bAttached = TRUE;
    }

    BOOL CZipFileReaderUI::Open(void* ctx)
    {
        if (INVALID_HANDLE_VALUE == m_hFile) {
            char* pwd = w2a((wchar_t*)m_sPassword.GetData());
            if (NULL == pwd) {
                return FALSE;
            }
            
            m_hFile = OpenZip(m_sFileName.GetData(), pwd);
            delete[] pwd;
            if (m_hFile == INVALID_HANDLE_VALUE) {
                return FALSE;
            }
        }
    
        if (m_hFile == NULL) {
            return _Failed(_T("Error opening zip file"));
        }

        return TRUE;
    }

    BOOL CZipFileReaderUI::Read(BYTE * pCache, DWORD dwCacheSize, DWORD * pdwReadSize)
    {
        if (NULL == m_cCache) {
            ZIPENTRY tEntry;
            CStringUI key = m_sFileName;
            key.Replace(_T("\\"), _T("/"));
            int i = 0;
            if (FindZipItem((HZIP)m_hFile, key, true, &i, &tEntry) != 0) {
                return _Failed(_T("Could not find ziped file"));
            }

            DWORD dwSize = tEntry.unc_size;
            if (dwSize == 0) {
                return _Failed(_T("File is empty"));
            }

            BYTE* cCache = new BYTE[dwSize];
            int res = UnzipItem((HZIP)m_hFile, i, cCache, dwSize);
            if ((res != 0x00000000) && (res != 0x00000600)) {
                delete[] cCache;
                return _Failed(_T("Could not unzip file"));
            }

            m_dwLen = dwSize;
            m_dwPos = 0;
            m_cCache = cCache;
        }

        DWORD dwRemainLen = m_dwLen - m_dwPos;
        DWORD dwCopyLen = dwRemainLen;
        if (dwRemainLen > *pdwReadSize) {
            dwCopyLen = *pdwReadSize;
        }

        ::CopyMemory(pCache, m_cCache, dwCopyLen);
        *pdwReadSize = dwCopyLen;
        m_dwPos += dwCopyLen;
        return TRUE;
    }

    void CZipFileReaderUI::Close()
    {
        if (m_hFile != INVALID_HANDLE_VALUE) {
            if (!m_bAttached) {
                ::CloseZip((HZIP)m_hFile);
                m_hFile = INVALID_HANDLE_VALUE;
            }
        }
    }



} // namespace DUI
