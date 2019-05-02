#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#include "Core/UIContainer.h"

namespace DUI
{
    class DUILIB_API CChildLayoutUI : public CContainerUI
    {
        UI_DECLARE_CONTROL(CChildLayoutUI)
    public:
        CChildLayoutUI();

        void Init();
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
        void SetChildLayoutXML(CStringUI pXML);
        CStringUI GetChildLayoutXML();
        virtual LPVOID GetInterface(LPCTSTR pstrName);
        virtual LPCTSTR GetClass() const;

    private:
        CStringUI m_pstrXMLFile;
    };
} // namespace DUI
#endif // __UICHILDLAYOUT_H__
