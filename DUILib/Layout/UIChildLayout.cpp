#include "Layout/UIChildLayout.h"
#include "Control/UIDialogBuilder.h"

namespace DUI
{
    UI_IMPLEMENT_CONTROL(CChildLayoutUI)

        CChildLayoutUI::CChildLayoutUI()
    {

    }

    void CChildLayoutUI::Init()
    {
        if (!m_pstrXMLFile.IsEmpty()) {
            CDialogBuilderUI builder;
            CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(m_pstrXMLFile.GetData(), (UINT)0, NULL, m_pManager));
            if (pChildWindow) {
                this->Add(pChildWindow);
            } else {
                this->RemoveAll();
            }
        }
    }

    void CChildLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if (_tcsicmp(pstrName, _T("xmlfile")) == 0)
            SetChildLayoutXML(pstrValue);
        else
            CContainerUI::SetAttribute(pstrName, pstrValue);
    }

    void CChildLayoutUI::SetChildLayoutXML(CStringUI pXML)
    {
        m_pstrXMLFile = pXML;
    }

    CStringUI CChildLayoutUI::GetChildLayoutXML()
    {
        return m_pstrXMLFile;
    }

    LPVOID CChildLayoutUI::GetInterface(LPCTSTR pstrName)
    {
        if (_tcsicmp(pstrName, UICONTROL_CHILDLAYOUT) == 0) return static_cast<CChildLayoutUI*>(this);
        return CControlUI::GetInterface(pstrName);
    }

    LPCTSTR CChildLayoutUI::GetClass() const
    {
        return _T("ChildLayoutUI");
    }
} // namespace DUI
