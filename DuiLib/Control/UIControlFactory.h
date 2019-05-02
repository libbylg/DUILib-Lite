#ifndef _UIControlFacoty_H_
#define _UIControlFacoty_H_

#include "Utils/Utils.h"

namespace DUI
{
    typedef CControlUI* (*CreateClass)();
    typedef std::map<CDuiString, CreateClass> MAP_CTREATECLASS_UI;

    class DUILIB_API CControlFactoryUI
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
        MAP_CTREATECLASS_UI m_mapControl;
    };




#define UI_REGISTER_CONTROL(class_name)\
	CControlFactoryUI::GetInstance()->RegistControl(_T(#class_name), (CreateClass)class_name::CreateControl);



}//namespace DUI


#endif//_UIControlFacoty_H_



