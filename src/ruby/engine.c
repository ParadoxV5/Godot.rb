#include "variants.h"

void godot_rb_init_Engine(void) {
  GDExtensionStringName engine_string_name = godot_rb_chars_to_string_name("Engine");
  GDExtensionObjectPtr engine_object = (
    (GDExtensionInterfaceGlobalGetSingleton)godot_rb_get_proc("global_get_singleton")
  )(&engine_string_name);
  godot_rb_gdextension.string_name_destroy(&engine_string_name);
  GDExtensionVariantPtr engine_variant = godot_rb_variant_alloc();
  godot_rb_gdextension.object_from_variant(&engine_object, engine_variant);
  // Probably should not free an actually-returned pointer
  VALUE m_Engine = godot_rb_wrap_variant(godot_rb_cObject, engine_variant);
  rb_const_set(godot_rb_mGodot, rb_intern("Engine"), m_Engine);
}
