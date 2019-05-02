#ifndef _UIDEFINE_H_
#define _UIDEFINE_H_

#include "DUILIB.h"
#include "Utils/Utils.h"

namespace DUI
{


#define UIFONTID_MAX        30000
#define UITIMERID_CARET	    0x1999


    // 列表类型
    enum LISTTYPE_UI
    {
        UILIST_NORMAL = 0,
        UILIST_COMBO,
        UILIST_TREE,
        UIISLT_MENU,
    };


    // 鼠标光标定义
    #define  UICURSOR_ARROW          32512
    #define  UICURSOR_IBEAM          32513
    #define  UICURSOR_WAIT           32514
    #define  UICURSOR_CROSS          32515
    #define  UICURSOR_UPARROW        32516
    #define  UICURSOR_SIZE           32640
    #define  UICURSOR_ICON           32641
    #define  UICURSOR_SIZENWSE       32642
    #define  UICURSOR_SIZENESW       32643
    #define  UICURSOR_SIZEWE         32644
    #define  UICURSOR_SIZENS         32645
    #define  UICURSOR_SIZEALL        32646
    #define  UICURSOR_NO             32648
    #define  UICURSOR_HAND           32649


    // 消息类型
    enum SIGTYPE_UI
    {
        UISIG_end = 0, // [marks end of message map]
        UISIG_lwl,     // LRESULT (WPARAM, LPARAM)
        UISIG_vn,      // void (TNotifyUI)
    };


    // 核心控件
    class CControlUI;


    //定义所有消息类型
#define UIMSG_MENU                  (_T("menu"))
#define UIMSG_LINK                  (_T("link"))

#define UIMSG_TIMER                 (_T("timer"))
#define UIMSG_CLICK                 (_T("click"))
#define UIMSG_DBCLICK               (_T("dbclick"))

#define UIMSG_RETURN                (_T("return"))
#define UIMSG_SCROLL                (_T("scroll"))

#define UIMSG_PREDROPDOWN           (_T("predropdown"))
#define UIMSG_DROPDOWN              (_T("dropdown"))
#define UIMSG_SETFOCUS              (_T("setfocus"))

#define UIMSG_KILLFOCUS             (_T("killfocus"))
#define UIMSG_ITEMCLICK             (_T("itemclick"))
#define UIMSG_ITEMRCLICK            (_T("itemrclick"))
#define UIMSG_TABSELECT             (_T("tabselect"))

#define UIMSG_ITEMSELECT            (_T("itemselect"))
#define UIMSG_ITEMEXPAND            (_T("itemexpand"))
#define UIMSG_WINDOWINIT            (_T("windowinit"))
#define UIMSG_WINDOWSIZE            (_T("windowsize"))
#define UIMSG_BUTTONDOWN            (_T("buttondown"))
#define UIMSG_MOUSEENTER            (_T("mouseenter"))
#define UIMSG_MOUSELEAVE            (_T("mouseleave"))

#define UIMSG_TEXTCHANGED            (_T("textchanged"))
#define UIMSG_HEADERCLICK            (_T("headerclick"))
#define UIMSG_ITEMDBCLICK            (_T("itemdbclick"))
#define UIMSG_SHOWACTIVEX            (_T("showactivex"))

#define UIMSG_ITEMCOLLAPSE          (_T("itemcollapse"))
#define UIMSG_ITEMACTIVATE          (_T("itemactivate"))
#define UIMSG_VALUECHANGED          (_T("valuechanged"))
#define UIMSG_VALUECHANGED_MOVE     (_T("movevaluechanged"))

#define UIMSG_SELECTCHANGED         (_T("selectchanged"))
#define UIMSG_UNSELECTED            (_T("unselected"))

#define UIMSG_TREEITEMDBCLICK       (_T("treeitemdbclick"))
#define UIMSG_CHECKCLICK            (_T("checkclick"))
#define UIMSG_TEXTROLLEND           (_T("textrollend"))
#define UIMSG_COLORCHANGED          (_T("colorchanged"))

#define UIMSG_LISTITEMSELECT        (_T("listitemselect"))
#define UIMSG_LISTITEMCHECKED       (_T("listitemchecked"))
#define UIMSG_COMBOITEMSELECT       (_T("comboitemselect"))
#define UIMSG_LISTHEADERCLICK       (_T("listheaderclick"))
#define UIMSG_LISTHEADITEMCHECKED   (_T("listheaditemchecked"))
#define UIMSG_LISTPAGECHANGED       (_T("listpagechanged"))

    //////////////////////////////////////////////////////////////////////////
    
    class CNotifyPumpUI;
    struct TNOTIFY_UI;
    typedef void (CNotifyPumpUI::* PMSG_UI)(TNOTIFY_UI& msg);  //指针类型


    struct TMSGMAPENTRY_UI;
    struct TMSGMAP_UI
    {
#ifndef DUILIB_STATIC
        const TMSGMAP_UI* (PASCAL* pfnGetBaseMap)();
#else
        const TMSGMAP_UI* pBaseMap;
#endif
        const TMSGMAPENTRY_UI* lpEntries;
    };

    //结构定义
    struct TMSGMAPENTRY_UI //定义一个结构体，来存放消息信息
    {
        CStringUI sMsgType;          // DUI消息类型
        CStringUI sCtrlName;         // 控件名称
        UINT      nSig;              // 标记函数指针类型
        PMSG_UI   pfn;               // 指向函数的指针
    };

    //定义
#ifndef DUILIB_STATIC
#define UI_DECLARE_MESSAGE_MAP()                                            \
private:                                                                    \
	static const TMSGMAPENTRY_UI _messageEntries[];                         \
protected:                                                                  \
	static const  TMSGMAP_UI messageMap;                                    \
	static const  TMSGMAP_UI* PASCAL _GetBaseMessageMap();                  \
	virtual const TMSGMAP_UI* GetMessageMap() const;                        \

#else
#define UI_DECLARE_MESSAGE_MAP()                                            \
private:                                                                    \
	static const TMSGMAPENTRY_UI _messageEntries[];                         \
protected:                                                                  \
	static  const TMSGMAP_UI    messageMap;				                    \
	virtual const TMSGMAP_UI*   GetMessageMap() const;                      \

#endif


    //基类声明开始
#ifndef DUILIB_STATIC
#define UI_BASE_BEGIN_MESSAGE_MAP(theClass)                                 \
	const TMSGMAP_UI* PASCAL theClass::_GetBaseMessageMap()                 \
	{ return NULL; }                                                        \
	const TMSGMAP_UI* theClass::GetMessageMap() const                       \
	{ return &theClass::messageMap; }                                       \
	DUILIB_COMDAT const TMSGMAP_UI theClass::messageMap =                   \
	{  &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] };      \
	DUILIB_COMDAT const TMSGMAPENTRY_UI theClass::_messageEntries[] =       \
	{                                                                       \

#else
#define UI_BASE_BEGIN_MESSAGE_MAP(theClass)                                 \
	const TMSGMAP_UI* theClass::GetMessageMap() const                       \
	{ return &theClass::messageMap; }                                       \
	DUILIB_COMDAT const TMSGMAP_UI theClass::messageMap =                   \
	{  NULL, &theClass::_messageEntries[0] };                               \
	DUILIB_COMDAT const TMSGMAPENTRY_UI theClass::_messageEntries[] =       \
	{                                                                       \

#endif


    //子类声明开始
#ifndef DUILIB_STATIC
#define UI_BEGIN_MESSAGE_MAP(theClass, baseClass)                           \
	const TMSGMAP_UI* PASCAL theClass::_GetBaseMessageMap()                 \
	{ return &baseClass::messageMap; }                                      \
	const TMSGMAP_UI* theClass::GetMessageMap() const                       \
	{ return &theClass::messageMap; }                                       \
	UILIB_COMDAT const TMSGMAP_UI theClass::messageMap =                    \
	{ &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] };       \
	UILIB_COMDAT const TMSGMAPENTRY_UI theClass::_messageEntries[] =        \
	{                                                                       \

#else
#define UI_BEGIN_MESSAGE_MAP(theClass, baseClass)                           \
	const TMSGMAP_UI* theClass::GetMessageMap() const                       \
	{ return &theClass::messageMap; }                                       \
	UILIB_COMDAT const TMSGMAP_UI theClass::messageMap =                    \
	{ &baseClass::messageMap, &theClass::_messageEntries[0] };              \
	UILIB_COMDAT const TMSGMAPENTRY_UI theClass::_messageEntries[] =       \
	{                                                                       \

#endif


    //声明结束
#define UI_END_MESSAGE_MAP()                                                \
	{ _T(""), _T(""), UISIG_end, (PMSG_UI)0 }                               \
	};                                                                      \


    //定义消息类型--执行函数宏
#define UI_ON_MSGTYPE(msgtype, memberFxn)                                   \
	{ msgtype, _T(""), UISIG_vn, (PMSG_UI)&memberFxn},                      \


    //定义消息类型--控件名称--执行函数宏
#define UI_ON_MSGTYPE_CTRNAME(msgtype,ctrname,memberFxn)                    \
	{ msgtype, ctrname, UISIG_vn, (PMSG_UI)&memberFxn },                    \


    //定义click消息的控件名称--执行函数宏
#define UI_ON_CLICK_CTRNAME(ctrname,memberFxn)                              \
	{ UIMSG_CLICK, ctrname, UISIG_vn, (PMSG_UI)&memberFxn },            \


    //定义selectchanged消息的控件名称--执行函数宏
#define UI_ON_SELECTCHANGED_CTRNAME(ctrname,memberFxn)                      \
	{ UIMSG_SELECTCHANGED,ctrname,UISIG_vn,(PMSG_UI)&memberFxn },       \


    //定义killfocus消息的控件名称--执行函数宏
#define UI_ON_KILLFOCUS_CTRNAME(ctrname,memberFxn)                          \
	{ UIMSG_KILLFOCUS,ctrname,UISIG_vn,(PMSG_UI)&memberFxn },           \


    //定义menu消息的控件名称--执行函数宏
#define UI_ON_MENU_CTRNAME(ctrname,memberFxn)                               \
	{ UIMSG_MENU,ctrname,UISIG_vn,(PMSG_UI)&memberFxn },                \


    //定义与控件名称无关的消息宏

    //定义timer消息--执行函数宏
#define UI_ON_TIMER()                                                       \
	{ UIMSG_TIMER, _T(""), UISIG_vn,(PMSG_UI)&OnTimer },                \


    ///
    //////////////END消息映射宏定义////////////////////////////////////////////////////


    //////////////BEGIN控件名称宏定义//////////////////////////////////////////////////
    ///

#define  UICONTROL_EDIT                            (_T("Edit"))
#define  UICONTROL_LIST                            (_T("List"))
#define  UICONTROL_TEXT                            (_T("Text"))

#define  UICONTROL_COMBO                           (_T("Combo"))
#define  UICONTROL_LABEL                           (_T("Label"))
#define  UICONTROL_FLASH							 (_T("Flash"))

#define  UICONTROL_BUTTON                          (_T("Button"))
#define  UICONTROL_OPTION                          (_T("Option"))
#define  UICONTROL_SLIDER                          (_T("Slider"))

#define  UICONTROL_CONTROL                         (_T("Control"))
#define  UICONTROL_ACTIVEX                         (_T("ActiveX"))
#define  UICONTROL_GIFANIM                         (_T("GifAnim"))

#define  UICONTROL_LISTITEM                        (_T("ListItem"))
#define  UICONTROL_PROGRESS                        (_T("Progress"))
#define  UICONTROL_RICHEDIT                        (_T("RichEdit"))
#define  UICONTROL_CHECKBOX                        (_T("CheckBox"))
#define  UICONTROL_COMBOBOX                        (_T("ComboBox"))
#define  UICONTROL_DATETIME                        (_T("DateTime"))
#define  UICONTROL_TREEVIEW                        (_T("TreeView"))
#define  UICONTROL_TREENODE                        (_T("TreeNode"))

#define  UICONTROL_CONTAINER                       (_T("Container"))
#define  UICONTROL_TABLAYOUT                       (_T("TabLayout"))
#define  UICONTROL_SCROLLBAR                       (_T("ScrollBar"))
#define  UICONTROL_IPADDRESS                       (_T("IPAddress"))

#define  UICONTROL_LISTHEADER                      (_T("ListHeader"))
#define  UICONTROL_LISTFOOTER                      (_T("ListFooter"))
#define  UICONTROL_TILELAYOUT                      (_T("TileLayout"))
#define  UICONTROL_WEBBROWSER                      (_T("WebBrowser"))

#define  UICONTROL_CHILDLAYOUT                     (_T("ChildLayout"))
#define  UICONTROL_LISTELEMENT                     (_T("ListElement"))

#define  UICONTROL_VERTICALLAYOUT                  (_T("VerticalLayout"))
#define  UICONTROL_LISTHEADERITEM                  (_T("ListHeaderItem"))

#define  UICONTROL_LISTTEXTELEMENT                 (_T("ListTextElement"))

#define  UICONTROL_HORIZONTALLAYOUT                (_T("HorizontalLayout"))
#define  UICONTROL_LISTLABELELEMENT                (_T("ListLabelElement"))

#define  UICONTROL_ANIMATIONTABLAYOUT				 (_T("AnimationTabLayout"))

#define  UICONTROL_LISTCONTAINERELEMENT            (_T("ListContainerElement"))

#define  UICONTROL_TEXTSCROLL						 (_T("TextScroll"))

#define  UICONTROL_COLORPALETTE					 (_T("ColorPalette"))
    ///
    //////////////END控件名称宏定义//////////////////////////////////////////////////

    }// namespace DUI


#endif//_UIDEFINE_H_

