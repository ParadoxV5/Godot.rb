#ifndef GODOT_RB_H
#define GODOT_RB_H

#include <ruby.h>
#include <godot/gdextension_interface.h>


// GDExtension (initialized by entry function) //

extern bool godot_rb_init_levels[GDEXTENSION_MAX_INITIALIZATION_LEVEL];

//extern GDExtensionInterfaceGetProcAddress godot_rb_get_proc;
extern GDExtensionClassLibraryPtr godot_rb_library;


// GDExtension Interface (initialized by entry function) //

/** @experimental Some members may get moved to file-private globals instead. */
extern struct godot_rb_gdextension {
  //TODO: Categorize
  GDExtensionInterfacePrintErrorWithMessage   print_error_with_message;
  GDExtensionInterfacePrintWarningWithMessage print_warning_with_message;
  GDExtensionInterfaceMemAlloc mem_alloc;
  GDExtensionInterfaceMemFree  mem_free;
  GDExtensionInterfaceGetVariantToTypeConstructor   get_variant_to_type_constructor;
  GDExtensionInterfaceGetVariantFromTypeConstructor get_variant_from_type_constructor;
  GDExtensionInterfaceVariantConstruct  variant_construct;
  GDExtensionInterfaceVariantNewNil     variant_new_nil;
  GDExtensionInterfaceVariantDuplicate  variant_duplicate;
  GDExtensionInterfaceVariantDestroy    variant_destroy;
  GDExtensionInterfaceVariantGetType    variant_get_type;
  GDExtensionInterfaceVariantGet        variant_get;
  GDExtensionInterfaceVariantSet        variant_set;
  GDExtensionInterfaceVariantCall       variant_call;
  GDExtensionInterfaceVariantBooleanize variant_booleanize;
  GDExtensionInterfaceStringToUtf8Chars  string_to_utf8_chars;
  GDExtensionInterfaceStringToUtf32Chars string_to_utf32_chars;
  GDExtensionInterfaceStringNewWithUtf32CharsAndLen string_new_with_utf32_chars_and_len;
  // String APIs
  GDExtensionVariantFromTypeConstructorFunc variant_from_string;
  GDExtensionTypeFromVariantConstructorFunc string_from_variant;
  GDExtensionPtrDestructor string_destroy;
  // StringName APIs
  GDExtensionPtrConstructor string_name_from_string;
  GDExtensionPtrDestructor string_name_destroy;
} godot_rb_gdextension;

/** Stand-ins for Godot Engine internal types */
typedef void* GDExtensionString, GDExtensionStringName;


// Ruby Binding Constants (initialized at level `SERVERS`) //

extern VALUE godot_rb_mGodot;
extern VALUE godot_rb_cVariant;
extern VALUE godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_VARIANT_MAX];

#define godot_rb_idVARIANT_TYPE rb_intern("VARIANT_TYPE")


// Variant Helpers (initialized at level `SERVERS`) //

extern GDExtensionVariantPtr godot_rb_variant_alloc();

/** @note The variant will be freed when the returned Variant gets GC-ed. */
VALUE godot_rb_wrap_variant(VALUE klass, GDExtensionVariantPtr variant);
/** @note
  The variant will be freed when the returned Variant gets GC-ed or,
  for auto-converted variants (i.e., the trileans), by the time this function returns.
*/
VALUE godot_rb_parse_variant(GDExtensionVariantPtr variant);

/**
  Like {#godot_rb_cVariant_to_variant}, but calls {#to_godot} as needed
  @note Do not free the returned variant; GC takes care of it
*/
GDExtensionVariantPtr godot_rb_obj_get_variant(VALUE self);
/**
  @param self must be a Godot::Variant; use {#godot_rb_obj_to_variant} if not necessarily
  @note Do not free the returned variant; GC takes care of it
*/
GDExtensionVariantPtr godot_rb_cVariant_get_variant(VALUE self);

/** @see godot_rb_variant_call */
typedef void (* godot_rb_variant_call_function)(
  GDExtensionVariantPtr self_variant,
  VALUE func,
  long argc,
  GDExtensionConstVariantPtr* argv,
  uintmax_t var,
  GDExtensionCallError* r_error
);
/**
  @param func
    a function name (RBS: `interned`);
    this is reported in the event of a {rb_eNoMethodError} ({GDEXTENSION_CALL_ERROR_INVALID_METHOD})
  @param var
    an extra arg directly passed to the function for your various purposes
  @return if no {GDExtensionCallError error} raised, otherwise it {rb_raise go-tos instead of returning}
*/
void godot_rb_variant_call(godot_rb_variant_call_function function, VALUE self, VALUE func, VALUE args, uintmax_t var);


// General Helpers //

#define godot_rb_require_relative(name) rb_require_string(rb_str_new_lit("./addons/Godot.rb/lib/godot/"#name".rb"))
#include <ruby/encoding.h>
/** (initialized at level `SERVERS`) */
extern rb_encoding* godot_rb_encoding_UTF32;

#define godot_rb_error(message) \
  godot_rb_gdextension.print_error_with_message  (message, message, __func__, __FILE__, __LINE__, false)
/**
  (assumes level `CORE` set up)
  @note This passes {Qnil} to the passed function and discards its return
*/
bool godot_rb_protect(VALUE (*function)(__attribute__((unused)) VALUE value));

/** Entry Function */
__attribute__((used)) GDExtensionBool godot_rb_main(
  GDExtensionInterfaceGetProcAddress p_get_proc_address,
  GDExtensionClassLibraryPtr p_library,
  GDExtensionInitialization* r_initialization
);


#endif
