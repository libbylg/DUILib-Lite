#ifndef __UITEXT_H__
#define __UITEXT_H__

#include "Control/UILabel.h"

namespace DUI
{
    class DUILIB_API CTextUI : public CLabelUI
    {
        UI_DECLARE_CONTROL(CTextUI)

    public:
        CTextUI();
        ~CTextUI();

        LPCTSTR GetClass() const;
        UINT GetControlFlags() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        CStringUI* GetLinkContent(int iIndex);

        void DoEvent(struct TEVENT_UI& event);
        SIZE EstimateSize(SIZE szAvailable);

        void PaintText(HDC hDC);

    protected:
        enum { MAX_LINK = 8 };
        int m_nLinks;
        RECT m_rcLinks[MAX_LINK];
        CStringUI m_sLinks[MAX_LINK];
        int m_nHoverLink;
    };

} // namespace DUI

#endif //__UITEXT_H__