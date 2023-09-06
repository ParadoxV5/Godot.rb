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
  GDExtensionInterfaceGetVariantFromTypeConstructor get_variant_from_type_constructor;
  GDExtensionInterfaceVariantConstruct variant_construct;
  GDExtensionInterfaceVariantNewCopy variant_new_copy;
  GDExtensionInterfaceVariantDestroy variant_destroy;
  GDExtensionInterfaceVariantGetType variant_get_type;
  GDExtensionInterfaceVariantCall variant_call;
  GDExtensionInterfaceVariantBooleanize variant_booleanize;
  GDExtensionInterfaceStringToUtf8Chars string_to_utf8_chars;
  GDExtensionInterfaceStringToUtf32Chars string_to_utf32_chars;
} godot_rb_gdextension;

// Ruby Binding Constants (initialized at level `SCENE` except `Godot` at `SERVERS`)
extern VALUE godot_rb_mGodot, godot_rb_cVariant, godot_rb_cVariant_c_VARIANTS;

// Entry Function
__attribute__((used)) GDExtensionBool godot_rb_main(
  GDExtensionInterfaceGetProcAddress p_get_proc_address,
  GDExtensionClassLibraryPtr p_library,
  GDExtensionInitialization* r_initialization
);

// Variant Helpers
GDExtensionVariantPtr godot_rb_cVariant_to_variant(VALUE self);
// Return is `#initialize`d (usable)
VALUE godot_rb_cVariant_from_variant(GDExtensionConstVariantPtr variant);

// General Helpers //

#define godot_rb_require_relative(name) rb_require_string(rb_str_new_lit("./addons/Godot.rb/lib/godot/"#name".rb"))
#define godot_rb_idVARIANT_TYPE rb_intern("VARIANT_TYPE")

#define godot_rb_warn (message) \
  godot_rb_gdextension.print_warning_with_message(message, message, __func__, __FILE__, __LINE__, false)
#define godot_rb_error(message) \
  godot_rb_gdextension.print_error_with_message  (message, message, __func__, __FILE__, __LINE__, false)
// (assumes level `CORE` set up)
// Note: This discards the return of the passed function
bool godot_rb_protect(VALUE (* function)(__attribute__((unused)) VALUE value));

#endif
