#include "variants.h"

GDExtensionStringName godot_rb_obj_to_string_name(VALUE self) {
  // Specialized Strings come from regular Strings, so we convert to String rather than Symbol
  VALUE self_string;
  if RB_LIKELY(SYMBOL_P(self))
    self_string = rb_sym2str(self);
  else {
    self_string = rb_check_string_type(self);
    if RB_UNLIKELY(NIL_P(self_string))
      rb_raise(rb_eTypeError, "%+"PRIsVALUE" is not a symbol nor a string", self);
  }
  GDExtensionString string = godot_rb_mString_to_string(self_string);
  GDExtensionStringName string_name;
  godot_rb_gdextension.string_name_from_string(&string_name, (GDExtensionConstStringNamePtr[]){&string});
  godot_rb_gdextension.string_destroy(&string);
  return string_name;
}

VALUE godot_rb_sym_from_string_name(GDExtensionConstStringNamePtr string_name) {
  GDExtensionString string;
  godot_rb_gdextension.string_from_string_name(&string, string_name);
  // Because symbols of different encodings are still considered distinct, we must also serialize to UTF-8.
  string2str_utf8
  return rb_intern3(str, (long)length, rb_utf8_encoding());
}
//FIXME: possible multiplication and casting overflows (though one should use buffers instead if they need 2GiB strings)

init(StringName, STRING_NAME)
}
