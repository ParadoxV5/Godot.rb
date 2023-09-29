#include "variants.h"

VALUE godot_rb_cObject;
ID idRUBY_SCRIPT;

GDExtensionInterfaceClassdbConstructObject gdext_classdb_construct_object;
VALUE godot_rb_object_ptr_class(GDExtensionConstObjectPtr object_ptr) {
  GDExtensionStringName class_name_str;
  godot_rb_gdextension.object_get_class_name(object_ptr, godot_rb_library, &class_name_str);
  VALUE klass = rb_const_get_at(godot_rb_mGodot, godot_rb_id_from_string_name(&class_name_str, 0));
  // calls {Godot#const_missing} as needed
  godot_rb_gdextension.string_name_destroy(class_name_str);
  return klass;
}

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

init(Object, OBJECT)
  godot_rb_cObject = cObject;
  idRUBY_SCRIPT = rb_intern("RUBY_SCRIPT");
  /** The {RubyScript} that manages this subclass, or `nil` if it didn’t come from Godot.rb */
  rb_const_set(cObject, idRUBY_SCRIPT, Qnil);
  rb_alias(cObject, rb_intern("call"), rb_intern("godot_send"));
  gdext_classdb_construct_object =
    (GDExtensionInterfaceClassdbConstructObject)godot_rb_get_proc("classdb_construct_object");
  rb_define_method(cObject, "initialize", godot_rb_cObject_i_initialize, -1);
}
