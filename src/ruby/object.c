#include "variants.h"

VALUE godot_rb_cObject;
ID idRUBY_SCRIPT;

GDExtensionInterfaceClassdbConstructObject gdext_classdb_construct_object;

VALUE godot_rb_cObject_i_initialize(int argc, VALUE* argv, VALUE self) {
  rb_call_super_kw(argc, argv, RB_PASS_CALLED_KEYWORDS); //TODO how are constructor args passed exactly?
  VALUE ruby_script = rb_const_get_from(CLASS_OF(self), idRUBY_SCRIPT);
  GDExtensionStringName class_name = godot_rb_obj_to_string_name(
    RB_UNLIKELY(NIL_P(ruby_script))
    // Godot native type
    ? rb_funcallv_public(CLASS_OF(self), rb_intern("demodulized_name"), 0, (VALUE[]){})
    // Godot.rb {RubyScript} class
    : rb_funcallv_public(ruby_script, rb_intern("_get_instance_base_type"), 0, (VALUE[]){})
  );
  GDExtensionObjectPtr self_object_ptr = gdext_classdb_construct_object(&class_name);
  godot_rb_gdextension.variant_from_object_ptr(
    godot_rb_cVariant_get_variant(self),
    &self_object_ptr
  );
  godot_rb_gdextension.string_name_destroy(&class_name);
  if RB_LIKELY(!NIL_P(ruby_script)) // Godot.rb {RubyScript} class
    rb_funcall(self, rb_intern("set_script"), 1, ruby_script);
  return self;
}

void godot_rb_init_Object(void) {
  godot_rb_require_relative(object);
  godot_rb_cObject = godot_rb_get_module(Object);
  godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_OBJECT] = godot_rb_cObject;
  idRUBY_SCRIPT = rb_intern("RUBY_SCRIPT");
  gdext_classdb_construct_object =
    (GDExtensionInterfaceClassdbConstructObject)godot_rb_get_proc("classdb_construct_object");
  rb_define_method(godot_rb_cObject, "initialize", godot_rb_cObject_i_initialize, -1);
}
