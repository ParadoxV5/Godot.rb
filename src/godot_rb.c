#include <godot_rb.h>
#include "setup.h"
#include "cleanup.h"

bool godot_rb_init_levels[GDEXTENSION_MAX_INITIALIZATION_LEVEL] = {};
GDExtensionInterfaceGetProcAddress godot_rb_get_proc = NULL;
GDExtensionClassLibraryPtr godot_rb_library = NULL;

struct godot_rb_gdextension godot_rb_gdextension;
VALUE godot_rb_mGodot;
VALUE godot_rb_cObject;

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
  // Load GDExtension Interface
  #define l(proc_t, proc) godot_rb_gdextension.proc = (GDExtensionInterface##proc_t)godot_rb_get_proc(#proc);
  l(PrintErrorWithMessage, print_error_with_message)
  l(PrintScriptErrorWithMessage, print_script_error_with_message)
  l(MemAlloc, mem_alloc)
  l(MemFree,  mem_free)
  l(GetVariantToTypeConstructor,   get_variant_to_type_constructor)
  l(GetVariantFromTypeConstructor, get_variant_from_type_constructor)
  l(VariantConstruct,  variant_construct)
  l(VariantNewNil,     variant_new_nil)
  l(VariantDuplicate,  variant_duplicate)
  l(VariantDestroy,    variant_destroy)
  l(VariantGetType,    variant_get_type)
  l(VariantGet,        variant_get)
  l(VariantSet,        variant_set)
  l(VariantCall,       variant_call)
  l(VariantBooleanize, variant_booleanize)
  l(StringToUtf8Chars,  string_to_utf8_chars)
  l(StringToUtf32Chars, string_to_utf32_chars)
  l(StringNewWithUtf32CharsAndLen, string_new_with_utf32_chars_and_len)
  l(ObjectGetClassName, object_get_class_name)
  // Load GDExtension API
  godot_rb_gdextension.variant_from_string      = godot_rb_gdextension.get_variant_from_type_constructor(GDEXTENSION_VARIANT_TYPE_STRING);
  godot_rb_gdextension.variant_from_string_name = godot_rb_gdextension.get_variant_from_type_constructor(GDEXTENSION_VARIANT_TYPE_STRING_NAME);
  godot_rb_gdextension.variant_from_object      = godot_rb_gdextension.get_variant_from_type_constructor(GDEXTENSION_VARIANT_TYPE_OBJECT);
  godot_rb_gdextension.string_from_variant      = godot_rb_gdextension.get_variant_to_type_constructor(GDEXTENSION_VARIANT_TYPE_STRING);
  godot_rb_gdextension.string_name_from_variant = godot_rb_gdextension.get_variant_to_type_constructor(GDEXTENSION_VARIANT_TYPE_STRING_NAME);
  godot_rb_gdextension.object_from_variant      = godot_rb_gdextension.get_variant_to_type_constructor(GDEXTENSION_VARIANT_TYPE_OBJECT);
  GDExtensionInterfaceVariantGetPtrConstructor variant_get_ptr_constructor =
    (GDExtensionInterfaceVariantGetPtrConstructor)godot_rb_get_proc("variant_get_ptr_constructor");
  godot_rb_gdextension.string_from_string_name = variant_get_ptr_constructor(GDEXTENSION_VARIANT_TYPE_STRING, 2);
  godot_rb_gdextension.string_name_from_string = variant_get_ptr_constructor(GDEXTENSION_VARIANT_TYPE_STRING_NAME, 2);
    // Both Constructor #2
  GDExtensionInterfaceVariantGetPtrDestructor variant_get_ptr_destructor =
    (GDExtensionInterfaceVariantGetPtrDestructor)godot_rb_get_proc("variant_get_ptr_destructor");
  godot_rb_gdextension.string_destroy      = variant_get_ptr_destructor(GDEXTENSION_VARIANT_TYPE_STRING     );
  godot_rb_gdextension.string_name_destroy = variant_get_ptr_destructor(GDEXTENSION_VARIANT_TYPE_STRING_NAME);
  godot_rb_gdextension.object_destroy      = variant_get_ptr_destructor(GDEXTENSION_VARIANT_TYPE_OBJECT     );
  
  // Success
  return true;
}

bool godot_rb_protect(VALUE (* function)(VALUE var), VALUE* var_p) {
  int state;
  bool has_var_p = var_p ? true : false;
  if RB_LIKELY(has_var_p) // there’ll be more user calls than our internal calls
    *var_p = rb_protect(function, *var_p, &state);
  else
    rb_protect(function, Qnil, &state);
  if RB_UNLIKELY(state) { // Handle exception
    VALUE error = rb_errinfo();
    rb_set_errinfo(Qnil); // Clear exception
    if RB_LIKELY(has_var_p) // ditto
      *var_p = error;
    VALUE full_message = rb_funcall(error, rb_intern("full_message"), 0);
    char* message = StringValueCStr(full_message);
    GDExtensionInterfacePrintErrorWithMessage print_error =
      RB_LIKELY(rb_obj_is_kind_of(error, rb_eStandardError))
      ? godot_rb_gdextension.print_script_error_with_message
      : godot_rb_gdextension.print_error_with_message;
    // So manu {Qnil} checks ugh
    VALUE backtrace = rb_funcall(error, rb_intern("backtrace_locations"), 0);
    if RB_LIKELY(!NIL_P(backtrace)) {
      backtrace = rb_ary_entry(backtrace, 0);
      if RB_LIKELY(!NIL_P(backtrace)) {
        VALUE func = rb_funcall(backtrace, rb_intern("label"), 0);
        VALUE file = rb_funcall(backtrace, rb_intern("path" ), 0);
        int32_t line;
        rb_integer_pack(rb_funcall(backtrace, rb_intern("lineno"), 0), &line, 1, sizeof(int32_t), 0, INTEGER_PACK_2COMP);
        print_error(
          message, message,
          StringValueCStr(func),
          RB_UNLIKELY(NIL_P(file)) ? "Godot.rb" : StringValueCStr(file), // may be {Qnil} too
          line,
          has_var_p
        );
        return false;
      }
    }
    // No backtrace info available if it reaches this point
    print_error(message, message, __func__, "Godot.rb", 0, has_var_p);
    return false;
  }
  return true;
}
