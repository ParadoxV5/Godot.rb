#include "variants.h"

/** Serialize into a non-frozen UTF-8 Ruby String */
VALUE godot_rb_cString_i_to_s(VALUE self) {
  GDExtensionString string;
  godot_rb_gdextension.string_from_variant(&string, godot_rb_cVariant_get_variant(self));
  string2str_utf8
  return rb_utf8_str_new(str, length);
}
//FIXME: possible multiplication and casting overflows (though one should use buffers instead if they need 2GiB strings)

rb_encoding* godot_rb_encoding_UTF32;
/**
  Convert into a frozen UTF-32 Ruby String;
  faster than {#to_s} for all but short strings since Godot Engine Strings are already UTF-32
*/
VALUE godot_rb_cString_i_to_str(VALUE self) {
  GDExtensionString string;
  godot_rb_gdextension.string_from_variant(&string, godot_rb_cVariant_get_variant(self));
  GDExtensionInt length = godot_rb_gdextension.string_to_utf32_chars(&string, NULL, 0);
  char32_t str[length];
  godot_rb_gdextension.string_to_utf32_chars(&string, str, length);
  godot_rb_gdextension.string_destroy(&string);
  return rb_obj_freeze(rb_enc_str_new((char*)str, length * sizeof(char32_t), godot_rb_encoding_UTF32));
}
//FIXME: possible multiplication and casting overflows (though one should use buffers instead if they need 2GiB strings)

init(String, STRING)
  // Endian test for UTF-32 -> `char[]`
  volatile char32_t sample = 1;
  godot_rb_encoding_UTF32 = rb_enc_find(RB_LIKELY(*(char*)&sample) ? "UTF-32LE" : "UTF-32BE");
  rb_define_method(cString, "to_s"  , godot_rb_cString_i_to_s  , 0);
  rb_define_method(cString, "to_str", godot_rb_cString_i_to_str, 0);
}
