#include "variants.h"

#define f(tril, nonzero) \
  VALUE godot_rb_m##tril##Class_variant; \
  VALUE godot_rb_m##tril##Class_i_to_godot(__attribute__((unused)) VALUE self) { return godot_rb_m##tril##Class_variant; } \
  VALUE godot_rb_m##tril##Class_i_nonzero_(__attribute__((unused)) VALUE self) { return Q##nonzero; } \
  VALUE godot_rb_m##tril##Class_i_inspect (__attribute__((unused)) VALUE self) { return rb_str_new_lit("<Godot::Variant:"#tril"Class>"); }
f(Nil, false)
f(True, true)
f(False, false)

VALUE encoding_UTF32;
GDExtensionString godot_rb_mString_to_string(VALUE self) {
  // Godot Engine Strings are UTF-32 Native Endian.
  // While the GDExtension API provides converters for the most popular encodings, Ruby support even more.
  self = rb_str_encode(self, encoding_UTF32, 0, Qnil);
  GDExtensionString string;
  godot_rb_gdextension.string_new_with_utf32_chars_and_len(
    &string,
    (char32_t*)StringValuePtr(self),
    RSTRING_LEN(self) / sizeof(char32_t) // UTF-32 has fixed bytes/char
  );
  return string;
}
__attribute__((used)) VALUE godot_rb_mString_i_to_godot(VALUE self) {
  GDExtensionString string = godot_rb_mString_to_string(self);
  GDExtensionVariantPtr self_variant = godot_rb_variant_alloc();
  godot_rb_gdextension.variant_from_string(self_variant, &string);
  godot_rb_gdextension.string_destroy(&string);
  return godot_rb_wrap_variant(godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_STRING], self_variant);
}

void godot_rb_init_Mixins() {
  godot_rb_require_relative(mixins);
  const VALUE space = rb_const_get(godot_rb_mGodot, rb_intern("Mixins"));
  
  #define d(module) \
    mod = rb_const_get(space, rb_intern(#module)); \
    rb_define_method(mod, "to_godot", godot_rb_m##module##_i_to_godot, 0);
  #define t(Trilean, variant_type) \
    d(Trilean##Class) \
    rb_gc_register_mark_object(godot_rb_m##Trilean##Class_variant); \
    mod = rb_singleton_class(godot_rb_m##Trilean##Class_variant); \
    rb_const_set(mod, godot_rb_idVARIANT_TYPE, INT2FIX(GDEXTENSION_VARIANT_TYPE_##variant_type)); \
    rb_define_method(mod, "nonzero?", godot_rb_m##Trilean##Class_i_nonzero_, 0); \
    rb_define_method(mod, "inspect",  godot_rb_m##Trilean##Class_i_inspect , 0);
  
  godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_NIL] = godot_rb_cVariant;
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
  godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_BOOL] = godot_rb_cVariant;
  
  rb_gc_register_mark_object(encoding_UTF32 = rb_enc_from_encoding(godot_rb_encoding_UTF32));
  d(String)
}
