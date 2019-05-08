#ifndef _UIFacoty_H_
#define _UIFacoty_H_

#include "Core/UIDefine.h"

namespace DUI
{
    typedef CControlUI* (*PCREATECLASS_UI)();

    class DUILIB_API CFactoryUI : public IFactoryUI
    {
        typedef std::map<CStringUI, PCREATECLASS_UI> MAP_CTREATECLASS_UI;

    public:
        virtual CControlUI* CreateControl(LPCTSTR pstrClass);
        void RegistControl(const CStringUI& strClassName, PCREATECLASS_UI pFunc);
        void Release();

        static CFactoryUI* GetInstance();

    private:
        CFactoryUI();
        virtual ~CFactoryUI();

    private:
        MAP_CTREATECLASS_UI m_mapControl;
    };




#define UI_REGISTER_CONTROL(class_name)\
	CFactoryUI::GetInstance()->RegistControl(_T(#class_name), (PCREATECLASS_UI)class_name::CreateControl);



}//namespace DUI


#endif//_UIControlFacoty_H_



