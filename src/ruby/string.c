#include "variants.h"

GDExtensionTypeFromVariantConstructorFunc variant_to_string;

// Serialize into a non-frozen UTF-8 Ruby String
VALUE godot_rb_cString_i_to_s(VALUE self) {
  GDExtensionString string;
  variant_to_string(&string, godot_rb_cVariant_to_variant(self));
  // UTF-32 is the fastest – possibly-constant-time – function for counting chars as no encoding conversions necessary.
  // UTF-8 is up to 4bytes/char.
  GDExtensionInt length = godot_rb_gdextension.string_to_utf32_chars(&string, NULL, 0) * 4;
  char str[length];
  length = godot_rb_gdextension.string_to_utf8_chars(&string, str, length);
  return rb_utf8_str_new(str, length);
}

rb_encoding* godot_rb_encoding_UTF32;
// Convert into a frozen UTF-32 Ruby String; faster than {#to_s} for long strings
VALUE godot_rb_cString_i_to_str(VALUE self) {
  GDExtensionString string;
  variant_to_string(&string, godot_rb_cVariant_to_variant(self));
  GDExtensionInt length = godot_rb_gdextension.string_to_utf32_chars(&string, NULL, 0);
  char32_t str[length];
  godot_rb_gdextension.string_to_utf32_chars(&string, str, length);
  return rb_obj_freeze(rb_enc_str_new((char*)str, length * sizeof(char32_t), godot_rb_encoding_UTF32));
}
//FIXME: possible multiplication overflow (though one should use a buffer instead if they need a quintillion chars)

void godot_rb_init_String() {
  VALUE cString = rb_define_class("String", godot_rb_cVariant);
  rb_const_set(cString, godot_rb_idVARIANT_TYPE, INT2FIX(GDEXTENSION_VARIANT_TYPE_STRING));
  godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_STRING] = cString;
  variant_to_string = godot_rb_gdextension.get_variant_to_type_constructor(GDEXTENSION_VARIANT_TYPE_STRING);
  // Endian test for UTF-32 -> `char[]`
  volatile char32_t sample = 1;
  godot_rb_encoding_UTF32 = rb_enc_find(RB_LIKELY(*(char*)&sample) ? "UTF-32LE" : "UTF-32BE");
  rb_define_method(cString, "to_s"  , godot_rb_cString_i_to_s  , 0);
  rb_define_method(cString, "to_str", godot_rb_cString_i_to_str, 0);
}
