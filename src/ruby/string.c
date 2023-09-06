#include "variants.h"

// Convert into a frozen UTF-32 Ruby String; faster than {#to_s}
// TODO: How to actually mark the String as UTF-32, complete with the correct endianness
VALUE godot_rb_cString_i_to_str(VALUE self) {
  GDExtensionVariantPtr self_variant = godot_rb_cVariant_to_variant(self);
  GDExtensionInt length = godot_rb_gdextension.string_to_utf32_chars(self_variant, NULL, 0);
  char32_t str[length];
  godot_rb_gdextension.string_to_utf32_chars(self_variant, str, length);
  return rb_obj_freeze(rb_str_new((char*)str, length * sizeof(char32_t)));
}
// Serialize into a non-frozen UTF-8 Ruby String
VALUE godot_rb_cString_i_to_s(VALUE self) {
  GDExtensionVariantPtr self_variant = godot_rb_cVariant_to_variant(self);
  // UTF-32 is the fastest – possibly-constant-time – function for counting chars as no encoding conversions necessary.
  // UTF-8 is up to 4bytes/char.
  GDExtensionInt length = godot_rb_gdextension.string_to_utf32_chars(self_variant, NULL, 0) * 4;
  char str[length];
  length = godot_rb_gdextension.string_to_utf8_chars(self_variant, str, length);
  return rb_utf8_str_new(str, length);
}
//FIXME: possible multiplication overflow (though one should use a buffer instead if they need a quintillion chars)

void godot_rb_init_String() {
  VALUE cString = rb_define_class("String", godot_rb_cVariant);
  rb_const_set(cString, godot_rb_idVARIANT_TYPE, INT2FIX(GDEXTENSION_VARIANT_TYPE_STRING));
  rb_ary_store(godot_rb_cVariant_c_VARIANTS, GDEXTENSION_VARIANT_TYPE_STRING, cString);
  rb_define_method(cString, "to_s"  , godot_rb_cString_i_to_s  , 0);
  rb_define_method(cString, "to_str", godot_rb_cString_i_to_str, 0);
}
