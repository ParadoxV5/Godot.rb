#include <godot_rb.h>
#include "setup.h"
#include "cleanup.h"

// Uninitialized API variables
bool godot_rb_init_levels[GDEXTENSION_MAX_INITIALIZATION_LEVEL];
GDExtensionInterfaceGetProcAddress godot_rb_get_proc = NULL;
GDExtensionClassLibraryPtr godot_rb_library = NULL;

__attribute__((used)) GDExtensionBool godot_rb_main(
  GDExtensionInterfaceGetProcAddress p_get_proc_address,
  GDExtensionClassLibraryPtr p_library,
  GDExtensionInitialization* r_initialization
) {
  // Set up de/initialization functions
  r_initialization->minimum_initialization_level = GDEXTENSION_INITIALIZATION_CORE;
  r_initialization->initialize = &godot_rb_setup;
  r_initialization->deinitialize = &godot_rb_cleanup;
  // Save GDExtension API
  godot_rb_get_proc = p_get_proc_address;
  godot_rb_library = p_library;
  // Initialize API variables
  for(GDExtensionInitializationLevel i = 0; i < GDEXTENSION_MAX_INITIALIZATION_LEVEL; ++i)
    godot_rb_init_levels[i] = false;
  // Success
  return true;
}
