#ifndef GODOT_RB_H
#define GODOT_RB_H

#include <ruby.h>
#include <godot/godot/gdextension_interface.h>


// GDExtension (initialized by entry function) //

// The level Godot.rb have initialized + 1
extern GDExtensionInitializationLevel godot_rb_init_level_next;

extern GDExtensionInterfaceGetProcAddress godot_rb_get_proc;
extern GDExtensionClassLibraryPtr godot_rb_library;


// GDExtension Interface (initialized by entry function) //

/** @deprecated Members shall be moved to file-private globals instead. */
extern struct godot_rb_gdextension {
  GDExtensionInterfacePrintErrorWithMessage print_error_with_message;
  GDExtensionInterfacePrintScriptErrorWithMessage print_script_error_with_message;
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
  GDExtensionInterfaceObjectGetClassName object_get_class_name;
  // String APIs
  GDExtensionTypeFromVariantConstructorFunc string_from_variant;
  GDExtensionVariantFromTypeConstructorFunc variant_from_string;
  GDExtensionPtrDestructor string_destroy;
  // StringName APIs
  GDExtensionTypeFromVariantConstructorFunc string_name_from_variant;
  GDExtensionPtrConstructor string_from_string_name;
  GDExtensionPtrConstructor string_name_from_string;
  GDExtensionVariantFromTypeConstructorFunc variant_from_string_name;
  GDExtensionPtrDestructor string_name_destroy;
  // Object APIs
  GDExtensionTypeFromVariantConstructorFunc object_ptr_from_variant;
  GDExtensionVariantFromTypeConstructorFunc variant_from_object_ptr;
  GDExtensionPtrDestructor object_destroy;
} godot_rb_gdextension;


// Ruby Binding Constants (initialized at level `SERVERS`) //

extern VALUE godot_rb_mGodot;
extern VALUE godot_rb_cVariant;
extern VALUE godot_rb_cObject;
extern VALUE godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_VARIANT_MAX];

#define godot_rb_idVARIANT_TYPE rb_intern("VARIANT_TYPE")


// Variant Helpers (initialized at level `SERVERS`) //

extern GDExtensionVariantPtr godot_rb_variant_alloc();

/** @note The variant will be freed when the returned Variant gets GC-ed. */
VALUE godot_rb_wrap_variant(VALUE klass, GDExtensionVariantPtr variant);
/** @note
  The variant will be freed when the returned Variant gets GC-ed or,
  for auto-converted variants (trileans, ints and floats), by the time this function returns.
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
  GDExtensionConstVariantPtr const* argv,
  VALUE var,
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
void godot_rb_variant_call(
  godot_rb_variant_call_function function,
  VALUE self,
  VALUE func,
  int argc, const VALUE* argv,
  VALUE var
);


// General Helpers //

#define godot_rb_require_relative(name) rb_require_string(rb_str_new_lit("./addons/Godot.rb/lib/godot/"#name".rb"))
#define godot_rb_get_module(name) rb_const_get_at(godot_rb_mGodot, rb_intern(#name))

#include <ruby/encoding.h>
/** (initialized at level `SERVERS`) */
extern rb_encoding* godot_rb_encoding_UTF32;

#define godot_rb_error(message) \
  godot_rb_gdextension.print_error_with_message(message, message, __func__, __FILE__, __LINE__, false)
/** `begin`–`rescue`s the passed function.
  If an exception occurs, log a Script Error or, for non-{rb_eStandardError}s, a generic Error;
  also ping the Godot Engine Editor if exception backtrace is available
  
  (assumes level `CORE` set up)
*/
bool godot_rb_protect(void (*function)(va_list* args), ...);

/** Entry Function */
__attribute__((used)) GDExtensionBool godot_rb_main(
  GDExtensionInterfaceGetProcAddress get_proc_address,
  GDExtensionClassLibraryPtr library,
  GDExtensionInitialization* r_initialization
);


#endif
