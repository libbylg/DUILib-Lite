#ifndef _UIDEFINE_H_
#define _UIDEFINE_H_

#include "DUILIB.h"
#include "Utils/Utils.h"

namespace DUI
{


#define UIFONTID_MAX        30000
#define UITIMERID_CARET	    0x1999




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
        UISIG_vn,      // void (struct TNOTIFY_UI)
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


    /////////////////////////////////////////////////////////////////////////////////////
    //
    //  资源类型
    enum RESTYPE_UI
    {
        UIRES_FILE = 1,		// 来自磁盘文件
        UIRES_ZIP,			// 来自磁盘zip压缩包
        UIRES_RESOURCE,		// 来自资源
        UIRES_ZIPRESOURCE,	// 来自资源的zip压缩包
    };


    /////////////////////////////////////////////////////////////////////////////////////
    //
    //  事件类型
    enum EVENTTYPE_UI
    {
        UIEVENT__FIRST = 1,
        UIEVENT__KEYBEGIN,
        UIEVENT_KEYDOWN,
        UIEVENT_KEYUP,
        UIEVENT_CHAR,
        UIEVENT_SYSKEY,
        UIEVENT__KEYEND,
        UIEVENT__MOUSEBEGIN,
        UIEVENT_MOUSEMOVE,
        UIEVENT_MOUSELEAVE,
        UIEVENT_MOUSEENTER,
        UIEVENT_MOUSEHOVER,
        UIEVENT_BUTTONDOWN,
        UIEVENT_BUTTONUP,
        UIEVENT_RBUTTONDOWN,
        UIEVENT_RBUTTONUP,
        UIEVENT_MBUTTONDOWN,
        UIEVENT_MBUTTONUP,
        UIEVENT_DBLCLICK,
        UIEVENT_CONTEXTMENU,
        UIEVENT_SCROLLWHEEL,
        UIEVENT__MOUSEEND,
        UIEVENT_KILLFOCUS,
        UIEVENT_SETFOCUS,
        UIEVENT_WINDOWSIZE,
        UIEVENT_SETCURSOR,
        UIEVENT_TIMER,
        UIEVENT__LAST,
    };


    /////////////////////////////////////////////////////////////////////////////////////
    //
    // 内部保留的消息
    enum MSGTYPE_UI
    {
        UIMSG_TRAYICON = WM_USER + 1,// 托盘消息
        UIMSG_SET_DPI,				 // DPI
        WM_MENUCLICK,				 // 菜单消息
        UIMSG_USER = WM_USER + 100,	 // 程序自定义消息
    };


    /////////////////////////////////////////////////////////////////////////////////////
    //
    // Flags for FindControl()
    enum FINDFLAG_UI
    {
        UIFIND_ALL = 0x00000000,
        UIFIND_VISIBLE = 0x00000001,
        UIFIND_ENABLED = 0x00000002,
        UIFIND_HITTEST = 0x00000004,
        UIFIND_UPDATETEST = 0x00000008,
        UIFIND_TOP_FIRST = 0x00000010,
        UIFIND_ME_FIRST = 0x80000000,
    };

    /////////////////////////////////////////////////////////////////////////////////////
    //
    // Flags used for controlling the paint
    enum STATEFLAG_UI
    {
        UISTATE_FOCUSED = 0x00000001,
        UISTATE_SELECTED = 0x00000002,
        UISTATE_DISABLED = 0x00000004,
        UISTATE_HOT = 0x00000008,
        UISTATE_PUSHED = 0x00000010,
        UISTATE_READONLY = 0x00000020,
        UISTATE_CAPTURED = 0x00000040,
    };


    //////////////////////////////////////////////////////////////////////////

    class CNotifyPumpUI;
    struct TNOTIFY_UI;
    typedef void (CNotifyPumpUI::* PMSG_UI)(TNOTIFY_UI& msg);  //指针类型


    //  定义一个结构体，来存放消息信息
    struct TMSGMAPENTRY_UI
    {
        CStringUI sMsgType;          // DUI消息类型
        CStringUI sCtrlName;         // 控件名称
        UINT      nSig;              // 标记函数指针类型
        PMSG_UI   pfn;               // 指向函数的指针
    };


    struct TMSGMAP_UI
    {
#ifndef DUILIB_STATIC
        const TMSGMAP_UI* (PASCAL* pfnGetBaseMap)();
#else
        const TMSGMAP_UI* pBaseMap;
#endif
        const TMSGMAPENTRY_UI* lpEntries;
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


    //  子类声明开始
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
	UILIB_COMDAT const TMSGMAPENTRY_UI theClass::_messageEntries[] =        \
	{                                                                       \

#endif


    //  声明结束
#define UI_END_MESSAGE_MAP()                                                \
	{ _T(""), _T(""), UISIG_end, (PMSG_UI)0 }                               \
	};                                                                      \


    //  定义消息类型--执行函数宏
#define UI_ON_MSGTYPE(msgtype, memberFxn)                                   \
	{ msgtype, _T(""), UISIG_vn, (PMSG_UI)&memberFxn},                      \


    //  定义消息类型--控件名称--执行函数宏
#define UI_ON_MSGTYPE_CTRNAME(msgtype,ctrname,memberFxn)                    \
	{ msgtype, ctrname, UISIG_vn, (PMSG_UI)&memberFxn },                    \


    //  定义click消息的控件名称--执行函数宏
#define UI_ON_CLICK_CTRNAME(ctrname,memberFxn)                              \
	{ UIMSG_CLICK, ctrname, UISIG_vn, (PMSG_UI)&memberFxn },                \


    //  定义selectchanged消息的控件名称--执行函数宏
#define UI_ON_SELECTCHANGED_CTRNAME(ctrname,memberFxn)                      \
	{ UIMSG_SELECTCHANGED,ctrname,UISIG_vn,(PMSG_UI)&memberFxn },           \


    //  定义killfocus消息的控件名称--执行函数宏
#define UI_ON_KILLFOCUS_CTRNAME(ctrname,memberFxn)                          \
	{ UIMSG_KILLFOCUS,ctrname,UISIG_vn,(PMSG_UI)&memberFxn },               \


    //  定义menu消息的控件名称--执行函数宏
#define UI_ON_MENU_CTRNAME(ctrname,memberFxn)                               \
	{ UIMSG_MENU,ctrname,UISIG_vn,(PMSG_UI)&memberFxn },                    \


    //  定义与控件名称无关的消息宏

    //  定义timer消息--执行函数宏
#define UI_ON_TIMER()                                                       \
	{ UIMSG_TIMER, _T(""), UISIG_vn,(PMSG_UI)&OnTimer },                    \


    ///
    //////////////END消息映射宏定义////////////////////////////////////////////////////


    //////////////BEGIN控件名称宏定义//////////////////////////////////////////////////
    ///

#define UICONTROL_EDIT                          (_T("Edit"))
#define UICONTROL_LIST                          (_T("List"))
#define UICONTROL_TEXT                          (_T("Text"))

#define UICONTROL_COMBO                         (_T("Combo"))
#define UICONTROL_LABEL                         (_T("Label"))
#define UICONTROL_FLASH                         (_T("Flash"))

#define UICONTROL_BUTTON                        (_T("Button"))
#define UICONTROL_OPTION                        (_T("Option"))
#define UICONTROL_SLIDER                        (_T("Slider"))

#define UICONTROL_CONTROL                       (_T("Control"))
#define UICONTROL_ACTIVEX                       (_T("ActiveX"))
#define UICONTROL_GIFANIM                       (_T("GifAnim"))

#define UICONTROL_LISTITEM                      (_T("ListItem"))
#define UICONTROL_PROGRESS                      (_T("Progress"))
#define UICONTROL_RICHEDIT                      (_T("RichEdit"))
#define UICONTROL_CHECKBOX                      (_T("CheckBox"))
#define UICONTROL_COMBOBOX                      (_T("ComboBox"))
#define UICONTROL_DATETIME                      (_T("DateTime"))
#define UICONTROL_TREEVIEW                      (_T("TreeView"))
#define UICONTROL_TREENODE                      (_T("TreeNode"))

#define UICONTROL_CONTAINER                     (_T("Container"))
#define UICONTROL_TABLAYOUT                     (_T("TabLayout"))
#define UICONTROL_SCROLLBAR                     (_T("ScrollBar"))
#define UICONTROL_IPADDRESS                     (_T("IPAddress"))

#define UICONTROL_LISTHEADER                    (_T("ListHeader"))
#define UICONTROL_LISTFOOTER                    (_T("ListFooter"))
#define UICONTROL_TILELAYOUT                    (_T("TileLayout"))
#define UICONTROL_WEBBROWSER                    (_T("WebBrowser"))

#define UICONTROL_CHILDLAYOUT                   (_T("ChildLayout"))
#define UICONTROL_LISTELEMENT                   (_T("ListElement"))

#define UICONTROL_VERTICALLAYOUT                (_T("VerticalLayout"))
#define UICONTROL_LISTHEADERITEM                (_T("ListHeaderItem"))

#define UICONTROL_LISTTEXTELEMENT               (_T("ListTextElement"))

#define UICONTROL_HORIZONTALLAYOUT              (_T("HorizontalLayout"))
#define UICONTROL_LISTLABELELEMENT              (_T("ListLabelElement"))

#define  UICONTROL_ANIMATIONTABLAYOUT           (_T("AnimationTabLayout"))

#define  UICONTROL_LISTCONTAINERELEMENT         (_T("ListContainerElement"))

#define  UICONTROL_TEXTSCROLL                   (_T("TextScroll"))

#define  UICONTROL_COLORPALETTE                 (_T("ColorPalette"))


    //  称宏定义//////////////////////////////////////////////////
    struct IFactoryUI
    {
        virtual CControlUI* CreateControl(LPCTSTR pstrClass) = 0;
    };


//////////////END控件名
}// namespace DUI


#endif//_UIDEFINE_H_

