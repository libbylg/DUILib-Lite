#ifndef __UIDLGBUILDER_H__
#define __UIDLGBUILDER_H__

#include "Core/UIControl.h"
#include "Core/UIMarkup.h"

namespace DUI
{

    class CManagerUI;
    class CRenderUI;


    class DUILIB_API CDialogBuilderUI
    {
    public:
        CDialogBuilderUI();
        CControlUI* Create(TSTRID_UI xml, LPCTSTR type = NULL, IFactoryUI* pCallback = NULL, CManagerUI* pManager = NULL, CControlUI* pParent = NULL);
        CControlUI* Create(IFactoryUI* pCallback = NULL, CManagerUI* pManager = NULL, CControlUI* pParent = NULL);

        CMarkupUI* GetMarkup();

        void GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const;
        void GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const;
        void SetInstance(HINSTANCE instance) { m_instance = instance; };
    private:
        CControlUI* _Parse(CMarkupNodeUI* parent, CControlUI* pParent = NULL, CManagerUI* pManager = NULL);

        CMarkupUI   m_xml;
        IFactoryUI* m_pCallback;
        LPCTSTR     m_pstrtype;
        HINSTANCE   m_instance;
    };

} // namespace DUI

#endif // __UIDLGBUILDER_H__
