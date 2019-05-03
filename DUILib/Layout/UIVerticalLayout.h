#ifndef __UIVERTICALLAYOUT_H__
#define __UIVERTICALLAYOUT_H__

#include "Core/UIContainer.h"

namespace DUI
{
    class DUILIB_API CVerticalLayoutUI : public CContainerUI
    {
        UI_DECLARE_CONTROL(CVerticalLayoutUI)
    public:
        CVerticalLayoutUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);
        UINT GetControlFlags() const;

        void SetSepHeight(int iHeight);
        int GetSepHeight() const;
        void SetSepImmMode(BOOL bImmediately);
        BOOL IsSepImmMode() const;
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
        void DoEvent(TEVENT_UI& event);

        void SetPos(RECT rc, BOOL bNeedInvalidate = TRUE);
        void DoPostPaint(HDC hDC, const RECT& rcPaint);

        RECT GetThumbRect(BOOL bUseNew = FALSE) const;

    protected:
        int m_iSepHeight;
        UINT m_uButtonState;
        POINT ptLastMouse;
        RECT m_rcNewPos;
        BOOL m_bImmMode;
    };
}
#endif // __UIVERTICALLAYOUT_H__
