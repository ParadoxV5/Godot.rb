#include "variants.h"

VALUE godot_rb_cObject;
ID idRUBY_SCRIPT;
GDExtensionInterfaceClassdbConstructObject gdext_classdb_construct_object;

VALUE godot_rb_cObject_i_initialize(VALUE self) {
  rb_call_super(0, (VALUE[]){});
  VALUE ruby_script = rb_const_get_from(CLASS_OF(self), idRUBY_SCRIPT);
  GDExtensionStringName class_name = godot_rb_obj_to_string_name(rb_class_name(self));
    //FIXME: support script-based classes
  godot_rb_gdextension.variant_from_object_ptr(
    godot_rb_cVariant_get_variant(self),
    gdext_classdb_construct_object(&class_name)
  );
  godot_rb_gdextension.string_name_destroy(class_name);
  if RB_LIKELY(!NIL_P(ruby_script))
    rb_funcall(self, rb_intern("set_script"), 1, ruby_script);
  return self;
}

init(Object, OBJECT)
  godot_rb_cObject = cObject;
  idRUBY_SCRIPT = rb_intern("RUBY_SCRIPT");
  rb_const_set(cObject, idRUBY_SCRIPT, Qnil);
  rb_alias(cObject, rb_intern("call"), rb_intern("godot_send"));
  gdext_classdb_construct_object =
    (GDExtensionInterfaceClassdbConstructObject)godot_rb_get_proc("classdb_construct_object");
  rb_define_method(cObject, "initialize", godot_rb_cObject_i_initialize, 0);
}
