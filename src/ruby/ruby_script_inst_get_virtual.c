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
const GDExtensionConstTypePtr* arg_ptr // ignored if `convert_arg` is {NULL}
``` */
void godot_rb_RubyScript_virtual_impl(va_list* args) {
  VALUE self = va_arg(*args, VALUE);
  const char* name = va_arg(*args, const char*);
  godot_rb_RubyScript_virtual_convert_ret convert_ret = va_arg(*args, godot_rb_RubyScript_virtual_convert_ret);
  GDExtensionUninitializedTypePtr r_ret = va_arg(*args, GDExtensionUninitializedTypePtr);
  godot_rb_RubyScript_virtual_convert_arg convert_arg = va_arg(*args, godot_rb_RubyScript_virtual_convert_arg);
  const GDExtensionConstTypePtr* arg_ptr = va_arg(*args, const GDExtensionConstTypePtr*);
  VALUE ret = rb_funcallv_public(
    self,
    rb_intern(name),
    RB_UNLIKELY(convert_arg) ? 1 : 0,
    RB_UNLIKELY(convert_arg) ? (VALUE[]){convert_arg(*arg_ptr)} : (VALUE[]){}
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

void godot_rb_RubyScript_impl_get_class(va_list* args) {
  VALUE* self = va_arg(*args, VALUE*); // reüsable variable
  *self = rb_funcall(*self, rb_intern("klass"), 0);
}
GDExtensionInterfaceScriptInstanceCreate gdext_script_instance_create;
/** @return {GDExtensionScriptInstancePtr} in {Variant} form */
void godot_rb_RubyScript_virtual_instance_create(
  VALUE self, const GDExtensionObjectPtr*const* for_object_ptr_ptr, GDExtensionScriptInstancePtr* r_ret
) {
  //FIXME: Check superclass
  GDExtensionObjectPtr for_object_object_ptr = **for_object_ptr_ptr;
  GDExtensionVariantPtr for_object_variant = godot_rb_variant_alloc();
  godot_rb_gdextension.variant_from_object_ptr(for_object_variant, &for_object_object_ptr);
  if RB_LIKELY(godot_rb_protect(godot_rb_RubyScript_impl_get_class, &self)) {
    self = godot_rb_wrap_variant(self, for_object_variant); // `klass.allocate` with variable reüse
    rb_gc_register_mark_object(self); // Let Godot Engine lock GC
    *r_ret = gdext_script_instance_create(
      &godot_rb_RubyScript_inst_info,
      (GDExtensionScriptInstanceDataPtr)self // typedef VALUE GDExtensionScriptInstanceDataPtr
    );
  }
}


/** `Hash[GDExtensionConstStringNamePtr, GDExtensionClassCallVirtual]` – mapping pointers as integers
  (`GDExtensionConstStringName` are de-duplicated)
*/
VALUE TABLE;

GDExtensionClassCallVirtual godot_rb_RubyScript_inst_get_virtual(
  RB_UNUSED_VAR(void* class_userdata), GDExtensionConstStringNamePtr name
) {
  VALUE address = rb_hash_aref(TABLE, LL2NUM(*(intptr_t*)name));
  // This entire file would’ve been DRYed away by the time this becomes {RB_UNLIKELY}
  return RB_LIKELY(NIL_P(address)) ? NULL : (GDExtensionClassCallVirtual)NUM2LL(address);
}


GDExtensionClassGetVirtual godot_rb_init_RubyScript_inst_get_virtual() {
  gdext_script_instance_create = (GDExtensionInterfaceScriptInstanceCreate)godot_rb_get_proc("script_instance_create");
  TABLE = rb_hash_new_capa(32); // Count from docs/JSON
  rb_gc_register_address(&TABLE);
  #define x(_0, func, _2) rb_hash_aset(TABLE, \
    LL2NUM((intptr_t)godot_rb_chars_to_string_name("_"#func)), \
    LL2NUM((intptr_t)godot_rb_RubyScript_virtual_##func) \
  );
  X
  x(void*, instance_create, Object)
  #undef x
  rb_hash_freeze(TABLE);
  return godot_rb_RubyScript_inst_get_virtual;
}

static int destroy_key(VALUE key, RB_UNUSED_VAR(VALUE value), RB_UNUSED_VAR(VALUE arg)) {
  GDExtensionStringName name = (GDExtensionStringName)NUM2LL(key);
  godot_rb_gdextension.string_name_destroy(&name);
  return ST_CONTINUE;
}
void godot_rb_destroy_RubyScript_inst_get_virtual() {
  rb_hash_foreach(TABLE, destroy_key, Qnil);
  rb_gc_unregister_address(&TABLE);
  TABLE = Qnil;
}
