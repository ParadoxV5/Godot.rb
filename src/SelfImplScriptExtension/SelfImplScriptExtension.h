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
  @param variant_from_object_ptr
    need this thanks to https://github.com/godotengine/godot/issues/81734
  @param script_script_instance_from_object
    Given the scripts’ script’s script-less object pointer, return its {GDExtensionScriptInstancePtr}. I.e., your
    `#instance_create` implementation, but only used to set the scripts’ script up. Like `#instance_create`, you must
    promise that the returned instance remain valid until Godot Engine calls the {GDExtensionScriptInstanceInfo}’s
    `free_func`, in which you can free the custom {GDExtensionScriptInstanceDataPtr} or do it whenëver convenient later.
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
  GDExtensionInterfaceMemAlloc mem_alloc,
  GDExtensionInterfaceObjectMethodBindPtrcall object_method_bind_ptrcall,
  GDExtensionInterfaceObjectSetInstance object_set_instance,
  GDExtensionInterfaceClassdbConstructObject classdb_construct_object,
  GDExtensionInterfaceClassdbGetMethodBind classdb_get_method_bind,
  GDExtensionInterfaceClassdbRegisterExtensionClass classdb_register_extension_class,
  GDExtensionVariantFromTypeConstructorFunc variant_from_object_ptr,
  GDExtensionPtrDestructor string_name_destroy,
  GDExtensionStringName (* string_name_from_ascii_chars)(const char* ascii),
  GDExtensionScriptInstancePtr (* script_script_instance_from_object)(GDExtensionConstObjectPtr script_script_object),
  GDExtensionClassLibraryPtr p_library
);

/** Unregister the extension class and free up `class_userdata`
  @param variant_destroy
    corresponds to {init_SelfImplScriptExtension}’s `variant_from_object_ptr` arg:
    https://github.com/godotengine/godot/issues/81734
  @see init_SelfImplScriptExtension
*/
void destroy_SelfImplScriptExtension(struct SISEClassData* class_userdata,
  GDExtensionInterfaceMemFree mem_free,
  GDExtensionInterfaceVariantDestroy variant_destroy,
  GDExtensionInterfaceClassdbUnregisterExtensionClass classdb_unregister_extension_class,
  GDExtensionPtrDestructor string_name_destroy,
  GDExtensionClassLibraryPtr p_library
);

#endif
