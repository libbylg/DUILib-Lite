#ifndef __UIANIMATIONTABLAYOUT_H__
#define __UIANIMATIONTABLAYOUT_H__

#include "Layout/UITabLayout.h"
#include "Control/UIAnimation.h"

namespace DUI
{
    class DUILIB_API CAnimationTabLayoutUI : public CTabLayoutUI, public CAnimationUI
    {
        UI_DECLARE_CONTROL(CAnimationTabLayoutUI)
    public:
        CAnimationTabLayoutUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        BOOL SelectItem(int iIndex);
        void AnimationSwitch();
        void DoEvent(TEVENT_UI& event);
        void OnTimer(int nTimerID);

        virtual void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop) {}
        virtual void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
        virtual void OnAnimationStop(INT nAnimationID);

        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    protected:
        BOOL m_bIsVerticalDirection;
        int m_nPositiveDirection;
        RECT m_rcCurPos;
        RECT m_rcItemOld;
        CControlUI* m_pCurrentControl;
        BOOL m_bControlVisibleFlag;
        enum
        {
            TAB_ANIMATION_ID = 1,

            TAB_ANIMATION_ELLAPSE = 10,
            TAB_ANIMATION_FRAME_COUNT = 15,
        };
    };
}
#endif // __UIANIMATIONTABLAYOUT_H__