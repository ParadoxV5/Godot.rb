#include <godot_rb.h>
#include "setup.h"
#include "cleanup.h"

GDExtensionInitializationLevel godot_rb_init_level_next = GDEXTENSION_INITIALIZATION_CORE;
GDExtensionInterfaceGetProcAddress godot_rb_get_proc = NULL;
GDExtensionClassLibraryPtr godot_rb_library = NULL;

struct godot_rb_gdextension godot_rb_gdextension;
VALUE godot_rb_mGodot;

GDExtensionBool godot_rb_main(
  GDExtensionInterfaceGetProcAddress get_proc_address,
  GDExtensionClassLibraryPtr library,
  GDExtensionInitialization* r_initialization
) {
  // Set up de/initialization functions
  r_initialization->minimum_initialization_level = GDEXTENSION_INITIALIZATION_CORE;
  r_initialization->initialize = godot_rb_setup;
  r_initialization->deinitialize = godot_rb_cleanup;
  // Save GDExtension
  godot_rb_get_proc = get_proc_address;
  godot_rb_library = library;
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
  l(StringNewWithLatin1Chars, string_new_with_latin1_chars)
  l(StringNewWithUtf32CharsAndLen, string_new_with_utf32_chars_and_len)
  l(ObjectGetClassName, object_get_class_name)
  // Load GDExtension API
  godot_rb_gdextension.variant_from_string      = godot_rb_gdextension.get_variant_from_type_constructor(GDEXTENSION_VARIANT_TYPE_STRING);
  godot_rb_gdextension.variant_from_string_name = godot_rb_gdextension.get_variant_from_type_constructor(GDEXTENSION_VARIANT_TYPE_STRING_NAME);
  godot_rb_gdextension.variant_from_object_ptr  = godot_rb_gdextension.get_variant_from_type_constructor(GDEXTENSION_VARIANT_TYPE_OBJECT);
  godot_rb_gdextension.string_from_variant      = godot_rb_gdextension.get_variant_to_type_constructor(GDEXTENSION_VARIANT_TYPE_STRING);
  godot_rb_gdextension.string_name_from_variant = godot_rb_gdextension.get_variant_to_type_constructor(GDEXTENSION_VARIANT_TYPE_STRING_NAME);
  godot_rb_gdextension.object_ptr_from_variant  = godot_rb_gdextension.get_variant_to_type_constructor(GDEXTENSION_VARIANT_TYPE_OBJECT);
  GDExtensionInterfaceVariantGetPtrConstructor variant_get_ptr_constructor =
    (GDExtensionInterfaceVariantGetPtrConstructor)godot_rb_get_proc("variant_get_ptr_constructor");
  godot_rb_gdextension.string_from_string_name = variant_get_ptr_constructor(GDEXTENSION_VARIANT_TYPE_STRING, 2);
  godot_rb_gdextension.string_name_from_string = variant_get_ptr_constructor(GDEXTENSION_VARIANT_TYPE_STRING_NAME, 2);
    // Both Constructor #2
  GDExtensionInterfaceVariantGetPtrDestructor variant_get_ptr_destructor =
    (GDExtensionInterfaceVariantGetPtrDestructor)godot_rb_get_proc("variant_get_ptr_destructor");
  godot_rb_gdextension.string_destroy      = variant_get_ptr_destructor(GDEXTENSION_VARIANT_TYPE_STRING     );
  godot_rb_gdextension.string_name_destroy = variant_get_ptr_destructor(GDEXTENSION_VARIANT_TYPE_STRING_NAME);
  
  // Success
  return true;
}

bool godot_rb_protect(void (* function)(va_list* args), ...) {
  va_list va;
  va_start(va, function);
  int state;
  // {VALUE} ↔️ {void*} casts are exact; return is `void`ed
  rb_protect((VALUE (*)(VALUE))function, (VALUE)&va, &state);
  va_end(va);
  if RB_UNLIKELY(state) { // Handle exception
    VALUE error = rb_errinfo();
    rb_set_errinfo(Qnil); // Clear exception
    VALUE full_message = rb_funcall(error, rb_intern("full_message"), 0);
    char* message = StringValueCStr(full_message);
    GDExtensionInterfacePrintErrorWithMessage print_error =
      RB_LIKELY(rb_obj_is_kind_of(error, rb_eStandardError))
      ? godot_rb_gdextension.print_script_error_with_message
      : godot_rb_gdextension.print_error_with_message;
    // So many {Qnil} checks ugh
    VALUE backtrace = rb_funcall(error, rb_intern("backtrace_locations"), 0);
    if RB_LIKELY(!NIL_P(backtrace)) {
      backtrace = rb_ary_entry(backtrace, 0);
      if RB_LIKELY(!NIL_P(backtrace)) {
        VALUE func = rb_funcall(backtrace, rb_intern("label"), 0);
        VALUE file = rb_funcall(backtrace, rb_intern("path" ), 0);
        int32_t line;
        rb_integer_pack(
          rb_funcall(backtrace, rb_intern("lineno"), 0),
          &line,
          1,
          sizeof(int32_t),
          0,
          INTEGER_PACK_2COMP | INTEGER_PACK_NATIVE_BYTE_ORDER
        );
        print_error(
          message, message,
          StringValueCStr(func),
          RB_UNLIKELY(NIL_P(file)) ? "Godot.rb" : StringValueCStr(file), // may be {Qnil} too
          line,
          true
        );
        return false;
      }
    }
    // No backtrace info available if it reaches this point
    print_error(message, message, __func__, "Godot.rb", 0, false);
    return false;
  }
  return true;
}

void Init_godot_rb() {}
