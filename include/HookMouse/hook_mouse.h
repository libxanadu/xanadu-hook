#ifndef			_X_WINDOW_DROP_EASY_HOOK_MOUSE_H_
#define			_X_WINDOW_DROP_EASY_HOOK_MOUSE_H_

// Calling conventions used by Xanadu
#if defined(_MSC_VER)
#define			__HMCALL__						__cdecl
#elif defined(__GNUC__) || defined(__clang__)
// #define		__HMCALL__						__attribute__((__cdecl__))
#define			__HMCALL__
#else
#define			__HMCALL__
#endif


// Xanadu.Runtime export declaration used by the project
#if defined(_XANADU_CORE_BUILD_SHARED)
#if defined(_MSC_VER)
#define			_AHOOKMOUSEIMP						__declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#define			_AHOOKMOUSEIMP						__attribute__((visibility("default")))
#else
#define			_AHOOKMOUSEIMP
#endif
#else
#if defined(_MSC_VER)
#define			_AHOOKMOUSEIMP						__declspec(dllimport)
#elif defined(__GNUC__) || defined(__clang__)
#define			_AHOOKMOUSEIMP						__attribute__((visibility("default")))
#else
#define			_AHOOKMOUSEIMP
#endif
#endif


#include <windows.h>


#if defined(__cplusplus)
extern "C" {
#endif


/// HOOK : 安装勾子
_AHOOKMOUSEIMP bool __HMCALL__ easy_hook_mouse_install(HWND _Window, UINT _Message);

/// HOOK : 卸载勾子
_AHOOKMOUSEIMP bool __HMCALL__ easy_hook_mouse_uninstall(HWND _Window);

/// HOOK : 销毁所有勾子
_AHOOKMOUSEIMP bool __HMCALL__ easy_hook_mouse_clear();


#if defined(__cplusplus)
}
#endif

#endif
