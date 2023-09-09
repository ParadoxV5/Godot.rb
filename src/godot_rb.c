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
  // Load GDExtension Interface
  #define l(proc_t, proc) godot_rb_gdextension.proc = (GDExtensionInterface##proc_t)godot_rb_get_proc(#proc);
  l(PrintErrorWithMessage, print_error_with_message)
  l(PrintWarningWithMessage, print_warning_with_message)
  l(MemAlloc, mem_alloc)
  l(MemFree,  mem_free)
  l(GetVariantToTypeConstructor,   get_variant_to_type_constructor)
  l(GetVariantFromTypeConstructor, get_variant_from_type_constructor)
  l(VariantConstruct,  variant_construct)
  l(VariantNewNil,     variant_new_nil)
  l(VariantNewCopy,    variant_new_copy)
  l(VariantDestroy,    variant_destroy)
  l(VariantGetType,    variant_get_type)
  l(VariantCall,       variant_call)
  l(VariantBooleanize, variant_booleanize)
  l(StringToUtf8Chars,  string_to_utf8_chars)
  l(StringToUtf32Chars, string_to_utf32_chars)
  l(StringNewWithUtf32CharsAndLen, string_new_with_utf32_chars_and_len)
  // Load GDExtension API
  godot_rb_gdextension.variant_from_string = (
    (GDExtensionInterfaceGetVariantFromTypeConstructor)godot_rb_get_proc("get_variant_from_type_constructor")
  )(GDEXTENSION_VARIANT_TYPE_STRING);
  godot_rb_gdextension.string_from_variant = (
    (GDExtensionInterfaceGetVariantToTypeConstructor)godot_rb_get_proc("get_variant_to_type_constructor")
  )(GDEXTENSION_VARIANT_TYPE_STRING);
  godot_rb_gdextension.string_name_from_string = (
    (GDExtensionInterfaceVariantGetPtrConstructor)godot_rb_get_proc("variant_get_ptr_constructor")
  )(GDEXTENSION_VARIANT_TYPE_STRING_NAME, 2); // Constructor #2
  GDExtensionInterfaceVariantGetPtrDestructor variant_get_ptr_destructor =
    (GDExtensionInterfaceVariantGetPtrDestructor)godot_rb_get_proc("variant_get_ptr_destructor");
  godot_rb_gdextension.string_destroy      = variant_get_ptr_destructor(GDEXTENSION_VARIANT_TYPE_STRING     );
  godot_rb_gdextension.string_name_destroy = variant_get_ptr_destructor(GDEXTENSION_VARIANT_TYPE_STRING_NAME);
  
  // Success
  return true;
}

bool godot_rb_protect(VALUE (* function)(__attribute__((unused)) VALUE value)) {
  int state;
  rb_protect(function, Qnil, &state);
  if RB_UNLIKELY(state) { // Handle exception
    VALUE error = rb_errinfo();
    rb_set_errinfo(Qnil); // Clear exception
    error = rb_funcall(error, rb_intern("full_message"), 0);
    godot_rb_error(StringValueCStr(error));
    return false;
  }
  return true;
}
