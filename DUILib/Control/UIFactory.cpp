#include "Control/UIFactory.h"
#include "Control/UIButton.h"
#include "Control/UICombo.h"
#include "Control/UIComboBox.h"
#include "Control/UIDateTime.h"
#include "Control/UIEdit.h"
#include "Control/UIGroupBox.h"
#include "Control/UIMenu.h"
#include "Control/UIOption.h"
#include "Control/UICheckBox.h"
#include "Control/UIProgress.h"
#include "Control/UISlider.h"
#include "Control/UIText.h"
#include "Control/UITreeView.h"
#include "Control/UIRollText.h"
#include "Control/UIColorPalette.h"
#include "Control/UIHotKey.h"
#include "Control/UIFadeButton.h"
#include "Control/UIRing.h"

#include "Layout/UIAnimationTabLayout.h"
#include "Layout/UIChildLayout.h"
#include "Layout/UIHorizontalLayout.h"
#include "Layout/UITabLayout.h"
#include "Layout/UITileLayout.h"

#include "Core/UIControl.h"
#include "Core/UIContainer.h"
#include "Core/UIScrollBar.h"


namespace DUI
{

#define UI_REGISTER_CONTROL_INNER(class_name)\
	RegistControl(_T(#class_name), (PCREATECLASS_UI)class_name::CreateControl);

    CFactoryUI::CFactoryUI()
    {
        UI_REGISTER_CONTROL_INNER(CControlUI);
        UI_REGISTER_CONTROL_INNER(CContainerUI);
        UI_REGISTER_CONTROL_INNER(CButtonUI);
        UI_REGISTER_CONTROL_INNER(CComboUI);
        UI_REGISTER_CONTROL_INNER(CComboBoxUI);
        UI_REGISTER_CONTROL_INNER(CDateTimeUI);
        UI_REGISTER_CONTROL_INNER(CEditUI);
        //UI_REGISTER_CONTROL_INNER(CActiveXUI);
        //UI_REGISTER_CONTROL_INNER(CFlashUI);
        //UI_REGISTER_CONTROL_INNER(CGifAnimUI);
//#ifdef USE_XIMAGE_EFFECT
//		UI_REGISTER_CONTROL_INNER(CGifAnimExUI);
//#endif
        UI_REGISTER_CONTROL_INNER(CGroupBoxUI);
        //UI_REGISTER_CONTROL_INNER(CIPAddressUI);
        //UI_REGISTER_CONTROL_INNER(CIPAddressExUI);
        UI_REGISTER_CONTROL_INNER(CLabelUI);
        UI_REGISTER_CONTROL_INNER(CListUI);
        UI_REGISTER_CONTROL_INNER(CListHeaderUI);
        UI_REGISTER_CONTROL_INNER(CListHeaderItemUI);
        UI_REGISTER_CONTROL_INNER(CListLabelElementUI);
        UI_REGISTER_CONTROL_INNER(CListTextElementUI);
        UI_REGISTER_CONTROL_INNER(CListContainerElementUI);
        UI_REGISTER_CONTROL_INNER(CMenuUI);
        //UI_REGISTER_CONTROL_INNER(CMenuElementUI);
        UI_REGISTER_CONTROL_INNER(COptionUI);
        UI_REGISTER_CONTROL_INNER(CCheckBoxUI);
        UI_REGISTER_CONTROL_INNER(CProgressUI);
        //UI_REGISTER_CONTROL_INNER(CRichEditUI);
        UI_REGISTER_CONTROL_INNER(CScrollBarUI);
        UI_REGISTER_CONTROL_INNER(CSliderUI);
        UI_REGISTER_CONTROL_INNER(CTextUI);
        UI_REGISTER_CONTROL_INNER(CTreeNodeUI);
        UI_REGISTER_CONTROL_INNER(CTreeViewUI);
        //UI_REGISTER_CONTROL_INNER(CWebBrowserUI);
        UI_REGISTER_CONTROL_INNER(CAnimationTabLayoutUI);
        UI_REGISTER_CONTROL_INNER(CChildLayoutUI);
        UI_REGISTER_CONTROL_INNER(CHorizontalLayoutUI);
        UI_REGISTER_CONTROL_INNER(CTabLayoutUI);
        UI_REGISTER_CONTROL_INNER(CTileLayoutUI);
        UI_REGISTER_CONTROL_INNER(CVerticalLayoutUI);
        UI_REGISTER_CONTROL_INNER(CRollTextUI);
        UI_REGISTER_CONTROL_INNER(CColorPaletteUI);
        //UI_REGISTER_CONTROL_INNER(CListExUI);
        //UI_REGISTER_CONTROL_INNER(CListContainerHeaderItemUI);
        //UI_REGISTER_CONTROL_INNER(CListTextExtElementUI);
        UI_REGISTER_CONTROL_INNER(CHotKeyUI);
        UI_REGISTER_CONTROL_INNER(CFadeButtonUI);
        UI_REGISTER_CONTROL_INNER(CRingUI);
    }

    CFactoryUI::~CFactoryUI()
    {
    }

    CControlUI* CFactoryUI::CreateControl(CStringUI strClassName)
    {
        strClassName.MakeLower();
        MAP_CTREATECLASS_UI::iterator iter = m_mapControl.find(strClassName);
        if (iter == m_mapControl.end()) {
            return NULL;
        } else {
            return (CControlUI*)(iter->second());
        }
    }

    void CFactoryUI::RegistControl(CStringUI strClassName, PCREATECLASS_UI pFunc)
    {
        strClassName.MakeLower();
        m_mapControl.insert(MAP_CTREATECLASS_UI::value_type(strClassName, pFunc));
    }

    CFactoryUI* CFactoryUI::GetInstance()
    {
        static CFactoryUI* pInstance = new CFactoryUI;
        return pInstance;
    }

    void CFactoryUI::Release()
    {
        delete this;
    }
}