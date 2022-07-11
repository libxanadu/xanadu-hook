#ifndef			_XANADU_HOOK_MOUSE_H_
#define			_XANADU_HOOK_MOUSE_H_

#include <xanadu-hook/header.h>

XANADU_CXX_EXTERN_BEGIN



/// X-Hook mouse events
typedef struct x_hook_mouse_event
{
	int			_PointX;
	int			_PointY;
}x_hook_mouse_event_t;

/// Mouse event callback function type
/// \param _PointX : Mouse X coordinate
/// \param _PointY : Mouse Y coordinate
/// \param _UserData : Caller's custom parameters
/// \return : True is returned for continued downloading and false is returned for aborted transmission
typedef bool (__xcall__ * x_hook_mouse_event_cb_t)(const x_hook_mouse_event_t* _Event, void* _UserData);



/// initialize mouse hook
/// \return : Return true to indicate success and false to indicate failure
_XHOOKAPI_ bool __xcall__ x_hook_mouse_initialize();

// release mouse hook
_XHOOKAPI_ void __xcall__ x_hook_mouse_release();



/// Install a mouse hook
/// \param _Context : Context handle pointer to hook
/// \param _Function : Mouse event callback function
/// \param _UserData : Caller's custom parameters
/// \return : Return true to indicate success and false to indicate failure
_XHOOKAPI_ bool __xcall__ x_hook_mouse_install(x_hook_context_t* _Context, x_hook_mouse_event_cb_t _Function, void* _UserData);

/// Uninstall a mouse hook
/// \param _Context : context handle
/// \return : Return true to indicate success and false to indicate failure
_XHOOKAPI_ bool __xcall__ x_hook_mouse_uninstall(x_hook_context_t _Context);



XANADU_CXX_EXTERN_END

#endif
