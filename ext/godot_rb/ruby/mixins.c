#include "variants.h"

#define f(tril, nonzero) \
  VALUE godot_rb_m##tril##Class_variant; \
  VALUE godot_rb_m##tril##Class_i_to_godot(RB_UNUSED_VAR(VALUE self)) { return godot_rb_m##tril##Class_variant; } \
  VALUE godot_rb_m##tril##Class_i_nonzero_(RB_UNUSED_VAR(VALUE self)) { return Q##nonzero; } \
  VALUE godot_rb_m##tril##Class_i_inspect (RB_UNUSED_VAR(VALUE self)) { return rb_str_new_lit("<Godot::Variant:"#tril"Class>"); }
f(Nil, false)
f(True, true)
f(False, false)

//GDExtensionTypeFromVariantConstructorFunc gdext_int_from_variant;
GDExtensionVariantFromTypeConstructorFunc gdext_variant_from_int;
VALUE godot_rb_mInteger_i_to_godot(VALUE self) {
  GDExtensionInt integer = NUM2LL(self); // SIZEOF_LONG_LONG ≥ 64 = sizeof(GDExtensionInt)
  GDExtensionVariantPtr self_variant = godot_rb_variant_alloc();
  gdext_variant_from_int(self_variant, &integer);
  return godot_rb_wrap_variant(godot_rb_cVariant, self_variant);
}

VALUE godot_rb_mString_i_to_godot(VALUE self) {
  GDExtensionString string = godot_rb_obj_to_string(self);
  GDExtensionVariantPtr self_variant = godot_rb_variant_alloc();
  godot_rb_gdextension.variant_from_string(self_variant, &string);
  godot_rb_gdextension.string_destroy(&string);
  return godot_rb_wrap_variant(godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_STRING], self_variant);
}

VALUE godot_rb_mSymbol_i_to_godot(VALUE self) {
  GDExtensionStringName string_name = godot_rb_obj_to_string_name(self);
  GDExtensionVariantPtr self_variant = godot_rb_variant_alloc();
  godot_rb_gdextension.variant_from_string_name(self_variant, &string_name);
  godot_rb_gdextension.string_name_destroy(&string_name);
  return godot_rb_wrap_variant(godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_STRING_NAME], self_variant);
}

void godot_rb_init_Mixins() {
  const VALUE mMixins = rb_define_module_under(godot_rb_mGodot, "Mixins");
  #define d(module) \
    mod = rb_define_module_under(mMixins, #module); \
    rb_include_module(rb_c##module, mod); \
    rb_define_method(mod, "to_godot", godot_rb_m##module##_i_to_godot, 0);
  #define t(Trilean, variant_type) \
    d(Trilean##Class) \
    rb_gc_register_address(&godot_rb_m##Trilean##Class_variant); \
    mod = rb_singleton_class(godot_rb_m##Trilean##Class_variant); \
    rb_const_set(mod, godot_rb_idVARIANT_TYPE, INT2FIX(GDEXTENSION_VARIANT_TYPE_##variant_type)); \
    rb_define_method(mod, "nonzero?", godot_rb_m##Trilean##Class_i_nonzero_, 0); \
    rb_define_method(mod, "inspect",  godot_rb_m##Trilean##Class_i_inspect , 0);
  
  godot_rb_mNilClass_variant = rb_obj_alloc(godot_rb_cVariant); // Utilizing `Variant.allocate`
  VALUE t(Nil, NIL)
  
  GDExtensionVariantFromTypeConstructorFunc variant_from_bool =
    godot_rb_gdextension.get_variant_from_type_constructor(GDEXTENSION_VARIANT_TYPE_BOOL);
  GDExtensionBool the_bool;
  #define b(Boolean, boolean) \
    variant = godot_rb_variant_alloc(); \
    the_bool = boolean; \
    variant_from_bool(variant, &the_bool); \
    godot_rb_m##Boolean##Class_variant = godot_rb_wrap_variant(godot_rb_cVariant, variant); \
    t(Boolean, BOOL)
  GDExtensionVariantPtr b(True, true) b(False, false)
  
  gdext_variant_from_int = godot_rb_gdextension.get_variant_from_type_constructor(GDEXTENSION_VARIANT_TYPE_INT);
  d(Integer)
  d(String)
  d(Symbol)
}
