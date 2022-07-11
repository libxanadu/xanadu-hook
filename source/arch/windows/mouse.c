#include <source/arch/mouse.h>


/// system common context
typedef struct x_hook_mouse_system
{
	x_hook_mouse_context		context_array[X_HOOK_MAX_CONTEXT];
	x_mutex_t 			hook_mutex;
	HHOOK				hook_handle;
}x_hook_mouse_system;

#define		X_HOOK_MESSAGE_MOUSE			(WM_USER + 0x1000)

/// shared data area
#pragma data_seg("x_hook_mouse_shared_data")
HGLOBAL					x_hook_mouse_shared_memory = NULL;
HWND 					x_hook_mouse_shared_window = NULL;
HHOOK					x_hook_mouse_shared_handle = NULL;
#pragma data_seg()

/// current process shared data
x_mutex_t 				x_hook_mouse_process_mutex;
x_hook_mouse_context*			x_hook_mouse_process_array = NULL;



/// System hook callback
static LRESULT CALLBACK x_hook_mouse_callback(int _Code, WPARAM wParam, LPARAM lParam)
{
	XANADU_UNUSED(wParam);

	if(x_hook_mouse_shared_window)
	{
		PostMessageW(x_hook_mouse_shared_window, X_HOOK_MESSAGE_MOUSE, (WPARAM)x_hook_mouse_shared_memory, lParam);

		int*			vGlobalMemory = (int*) GlobalLock(x_hook_mouse_shared_memory);
		if(vGlobalMemory && *vGlobalMemory == X_HOOK_EVENT_ACCEPT)
		{
			GlobalUnlock((HGLOBAL)wParam);
			return TRUE;
		}
		GlobalUnlock((HGLOBAL)wParam);
	}
	return CallNextHookEx(x_hook_mouse_shared_handle, _Code, wParam, lParam);
}



// windows : window message handle
static LRESULT CALLBACK x_hook_windows_window_message_handle(HWND _Hwnd, UINT _Message, WPARAM wParam, LPARAM lParam)
{
	if(_Message == WM_CREATE)
	{
		x_hook_mouse_shared_handle = SetWindowsHookExW(WH_MOUSE_LL, x_hook_mouse_callback, NULL, 0);
	}
	else if(_Message == WM_CLOSE)
	{
		UnhookWindowsHookEx(x_hook_mouse_shared_handle);
		DestroyWindow(_Hwnd);
	}
	else if(_Message == WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	else if(_Message == X_HOOK_MESSAGE_MOUSE)
	{
		x_mutex_lock(&x_hook_mouse_process_mutex);

		int*			vGlobalMemory = (int*) GlobalLock((HGLOBAL)wParam);
		MOUSEHOOKSTRUCT*	vMouseStruct = (MOUSEHOOKSTRUCT*)lParam;
		x_hook_mouse_event_t	vMouseEvent = {	vMouseStruct->pt.x, vMouseStruct->pt.y	};
		x_hook_mouse_context*	vNode = x_hook_mouse_process_array;
		if(vGlobalMemory)
		{
			(*vGlobalMemory) = X_HOOK_EVENT_IGNORE;
			while (vNode && (*vGlobalMemory) == X_HOOK_EVENT_IGNORE)
			{
				(*vGlobalMemory) = vNode->cb_address(&vMouseEvent, vNode->cb_user_data);
				vNode = vNode->next;
			}
		}

		x_mutex_unlock(&x_hook_mouse_process_mutex);
		GlobalUnlock((HGLOBAL)wParam);
	}
	return DefWindowProcW(_Hwnd, _Message, wParam, lParam);
}

/// thread windows message window
static void __xcall__ x_hook_windows_window_loop(void* _Param)
{
	XANADU_UNUSED(_Param);

	wchar_t 	vWindowName[XANADU_PATH_MAX] = {0};
	HMODULE		vModuleHandle = GetModuleHandleW(NULL);
	HWND		vWindowHandle = NULL;
	MSG		vMessage;
	WNDCLASSEXW	vWCEX;

	swprintf(vWindowName, XANADU_PATH_MAX, L"x_hook_mouse_window_0x%016llX", (uint64_t)GetCurrentProcessId());

	vWCEX.cbSize = sizeof(WNDCLASSEX);
	vWCEX.style = CS_HREDRAW | CS_VREDRAW;
	vWCEX.lpfnWndProc = x_hook_windows_window_message_handle;
	vWCEX.cbClsExtra = 0;
	vWCEX.cbWndExtra = 0;
	vWCEX.hInstance = vModuleHandle;
	vWCEX.hIcon = NULL;
	vWCEX.hCursor = LoadCursorA(NULL, IDC_ARROW);
	vWCEX.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	vWCEX.lpszMenuName = NULL;
	vWCEX.lpszClassName = vWindowName;
	vWCEX.hIconSm = NULL;
	RegisterClassExW(&vWCEX);

	vWindowHandle = CreateWindowExW(WS_EX_TOPMOST, vWindowName, vWindowName, WS_POPUP, 0, 0, 400, 300, NULL, NULL, vModuleHandle, NULL);
	if(vWindowHandle == NULL)
	{
		return;
	}

	ShowWindow(vWindowHandle, SW_HIDE);
	UpdateWindow(vWindowHandle);
	SetWindowTextW(vWindowHandle, vWindowName);

	x_hook_mouse_shared_window = vWindowHandle;
	while(GetMessageW(&vMessage, NULL, 0U, 0U) > 0)
	{
		TranslateMessage(&vMessage);
		DispatchMessageW(&vMessage);
	}
	x_hook_mouse_shared_window = NULL;
}

/// initialize mouse hook
_XHOOKAPI_ bool __xcall__ x_hook_mouse_initialize()
{
	if(x_hook_mouse_shared_window)
	{
		return true;
	}

	x_hook_mouse_shared_memory = GlobalAlloc(GMEM_FIXED, sizeof(int));
	x_mutex_init(&x_hook_mouse_process_mutex);
	x_thread_create(x_hook_windows_window_loop, NULL);
	for(int vIndex = 0; vIndex < 1000 && x_hook_mouse_shared_window == NULL; ++vIndex)
	{
		x_posix_msleep(1);
	}
	return x_hook_mouse_shared_window;
}

// release mouse hook
_XHOOKAPI_ void __xcall__ x_hook_mouse_release()
{
	if(x_hook_mouse_shared_window)
	{
		CloseWindow(x_hook_mouse_shared_window);
		for(int vIndex = 0; vIndex < 1000 && x_hook_mouse_shared_window; ++vIndex)
		{
			x_posix_msleep(1);
		}
		x_mutex_destroy(&x_hook_mouse_process_mutex);
		GlobalFree(x_hook_mouse_shared_memory);
	}
}





/// Install a mouse hook
_XHOOKAPI_ bool __xcall__ x_hook_mouse_install(x_hook_context_t* _Context, x_hook_mouse_event_cb_t _Function, void* _UserData)
{
	if(_Context == NULL || _Function == NULL)
	{
		return false;
	}

	x_hook_mouse_context*	vNode = (x_hook_mouse_context*) x_posix_malloc(sizeof(x_hook_mouse_context));
	if(vNode == NULL)
	{
		return false;
	}
	x_posix_memset(vNode, 0, sizeof(x_hook_mouse_context));
	vNode->cb_address = _Function;
	vNode->cb_user_data = _UserData;

	x_mutex_lock(&x_hook_mouse_process_mutex);

	if(x_hook_mouse_process_array)
	{
		vNode->next = x_hook_mouse_process_array;
		x_hook_mouse_process_array->prev = vNode;
		x_hook_mouse_process_array = vNode;
	}
	else
	{
		x_hook_mouse_process_array = vNode;
	}
	*_Context = vNode;

	x_mutex_unlock(&x_hook_mouse_process_mutex);
	return true;
}

/// Uninstall a mouse hook
_XHOOKAPI_ bool __xcall__ x_hook_mouse_uninstall(x_hook_context_t _Context)
{
	if(_Context == NULL)
	{
		return false;
	}

	x_mutex_lock(&x_hook_mouse_process_mutex);

	x_hook_mouse_context*	vNode = (x_hook_mouse_context*)_Context;
	if(x_hook_mouse_process_array == vNode)
	{
		x_hook_mouse_process_array = x_hook_mouse_process_array->next;
	}
	else
	{
		vNode->prev->next = vNode->next;
		if(vNode->next)
		{
			vNode->next->prev = vNode->prev;
		}
	}
	x_posix_free(vNode);

	x_mutex_unlock(&x_hook_mouse_process_mutex);
	return true;
}
