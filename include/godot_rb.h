#ifndef GODOT_RB_H
#define GODOT_RB_H

#include <ruby.h>
#include <godot/gdextension_interface.h>

extern bool godot_rb_init_levels[GDEXTENSION_MAX_INITIALIZATION_LEVEL];
extern GDExtensionInterfaceGetProcAddress godot_rb_get_proc;
extern GDExtensionClassLibraryPtr godot_rb_library;

__attribute__((used)) GDExtensionBool godot_rb_main(
  GDExtensionInterfaceGetProcAddress p_get_proc_address,
  GDExtensionClassLibraryPtr p_library,
  GDExtensionInitialization* r_initialization
);

#endif
