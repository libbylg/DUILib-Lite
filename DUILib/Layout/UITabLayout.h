#ifndef __UITABLAYOUT_H__
#define __UITABLAYOUT_H__

#include "Core/UIContainer.h"

namespace DUI
{
    class DUILIB_API CTabLayoutUI : public CContainerUI
    {
        UI_DECLARE_CONTROL(CTabLayoutUI)
    public:
        CTabLayoutUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        BOOL Add(CControlUI* pControl);
        BOOL AddAt(CControlUI* pControl, int iIndex);
        BOOL Remove(CControlUI* pControl);
        void RemoveAll();
        int GetCurSel() const;
        virtual BOOL SelectItem(int iIndex);
        virtual BOOL SelectItem(CControlUI* pControl);

        void SetPos(RECT rc, BOOL bNeedInvalidate = true);

        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    protected:
        int m_iCurSel;
    };
}
#endif // __UITABLAYOUT_H__
