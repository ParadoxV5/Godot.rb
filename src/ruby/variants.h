#ifndef GODOT_RB_VARIANTS_H
#define GODOT_RB_VARIANTS_H
#include <godot_rb.h>

/** Stand-ins for Godot Engine internal types */
typedef void* GDExtensionString, * GDExtensionStringName;

/**
  Serialize a Ruby String into a {GDExtensionString Godot internal String}
  @param self (probably) must be a Ruby String
  @return the Godot String (don’t forget to {godot_rb_gdextension#string_destroy destroy} it when done with it!)
*/
GDExtensionString godot_rb_mString_to_string(VALUE self);
/**
  Serialize a Ruby String or Symbol into a {GDExtensionStringName Godot internal StringName}
  @param self RBS: `interned`; otherwise this {rb_raise} {rb_eTypeError}
  @return the StringName (don’t forget to {godot_rb_gdextension#string_name_destroy destroy} it when done with it!)
*/
GDExtensionStringName godot_rb_obj_to_string_name(VALUE self);

#define init(klass, type) void godot_rb_init_##klass(void) { \
  VALUE c##klass = rb_define_class_under(godot_rb_mGodot, #klass, godot_rb_cVariant); \
  rb_const_set(c##klass, godot_rb_idVARIANT_TYPE, INT2FIX(GDEXTENSION_VARIANT_TYPE_##type)); \
  godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_##type] = c##klass;

void godot_rb_init_Variant(void);
void godot_rb_init_String(void);
void godot_rb_init_StringName(void);
void godot_rb_init_Mixins(void);

#endif
