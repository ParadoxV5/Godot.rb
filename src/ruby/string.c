#include "variants.h"

VALUE encoding_UTF32;
GDExtensionString godot_rb_obj_to_string(VALUE self) {
  GDExtensionString string;
  // First, optimize for {Godot::String}s
  if RB_LIKELY(rb_obj_is_instance_of(self, godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_STRING]))
    godot_rb_gdextension.string_name_from_variant(&string, godot_rb_cVariant_get_variant(self));
  else {
    // Godot Engine Strings are UTF-32 Native Endian.
    // While the GDExtension API provides converters for the most popular encodings, Ruby support even more.
    self = rb_str_encode(rb_str_to_str(self), encoding_UTF32, 0, Qnil);
    godot_rb_gdextension.string_new_with_utf32_chars_and_len(
      &string,
      (char32_t*)RSTRING_PTR(self),
      RSTRING_LEN(self) / sizeof(char32_t) // UTF-32 has fixed bytes/char
    );
  }
  return string;
}

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
  rb_gc_register_mark_object(encoding_UTF32 = rb_enc_from_encoding(godot_rb_encoding_UTF32));
  rb_define_method(cString, "to_s"  , godot_rb_cString_i_to_s  , 0);
  rb_define_method(cString, "to_str", godot_rb_cString_i_to_str, 0);
}
