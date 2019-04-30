#ifndef _UIControlFacoty_H_
#define _UIControlFacoty_H_

#include "UILIB.h"
#include "Utils/Utils.h"

#include <map>

namespace DuiLib 
{
	typedef CControlUI* (*CreateClass)();
	typedef std::map<CDuiString, CreateClass> MAP_DUI_CTRATECLASS;

	class UILIB_API CControlFactoryUI
	{
	public:
		CControlUI* CreateControl(CDuiString strClassName);
		void RegistControl(CDuiString strClassName, CreateClass pFunc);

		static CControlFactoryUI* GetInstance();
		void Release();

	private:	
		CControlFactoryUI();
		virtual ~CControlFactoryUI();

	private:
		MAP_DUI_CTRATECLASS m_mapControl;
	};




#define REGIST_DUICONTROL(class_name)\
	CControlFactoryUI::GetInstance()->RegistControl(_T(#class_name), (CreateClass)class_name::CreateControl);

#define INNER_REGISTER_DUICONTROL(class_name)\
	RegistControl(_T(#class_name), (CreateClass)class_name::CreateControl);
}


#endif//_UIControlFacoty_H_



