#include <godot_rb.h>
#include "setup.h"
#include "cleanup.h"

bool godot_rb_init_levels[GDEXTENSION_MAX_INITIALIZATION_LEVEL] = {};
GDExtensionInterfaceGetProcAddress godot_rb_get_proc = NULL;
GDExtensionClassLibraryPtr godot_rb_library = NULL;

struct godot_rb_gdextension godot_rb_gdextension;
VALUE godot_rb_mGodot;

__attribute__((used)) GDExtensionBool godot_rb_main(
  GDExtensionInterfaceGetProcAddress p_get_proc_address,
  GDExtensionClassLibraryPtr p_library,
  GDExtensionInitialization* r_initialization
) {
  // Set up de/initialization functions
  r_initialization->minimum_initialization_level = GDEXTENSION_INITIALIZATION_CORE;
  r_initialization->initialize = godot_rb_setup;
  r_initialization->deinitialize = godot_rb_cleanup;
  // Save GDExtension
  godot_rb_get_proc = p_get_proc_address;
  godot_rb_library = p_library;
  // Initialize API variables
  for(GDExtensionInitializationLevel i = 0; i < GDEXTENSION_MAX_INITIALIZATION_LEVEL; ++i)
    godot_rb_init_levels[i] = false;
  // Save GDExtension Interface
  #define l(proc_t, proc) godot_rb_gdextension.proc = (GDExtensionInterface##proc_t)godot_rb_get_proc(#proc);
  l(PrintErrorWithMessage, print_error_with_message)
  l(PrintWarningWithMessage, print_warning_with_message)
  l(MemAlloc, mem_alloc)
  l(MemFree, mem_free)
  l(VariantConstruct, variant_construct)
  l(VariantNewCopy, variant_new_copy)
  l(VariantDestroy, variant_destroy)
  l(VariantGetType, variant_get_type)
  l(VariantCall, variant_call)
  l(VariantBooleanize, variant_booleanize)
  // Success
  return true;
}

void godot_rb_warn (const char* message, const char* func, const char* file, int32_t line) {
  godot_rb_gdextension.print_warning_with_message(message, message, func, file, line, false);
}
void godot_rb_error(const char* message, const char* func, const char* file, int32_t line) {
  godot_rb_gdextension.print_error_with_message  (message, message, func, file, line, false);
}

bool godot_rb_protect(VALUE (* function)(VALUE value), VALUE value, const char* func, const char* file, int32_t line) {
  int state;
  rb_protect(function, value, &state);
  if(state) { // Handle exception
    value = rb_funcall(rb_errinfo(), rb_intern("inspect"), 0);
    rb_set_errinfo(Qnil); // Clear exception
    godot_rb_error(StringValueCStr(value), __func__ , __FILE__, line);
    return false;
  }
  return true;
}
