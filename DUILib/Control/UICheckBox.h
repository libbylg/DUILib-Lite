#ifndef __UICHECKBOX_H_
#define __UICHECKBOX_H_

#include "Control/UIOption.h"

namespace DUI
{
    class DUILIB_API CCheckBoxUI : public COptionUI
    {
        UI_DECLARE_CONTROL(CCheckBoxUI)
    public:
        CCheckBoxUI();

    public:
        virtual LPCTSTR GetClass() const;
        virtual LPVOID GetInterface(LPCTSTR pstrName);

        void SetCheck(BOOL bCheck);
        BOOL GetCheck() const;

    public:
        virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
        void SetAutoCheck(BOOL bEnable);
        virtual void DoEvent(struct TEVENT_UI& event);
        virtual void Selected(BOOL bSelected, BOOL bMsg = TRUE);

    protected:
        BOOL m_bAutoCheck;
    };

};


#endif//__UICHECKBOX_H_
