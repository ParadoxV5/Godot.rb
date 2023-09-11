#ifndef GODOT_RB_VARIANTS_H
#define GODOT_RB_VARIANTS_H
#include <godot_rb.h>

/** Stand-ins for Godot Engine internal types */
typedef void* GDExtensionString, GDExtensionStringName;

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

void godot_rb_init_Variant();
void godot_rb_init_String();
void godot_rb_init_Mixins();

#endif
