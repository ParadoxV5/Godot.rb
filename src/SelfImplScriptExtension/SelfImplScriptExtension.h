#ifndef GODOT_ENGINE_SELF_IMPL_SCRIPT_EXTENSION_H
#define GODOT_ENGINE_SELF_IMPL_SCRIPT_EXTENSION_H
#include <stdbool.h>
#include <godot/gdextension_interface.h>

/** Stand-in type for de-referencing a `GDExtensionStringNamePtr` (good for stack allocations) */
typedef void* GDExtensionStringName;

/** Backbone implementation function type for `#_instance_create`
  @see init_SelfImplScriptExtension
*/
typedef GDExtensionScriptInstancePtr (* GDExtensionInstanceCreate)(GDExtensionConstObjectPtr self, GDExtensionConstObjectPtr for_object);

/**
  Register a `ScriptExtension` extension class whose instances attach
  to one the class’s own script instances for self-implementation.
  @param instance_create
    the backbone implementation for `#_instance_create` (Make sure it can instantiate its own script!)
  @return
    an opaque pointer for {destroy_SelfImplScriptExtension}
    (Assuming expectations, this can cast to a `GDExtensionConstStringNamePtr` of the `script_name` arg.)
  @note
    Mandatory methods `#_can_instantiate` and `#_instance_create` drive script instantiations;
    therefore, a script requires them defined in order attach even if minimally implemented.
    * The extension class always return `true` for `#_can_instantiate` to allow instances to attach one of their own
      (done by the constructor). Therefore, your self-implementation should still override it according to your states.
    * The extension class implements `#_instance_create` as merely a delegate to the provided `instance_create` arg. 
*/
struct SISEClassData* init_SelfImplScriptExtension(
  char* script_name,
  GDExtensionInstanceCreate instance_create_impl,
  GDExtensionInterfaceMemAlloc mem_alloc,
  GDExtensionInterfaceObjectMethodBindPtrcall object_method_bind_ptrcall,
  GDExtensionInterfaceObjectSetInstance object_set_instance,
  GDExtensionInterfaceClassdbConstructObject classdb_construct_object,
  GDExtensionInterfaceClassdbGetMethodBind classdb_get_method_bind,
  GDExtensionInterfaceClassdbRegisterExtensionClass classdb_register_extension_class,
  GDExtensionInterfaceClassdbRegisterExtensionClassMethod classdb_register_extension_class_method,
  GDExtensionVariantFromTypeConstructorFunc variant_from_bool,
  GDExtensionVariantFromTypeConstructorFunc variant_from_object_ptr,
  GDExtensionTypeFromVariantConstructorFunc object_ptr_from_variant,
  GDExtensionStringName (* string_name_from_latin1)(const char* latin1),
  GDExtensionPtrDestructor string_name_destroy,
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
