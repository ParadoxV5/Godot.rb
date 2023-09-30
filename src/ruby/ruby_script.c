/** {RubyLanguage} & co. */

#include "variants.h"
#include "../SelfImplScriptExtension/SelfImplScriptExtension.h"

/* General notes for developers:
* {VALUE} ↔️ {GDExtensionScriptInstanceDataPtr} casts are exact
* C does not specify evaluation order of sibling args (which is important for {va_arg})
*/


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


GDExtensionBool godot_rb_script_instance_set(
  GDExtensionScriptInstanceDataPtr self, GDExtensionConstStringNamePtr name, GDExtensionConstVariantPtr value
) { return godot_rb_protect(call_impl, self, name, '=', 1, (GDExtensionConstVariantPtr[]){value}, NULL); }

GDExtensionBool godot_rb_script_instance_get(
  GDExtensionScriptInstanceDataPtr self, GDExtensionConstStringNamePtr name, GDExtensionVariantPtr r_value
) { return godot_rb_protect(call_impl, self, name, 0, 0, (GDExtensionConstVariantPtr[]){}, r_value); }

/** @return the {GDExtensionObjectPtr} represented by this {Variant} */
GDExtensionObjectPtr godot_rb_script_instance_get_owner(GDExtensionScriptInstanceDataPtr self) {
  GDExtensionObjectPtr object;
  godot_rb_gdextension.object_ptr_from_variant(&object, godot_rb_cVariant_get_variant((VALUE)self));
  return object;
}

void godot_rb_script_instance_call(GDExtensionScriptInstanceDataPtr self,
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

void to_string_unprotected(va_list* args) {
  VALUE string = rb_obj_as_string(va_arg(*args, VALUE));
  *va_arg(*args, GDExtensionString*) = godot_rb_obj_to_string(string);
}
void godot_rb_script_instance_to_string(
  GDExtensionScriptInstanceDataPtr self, GDExtensionBool* r_is_valid, GDExtensionStringPtr r_out
) { *r_is_valid = godot_rb_protect(to_string_unprotected, self, r_out); }

/** {RefCounted} Callback (here, a no-op)
  @return `true`
    (the default `false` prevents the {RefCounted} from `free`ing itself; see also {godot_rb_script_instance_free}) 
*/
GDExtensionBool godot_rb_script_instance_refcount_decremented(
  RB_UNUSED_VAR(GDExtensionScriptInstanceDataPtr self)
) { return true; }

GDExtensionScriptLanguagePtr godot_rb_RubyLanguage_object;
/** @param self How can this instance not be Godot.rb? Only if Godot Engine confuses something up. */
GDExtensionScriptLanguagePtr godot_rb_script_instance_get_language(
  RB_UNUSED_VAR(GDExtensionScriptInstanceDataPtr self)
) { return godot_rb_RubyLanguage_object; }

/** Release GC protection, allowing the GC to reap the {VALUE} whenever (when no Ruby-land references either). */
void godot_rb_script_instance_free(GDExtensionScriptInstanceDataPtr self) {
  rb_gc_unregister_address((VALUE*)&self);
}

/*
! Standard Reflection
const GDExtensionMethodInfo* get_method_list(GDExtensionScriptInstanceDataPtr p_instance, uint32_t* r_count);
void free_method_list(GDExtensionScriptInstanceDataPtr p_instance, const GDExtensionMethodInfo* p_list);
GDExtensionBool has_method(GDExtensionScriptInstanceDataPtr p_instance, GDExtensionConstStringNamePtr p_name);

! Extended Reflection
const GDExtensionPropertyInfo get_property_list(GDExtensionScriptInstanceDataPtr self, uint32_t* r_count);
void free_property_list(GDExtensionScriptInstanceDataPtr p_instance, const GDExtensionPropertyInfo* p_list);
GDExtensionBool property_can_revert(GDExtensionScriptInstanceDataPtr p_instance, GDExtensionConstStringNamePtr p_name);
GDExtensionBool property_get_revert(
  GDExtensionScriptInstanceDataPtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionVariantPtr r_ret
);
void get_property_state(
  GDExtensionScriptInstanceDataPtr p_instance, GDExtensionScriptInstancePropertyStateAdd p_add_func, void* p_userdata
);
GDExtensionVariantType get_property_type(
  GDExtensionScriptInstanceDataPtr p_instance, GDExtensionConstStringNamePtr p_name, GDExtensionBool* r_is_valid
);
void notification(GDExtensionScriptInstanceDataPtr p_instance, int32_t p_what);
GDExtensionObjectPtr godot_rb_script_instance_get_script(GDExtensionScriptInstanceDataPtr self);
*/
/*TODO: Ask the Godot community for a description on placeholders and fallbacks
GDExtensionBool godot_rb_script_instance_is_placeholder(GDExtensionScriptInstanceDataPtr self);
GDExtensionBool godot_rb_script_instance_set_fallback(
  GDExtensionScriptInstanceDataPtr self, GDExtensionConstStringNamePtr name, GDExtensionConstVariantPtr value
);
GDExtensionBool godot_rb_script_instance_get_fallback(
  GDExtensionScriptInstanceDataPtr self, GDExtensionConstStringNamePtr name, GDExtensionVariantPtr r_value
);
*/


const GDExtensionScriptInstanceInfo godot_rb_script_instance_info = {
  #define i(func) .func##_func = godot_rb_script_instance_##func
  i(set),
  i(get),
  //i(get_property_list),
  //i(free_property_list),
  //i(property_can_revert),
  //i(property_get_revert),
  i(get_owner),
  //i(get_property_state),
  //i(get_method_list),
  //i(free_method_list),
  //i(get_property_type),
  //i(has_method),
  i(call),
  //i(notification),
  i(to_string),
  i(refcount_decremented),
  //i(get_script),
  i(get_language),
  i(free)
};

GDExtensionInterfaceScriptInstanceCreate script_instance_create;
/** @return {GDExtensionScriptInstancePtr} in {Variant} form */
VALUE godot_rb_cRubyScript_i_instance_create(VALUE self, VALUE for_object) {
  //FIXME: Check superclass
  for_object = rb_class_new_instance(
    0, (VALUE[]){}, // TODO: pass constructor vars… somehow
    godot_rb_wrap_variant(
      rb_funcall(self, rb_intern("klass"), 0),
      godot_rb_cVariant_get_variant(for_object)
    )
  );
  rb_gc_register_mark_object(for_object); // Let Godot Engine lock GC
  return godot_rb_wrap_variant(
    godot_rb_cObject, // GDExtension API implementation uses GDExtensionObjectPtr
    script_instance_create(
      &godot_rb_script_instance_info,
      (GDExtensionScriptInstanceDataPtr)for_object // typedef GDExtensionScriptInstanceDataPtr VALUE
    )
  );
}


struct SISEClassData* SISEClassData;
/**
  Don’t save in `RubyScript::RUBY_SCRIPT` –
  `SelfImplScriptExtension` already covers `RubyScript#initialize`’s attach script step.
*/
VALUE godot_rb_cRubyScript_RubyScript;
VALUE cRubyLanguage_INSTANCE;

void godot_rb_init_RubyScript(void) {
  gdext_variant_new_copy = (GDExtensionInterfaceVariantNewCopy)godot_rb_get_proc("variant_new_copy");
  script_instance_create = (GDExtensionInterfaceScriptInstanceCreate)godot_rb_get_proc("script_instance_create");
  
  godot_rb_require_relative(ruby_script);
  VALUE cRubyScript = godot_rb_get_module(RubyScript);
  rb_define_method(cRubyScript, "_instance_create", godot_rb_cRubyScript_i_instance_create, 1);
  godot_rb_cRubyScript_RubyScript = rb_obj_alloc(cRubyScript); // don’t `#initialize` yet (which is guaranteed to fail)
  rb_gc_register_mark_object(godot_rb_cRubyScript_RubyScript);
  
  SISEClassData = init_SelfImplScriptExtension(
    "RubyScript",
    godot_rb_cVariant_get_variant(godot_rb_cRubyScript_RubyScript),
    script_instance_create(
      &godot_rb_script_instance_info,
      (GDExtensionScriptInstanceDataPtr)godot_rb_cRubyScript_RubyScript // see {#_instance_create}
    ),
    godot_rb_gdextension.mem_alloc,
    godot_rb_gdextension.variant_destroy,
    (GDExtensionInterfaceObjectMethodBindPtrcall)godot_rb_get_proc("object_method_bind_ptrcall"),
    (GDExtensionInterfaceObjectSetInstance)godot_rb_get_proc("object_set_instance"),
    (GDExtensionInterfaceClassdbConstructObject)godot_rb_get_proc("classdb_construct_object"),
    (GDExtensionInterfaceClassdbGetMethodBind)godot_rb_get_proc("classdb_get_method_bind"),
    (GDExtensionInterfaceClassdbRegisterExtensionClass)godot_rb_get_proc("classdb_register_extension_class"),
    godot_rb_gdextension.variant_from_object_ptr,
    godot_rb_gdextension.string_name_destroy,
    godot_rb_chars_to_string_name,
    godot_rb_library
  );
  rb_funcall(godot_rb_cRubyScript_RubyScript, rb_intern("initialize"), 0); // can `#initialize` now
  
  godot_rb_require_relative(ruby_language);
  cRubyLanguage_INSTANCE = rb_const_get_at(godot_rb_get_module(RubyLanguage), rb_intern("INSTANCE"));
  rb_gc_register_mark_object(cRubyLanguage_INSTANCE);
  godot_rb_gdextension.object_ptr_from_variant(
    &godot_rb_RubyLanguage_object,
    godot_rb_cVariant_get_variant(cRubyLanguage_INSTANCE)
  );
}

void godot_rb_destroy_RubyLanguage(void) {
  godot_rb_RubyLanguage_object = NULL;
  rb_funcallv_public(godot_rb_get_module(Engine),
    rb_intern("unregister_script_language"),
    1, (VALUE[]){cRubyLanguage_INSTANCE}
  );
  destroy_SelfImplScriptExtension(
    SISEClassData,
    godot_rb_gdextension.mem_free,
    (GDExtensionInterfaceClassdbUnregisterExtensionClass)godot_rb_get_proc("classdb_unregister_extension_class"),
    godot_rb_gdextension.string_name_destroy,
    godot_rb_library
  );
}
