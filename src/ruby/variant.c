#include "variants.h"
VALUE godot_rb_cVariant, godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_VARIANT_MAX];

/** Fetch size from `extension_api.json` */
#define VARIANT_SIZE 40
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
  return rb_check_typeddata(self, &godot_rb_cVariant_type);
}
GDExtensionVariantPtr godot_rb_obj_to_variant(VALUE self) {
  return godot_rb_cVariant_to_variant(rb_convert_type(self, RUBY_T_DATA, "Godot::Variant", "to_godot"));
}

GDExtensionTypeFromVariantConstructorFunc variant_to_bool;
VALUE godot_rb_obj_from_variant(GDExtensionVariantPtr variant) {
  GDExtensionVariantType variant_type = godot_rb_gdextension.variant_get_type(variant);
  switch(variant_type) {
    case GDEXTENSION_VARIANT_TYPE_OBJECT:
      if RB_LIKELY(godot_rb_gdextension.variant_booleanize(variant)) // Non-null check
        break;
      return Qnil;
    case GDEXTENSION_VARIANT_TYPE_BOOL: {
      GDExtensionBool the_bool;
      variant_to_bool(&the_bool, variant);
      return the_bool ? Qtrue : Qfalse;
    } case GDEXTENSION_VARIANT_TYPE_NIL:
      return Qnil;
    default: break;
  }
  VALUE self = rb_obj_alloc(godot_rb_cVariants[variant_type]);
  godot_rb_gdextension.variant_new_copy(godot_rb_cVariant_to_variant(self), variant);
  return self;
}

VALUE godot_rb_cVariant_i___godot_send__(VALUE self, VALUE meth, VALUE args) {
  GDExtensionVariantPtr self_variant = godot_rb_obj_to_variant(self);
  long argc = rb_array_len(args);
  GDExtensionConstVariantPtr argv[argc];
  for(long i = 0; i < argc; ++i)
    argv[i] = godot_rb_obj_to_variant(rb_ary_entry(args, i));
  VALUE ret;
  GDExtensionCallError error;
  if RB_UNLIKELY(NIL_P(meth)) { // Constructor
    godot_rb_gdextension.variant_construct(
      FIX2INT(rb_const_get_from(CLASS_OF(self), godot_rb_idVARIANT_TYPE)),
      self_variant,
      argv,
      argc,
      &error
    );
    ret = self;
  } else { // Method call
    GDExtensionUninitializedVariantPtr ret_variant = godot_rb_gdextension.mem_alloc(VARIANT_SIZE);
    godot_rb_gdextension.variant_call(
      self_variant,
      godot_rb_cVariant_to_variant(rb_to_symbol(meth)), //FIXME: actually this needs to be a StringName not a Variant; it’s C++ magic even if it works
      argv,
      argc,
      ret_variant,
      &error
    );
    ret = godot_rb_obj_from_variant(ret_variant); // Dubious reuse of variable <:
    //FIXME: A variant is built regardless if the call raised error
    godot_rb_gdextension.mem_free(ret_variant);
  }
  switch(error.error) {
    case GDEXTENSION_CALL_OK:
      break;
    case GDEXTENSION_CALL_ERROR_METHOD_NOT_CONST:
      rb_frozen_error_raise(self, "can't modify %+"PRIsVALUE" frozen by Godot Engine", self);
    case GDEXTENSION_CALL_ERROR_INSTANCE_IS_NULL:
      rb_frozen_error_raise(self, "%+"PRIsVALUE" is null (you might have freed it)", self);
    case GDEXTENSION_CALL_ERROR_INVALID_ARGUMENT: {
      char* expectation; switch(error.expected) {
        case GDEXTENSION_VARIANT_TYPE_OBJECT: expectation = ""; break;
        case GDEXTENSION_VARIANT_TYPE_BOOL  : expectation = " (expected true|false)"; break;
        case GDEXTENSION_VARIANT_TYPE_NIL   : expectation = " (expected nil)"; break;
        default: expectation = " (expected %"PRIsVALUE")";
      }
      rb_raise(rb_eTypeError, "wrong argument type %"PRIsVALUE"%s", CLASS_OF(self), expectation, godot_rb_cVariants[error.expected]);
    } case GDEXTENSION_CALL_ERROR_INVALID_METHOD: {
      VALUE kwargs = rb_hash_new_capa(1);
      rb_hash_aset(kwargs, rb_intern("receiver"), self);
      rb_exc_raise(rb_class_new_instance_kw(4, (VALUE[]) {
        RB_UNLIKELY(NIL_P(meth))
          ? rb_sprintf("don't know how to initialize with %+"PRIsVALUE, args)
          : rb_sprintf("undefined or invalid method `%"PRIsVALUE"'", meth),
        RB_UNLIKELY(NIL_P(meth)) ? meth : rb_intern("initialize"),
        args,
        kwargs
      }, rb_eNoMethodError, RB_PASS_KEYWORDS));
    } default: // TOO_MANY_ARGUMENTS, TOO_FEW_ARGUMENTS
      rb_raise(rb_eArgError,
        "wrong number of arguments (given %ld, expected %"PRId32")",
        argc,
        error.expected ? error.expected : error.argument // https://github.com/godotengine/godot/pull/80844
      );
  }
  return ret;
}

VALUE godot_rb_cVariant_i_nonzero_(VALUE self) {
  return godot_rb_gdextension.variant_booleanize(godot_rb_cVariant_to_variant(self)) ? Qtrue : Qfalse;
}

void godot_rb_init_Variant() {
  godot_rb_require_relative(variant);
  godot_rb_cVariant = rb_const_get(godot_rb_mGodot, rb_intern("Variant"));
  rb_define_alloc_func(godot_rb_cVariant, godot_rb_cVariant_alloc);
  rb_define_method(godot_rb_cVariant, "__godot_send__", godot_rb_cVariant_i___godot_send__, 2);
  rb_define_method(godot_rb_cVariant, "nonzero?", godot_rb_cVariant_i_nonzero_, 0);
}
