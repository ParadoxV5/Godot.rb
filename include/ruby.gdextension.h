#include <stdbool.h>
#include <ruby.h>
#include <godot/gdextension_interface.h>

#ifndef RUBY_GDEXTENSION_H
#define RUBY_GDEXTENSION_H

// API
struct ruby_gdextension {
  GDExtensionInterfaceGetProcAddress get_proc_address;
  GDExtensionClassLibraryPtr library;
  bool ruby_setup;
};
extern struct ruby_gdextension ruby_gdextension;

GDExtensionBool ruby_gdextension_main(
  GDExtensionInterfaceGetProcAddress p_get_proc_address,
  GDExtensionClassLibraryPtr p_library,
  GDExtensionInitialization* r_initialization
);

#endif
