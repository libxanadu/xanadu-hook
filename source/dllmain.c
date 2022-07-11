#include <xanadu-hook/mouse.h>


// Dll main
#if defined(XANADU_SYSTEM_WINDOWS)
BOOL WINAPI DllMain(HANDLE _HDllHandle, DWORD _Reason, LPVOID _Reserved)
{
	XANADU_UNUSED(_HDllHandle);
	XANADU_UNUSED(_Reserved);

	switch(_Reason)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_PROCESS_DETACH:
			x_hook_mouse_release();
			break;
		default:
			break;
	}
	return TRUE;
}
#else
__attribute((constructor)) void xanadu_hook_dynamic_library_init(void)
{
};

__attribute((destructor)) void xanadu_hook_dynamic_library_fini(void)
{
	x_hook_mouse_release();
};
#endif
