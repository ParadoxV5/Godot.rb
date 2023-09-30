#ifndef GODOT_ENGINE_SELF_IMPL_SCRIPT_EXTENSION_H
#define GODOT_ENGINE_SELF_IMPL_SCRIPT_EXTENSION_H
#include <stdbool.h>
#include <godot/gdextension_interface.h>

/** Stand-in type for de-referencing a `GDExtensionStringNamePtr` (good for stack allocations) */
typedef void* GDExtensionStringName;

/**
  Register a `ScriptExtension` extension class whose instances attach
  to one the class’s own script instances for self-implementation.
  @param script_name
    the common name of the scripts (really only used for the extension class’s name – it’s just good to be consistent)
  @param script_script
    This is the script for the scripts.
    You must keep it valid until {destroy_SelfImplScriptExtension}, after which YOU free it whenëver convenient.
  @param script_script_instance
    This is the `ScriptInstance` for `script_script`.
    Like `script_script`, you must keep it valid and free it after {destroy_SelfImplScriptExtension}.
  @return
    an opaque pointer for {destroy_SelfImplScriptExtension}
    (Assuming expectations, this can cast to a `GDExtensionConstStringNamePtr` of the `script_name` arg.)
  @note
    Mandatory virtual methods `#_can_instantiate` and `#_instance_create` drive script instantiations;
    therefore, a script requires them defined in order attach even if minimally implemented.
    The template extension class supply implementations for the two methods, but only with self-attaching in mind.
    Your self-implementation should still override both of them according to your states and logic.
  @see
    destroy_SelfImplScriptExtension
*/
struct SISEClassData* init_SelfImplScriptExtension(
  char* script_name,
  GDExtensionVariantPtr script_script,
  GDExtensionScriptInstancePtr script_script_instance,
  GDExtensionInterfaceMemAlloc mem_alloc,
  GDExtensionInterfaceVariantDestroy variant_destroy,
  GDExtensionInterfaceObjectMethodBindPtrcall object_method_bind_ptrcall,
  GDExtensionInterfaceObjectSetInstance object_set_instance,
  GDExtensionInterfaceClassdbConstructObject classdb_construct_object,
  GDExtensionInterfaceClassdbGetMethodBind classdb_get_method_bind,
  GDExtensionInterfaceClassdbRegisterExtensionClass classdb_register_extension_class,
  GDExtensionVariantFromTypeConstructorFunc variant_from_object_ptr,
  GDExtensionPtrDestructor string_name_destroy,
  GDExtensionStringName (* string_name_from_ascii_chars)(const char* ascii),
  GDExtensionClassLibraryPtr p_library
);

/** Unregister the extension class and free up `class_userdata`
  @see init_SelfImplScriptExtension
*/
void destroy_SelfImplScriptExtension(struct SISEClassData* class_userdata,
  GDExtensionInterfaceMemFree mem_free,
  GDExtensionInterfaceClassdbUnregisterExtensionClass classdb_unregister_extension_class,
  GDExtensionPtrDestructor string_name_destroy,
  GDExtensionClassLibraryPtr p_library
);

#endif
