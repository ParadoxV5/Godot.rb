#include "variants.h"

#define f(tril, nonzero) \
  VALUE godot_rb_m##tril##Class_variant; \
  VALUE godot_rb_m##tril##Class_i_to_godot(__attribute__((unused)) VALUE self) { return godot_rb_m##tril##Class_variant; } \
  VALUE godot_rb_m##tril##Class_i_nonzero_(__attribute__((unused)) VALUE self) { return Q##nonzero; } \
  VALUE godot_rb_m##tril##Class_i_inspect (__attribute__((unused)) VALUE self) { return rb_str_new_lit("<Godot::Variant:"#tril"Class>"); }
f(Nil, false)
f(True, true)
f(False, false)

void godot_rb_init_Mixins() {
  godot_rb_require_relative(mixins);
  const VALUE space = rb_const_get(godot_rb_mGodot, rb_intern("Mixins"));
  
  #define d(module) \
    mod = rb_const_get(space, rb_intern(#module)); \
    rb_define_method(mod, "to_godot", godot_rb_m##module##_i_to_godot, 0);
  #define t(tril, variant_type, get_variant) \
    d(tril##Class) \
    godot_rb_m##tril##Class_variant = rb_obj_alloc(godot_rb_cVariant); \
    get_variant; \
    mod = rb_singleton_class(godot_rb_m##tril##Class_variant); \
    rb_const_set(mod, godot_rb_idVARIANT_TYPE, INT2FIX(GDEXTENSION_VARIANT_TYPE_##variant_type)); \
    rb_define_method(mod, "nonzero?", godot_rb_m##tril##Class_i_nonzero_, 0); \
    rb_define_method(mod,  "inspect",  godot_rb_m##tril##Class_i_inspect, 0);
  VALUE t( Nil, NIL, (
    (GDExtensionInterfaceVariantNewNil)godot_rb_get_proc("variant_new_nil")
  )(godot_rb_cVariant_to_variant(godot_rb_mNilClass_variant)) )
  
  GDExtensionVariantFromTypeConstructorFunc variant_from_bool =
    godot_rb_gdextension.get_variant_from_type_constructor(GDEXTENSION_VARIANT_TYPE_BOOL);
  #define b(capscase, downcase) \
    a_bool = downcase; \
    t(capscase, BOOL, variant_from_bool(godot_rb_cVariant_to_variant(godot_rb_m##capscase##Class_variant), &a_bool))
  GDExtensionBool
    b(True , true )
    b(False, false)
}
