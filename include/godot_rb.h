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
  GDExtensionInterfaceGetVariantToTypeConstructor get_variant_to_type_constructor;
  GDExtensionInterfaceGetVariantFromTypeConstructor get_variant_from_type_constructor;
  GDExtensionInterfaceVariantConstruct variant_construct;
  GDExtensionInterfaceVariantNewNil variant_new_nil;
  GDExtensionInterfaceVariantNewCopy variant_new_copy;
  GDExtensionInterfaceVariantDestroy variant_destroy;
  GDExtensionInterfaceVariantGetType variant_get_type;
  GDExtensionInterfaceVariantCall variant_call;
  GDExtensionInterfaceVariantBooleanize variant_booleanize;
  GDExtensionInterfaceStringToUtf8Chars string_to_utf8_chars;
  GDExtensionInterfaceStringToUtf32Chars string_to_utf32_chars;
  GDExtensionInterfaceStringNewWithUtf32CharsAndLen string_new_with_utf32_chars_and_len;
} godot_rb_gdextension;

// Ruby Binding Constants (initialized at level `SCENE` except `Godot` at `SERVERS`)
extern VALUE godot_rb_mGodot, godot_rb_cVariant, godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_VARIANT_MAX];

// Entry Function
__attribute__((used)) GDExtensionBool godot_rb_main(
  GDExtensionInterfaceGetProcAddress p_get_proc_address,
  GDExtensionClassLibraryPtr p_library,
  GDExtensionInitialization* r_initialization
);

// Variant Helpers //
extern GDExtensionVariantPtr godot_rb_variant_alloc();
// @note The variant will be freed when the returned Variant gets GC-ed.
VALUE godot_rb_wrap_variant(VALUE klass, GDExtensionVariantPtr variant);
// @note The variant will be freed when the returned Variant gets GC-ed or,
//   for auto-converted variants (i.e., the trileans), by the time this function returns.
VALUE godot_rb_parse_variant(GDExtensionVariantPtr variant);
// Like {#godot_rb_cVariant_to_variant}, but calls {#to_godot} as needed
// @note Do not free the returned variant; GC takes care of it
GDExtensionVariantPtr godot_rb_obj_get_variant(VALUE self);
// @param self must be a Godot::Variant; use {#godot_rb_obj_to_variant} if not necessarily
// @note Do not free the returned variant; GC takes care of it
GDExtensionVariantPtr godot_rb_cVariant_get_variant(VALUE self);

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
