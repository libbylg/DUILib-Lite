#ifndef _UIDEFINE_H_
#define _UIDEFINE_H_

#include "DUILIB.h"
#include "Utils/Utils.h"

namespace DUILIB
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
#define UIMSGTYPE_MENU                  (_T("menu"))
#define UIMSGTYPE_LINK                  (_T("link"))

#define UIMSGTYPE_TIMER                 (_T("timer"))
#define UIMSGTYPE_CLICK                 (_T("click"))
#define UIMSGTYPE_DBCLICK               (_T("dbclick"))

#define UIMSGTYPE_RETURN                (_T("return"))
#define UIMSGTYPE_SCROLL                (_T("scroll"))

#define UIMSGTYPE_PREDROPDOWN           (_T("predropdown"))
#define UIMSGTYPE_DROPDOWN              (_T("dropdown"))
#define UIMSGTYPE_SETFOCUS              (_T("setfocus"))

#define UIMSGTYPE_KILLFOCUS             (_T("killfocus"))
#define UIMSGTYPE_ITEMCLICK             (_T("itemclick"))
#define UIMSGTYPE_ITEMRCLICK            (_T("itemrclick"))
#define UIMSGTYPE_TABSELECT             (_T("tabselect"))

#define UIMSGTYPE_ITEMSELECT            (_T("itemselect"))
#define UIMSGTYPE_ITEMEXPAND            (_T("itemexpand"))
#define UIMSGTYPE_WINDOWINIT            (_T("windowinit"))
#define UIMSGTYPE_WINDOWSIZE            (_T("windowsize"))
#define UIMSGTYPE_BUTTONDOWN            (_T("buttondown"))
#define UIMSGTYPE_MOUSEENTER            (_T("mouseenter"))
#define UIMSGTYPE_MOUSELEAVE            (_T("mouseleave"))

#define UIMSGTYPE_TEXTCHANGED            (_T("textchanged"))
#define UIMSGTYPE_HEADERCLICK            (_T("headerclick"))
#define UIMSGTYPE_ITEMDBCLICK            (_T("itemdbclick"))
#define UIMSGTYPE_SHOWACTIVEX            (_T("showactivex"))

#define UIMSGTYPE_ITEMCOLLAPSE          (_T("itemcollapse"))
#define UIMSGTYPE_ITEMACTIVATE          (_T("itemactivate"))
#define UIMSGTYPE_VALUECHANGED          (_T("valuechanged"))
#define UIMSGTYPE_VALUECHANGED_MOVE     (_T("movevaluechanged"))

#define UIMSGTYPE_SELECTCHANGED         (_T("selectchanged"))
#define UIMSGTYPE_UNSELECTED            (_T("unselected"))

#define UIMSGTYPE_TREEITEMDBCLICK       (_T("treeitemdbclick"))
#define UIMSGTYPE_CHECKCLICK            (_T("checkclick"))
#define UIMSGTYPE_TEXTROLLEND           (_T("textrollend"))
#define UIMSGTYPE_COLORCHANGED          (_T("colorchanged"))

#define UIMSGTYPE_LISTITEMSELECT        (_T("listitemselect"))
#define UIMSGTYPE_LISTITEMCHECKED       (_T("listitemchecked"))
#define UIMSGTYPE_COMBOITEMSELECT       (_T("comboitemselect"))
#define UIMSGTYPE_LISTHEADERCLICK       (_T("listheaderclick"))
#define UIMSGTYPE_LISTHEADITEMCHECKED   (_T("listheaditemchecked"))
#define UIMSGTYPE_LISTPAGECHANGED       (_T("listpagechanged"))

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
        const DUI_MSGMAP* pBaseMap;
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
#define DUI_DECLARE_MESSAGE_MAP()                                           \
private:                                                                    \
	static const TMSGMAPENTRY_UI _messageEntries[];                         \
protected:                                                                  \
	static const  TMSGMAP_UI messageMap;                                    \
	static const  TMSGMAP_UI* PASCAL _GetBaseMessageMap();                  \
	virtual const TMSGMAP_UI* GetMessageMap() const;                        \

#else
#define DUI_DECLARE_MESSAGE_MAP()                                           \
private:                                                                    \
	static const DUI_MSGMAP_ENTRY _messageEntries[];                        \
protected:                                                                  \
	static  const DUI_MSGMAP messageMap;				                    \
	virtual const DUI_MSGMAP* GetMessageMap() const;                        \

#endif


    //基类声明开始
#ifndef DUILIB_STATIC
#define DUI_BASE_BEGIN_MESSAGE_MAP(theClass)                                \
	const TMSGMAP_UI* PASCAL theClass::_GetBaseMessageMap()                 \
	{ return NULL; }                                                        \
	const TMSGMAP_UI* theClass::GetMessageMap() const                       \
	{ return &theClass::messageMap; }                                       \
	DUILIB_COMDAT const TMSGMAP_UI theClass::messageMap =                   \
	{  &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] };      \
	DUILIB_COMDAT const TMSGMAPENTRY_UI theClass::_messageEntries[] =       \
	{                                                                       \

#else
#define DUI_BASE_BEGIN_MESSAGE_MAP(theClass)                                \
	const DUI_MSGMAP* theClass::GetMessageMap() const                       \
	{ return &theClass::messageMap; }                                       \
	DUILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                   \
	{  NULL, &theClass::_messageEntries[0] };                               \
	DUILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] =      \
	{                                                                       \

#endif


    //子类声明开始
#ifndef DUILIB_STATIC
#define DUI_BEGIN_MESSAGE_MAP(theClass, baseClass)                          \
	const TMSGMAP_UI* PASCAL theClass::_GetBaseMessageMap()                 \
	{ return &baseClass::messageMap; }                                      \
	const TMSGMAP_UI* theClass::GetMessageMap() const                       \
	{ return &theClass::messageMap; }                                       \
	UILIB_COMDAT const TMSGMAP_UI theClass::messageMap =                    \
	{ &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] };       \
	UILIB_COMDAT const TMSGMAPENTRY_UI theClass::_messageEntries[] =        \
	{                                                                       \

#else
#define DUI_BEGIN_MESSAGE_MAP(theClass, baseClass)                          \
	const DUI_MSGMAP* theClass::GetMessageMap() const                       \
	{ return &theClass::messageMap; }                                       \
	UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                    \
	{ &baseClass::messageMap, &theClass::_messageEntries[0] };              \
	UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] =       \
	{                                                                       \

#endif


    //声明结束
#define DUI_END_MESSAGE_MAP()                                               \
	{ _T(""), _T(""), UISIG_end, (PMSG_UI)0 }                               \
	};                                                                      \


    //定义消息类型--执行函数宏
#define DUI_ON_MSGTYPE(msgtype, memberFxn)                                  \
	{ msgtype, _T(""), UISIG_vn, (PMSG_UI)&memberFxn},                      \


    //定义消息类型--控件名称--执行函数宏
#define DUI_ON_MSGTYPE_CTRNAME(msgtype,ctrname,memberFxn)                   \
	{ msgtype, ctrname, UISIG_vn, (PMSG_UI)&memberFxn },                    \


    //定义click消息的控件名称--执行函数宏
#define DUI_ON_CLICK_CTRNAME(ctrname,memberFxn)                             \
	{ UIMSGTYPE_CLICK, ctrname, UISIG_vn, (PMSG_UI)&memberFxn },            \


    //定义selectchanged消息的控件名称--执行函数宏
#define DUI_ON_SELECTCHANGED_CTRNAME(ctrname,memberFxn)                     \
	{ UIMSGTYPE_SELECTCHANGED,ctrname,UISIG_vn,(PMSG_UI)&memberFxn },       \


    //定义killfocus消息的控件名称--执行函数宏
#define DUI_ON_KILLFOCUS_CTRNAME(ctrname,memberFxn)                         \
	{ UIMSGTYPE_KILLFOCUS,ctrname,UISIG_vn,(PMSG_UI)&memberFxn },           \


    //定义menu消息的控件名称--执行函数宏
#define DUI_ON_MENU_CTRNAME(ctrname,memberFxn)                              \
	{ UIMSGTYPE_MENU,ctrname,UISIG_vn,(PMSG_UI)&memberFxn },                \


    //定义与控件名称无关的消息宏

    //定义timer消息--执行函数宏
#define DUI_ON_TIMER()                                                      \
	{ UIMSGTYPE_TIMER, _T(""), UISIG_vn,(PMSG_UI)&OnTimer },                \


    ///
    //////////////END消息映射宏定义////////////////////////////////////////////////////


    //////////////BEGIN控件名称宏定义//////////////////////////////////////////////////
    ///

#define  DUI_CTR_EDIT                            (_T("Edit"))
#define  DUI_CTR_LIST                            (_T("List"))
#define  DUI_CTR_TEXT                            (_T("Text"))

#define  DUI_CTR_COMBO                           (_T("Combo"))
#define  DUI_CTR_LABEL                           (_T("Label"))
#define  DUI_CTR_FLASH							 (_T("Flash"))

#define  DUI_CTR_BUTTON                          (_T("Button"))
#define  DUI_CTR_OPTION                          (_T("Option"))
#define  DUI_CTR_SLIDER                          (_T("Slider"))

#define  DUI_CTR_CONTROL                         (_T("Control"))
#define  DUI_CTR_ACTIVEX                         (_T("ActiveX"))
#define  DUI_CTR_GIFANIM                         (_T("GifAnim"))

#define  DUI_CTR_LISTITEM                        (_T("ListItem"))
#define  DUI_CTR_PROGRESS                        (_T("Progress"))
#define  DUI_CTR_RICHEDIT                        (_T("RichEdit"))
#define  DUI_CTR_CHECKBOX                        (_T("CheckBox"))
#define  DUI_CTR_COMBOBOX                        (_T("ComboBox"))
#define  DUI_CTR_DATETIME                        (_T("DateTime"))
#define  DUI_CTR_TREEVIEW                        (_T("TreeView"))
#define  DUI_CTR_TREENODE                        (_T("TreeNode"))

#define  DUI_CTR_CONTAINER                       (_T("Container"))
#define  DUI_CTR_TABLAYOUT                       (_T("TabLayout"))
#define  DUI_CTR_SCROLLBAR                       (_T("ScrollBar"))
#define  DUI_CTR_IPADDRESS                       (_T("IPAddress"))

#define  DUI_CTR_LISTHEADER                      (_T("ListHeader"))
#define  DUI_CTR_LISTFOOTER                      (_T("ListFooter"))
#define  DUI_CTR_TILELAYOUT                      (_T("TileLayout"))
#define  DUI_CTR_WEBBROWSER                      (_T("WebBrowser"))

#define  DUI_CTR_CHILDLAYOUT                     (_T("ChildLayout"))
#define  DUI_CTR_LISTELEMENT                     (_T("ListElement"))

#define  DUI_CTR_VERTICALLAYOUT                  (_T("VerticalLayout"))
#define  DUI_CTR_LISTHEADERITEM                  (_T("ListHeaderItem"))

#define  DUI_CTR_LISTTEXTELEMENT                 (_T("ListTextElement"))

#define  DUI_CTR_HORIZONTALLAYOUT                (_T("HorizontalLayout"))
#define  DUI_CTR_LISTLABELELEMENT                (_T("ListLabelElement"))

#define  DUI_CTR_ANIMATIONTABLAYOUT				 (_T("AnimationTabLayout"))

#define  DUI_CTR_LISTCONTAINERELEMENT            (_T("ListContainerElement"))

#define  DUI_CTR_TEXTSCROLL						 (_T("TextScroll"))

#define  DUI_CTR_COLORPALETTE					 (_T("ColorPalette"))
    ///
    //////////////END控件名称宏定义//////////////////////////////////////////////////

    }// namespace DUILIB


#endif//_UIDEFINE_H_

