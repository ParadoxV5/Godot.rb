#ifndef GODOT_RB_VARIANTS_H
#define GODOT_RB_VARIANTS_H
#include <godot_rb.h>

/** Stand-ins for Godot Engine internal types (for stack-allocation) */
typedef void* GDExtensionString, * GDExtensionStringName;

/**
  Serialize a Ruby String into a {GDExtensionString Godot internal String}
  @param self RBS: `string`; otherwise this {rb_raise}s {rb_eTypeError}
  @return the Godot String (don’t forget to {godot_rb_gdextension#string_destroy destroy} it when done with it!)
*/
GDExtensionString godot_rb_obj_to_string(VALUE self);
/**
  Serialize a Ruby String or Symbol into a {GDExtensionStringName Godot internal StringName}
  @param self RBS: `interned`; otherwise this {rb_raise}s {rb_eTypeError}
  @return the StringName (don’t forget to {godot_rb_gdextension#string_name_destroy destroy} it when done with it!)
*/
GDExtensionStringName godot_rb_obj_to_string_name(VALUE self);
/**
  Convert a C String into a {GDExtensionStringName Godot internal StringName}
  @param chars C String (0-terminated Latin-1 `char`s)
  @return the StringName (don’t forget to {godot_rb_gdextension#string_name_destroy destroy} it when done with it!)
*/
GDExtensionStringName godot_rb_chars_to_string_name(const char* chars);


/** @param suffix not included if it’s `'\0'` */
ID godot_rb_id_from_string_name(GDExtensionConstStringNamePtr string_name, char suffix);

#define string2str_utf8 \
  /*
  * Since Godot Engine Strings are UTF-32,
    counting chars in UTF-32 does not require re-encoding and thus is the fastest ((probably) constant time).
  * UTF-8 is up to 4bytes/char.
  */ \
  GDExtensionInt length = godot_rb_gdextension.string_to_utf32_chars(&string, NULL, 0) * 4; \
  char str[length + 1]; \
  length = godot_rb_gdextension.string_to_utf8_chars(&string, str, length); \
  godot_rb_gdextension.string_destroy(&string);

#define init(klass, type) void godot_rb_init_##klass(void) { \
  VALUE c##klass = rb_define_class_under(godot_rb_mGodot, #klass, godot_rb_cVariant); \
  rb_const_set(c##klass, godot_rb_idVARIANT_TYPE, INT2FIX(GDEXTENSION_VARIANT_TYPE_##type)); \
  godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_##type] = c##klass;

void godot_rb_init_Variant(void);
void godot_rb_init_String(void);
void godot_rb_init_StringName(void);
void godot_rb_init_Object(void);
void godot_rb_init_Mixins(void);
void godot_rb_init_Engine(void);
void godot_rb_init_RubyScript(void);
void godot_rb_destroy_RubyLanguage(void);

#endif
