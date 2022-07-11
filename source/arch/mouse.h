#ifndef			_XANADU_HOOK_ARCH_MOUSE_H_
#define			_XANADU_HOOK_ARCH_MOUSE_H_

#include <xanadu-hook/mouse.h>



/// Mouse hook context type
typedef struct x_hook_mouse_context
{
	struct x_hook_mouse_context*	prev;
	struct x_hook_mouse_context*	next;

	x_hook_mouse_event_cb_t		cb_address;
	void*				cb_user_data;
}x_hook_mouse_context;



#endif
