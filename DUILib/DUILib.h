//
//  DUILIB.h和UILIB.cpp是UILib库最第顶层的文件，它用来屏蔽所有OS和编译期的差异，对UILib库的其他文件提供统一的系统接口界面
//
//  如果你试图在文件中包含 UILib.h 头文件，那么实际你可能真正想包含的一定是 UIUtils 或者 UIDefine.h。
//
#ifndef __DUILIB_H__
#define __DUILIB_H__


#ifdef DUILIB_STATIC
    #define DUILIB_API 
#else
    #if defined(DUILIB_EXPORTS)
    #	if defined(_MSC_VER)
    #		define DUILIB_API __declspec(dllexport)
    #	else
    #		define DUILIB_API 
    #	endif
    #else
    #	if defined(_MSC_VER)
    #		define DUILIB_API __declspec(dllimport)
    #	else
    #		define DUILIB_API 
    #	endif
    #endif
#endif


#define DUILIB_COMDAT __declspec(selectany)


#pragma warning(disable:4505)
#pragma warning(disable:4251)
#pragma warning(disable:4189)
#pragma warning(disable:4121)
#pragma warning(disable:4100)



#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <richedit.h>
#include <comdef.h>
#pragma warning(disable:4458)
#include <gdiplus.h>
#pragma warning(default:4458)
#include <shellapi.h>
#include <zmouse.h>
#include <shlobj.h>
#include <oaidl.h>
#include <atlbase.h>
#include <atlconv.h>
#include <olectl.h>
#ifdef _DEBUG
#include <shlwapi.h>
#endif


#pragma comment( lib, "comctl32.lib" )
#pragma comment( lib, "GdiPlus.lib" )
#pragma comment( lib, "Imm32.lib" )
#pragma comment( lib, "winmm.lib" )
#ifdef _DEBUG
#pragma comment(lib, "shlwapi.lib")
#endif


#include <stddef.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <math.h>
#include <crtdbg.h>

#include <map>
#include <algorithm>
#include <vector>


// Remove pointless warning messages
#if defined(_MSC_VER)
    #pragma warning (disable : 4511) // copy operator could not be generated
    #pragma warning (disable : 4512) // assignment operator could not be generated
    #pragma warning (disable : 4702) // unreachable code (bugs in Microsoft's STL)
    #pragma warning (disable : 4786) // identifier was truncated
    #pragma warning (disable : 4996) // function or variable may be unsafe (deprecated)
    #pragma warning (disable : 4458) // “nativeCap”的声明隐藏了类成员
    //#define _CRT_SECURE_NO_WARNINGS  // eliminate deprecation warnings for VS2005
    //#define _CRT_SECURE_NO_DEPRECATE
#endif

#if defined(__BORLANDC__)
    #pragma option -w-8027		     // function not expanded inline
#endif

#ifdef __GNUC__
    // 怎么都没找到min，max的头文件-_-
    #ifndef min
    #define min(a,b) (((a) < (b)) ? (a) : (b))
    #endif
    #ifndef max
    #define max(a,b) (((a) > (b)) ? (a) : (b))
    #endif
#endif

// Required for VS 2008 (fails on XP and Win2000 without this fix)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT _WIN32_WINNT_WINXP//0x0501
#endif


#ifndef __FILET__
#define __DUILIB_STR2WSTR(str)	L##str
#define _DUILIB_STR2WSTR(str)	__DUILIB_STR2WSTR(str)
#ifdef _UNICODE
#define __FILET__	    _DUILIB_STR2WSTR(__FILE__)
#define __FUNCTIONT__	_DUILIB_STR2WSTR(__FUNCTION__)
#else
#define __FILET__	    __FILE__
#define __FUNCTIONT__	__FUNCTION__
#endif
#endif


//#define USE_XIMAGE_EFFECT //使用ximage的gif控件CGifAnimExUI开关，提升性能,默认不使用

#define lengthof(x) (sizeof(x)/sizeof(*x))
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))

/////////////////////////////////////////////////////////////////////////////////////
//
#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif




/// 定义了一个适用于C语言的编译期断言宏，用于对编译环境进行一些基本的检查
///@{
#if     defined(__cplusplus)
#define UI_STATIC_ASSERT(expr,message)     static_assert(expr,message)
#else
#if   defined(_MSC_VER)
#define UI_STATIC_ASSERT(expr,message)     typedef char __SMQ_STATIC_ASSERT[(expr)?1:-1]
#else
#define UI_STATIC_ASSERT(expr,message)     _Static_assert(expr,message)
#endif
#endif



#endif//__DUILIB_H__


