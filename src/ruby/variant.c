#include "variant.h"
VALUE godot_rb_cVariant;
VALUE godot_rb_Variants[GDEXTENSION_VARIANT_TYPE_VARIANT_MAX] = {};

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

GDExtensionVariantPtr godot_rb_cVariant_to_variant(VALUE self) {
  return rb_check_typeddata(
    rb_convert_type(self, RUBY_T_DATA, "Godot::Variant", "to_godot"),
    &godot_rb_cVariant_type
  );
}
VALUE godot_rb_cVariant_from_variant(GDExtensionConstVariantPtr variant) {
  // TODO find which subclass to instantiate
  VALUE self = rb_obj_alloc(godot_rb_cVariant);
  GDExtensionVariantPtr self_variant = godot_rb_cVariant_to_variant(self);
  godot_rb_gdextension.variant_new_copy(self_variant, variant);
  return self;
}

VALUE godot_rb_cVariant_i___godot_send__(VALUE self, VALUE meth, VALUE args) {
  GDExtensionUninitializedVariantPtr self_variant = godot_rb_cVariant_to_variant(self);
  long argc = rb_array_len(args);
  GDExtensionConstVariantPtr argv[argc];
  for(long i = 0; i < argc; ++i)
    argv[i] = godot_rb_cVariant_to_variant(rb_ary_entry(args, i));
  GDExtensionCallError error;
  if(NIL_P(meth)) { // Constructor
    godot_rb_gdextension.variant_construct(
      FIX2INT(rb_const_get_from(CLASS_OF(self), rb_intern("VARIANT_TYPE"))),
      self_variant,
      argv,
      argc,
      &error
    );
  } else { // Method call
    GDExtensionUninitializedVariantPtr ret;
    godot_rb_gdextension.variant_call(
      self_variant,
      godot_rb_cVariant_to_variant(rb_to_symbol(meth)),
      argv,
      argc,
      ret,
      &error
    );
    self = godot_rb_cVariant_from_variant(ret); // Dubious reuse of variable <:
  }
  // ret, error, argument, expected
  return rb_ary_new_from_args(
    4,
    self,
    /*
      These latter two are fixed 32-bits, which C standard specifies as `long` or shorter.
      Although Fixnum is 1 bit shorter than `long`, I doubt anyöne’s gonna go to billions anyways.
      The enum has but half a dozen possibilities. Not our problem if someone gave an out-of-range error code.
    */
    INT2FIX(error.error),
    LONG2FIX(error.argument),
    LONG2FIX(error.expected)
  );
}

__attribute__((used)) VALUE godot_rb_init_Variant(__attribute__((unused)) VALUE value) {
  godot_rb_cVariant = rb_const_get(godot_rb_mGodot, rb_intern("Variant"));
  rb_gc_register_mark_object(godot_rb_cVariant);
  rb_define_alloc_func(godot_rb_cVariant, godot_rb_cVariant_alloc);
  rb_define_method(godot_rb_cVariant, "__godot_send__", godot_rb_cVariant_i___godot_send__, 2);
  return godot_rb_cVariant;
}
