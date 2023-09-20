#include "variants.h"

GDExtensionInterfaceGlobalGetSingleton gdext_global_get_singleton;
/** code around the general method call API so the singleton subclasses from Object rather than the singleton class */
VALUE godot_rb_Engine_impl_get_singleton(GDExtensionStringName name) {
  GDExtensionObjectPtr object_ptr = gdext_global_get_singleton(&name);
  godot_rb_gdextension.string_name_destroy(&name);
  GDExtensionVariantPtr variant = godot_rb_variant_alloc();
  godot_rb_gdextension.variant_from_object_ptr(variant, &object_ptr);
  // Probably should not free an actually-returned pointer
  return godot_rb_wrap_variant(godot_rb_cObject, variant);
}
VALUE godot_rb_Engine_get_singleton(RB_UNUSED_VAR(VALUE self), VALUE name) {
  return godot_rb_Engine_impl_get_singleton(godot_rb_obj_to_string_name(name));
}

void godot_rb_init_Engine(void) {
  gdext_global_get_singleton = (GDExtensionInterfaceGlobalGetSingleton)godot_rb_get_proc("global_get_singleton");
  VALUE Engine = godot_rb_Engine_impl_get_singleton(godot_rb_chars_to_string_name("Engine"));
  rb_const_set(godot_rb_mGodot, rb_intern("Engine"), Engine);
  rb_define_singleton_method(Engine, "get_singleton", godot_rb_Engine_get_singleton, 1);
}
