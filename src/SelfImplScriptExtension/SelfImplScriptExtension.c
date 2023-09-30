#include "SelfImplScriptExtension.h"

struct SISEClassData {
  GDExtensionStringName script_name, parent_class_name, _can_instantiate, _instance_create;
  GDExtensionScriptInstancePtr script_script;
  GDExtensionMethodBindPtr set_script;
  GDExtensionInterfaceVariantDestroy variant_destroy;
  GDExtensionInterfaceObjectMethodBindPtrcall object_method_bind_ptrcall;
  GDExtensionInterfaceObjectSetInstance object_set_instance;
  GDExtensionInterfaceClassdbConstructObject classdb_construct_object;
  GDExtensionVariantFromTypeConstructorFunc variant_from_object_ptr;
};


static GDExtensionObjectPtr create_instance(void* class_userdata) {
  struct SISEClassData* d = (struct SISEClassData*)class_userdata;
  GDExtensionObjectPtr object = d->classdb_construct_object(&d->parent_class_name);
  d->object_set_instance(object, &d->script_name, d->script_script);
    // typedef GDExtensionScriptInstancePtr GDExtensionClassInstancePtr
  /** GDExtensionVariant */char variant[40];
  d->variant_from_object_ptr(&variant, &object);
  d->object_method_bind_ptrcall(
    d->set_script, object, (GDExtensionConstTypePtr[]){variant}, /* NULL */ &class_userdata
  );
  d->variant_destroy(&variant);
  return object;
}

/** No-op. Confusing GDExtension API: {create_instance} returns an Object, but this receives a custom pointer. */
static void free_instance(void* _class_userdata, GDExtensionClassInstancePtr _self) {}


/** `bool _can_instantiate()` – return `true` like a YES man */
static void can_instantiate(
  GDExtensionClassInstancePtr _self, const GDExtensionConstTypePtr* _args, GDExtensionTypePtr r_ret)
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
  GDExtensionScriptInstancePtr script_script,
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
) {
  struct SISEClassData* class_userdata = mem_alloc(sizeof(struct SISEClassData));
  class_userdata->script_name = string_name_from_ascii_chars(script_name);
  class_userdata->parent_class_name = string_name_from_ascii_chars("ScriptExtension");
  class_userdata->_can_instantiate = string_name_from_ascii_chars("_can_instantiate");
  class_userdata->_instance_create = string_name_from_ascii_chars("_instance_create");
  class_userdata->script_script = script_script;
  class_userdata->variant_destroy = variant_destroy;
  class_userdata->object_method_bind_ptrcall = object_method_bind_ptrcall;
  class_userdata->object_set_instance = object_set_instance;
  class_userdata->classdb_construct_object = classdb_construct_object;
  class_userdata->variant_from_object_ptr = variant_from_object_ptr;
  
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
  
  return class_userdata;
}

void destroy_SelfImplScriptExtension(struct SISEClassData* class_userdata,
  GDExtensionInterfaceMemFree mem_free,
  GDExtensionInterfaceClassdbUnregisterExtensionClass classdb_unregister_extension_class,
  GDExtensionPtrDestructor string_name_destroy,
  GDExtensionClassLibraryPtr p_library
) {
  classdb_unregister_extension_class(p_library, class_userdata->script_name);
  string_name_destroy(class_userdata->script_name);
  string_name_destroy(class_userdata->parent_class_name);
  string_name_destroy(class_userdata->_can_instantiate);
  string_name_destroy(class_userdata->_instance_create);
  mem_free(class_userdata);
}
