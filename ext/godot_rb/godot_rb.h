#include "../include/godot_rb.h"

extern GDExtensionClassLibraryPtr godot_rb_lib_p;

#define godot_proc_get(proc_t, proc) godot_##proc = (GDExtensionInterface##proc_t)godot_proc(#proc)
#define GODOT_PROC_SAVE(proc_t, proc) GDExtensionInterface##proc_t godot_proc_get(proc_t, proc);
