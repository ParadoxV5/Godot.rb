#include "variants.h"

GDExtensionInterfaceGlobalGetSingleton gdext_global_get_singleton;
/** Also destroys the passed StringName */
VALUE godot_rb_Engine_impl_get_singleton(GDExtensionStringName name) {
  GDExtensionConstObjectPtr object_ptr = gdext_global_get_singleton(&name);
  godot_rb_gdextension.string_name_destroy(&name);
  GDExtensionVariantPtr variant = godot_rb_variant_alloc();
  godot_rb_gdextension.variant_from_object_ptr(variant, &object_ptr);
  return godot_rb_wrap_variant(godot_rb_cObject, variant);
}
/** code around the general method call API to return the singleton directly rather than getting the singleton class */
VALUE godot_rb_Engine_get_singleton(RB_UNUSED_VAR(VALUE self), VALUE name) {
  return godot_rb_Engine_impl_get_singleton(godot_rb_obj_to_string_name(name));
}

GDExtensionInterfaceObjectMethodBindCall gdext_object_method_bind_call;
// @param var Variable reüse: `method_bind` in, `ret_variant` out
void godot_rb_cObject_impl_call(
  GDExtensionVariantPtr self_variant,
  RB_UNUSED_VAR(VALUE func),
  long argc,
  GDExtensionConstVariantPtr const* argv,
  void** var,
  GDExtensionCallError* r_error
) {
  GDExtensionObjectPtr self_object;
  godot_rb_gdextension.object_ptr_from_variant(&self_object, self_variant);
  GDExtensionMethodBindPtr method_bind = (GDExtensionMethodBindPtr)*var;
  *(GDExtensionVariantPtr*)var = godot_rb_variant_alloc();
  gdext_object_method_bind_call(method_bind, self_object, argv, argc, (GDExtensionVariantPtr)*var, r_error);
}
VALUE godot_rb_cObject_call(
  RB_UNUSED_VAR(VALUE arg),
  VALUE var, // Variable reüse: `method_bind` in, `ret_variant` out
  int argc, const VALUE* argv,
  RB_UNUSED_VAR(VALUE blk)
) {
  ID meth;
  rb_frame_method_id_and_class(&meth, NULL);
  // {VALUE} ↔️ {void**} casts are exact
  godot_rb_variant_call(
    (godot_rb_variant_call_function)godot_rb_cObject_impl_call,
    rb_current_receiver(),
    ID2SYM(meth),
    argc, argv,
    (VALUE)&var
  );
  return godot_rb_parse_variant((GDExtensionVariantPtr)var);
}

GDExtensionInterfaceClassdbGetMethodBind gdext_classdb_get_method_bind;
VALUE godot_rb_cObject_m__godot_bind(VALUE self, VALUE name, VALUE hash) {
  GDExtensionStringName
    string_name = godot_rb_obj_to_string_name(name),
    class = godot_rb_obj_to_string_name(rb_funcallv_public(self, rb_intern("base_class_name"), 0, (VALUE[]){}));
      //TODO: at this frequency, might as well consider moving it to pure C
  name = rb_block_call(
    self,
    rb_intern("define_method"),
    1, (VALUE[]){name},
    godot_rb_cObject_call,
    (VALUE)gdext_classdb_get_method_bind(
      &class,
      &string_name,
      NUM2LL(hash) // SIZEOF_LONG_LONG ≥ 64 = sizeof(GDExtensionInt)
    )
  );
  godot_rb_gdextension.string_name_destroy(&string_name);
  godot_rb_gdextension.string_name_destroy(&class);
  return name;
}

VALUE DEPRECATED(rb_method_missing)(int argc, const VALUE* argv, VALUE self) {
  rb_raise(rb_eNoMethodError, "undefined method `%"PRIsVALUE"' for %+"PRIsVALUE, argc ? *argv : Qnil, self);
}
VALUE DEPRECATED(obj_respond_to_missing)(
  RB_UNUSED_VAR(VALUE obj), RB_UNUSED_VAR(VALUE mid), RB_UNUSED_VAR(VALUE priv)
) { return Qfalse; }

void godot_rb_init_Objects(bool first_time) {
  if RB_UNLIKELY(first_time) {
    gdext_global_get_singleton =
      (GDExtensionInterfaceGlobalGetSingleton)godot_rb_get_proc("global_get_singleton");
    gdext_object_method_bind_call =
      (GDExtensionInterfaceObjectMethodBindCall)godot_rb_get_proc("object_method_bind_call");
    gdext_classdb_get_method_bind =
      (GDExtensionInterfaceClassdbGetMethodBind)godot_rb_get_proc("classdb_get_method_bind");
    VALUE Engine = godot_rb_Engine_impl_get_singleton(godot_rb_chars_to_string_name("Engine"));
    rb_const_set(godot_rb_mGodot, rb_intern("Engine"), Engine);
    rb_define_singleton_method(Engine, "get_singleton", godot_rb_Engine_get_singleton, 1);
    rb_define_private_method(rb_singleton_class(godot_rb_cObject), "__godot_bind", godot_rb_cObject_m__godot_bind, 2);
  }
  rb_load(rb_str_new_lit("./addons/Godot.rb/lib/godot/objects.rb"), false);
  // Temporary solution: mute `Object#method_missing` and `ObjeGct#respond_to_missing?`
  if RB_UNLIKELY(first_time) {
    rb_define_method(godot_rb_cObject, "method_missing", rb_method_missing, -1);
    rb_define_method(godot_rb_cObject, "respond_to_missing?", obj_respond_to_missing, 2);
  }
}
