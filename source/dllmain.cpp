#include <HookMouse/hook_mouse.h>



bool __HMCALL__ _easy_hook_mouse_initialize() noexcept
{
	return true;
};

void __HMCALL__ _easy_hook_mouse_release() noexcept
{
	easy_hook_mouse_clear();
};

#if defined(_WIN32)
extern "C" BOOL WINAPI DllMain(HANDLE _HDllHandle, DWORD _Reason, LPVOID _Reserved)
{
	UNREFERENCED_PARAMETER(_HDllHandle);
	UNREFERENCED_PARAMETER(_Reserved);

	BOOL	vResult = TRUE;
	switch(_Reason)
	{
		case DLL_PROCESS_ATTACH:
			_easy_hook_mouse_initialize();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			_easy_hook_mouse_release();
			break;
		default:
			break;
	}
	return vResult;
}
#else
__attribute((constructor)) void _easy_hook_mouse_Dynamic_Library_Init(void)
{
	_easy_hook_mouse_initialize();
};

__attribute((destructor)) void _easy_hook_mouse_Dynamic_Library_Fini(void)
{
	_easy_hook_mouse_release();
};
#endif
