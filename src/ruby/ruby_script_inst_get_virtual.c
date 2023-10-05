/** Note: https://github.com/godotengine/godot/pull/80671 will DRYly replace this file when Godot 4.2 releases. */

#include "variants.h"
#include "ruby_script.h"


/** `Hash[GDExtensionConstStringNamePtr, GDExtensionClassCallVirtual]` – mapping pointers as integers
  (`GDExtensionConstStringName` are de-duplicated)
*/
VALUE TABLE;

GDExtensionClassCallVirtual godot_rb_RubyScript_inst_get_virtual(
  RB_UNUSED_VAR(void* class_userdata), GDExtensionConstStringNamePtr name
) {
  return (GDExtensionClassCallVirtual)NUM2ULL(rb_hash_aref(TABLE, ULL2NUM(*(uintptr_t*)name)));
}


GDExtensionClassGetVirtual godot_rb_init_RubyScript_inst_get_virtual() {
  TABLE = rb_hash_new_capa(32); // Count from docs/JSON
  #define h(func) rb_hash_aset(TABLE, \
    ULL2NUM((uintptr_t)godot_rb_chars_to_string_name("_"#func)), \
    ULL2NUM((uintptr_t)godot_rb_cRubyScript_virtual_##func) \
  );
  rb_hash_freeze(TABLE);
  rb_gc_register_mark_object(TABLE);
}

static int destroy_key(VALUE key, RB_UNUSED_VAR(VALUE value), RB_UNUSED_VAR(VALUE arg)) {
  GDExtensionStringName name = (GDExtensionStringName)NUM2ULL(key);
  godot_rb_gdextension.string_name_destroy(&name);
  return ST_CONTINUE;
}
void godot_rb_destroy_RubyScript_inst_get_virtual() {
  rb_hash_foreach(TABLE, destroy_key, Qnil);
  rb_gc_unregister_address(&TABLE);
  TABLE = Qnil;
}
