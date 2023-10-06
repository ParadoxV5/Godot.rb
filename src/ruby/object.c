#include "variants.h"

VALUE godot_rb_cObject;

GDExtensionInterfaceClassdbConstructObject gdext_classdb_construct_object;
VALUE godot_rb_object_ptr_class(GDExtensionConstObjectPtr object_ptr) {
  GDExtensionStringName class_name_str;
  godot_rb_gdextension.object_get_class_name(object_ptr, godot_rb_library, &class_name_str);
  VALUE klass = rb_const_get_at(godot_rb_mGodot, godot_rb_id_from_string_name(&class_name_str, 0));
  // calls {Godot#const_missing} as needed
  godot_rb_gdextension.string_name_destroy(&class_name_str);
  return klass;
}

VALUE godot_rb_cObject_m_allocate(VALUE self) {
  GDExtensionStringName base_class_name =
    godot_rb_obj_to_string_name(rb_funcallv_public(self, rb_intern("base_class_name"), 0, (VALUE[]){}));
  GDExtensionObjectPtr object_ptr = gdext_classdb_construct_object(&base_class_name);
  GDExtensionVariantPtr variant = godot_rb_variant_alloc();
  godot_rb_gdextension.variant_from_object_ptr(variant, &object_ptr);
  godot_rb_gdextension.string_name_destroy(&base_class_name);
  return godot_rb_wrap_variant(self, variant);
}
//TODO: translate `super` in {#initialize} as {#_init} … except how do I call `super` from Ruby to Godot anyways ??
/*FIXME: attach script, but that calls {#initialize}
if RB_LIKELY(!NIL_P(ruby_script)) // Godot.rb {RubyScript} class
  rb_funcall(instance, rb_intern("set_script"), 1, ruby_script);
*/

void godot_rb_init_Object(void) {
  godot_rb_require_relative(object);
  godot_rb_cObject = godot_rb_get_module(Object);
  godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_OBJECT] = godot_rb_cObject;
  gdext_classdb_construct_object =
    (GDExtensionInterfaceClassdbConstructObject)godot_rb_get_proc("classdb_construct_object");
  rb_define_alloc_func(godot_rb_cObject, godot_rb_cObject_m_allocate);
}
