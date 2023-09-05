#ifndef GODOT_RB_H
#define GODOT_RB_H

#include <ruby.h>
#include <godot/gdextension_interface.h>


// GDExtension (initialized by entry function) //
extern bool godot_rb_init_levels[GDEXTENSION_MAX_INITIALIZATION_LEVEL];
extern GDExtensionInterfaceGetProcAddress godot_rb_get_proc;
extern GDExtensionClassLibraryPtr godot_rb_library;

// GDExtension Interface (initialized by entry function) //
extern struct godot_rb_gdextension {
  GDExtensionInterfacePrintErrorWithMessage print_error_with_message;
  GDExtensionInterfacePrintWarningWithMessage print_warning_with_message;
  GDExtensionInterfaceMemAlloc mem_alloc;
  GDExtensionInterfaceMemFree mem_free;
  GDExtensionInterfaceVariantConstruct variant_construct;
  GDExtensionInterfaceVariantNewCopy variant_new_copy;
  GDExtensionInterfaceVariantDestroy variant_destroy;
  GDExtensionInterfaceVariantGetType variant_get_type;
  GDExtensionInterfaceVariantCall variant_call;
  GDExtensionInterfaceVariantBooleanize variant_booleanize;
} godot_rb_gdextension;

// Ruby Binding Modules/Classes (initialized at level `SCENE` except `Godot` at `SERVERS`)
extern VALUE godot_rb_mGodot, godot_rb_cVariant;

// Entry Function
__attribute__((used)) GDExtensionBool godot_rb_main(
  GDExtensionInterfaceGetProcAddress p_get_proc_address,
  GDExtensionClassLibraryPtr p_library,
  GDExtensionInitialization* r_initialization
);

// Helpers //

void godot_rb_warn (const char* message, const char* func, const char* file, int32_t line);
void godot_rb_error(const char* message, const char* func, const char* file, int32_t line);
// (assumes level `CORE` set up)
// Note: This discards the return of the passed function
bool godot_rb_protect(VALUE (* function)(VALUE value), VALUE value, const char* func, const char* file, int32_t line);

#endif
