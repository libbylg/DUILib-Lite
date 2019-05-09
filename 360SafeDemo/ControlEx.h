#ifndef __ControlEx_H_
#define __ControlEx_H_

#include "Core/UIContainer.h"
#include "Control/UIDialogBuilder.h"

using namespace DUI;

//class DUI_API CContainerUI;

class ComputerExamineUI : public CContainerUI
{
public:
	ComputerExamineUI()
	{
		CDialogBuilderUI builder;
		CContainerUI* pComputerExamine = static_cast<CContainerUI*>(builder.Create(_T("ComputerExamine.xml"), (UINT)0));
		if( pComputerExamine ) {
			this->Add(pComputerExamine);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class CDialogBuilderCallbackEx : public IFactoryUI
{
public:
	virtual CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
        if (_tcscmp(pstrClass, _T("ComputerExamine")) == 0) {
            return new ComputerExamineUI;
        }
		return NULL;
	}
};

#endif//__ControlEx_H_

