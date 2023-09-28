#include "SelfImplScriptExtension.h"

struct SISEClassData {
  GDExtensionStringName script_name;
  GDExtensionStringName parent_class_name;
  GDExtensionMethodBindPtr set_script;
  GDExtensionInterfaceObjectMethodBindPtrcall object_method_bind_ptrcall;
  GDExtensionInterfaceObjectSetInstance object_set_instance;
  GDExtensionInterfaceClassdbConstructObject classdb_construct_object;
  struct SISEICData {
    GDExtensionInstanceCreate instance_create_impl;
    GDExtensionVariantFromTypeConstructorFunc variant_from_object_ptr;
    GDExtensionTypeFromVariantConstructorFunc object_ptr_from_variant;
  } instance_create_data;
};


static GDExtensionObjectPtr create_instance(void* class_userdata) {
  struct SISEClassData* d = (struct SISEClassData*)class_userdata;
  GDExtensionObjectPtr object = d->classdb_construct_object(&d->parent_class_name);
  d->object_set_instance(object, &d->script_name, object); // typedef GDExtensionClassInstancePtr GDExtensionObjectPtr
  d->object_method_bind_ptrcall(d->set_script, object, (GDExtensionConstTypePtr[]){object}, NULL);
  return object;
}

/** No-op. Confusing GDExtension API: {create_instance} returns an Object, but this receives a custom pointer. */
static void free_instance(void* _class_userdata, GDExtensionClassInstancePtr _self) {}


/** `bool _can_instantiate()` – return `true` like a YES man */
static void can_instantiate_ptrcall(
  void* instance_create_data, GDExtensionObjectPtr _self,
  const GDExtensionConstTypePtr *_args,
  GDExtensionTypePtr r_ret
) { *(GDExtensionBool*)r_ret = true; }
static void can_instantiate(
  void* variant_from_bool, GDExtensionObjectPtr _self,
  const GDExtensionConstVariantPtr* _argv, GDExtensionInt _argc,
  GDExtensionUninitializedVariantPtr r_ret, GDExtensionCallError* r_error
) {
  const GDExtensionBool t = true;
  ((GDExtensionVariantFromTypeConstructorFunc)variant_from_bool)(r_ret, (GDExtensionBool*)&t);
  r_error->error = GDEXTENSION_CALL_OK;
}

/** `GDExtensionScriptInstancePtr _instance_create(Object for_object)` */
static void instance_create_ptrcall(
  void* instance_create_data, GDExtensionObjectPtr self,
  const GDExtensionConstObjectPtr* for_object_ptr,
  GDExtensionTypePtr r_ret
) {
  *(GDExtensionUninitializedObjectPtr*)r_ret =
    ((struct SISEICData*)instance_create_data)->instance_create_impl(self, *for_object_ptr);
}
static void instance_create(
  void* instance_create_data,
  GDExtensionObjectPtr self,
  const GDExtensionConstVariantPtr* argv, GDExtensionInt argc,
  GDExtensionUninitializedVariantPtr r_ret, GDExtensionCallError* r_error
) {
  if /* likely */(argc == 1) {
    struct SISEICData* d = (struct SISEICData*)instance_create_data;
    GDExtensionObjectPtr object;
    d->object_ptr_from_variant(&object, (GDExtensionVariantPtr)argv);
    instance_create_ptrcall(d->instance_create_impl, self, (GDExtensionConstObjectPtr*)&object, &object);
    d->variant_from_object_ptr(r_ret, &object);
    // `GDExtensionObjectPtr` does not need destruction
    r_error->error = GDEXTENSION_CALL_OK;
    return;
  }
  r_error->error = /* likely */ argc > 0
    ? GDEXTENSION_CALL_ERROR_TOO_MANY_ARGUMENTS
    : GDEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
  r_error->argument = (int32_t)argc;
  r_error->expected = 1;
}


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
) {
  struct SISEClassData* class_userdata = mem_alloc(sizeof(struct SISEClassData));
  class_userdata->script_name = string_name_from_latin1(script_name);
  class_userdata->parent_class_name = string_name_from_latin1("ScriptExtension");
  
  classdb_register_extension_class(p_library,
    &class_userdata->script_name,
    class_userdata->parent_class_name,
    &(GDExtensionClassCreationInfo){
      .class_userdata = class_userdata,
      .is_virtual = false,
      .is_abstract = false,
      .create_instance_func = create_instance,
      .free_instance_func = free_instance,
    }
  );
  
  GDExtensionStringName name = string_name_from_latin1("set_script");
  class_userdata->set_script = classdb_get_method_bind(script_name, &name,
    1114965689 // https://github.com/ParadoxV5/godot-headers/blob/godot-4.1.1-stable/extension_api.json#L147371
  );
  string_name_destroy(&name);
  class_userdata->object_method_bind_ptrcall = object_method_bind_ptrcall;
  class_userdata->object_set_instance = object_set_instance;
  class_userdata->classdb_construct_object = classdb_construct_object;
  class_userdata->instance_create_data.instance_create_impl = instance_create_impl;
  class_userdata->instance_create_data.variant_from_object_ptr = variant_from_object_ptr;
  class_userdata->instance_create_data.object_ptr_from_variant = object_ptr_from_variant;
  
  name = string_name_from_latin1("_can_instantiate");
  classdb_register_extension_class_method(p_library, &script_name, &(GDExtensionClassMethodInfo){
    .name = &name,
    .method_userdata = variant_from_bool, // typedef void GDExtensionVariantFromTypeConstructorFunc
    .call_func = can_instantiate,
    .ptrcall_func = can_instantiate_ptrcall,
    .method_flags = GDEXTENSION_METHOD_FLAG_CONST,
    .has_return_value = true,
    .return_value_info = &(GDExtensionPropertyInfo){.type = GDEXTENSION_VARIANT_TYPE_BOOL},
    .argument_count = 0,
    .default_argument_count = 0
  });
  string_name_destroy(&name);
  
  name = string_name_from_latin1("_instance_create");
  classdb_register_extension_class_method(p_library, &script_name, &(GDExtensionClassMethodInfo){
    .name = &name,
    .method_userdata = &class_userdata->instance_create_data, // typedef void SISEICData
    .call_func = instance_create,
    .ptrcall_func = instance_create_ptrcall,
    .method_flags = GDEXTENSION_METHOD_FLAG_CONST,
    .has_return_value = true,
    .return_value_info = &(GDExtensionPropertyInfo){.type = GDEXTENSION_VARIANT_TYPE_OBJECT}, // `void*`
    .argument_count = 1,
    .arguments_info = &(GDExtensionPropertyInfo){.type = GDEXTENSION_VARIANT_TYPE_OBJECT},
      // Accept any class once self-implemented; only accept instances without the self-implementation script.
    .default_argument_count = 0
  });
  string_name_destroy(&name);
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
  mem_free(class_userdata);
}
