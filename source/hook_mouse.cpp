#include <HookMouse/hook_mouse.h>
#include <list>



/// 共享数据类型
typedef struct _easy_hook_mouse_handle
{
	HHOOK			handle;
	HWND			window;
	UINT			message;

	_easy_hook_mouse_handle()
	{
		this->handle = nullptr;
		this->window = nullptr;
		this->message = 0;
	}
}easy_hook_mouse_handle;


// 共享数据区
#pragma data_seg("MyData")
easy_hook_mouse_handle*		_easy_hook_mouse_handle = nullptr;
#pragma data_seg()



/// HOOK : 勾子回调
static LRESULT CALLBACK easy_hook_mouse_function(int _Code, WPARAM wParam, LPARAM lParam) noexcept
{
	auto		vMouseStruct = (MOUSEHOOKSTRUCT*)lParam;
	if(_easy_hook_mouse_handle->window && _easy_hook_mouse_handle->message)
	{
		PostMessageW(_easy_hook_mouse_handle->window, _easy_hook_mouse_handle->message, vMouseStruct->pt.x, vMouseStruct->pt.y);
	}
	return CallNextHookEx(_easy_hook_mouse_handle->handle, _Code, wParam, lParam);
}


/// HOOK : 安装勾子
_AHOOKMOUSEIMP bool __HMCALL__ easy_hook_mouse_install(HWND _Window, UINT _Message)
{
	if(_easy_hook_mouse_handle == nullptr)
	{
		_easy_hook_mouse_handle = new(std::nothrow) easy_hook_mouse_handle();
	}
	if(_easy_hook_mouse_handle == nullptr)
	{
		return false;
	}
	_easy_hook_mouse_handle->window = _Window;
	_easy_hook_mouse_handle->message = _Message;
	_easy_hook_mouse_handle->handle = SetWindowsHookExW(WH_MOUSE_LL, easy_hook_mouse_function, nullptr, 0);
	if(_easy_hook_mouse_handle->handle == nullptr)
	{
		delete _easy_hook_mouse_handle;
		return false;
	}
	return true;
}

/// HOOK : 卸载勾子
_AHOOKMOUSEIMP bool __HMCALL__ easy_hook_mouse_uninstall(HWND _Window)
{
	if(_easy_hook_mouse_handle == nullptr)
	{
		return false;
	}
	if(_easy_hook_mouse_handle->window != _Window)
	{
		return false;
	}
	UnhookWindowsHookEx(_easy_hook_mouse_handle->handle);
	delete _easy_hook_mouse_handle;
	_easy_hook_mouse_handle = nullptr;
	return true;
}

/// HOOK : 销毁所有勾子
_AHOOKMOUSEIMP bool __HMCALL__ easy_hook_mouse_clear()
{
	if(_easy_hook_mouse_handle == nullptr)
	{
		return false;
	}
	return easy_hook_mouse_uninstall(_easy_hook_mouse_handle->window);
}
