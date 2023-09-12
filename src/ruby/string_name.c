#include "variants.h"

/*TODO:
  How about caching converted StringNames (possibly permanently)
  so we don’t have to serialize a new Variant[StringName] every time?
*/
GDExtensionStringName godot_rb_obj_to_string_name(VALUE self) {
  GDExtensionStringName string_name;
  // First, optimize for {StringName}s
  if RB_LIKELY(rb_obj_is_instance_of(self, godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_STRING_NAME]))
    godot_rb_gdextension.string_name_from_variant(&string_name, godot_rb_cVariant_get_variant(self));
  else {
    // Specialized Strings come from regular Strings, so we convert to String rather than Symbol
    VALUE self_string;
    if RB_LIKELY(SYMBOL_P(self))
      self_string = rb_sym2str(self);
    else {
      self_string = rb_check_string_type(self);
      if RB_UNLIKELY(NIL_P(self_string))
        rb_raise(rb_eTypeError, "%+"PRIsVALUE" is not a symbol nor a string", self);
    }
    GDExtensionString string = godot_rb_obj_to_string(self_string);
    godot_rb_gdextension.string_name_from_string(&string_name, (GDExtensionConstStringNamePtr[]){&string});
    godot_rb_gdextension.string_destroy(&string);
  }
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
VALUE godot_rb_cStringName_i_to_sym(VALUE self) {
  GDExtensionStringName string_name;
  VALUE symbol = godot_rb_sym_from_string_name(string_name);
  godot_rb_gdextension.string_name_destroy(&string_name);
  return symbol;
}

/** @return `self.`{#to_sym to_sym}`.`{Symbol#to_s to_s} (the return is not frozen)
  @see #to_str
*/
VALUE godot_rb_cStringName_i_to_s(VALUE self) { return rb_sym_to_s(godot_rb_cStringName_i_to_sym(self)); }
/** Ruby actually uses `#to_str` to convert to symbols implicitly; {#to_sym} is seldom used.
  @return `self.`{#to_sym to_sym}`.`{Symbol#name name} (i.e., {#to_s} but frozen)
*/
VALUE godot_rb_cStringName_i_to_str(VALUE self) { return rb_sym2str(godot_rb_cStringName_i_to_sym(self)); }

init(StringName, STRING_NAME)
  rb_define_method(cStringName, "to_sym", godot_rb_cStringName_i_to_sym, 0);
  rb_define_method(cStringName, "to_s"  , godot_rb_cStringName_i_to_s  , 0);
  rb_define_method(cStringName, "to_str", godot_rb_cStringName_i_to_str, 0);
}
