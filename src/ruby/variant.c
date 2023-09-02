#include "variant.h"
VALUE godot_rb_cVariant;

/** Fetch size from `extension_api.json` */
#define VARIANT_SIZE 24
size_t godot_rb_cVariant_size(__attribute__((unused)) const void* data) { return VARIANT_SIZE; }

void godot_rb_cVariant_free(GDExtensionVariantPtr data) {
  godot_rb_gdextension.variant_destroy(data);
  godot_rb_gdextension.mem_free(data);
}

rb_data_type_t godot_rb_cVariant_type = {
  .wrap_struct_name = "Godot::Variant",
  .function = {
    .dsize = godot_rb_cVariant_size,
    .dfree = godot_rb_cVariant_free
  },
  .flags = RUBY_TYPED_FREE_IMMEDIATELY
};

//TODO: Documentation: warn that `#allocate`d variants are unusable
VALUE godot_rb_cVariant_alloc(VALUE klass) {
  return TypedData_Wrap_Struct(klass, &godot_rb_cVariant_type, godot_rb_gdextension.mem_alloc(VARIANT_SIZE));
}

__attribute__((used)) VALUE godot_rb_init_Variant(__attribute__((unused)) VALUE value) {
  godot_rb_cVariant = rb_const_get(godot_rb_mGodot, rb_intern("Variant"));
  rb_define_alloc_func(godot_rb_cVariant, godot_rb_cVariant_alloc);
  return godot_rb_cVariant;
}

GDExtensionVariantPtr godot_rb_cVariant_to_variant(VALUE self) {
  return rb_check_typeddata(self, &godot_rb_cVariant_type);
}

VALUE godot_rb_cVariant_from_variant(GDExtensionConstVariantPtr variant) {
  // TODO find which subclass to instantiate
  VALUE self = rb_funcall(godot_rb_cVariant, rb_intern("allocate"), 0);
  GDExtensionVariantPtr self_variant = godot_rb_cVariant_to_variant(self);
  godot_rb_gdextension.variant_new_copy(self_variant, variant);
  return self;
}
