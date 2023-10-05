#include "variants.h"
#include "ruby_script.h"

/** Note:
  https://github.com/godotengine/godot/pull/80671 (Godot Engine 4.2)
  and LSP integration will DRYly replace this file one day.
*/
#define X \
  x(bool, can_instantiate, void) \
  x(bool, has_source_code, void) \
  x(Object, get_base_script, void) \
  x(StringName, get_global_name, void) \
  x(StringName, get_instance_base_type, void) \
  x(Object, get_language, void) \
  x(String, get_source_code, void) \
  x(Object, instance_create, Object) \
  x(bool, instance_has, Object) \
  x(bool, is_tool, void) \
  x(bool, is_valid, void) \
  x(void, set_source_code, String)


typedef void (* godot_rb_RubyScript_virtual_convert_ret)(GDExtensionUninitializedTypePtr r_ret, VALUE ret);
typedef VALUE (* godot_rb_RubyScript_virtual_convert_arg)(GDExtensionConstTypePtr arg);

/** ```c
VALUE self,
const char* name,
godot_rb_RubyScript_virtual_convert_ret convert_ret, // {NULL}able
GDExtensionUninitializedTypePtr r_ret, // ignored if `convert_ret` is {NULL}
godot_rb_RubyScript_virtual_convert_arg convert_arg, // {NULL}able
const GDExtensionConstTypePtr* arg_ptr // ignored (skipped) if `convert_arg` is {NULL}
``` */
void godot_rb_RubyScript_virtual_impl(va_list* args) {
  VALUE self = va_arg(*args, VALUE);
  const char* name = va_arg(*args, const char*);
  godot_rb_RubyScript_virtual_convert_ret convert_ret = va_arg(*args, godot_rb_RubyScript_virtual_convert_ret);
  GDExtensionUninitializedTypePtr r_ret = va_arg(*args, GDExtensionUninitializedTypePtr);
  godot_rb_RubyScript_virtual_convert_arg convert_arg = va_arg(*args, godot_rb_RubyScript_virtual_convert_arg);
  VALUE ret = rb_funcallv_public(
    self,
    rb_intern(name),
    RB_UNLIKELY(convert_arg) ? 1 : 0,
    RB_UNLIKELY(convert_arg) ? (VALUE[]){convert_arg(*va_arg(*args, const GDExtensionConstTypePtr*))} : (VALUE[]){}
  );
  if RB_LIKELY(convert_ret)
    convert_ret(r_ret, ret);
}


#define godot_rb_RubyScript_virtual_ret_void NULL
#define godot_rb_RubyScript_virtual_arg_void NULL

void godot_rb_RubyScript_virtual_ret_bool(GDExtensionBool* r_bool, VALUE obj) { *r_bool = RB_TEST(obj); }

void godot_rb_RubyScript_virtual_ret_String(GDExtensionString* r_string, VALUE obj) {
  *r_string = godot_rb_obj_to_string(obj);
}
VALUE godot_rb_RubyScript_virtual_arg_String(GDExtensionStringPtr string_ptr) {
  GDExtensionVariantPtr variant = godot_rb_variant_alloc();
  godot_rb_gdextension.variant_from_string(variant, string_ptr);
  return godot_rb_wrap_variant(godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_STRING], variant);
}

void godot_rb_RubyScript_virtual_ret_StringName(GDExtensionStringName* r_string_name, VALUE obj) {
  *r_string_name = godot_rb_obj_to_string_name(obj);
}

void godot_rb_RubyScript_virtual_ret_Object(GDExtensionUninitializedObjectPtr* r_object_ptr, VALUE obj) {
  godot_rb_gdextension.object_ptr_from_variant(r_object_ptr, godot_rb_obj_get_variant(obj));
}
VALUE godot_rb_RubyScript_virtual_arg_Object(GDExtensionObjectPtr* object_ptr_ptr) {
  GDExtensionVariantPtr variant = godot_rb_variant_alloc();
  godot_rb_gdextension.variant_from_object_ptr(variant, object_ptr_ptr);
  return godot_rb_wrap_variant(godot_rb_object_ptr_class(*object_ptr_ptr), variant);
}


#define x(ret, func, arg) void godot_rb_RubyScript_virtual_##func( \
  GDExtensionClassInstancePtr self, const GDExtensionConstTypePtr* arg_ptr, GDExtensionUninitializedTypePtr r_ret \
) { godot_rb_protect( \
  godot_rb_RubyScript_virtual_impl, \
  self, \
  "_"#func, \
  godot_rb_RubyScript_virtual_ret_##ret, r_ret, \
  godot_rb_RubyScript_virtual_arg_##arg, arg_ptr \
); }
X
#undef x


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
  #define x(_0, func, _2) rb_hash_aset(TABLE, \
    ULL2NUM((uintptr_t)godot_rb_chars_to_string_name("_"#func)), \
    ULL2NUM((uintptr_t)godot_rb_RubyScript_virtual_##func) \
  );
  X
  #undef x
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
