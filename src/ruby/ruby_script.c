/** {RubyLanguage} & co. */

#include "variants.h"
#include "ruby_script.h"

/* General notes for developers:
* `typedef VALUE GDExtensionClassInstancePtr, GDExtensionScriptInstanceDataPtr`
  * {VALUE} ↔️ pointer casts are exact
* C does not specify evaluation order of sibling args (which is important for {va_arg})
*/

VALUE godot_rb_cRubyScript;


GDExtensionInterfaceVariantNewCopy gdext_variant_new_copy;

/** ```c
  GDExtensionScriptInstanceDataPtr self,
  GDExtensionConstStringNamePtr name,
  int suffix, // auto-promoted from `char`
  int argc, // not `GDExtensionInt`!
  const GDExtensionConstVariantPtr* argv, 
  GDExtensionUninitializedVariantPtr r_ret
``` */
static void call_impl(va_list* args) {
  VALUE self = va_arg(*args, VALUE);
  GDExtensionConstStringNamePtr string_name = va_arg(*args, GDExtensionConstStringNamePtr);
  ID name = godot_rb_id_from_string_name(string_name, va_arg(*args, int));
  int argc = va_arg(*args, int);
  const GDExtensionConstVariantPtr* argv = va_arg(*args, const GDExtensionConstVariantPtr*);
  VALUE method_args[argc];
  for(GDExtensionInt i = 0; i < argc; ++i) {
    // Godot Engine would manage `argv` entries, so we make reference copies that the Ruby GC manages separately.
    GDExtensionVariantPtr arg = godot_rb_variant_alloc();
    gdext_variant_new_copy(arg, argv[i]);
    method_args[i] = godot_rb_parse_variant(arg);
  }
  self = rb_funcallv_public(self, name, argc, method_args); // variable reüse
  GDExtensionUninitializedVariantPtr r_ret = va_arg(*args, GDExtensionUninitializedVariantPtr);
  if(r_ret)
    // Godot Engine too would manage the `r_ret` reference.
    gdext_variant_new_copy(r_ret, godot_rb_obj_get_variant(self));
}
void godot_rb_RubyScript_inst_free_list(
  RB_UNUSED_VAR(GDExtensionScriptInstanceDataPtr self), void* list
) { godot_rb_gdextension.mem_free(list); }


GDExtensionBool godot_rb_RubyScript_inst_set(
  GDExtensionClassInstancePtr self, GDExtensionConstStringNamePtr name, GDExtensionConstVariantPtr value
) { return godot_rb_protect(call_impl, self, name, '=', 1, (GDExtensionConstVariantPtr[]){value}, NULL); }

GDExtensionBool godot_rb_RubyScript_inst_get(
  GDExtensionClassInstancePtr self, GDExtensionConstStringNamePtr name, GDExtensionVariantPtr r_value
) { return godot_rb_protect(call_impl, self, name, 0, 0, (GDExtensionConstVariantPtr[]){}, r_value); }

// TODO: Properties
const GDExtensionPropertyInfo* godot_rb_RubyScript_inst_get_property_list(
  GDExtensionClassInstancePtr self, uint32_t* r_count
) {
  *r_count = 0;
  return NULL;
}
GDExtensionBool godot_rb_RubyScript_inst_property_can_revert(
  GDExtensionClassInstancePtr self, GDExtensionConstStringNamePtr name
) { return false; }
GDExtensionBool godot_rb_RubyScript_inst_property_get_revert(
  GDExtensionClassInstancePtr self, GDExtensionConstStringNamePtr name, GDExtensionVariantPtr r_ret
) { return false; }

/** @return the {GDExtensionObjectPtr} attached by this {GDExtensionScriptInstancePtr} */
GDExtensionObjectPtr godot_rb_RubyScript_inst_get_owner(GDExtensionScriptInstanceDataPtr self) {
  GDExtensionObjectPtr object;
  godot_rb_gdextension.object_ptr_from_variant(&object, godot_rb_cVariant_get_variant((VALUE)self));
  return object;
}

// TODO
void godot_rb_RubyScript_inst_get_property_state(
  GDExtensionScriptInstanceDataPtr self, GDExtensionScriptInstancePropertyStateAdd add_func, void* userdata
) {
}

/**
  TODO? RBS Integration
  @return an array of methods including those of the `super` scripts’
*/
const GDExtensionMethodInfo* godot_rb_RubyScript_inst_get_method_list(
  GDExtensionScriptInstanceDataPtr self, uint32_t* r_count
) {
  VALUE methods = rb_funcallv_public((VALUE)self, rb_intern("public_methods"), 0, (VALUE[]){});
  long count = rb_array_len(methods);
  *r_count = (uint32_t)count; // `long` is at least 32 bits
  GDExtensionMethodInfo* method_info = godot_rb_gdextension.mem_alloc(sizeof(const GDExtensionMethodInfo[count]));
  for(long i = 0; i < count; ++i) {
    VALUE method_symbol = rb_ary_entry(methods, i);
    method_info[i].name = godot_rb_gdextension.mem_alloc(sizeof(GDExtensionStringName));
    *(GDExtensionStringName*)method_info[i].name = godot_rb_obj_to_string_name(method_symbol);
    method_info[i].return_value.type = GDEXTENSION_VARIANT_TYPE_NIL;
    method_info[i].return_value.name = godot_rb_gdextension.mem_alloc(sizeof(GDExtensionStringName));
    // method_info[i].return_value.class_name ?
    method_info[i].return_value.hint_string = godot_rb_gdextension.mem_alloc(sizeof(GDExtensionString));
    godot_rb_gdextension.string_new_with_latin1_chars(method_info[i].return_value.hint_string, "");
    godot_rb_gdextension.string_name_from_string(
      method_info[i].return_value.name,
      (const GDExtensionConstTypePtr[]){method_info[i].return_value.hint_string}
    );
    method_info[i].flags = GDEXTENSION_METHOD_FLAG_NORMAL;
    method_info[i].id = i;
    //FIXME: DRY pending
    method_info[i].argument_count = 0;
    method_info[i].arguments = (GDExtensionPropertyInfo[]){}; 
    method_info[i].default_argument_count = 0;
    method_info[i].default_arguments = (GDExtensionVariantPtr[]){};
  }
}

// TODO
GDExtensionVariantType godot_rb_RubyScript_inst_get_property_type(
  GDExtensionScriptInstanceDataPtr self, GDExtensionConstStringNamePtr name, GDExtensionBool* r_is_valid
) {
  *r_is_valid = false;
  return GDEXTENSION_VARIANT_TYPE_NIL;
}

GDExtensionBool godot_rb_RubyScript_inst_has_method(
  GDExtensionScriptInstanceDataPtr self, GDExtensionConstStringNamePtr name
) { return rb_respond_to((VALUE)self, godot_rb_id_from_string_name(name, '\0')); }

void godot_rb_RubyScript_inst_call(GDExtensionScriptInstanceDataPtr self,
  GDExtensionConstStringNamePtr name,
  const GDExtensionConstVariantPtr* argv, GDExtensionInt argc,
  GDExtensionVariantPtr r_ret,
  GDExtensionCallError* r_error
) {
  /*
    Only `TOO_MANY_ARGUMENTS/`TOO_FEW_ARGUMENTS`
    (“ArgumentError: wrong number of arguments (given `argument`, expected `expected`))
    make sense goïng from Ruby land to GDExtension world.
    For consistency and laziness, they too will join the `INVALID_METHOD` umbrella.
  */
  r_error->error = RB_LIKELY(
    godot_rb_protect(call_impl, self, name, 0, argc, argv, r_ret)
  ) ? GDEXTENSION_CALL_OK : GDEXTENSION_CALL_ERROR_INVALID_METHOD;
}

// TODO
void godot_rb_RubyScript_inst_notification(GDExtensionClassInstancePtr self, int32_t notification) {
}

static void to_string_unprotected(va_list* args) {
  VALUE string = rb_obj_as_string(va_arg(*args, VALUE));
  *va_arg(*args, GDExtensionString*) = godot_rb_obj_to_string(string);
}
void godot_rb_RubyScript_inst_to_string(
  GDExtensionClassInstancePtr self, GDExtensionBool* r_is_valid, GDExtensionStringPtr r_out
) { *r_is_valid = godot_rb_protect(to_string_unprotected, self, r_out); }

/** {RefCounted} Callback (here, a no-op)
  @return `true`
    (the default `false` prevents the {RefCounted} from `free`ing itself; see also {godot_rb_RubyScript_inst_free}) 
*/
GDExtensionBool godot_rb_RubyScript_inst_refcount_decremented(
  RB_UNUSED_VAR(GDExtensionScriptInstanceDataPtr self)
) { return true; }

GDExtensionObjectPtr godot_rb_RubyScript_inst_get_script(GDExtensionScriptInstanceDataPtr self) {
  GDExtensionObjectPtr object;
  godot_rb_gdextension.object_ptr_from_variant(
    &object,
    godot_rb_obj_get_variant(rb_const_get_at(CLASS_OF((VALUE)self), rb_intern("RUBY_SCRIPT")))
  );
  return object;
}

GDExtensionScriptLanguagePtr godot_rb_RubyLanguage_object;
/** @param self How can this instance not be Godot.rb? Only if Godot Engine confuses something up. */
GDExtensionScriptLanguagePtr godot_rb_RubyScript_inst_get_language(
  RB_UNUSED_VAR(GDExtensionScriptInstanceDataPtr self)
) { return godot_rb_RubyLanguage_object; }


GDExtensionStringName string_name_RubyScript;
GDExtensionInterfaceObjectSetInstance gdext_object_set_instance;
VALUE godot_rb_RubyScript_impl_allocate(VALUE self, GDExtensionObjectPtr* r_object_ptr) {
  VALUE instance = godot_rb_cObject_m_allocate(self); // manual {rb_call_super}
  godot_rb_gdextension.object_ptr_from_variant(r_object_ptr, godot_rb_cVariant_get_variant(instance));
  gdext_object_set_instance(*r_object_ptr, &string_name_RubyScript, (GDExtensionClassInstancePtr)instance);
  return instance;
}
VALUE godot_rb_cRubyScript_m_allocate(VALUE self) {
  GDExtensionObjectPtr unused_ret;
  return godot_rb_RubyScript_impl_allocate(self, &unused_ret);
}
GDExtensionObjectPtr godot_rb_RubyScript_inst_create_instance(RB_UNUSED_VAR(void* class_userdata)) {
  GDExtensionObjectPtr object_ptr;
  godot_rb_RubyScript_impl_allocate(godot_rb_cRubyScript, &object_ptr);
  return object_ptr;
}

/** Release GC protection, allowing the GC to reap the {VALUE} whenever (when no Ruby-land references either). */
void godot_rb_RubyScript_inst_free(GDExtensionScriptInstanceDataPtr self) {
  rb_gc_unregister_address((VALUE*)&self);
}
void godot_rb_RubyScript_inst_free_instance(RB_UNUSED_VAR(void* class_userdata), GDExtensionClassInstancePtr self) {
  godot_rb_RubyScript_inst_free(self);
}

/*TODO: Ask the Godot community for a description on placeholders and fallbacks
GDExtensionBool godot_rb_RubyScript_inst_is_placeholder(GDExtensionScriptInstanceDataPtr self);
GDExtensionBool godot_rb_RubyScript_inst_set_fallback(
  GDExtensionScriptInstanceDataPtr self, GDExtensionConstStringNamePtr name, GDExtensionConstVariantPtr value
);
GDExtensionBool godot_rb_RubyScript_inst_get_fallback(
  GDExtensionScriptInstanceDataPtr self, GDExtensionConstStringNamePtr name, GDExtensionVariantPtr r_value
);
*/


const GDExtensionScriptInstanceInfo godot_rb_RubyScript_inst_info = {
  #define i(func) .func##_func = godot_rb_RubyScript_inst_##func
  i(set),
  i(get),
  i(get_property_list),
  .free_property_list_func = (GDExtensionScriptInstanceFreePropertyList)godot_rb_RubyScript_inst_free_list,
  i(property_can_revert),
  i(property_get_revert),
  i(get_owner),
  i(get_property_state),
  i(get_method_list),
  .free_method_list_func = (GDExtensionScriptInstanceFreeMethodList)godot_rb_RubyScript_inst_free_list,
  i(get_property_type),
  i(has_method),
  i(call),
  i(notification),
  i(to_string),
  i(refcount_decremented),
  i(get_script),
  i(get_language),
  i(free)
};


VALUE godot_rb_cRubyLanguage_INSTANCE;

void godot_rb_init_RubyScript(void) {
  gdext_variant_new_copy = (GDExtensionInterfaceVariantNewCopy)godot_rb_get_proc("variant_new_copy");
  gdext_object_set_instance = (GDExtensionInterfaceObjectSetInstance)godot_rb_get_proc("object_set_instance");
  
  godot_rb_require_relative(ruby_script);
  godot_rb_cRubyScript = godot_rb_get_module(RubyScript);
  rb_gc_register_mark_object(godot_rb_cRubyScript);
  rb_define_alloc_func(godot_rb_cRubyScript, godot_rb_cRubyScript_m_allocate);
  
  string_name_RubyScript = godot_rb_chars_to_string_name("RubyScript");
  GDExtensionStringName string_name_ScriptExtension = godot_rb_chars_to_string_name("ScriptExtension");
  ((GDExtensionInterfaceClassdbRegisterExtensionClass)godot_rb_get_proc("classdb_register_extension_class"))(
    godot_rb_library, &string_name_RubyScript, &string_name_ScriptExtension, &(GDExtensionClassCreationInfo){
      .is_virtual = false,
      .is_abstract = false,
      i(set),
      i(get),
      i(get_property_list),
      .free_property_list_func = (GDExtensionClassFreePropertyList)godot_rb_RubyScript_inst_free_list,
      i(property_can_revert),
      i(property_get_revert),
      i(notification),
      i(to_string),
      i(create_instance),
      i(free_instance),
      .get_virtual_func = godot_rb_init_RubyScript_inst_get_virtual()
      //i(get_rid), // ?
    }
  );
  godot_rb_gdextension.string_name_destroy(&string_name_ScriptExtension);
  
  godot_rb_require_relative(ruby_language);
  godot_rb_cRubyLanguage_INSTANCE = rb_const_get_at(godot_rb_get_module(RubyLanguage), rb_intern("INSTANCE"));
  rb_gc_register_mark_object(godot_rb_cRubyLanguage_INSTANCE);
  godot_rb_gdextension.object_ptr_from_variant(
    &godot_rb_RubyLanguage_object,
    godot_rb_cVariant_get_variant(godot_rb_cRubyLanguage_INSTANCE)
  );
}

void godot_rb_destroy_RubyLanguage(void) {
  godot_rb_RubyLanguage_object = NULL;
  rb_funcallv_public(godot_rb_get_module(Engine),
    rb_intern("unregister_script_language"),
    1, (VALUE[]){godot_rb_cRubyLanguage_INSTANCE}
  );
  ((GDExtensionInterfaceClassdbUnregisterExtensionClass)godot_rb_get_proc("classdb_unregister_extension_class"))(
    godot_rb_library, string_name_RubyScript
  );
  godot_rb_gdextension.string_name_destroy(&string_name_RubyScript);
  godot_rb_destroy_RubyScript_inst_get_virtual();
  rb_gc_unregister_address(&godot_rb_cRubyLanguage_INSTANCE);
  rb_gc_unregister_address(&godot_rb_cRubyScript);
}
