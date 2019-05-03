#ifndef __UITILELAYOUT_H__
#define __UITILELAYOUT_H__

#include "Core/UIContainer.h"

namespace DUI
{
    class DUILIB_API CTileLayoutUI : public CContainerUI
    {
        UI_DECLARE_CONTROL(CTileLayoutUI)
    public:
        CTileLayoutUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        void SetPos(RECT rc, BOOL bNeedInvalidate = TRUE);

        SIZE GetItemSize() const;
        void SetItemSize(SIZE szItem);
        int GetColumns() const;
        void SetColumns(int nCols);

        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    protected:
        SIZE m_szItem;
        int m_nColumns;
    };
}
#endif // __UITILELAYOUT_H__
