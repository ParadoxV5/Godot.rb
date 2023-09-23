#include "variants.h"

/* General notes for developers:
* {VALUE} ↔️ {GDExtensionScriptInstanceDataPtr} casts are exact
* C does not specify evaluation order of sibling args (which is important for {va_arg})
*/


GDExtensionInterfaceVariantNewCopy gdext_variant_new_copy;


static void set_unprotected(va_list* args) {
  VALUE self = va_arg(*args, VALUE);
  ID name = godot_rb_id_from_string_name(va_arg(*args, GDExtensionConstStringNamePtr), '=');
  GDExtensionVariantPtr value = godot_rb_variant_alloc();
  gdext_variant_new_copy(value, va_arg(*args, GDExtensionConstVariantPtr));
  rb_funcallv_public(self, name, 1, (VALUE[]){godot_rb_parse_variant(value)});
}
GDExtensionBool godot_rb_script_instance_set(
  GDExtensionScriptInstanceDataPtr self, GDExtensionConstStringNamePtr name, GDExtensionConstVariantPtr value
) { return godot_rb_protect(set_unprotected, self, name, value); }

static void get_unprotected(va_list* args) {
  VALUE self = va_arg(*args, VALUE);
  ID name = godot_rb_id_from_string_name(va_arg(*args, GDExtensionConstStringNamePtr), 0);
  gdext_variant_new_copy(
    va_arg(*args, GDExtensionUninitializedVariantPtr),
    godot_rb_obj_get_variant(rb_funcallv_public(self, name, 0, (VALUE[]){}))
  );
}
GDExtensionBool godot_rb_script_instance_get(
  GDExtensionScriptInstanceDataPtr self, GDExtensionConstStringNamePtr name, GDExtensionVariantPtr r_value
) { return godot_rb_protect(get_unprotected, self, name, r_value); }

/** @return the {GDExtensionObjectPtr} represented by this {Variant} */
GDExtensionObjectPtr godot_rb_script_instance_get_owner(GDExtensionScriptInstanceDataPtr self) {
  GDExtensionObjectPtr object;
  godot_rb_gdextension.object_ptr_from_variant(&object, godot_rb_cVariant_get_variant((VALUE)self));
  return object;
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
void call(GDExtensionScriptInstanceDataPtr p_self,
  GDExtensionConstStringNamePtr p_method,
  const GDExtensionConstVariantPtr* p_args, GDExtensionInt p_argument_count,
  GDExtensionVariantPtr r_return,
  GDExtensionCallError* r_error
);

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
  #define i(f) .f##_func = godot_rb_script_instance_##f
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
  //i(call),
  //i(notification),
  i(to_string),
  i(refcount_decremented),
  //i(get_script),
  i(get_language),
  i(free)
};


void godot_rb_init_RubyLanguage(void) {
  gdext_variant_new_copy = (GDExtensionInterfaceVariantNewCopy)godot_rb_get_proc("variant_new_copy");
  godot_rb_require_relative(ruby);
  godot_rb_require_relative(ruby_language);
  godot_rb_gdextension.object_ptr_from_variant(
    &godot_rb_RubyLanguage_object,
    godot_rb_cVariant_get_variant(rb_const_get_from(godot_rb_mGodot, rb_intern("RubyLanguage")))
  );
  //TODO: Attach {RubyLanguage} singleton script to the object
}
