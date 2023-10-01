#include "SelfImplScriptExtension.h"

struct SISEClassData {
  GDExtensionStringName script_name, parent_class_name, _can_instantiate, _instance_create;
  GDExtensionMethodBindPtr set_script;
  GDExtensionInterfaceObjectMethodBindPtrcall object_method_bind_ptrcall;
  GDExtensionInterfaceObjectSetInstance object_set_instance;
  GDExtensionInterfaceClassdbConstructObject classdb_construct_object;
  GDExtensionScriptInstancePtr script_script_instance;
  /* GDExtensionVariant */ char script_script[40]; // https://github.com/godotengine/godot/issues/81734
};

static GDExtensionObjectPtr create_instance_impl_scriptless(struct SISEClassData* class_userdata) {
  GDExtensionObjectPtr object = class_userdata->classdb_construct_object(&class_userdata->parent_class_name);
  class_userdata->object_set_instance(object, &class_userdata->script_name, class_userdata->script_script_instance);
    // typedef GDExtensionScriptInstancePtr GDExtensionClassInstancePtr
  return object;
}
static void create_instance_impl_set_script(struct SISEClassData* class_userdata, GDExtensionObjectPtr object) {
  class_userdata->object_method_bind_ptrcall(
    class_userdata->set_script,
    object,
    (GDExtensionConstTypePtr[]){class_userdata->script_script},
    /* NULL */ &class_userdata
  );
}

/** {create_instance_impl_scriptless} + {create_instance_impl_set_script} */
static GDExtensionObjectPtr create_instance(void* class_userdata) {
  struct SISEClassData* d = (struct SISEClassData*)class_userdata;
  GDExtensionObjectPtr object = create_instance_impl_scriptless(d);
  create_instance_impl_set_script(d, object);
  return object;
}


/** No-op. Confusing GDExtension API: {create_instance} returns an Object, but this receives a custom pointer. */
static void free_instance(void* _class_userdata, GDExtensionScriptInstancePtr _self) {}


/** `bool _can_instantiate()` – return `true` like a YES man */
static void can_instantiate(
  GDExtensionScriptInstancePtr _self, const GDExtensionConstTypePtr* _args, GDExtensionTypePtr r_ret)
{ *(GDExtensionBool*)r_ret = true; }
/**
  `GDExtensionScriptInstancePtr _instance_create(Object for_object)`
  – the template class only knows the `ScriptInstance` for itself.
*/
static void instance_create(
  GDExtensionScriptInstancePtr self, const GDExtensionConstTypePtr* _args, GDExtensionTypePtr r_ret)
{ *(GDExtensionScriptInstancePtr*)r_ret = self; }

GDExtensionClassCallVirtual get_virtual(void* class_userdata, GDExtensionConstStringNamePtr name) {
  struct SISEClassData* d = (struct SISEClassData*)class_userdata;
  GDExtensionStringName n = *(const GDExtensionStringName*)name;
  // `StringName`s are de-duplicated
  if(n == d->_can_instantiate)
    return can_instantiate;
  else if(n == d->_instance_create)
    return instance_create;
  return NULL;
}


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
) {
  struct SISEClassData* class_userdata = mem_alloc(sizeof(struct SISEClassData));
  class_userdata->script_name = string_name_from_ascii_chars(script_name);
  class_userdata->parent_class_name = string_name_from_ascii_chars("ScriptExtension");
  class_userdata->_can_instantiate = string_name_from_ascii_chars("_can_instantiate");
  class_userdata->_instance_create = string_name_from_ascii_chars("_instance_create");
  class_userdata->object_method_bind_ptrcall = object_method_bind_ptrcall;
  class_userdata->object_set_instance = object_set_instance;
  class_userdata->classdb_construct_object = classdb_construct_object;
  
  classdb_register_extension_class(p_library,
    &class_userdata->script_name,
    &class_userdata->parent_class_name,
    &(GDExtensionClassCreationInfo){
      .is_virtual = false,
      .is_abstract = false,
      .create_instance_func = create_instance,
      .free_instance_func = free_instance,
      .get_virtual_func = get_virtual,
      .class_userdata = class_userdata
    }
  );
  
  GDExtensionStringName set_script = string_name_from_ascii_chars("set_script");
  class_userdata->set_script = classdb_get_method_bind(
    &class_userdata->script_name,
    &set_script,
    1114965689 // https://github.com/ParadoxV5/godot-headers/blob/godot-4.1.1-stable/extension_api.json#L147371
  );
  string_name_destroy(&set_script);
  
  // “`script_script = instance_create(…)`”, but `script_script` must be set halfway in `instance_create`
  GDExtensionObjectPtr script_script = create_instance_impl_scriptless(class_userdata);
  class_userdata->script_script_instance = script_script_instance_from_object(script_script);
  variant_from_object_ptr(class_userdata->script_script, &script_script);
  create_instance_impl_set_script(class_userdata, script_script);
  
  return class_userdata;
}

void destroy_SelfImplScriptExtension(struct SISEClassData* class_userdata,
  GDExtensionInterfaceMemFree mem_free,
  GDExtensionInterfaceVariantDestroy variant_destroy,
  GDExtensionInterfaceClassdbUnregisterExtensionClass classdb_unregister_extension_class,
  GDExtensionPtrDestructor string_name_destroy,
  GDExtensionClassLibraryPtr p_library
) {
  //FIXME: `script_script.free()` (not destroy, *free*)
  variant_destroy(class_userdata->script_script);
  classdb_unregister_extension_class(p_library, class_userdata->script_name);
  string_name_destroy(class_userdata->script_name);
  string_name_destroy(class_userdata->parent_class_name);
  string_name_destroy(class_userdata->_can_instantiate);
  string_name_destroy(class_userdata->_instance_create);
  mem_free(class_userdata);
}
