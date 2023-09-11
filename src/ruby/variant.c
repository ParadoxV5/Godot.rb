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

GDExtensionUninitializedVariantPtr godot_rb_variant_alloc() {
  GDExtensionUninitializedVariantPtr mem = godot_rb_gdextension.mem_alloc(VARIANT_SIZE);
  if RB_LIKELY(mem)
    return mem;
  rb_raise(rb_eNoMemError, "Godot Engine out of memory");
}
VALUE godot_rb_wrap_variant(VALUE klass, GDExtensionVariantPtr variant) {
  return TypedData_Wrap_Struct(klass, &godot_rb_cVariant_type, variant);
}
//TODO: Documentation: warn that `#allocate`d variants are semi-unusable
VALUE godot_rb_cVariant_c_allocate(VALUE self) {
  GDExtensionVariantPtr variant = godot_rb_variant_alloc();
  godot_rb_gdextension.variant_new_nil(variant); // Zero-initialize it to prevent invalid GC frees
  return godot_rb_wrap_variant(self, variant);
}

VALUE godot_rb_cVariant_i_initialize_copy(VALUE self, VALUE other) {
  godot_rb_gdextension.variant_duplicate(godot_rb_cVariant_get_variant(self), godot_rb_obj_get_variant(other), false);
  return other;
}


GDExtensionTypeFromVariantConstructorFunc variant_to_bool;
VALUE godot_rb_parse_variant(GDExtensionVariantPtr variant) {
  GDExtensionVariantType variant_type = godot_rb_gdextension.variant_get_type(variant);
  switch(variant_type) {
    case GDEXTENSION_VARIANT_TYPE_OBJECT:
      if RB_LIKELY(godot_rb_gdextension.variant_booleanize(variant)) // Non-null check
        break;
      //fall-through
    case GDEXTENSION_VARIANT_TYPE_NIL:
      godot_rb_gdextension.variant_destroy(variant);
      return Qnil;
    case GDEXTENSION_VARIANT_TYPE_BOOL: {
      GDExtensionBool the_bool;
      variant_to_bool(&the_bool, variant);
      godot_rb_gdextension.variant_destroy(variant);
      return the_bool ? Qtrue : Qfalse;
    }
    default:;
  }
  return godot_rb_wrap_variant(godot_rb_cVariants[variant_type], variant);
}

GDExtensionVariantPtr godot_rb_cVariant_get_variant(VALUE self) {
  return rb_check_typeddata(self, &godot_rb_cVariant_type);
}
GDExtensionVariantPtr godot_rb_obj_get_variant(VALUE self) {
  return godot_rb_cVariant_get_variant(rb_convert_type(self, RUBY_T_DATA, "Godot::Variant", "to_godot"));
}


void godot_rb_variant_call(godot_rb_variant_call_function function, VALUE self, VALUE func, VALUE args, uintmax_t var) {
  
  GDExtensionVariantPtr self_variant = godot_rb_obj_get_variant(self);
  long argc = RARRAY_LEN(args);
  GDExtensionConstVariantPtr argv[argc];
  VALUE* arg0_p = RARRAY_PTR(args);
  for(long i = 0; i < argc; ++i)
    argv[i] = godot_rb_obj_get_variant(arg0_p[i]);
  
  GDExtensionCallError error;
  function(self_variant, func, argc, argv, var, &error);
  switch(error.error) {
    case GDEXTENSION_CALL_OK:
      return;
      
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
    }
    
    case GDEXTENSION_CALL_ERROR_INVALID_METHOD: {
      VALUE kwargs = rb_hash_new_capa(1);
      rb_hash_aset(kwargs, rb_intern("receiver"), self);
      rb_exc_raise(rb_class_new_instance_kw(4, (VALUE[]){
        rb_sprintf("undefined method `%"PRIsVALUE"', or cannot call it with %+"PRIsVALUE, func, args),
        func,
        args,
        kwargs
      }, rb_eNoMethodError, RB_PASS_KEYWORDS));
    }
    
    default: // TOO_MANY_ARGUMENTS, TOO_FEW_ARGUMENTS
      rb_raise(rb_eArgError,
        "wrong number of arguments (given %ld, expected %"PRId32")",
        argc,
        error.expected ? error.expected : error.argument // https://github.com/godotengine/godot/pull/80844
      );
  }
}

static void godot_rb_cVariant_impl_initialize(
  GDExtensionVariantPtr self_variant,
  __attribute__((unused)) VALUE func,
  long argc,
  GDExtensionConstVariantPtr* argv,
  VALUE self,
  GDExtensionCallError* r_error
) { godot_rb_gdextension.variant_construct(
  FIX2INT(rb_const_get_from(CLASS_OF(self), godot_rb_idVARIANT_TYPE)),
  self_variant,
  argv,
  argc,
  r_error
); }
VALUE godot_rb_cVariant_i_initialize(VALUE self, VALUE args) {
  godot_rb_variant_call((godot_rb_variant_call_function) godot_rb_cVariant_impl_initialize, self, rb_intern("initialize"), args, self);
  return args;
}

static void godot_rb_cVariant_impl_godot_send(
  GDExtensionVariantPtr self_variant,
  VALUE meth,
  long argc,
  GDExtensionConstVariantPtr* argv,
  GDExtensionVariantPtr ret_variant,
  GDExtensionCallError* r_error
) { godot_rb_gdextension.variant_call(
  self_variant,
  godot_rb_cVariant_get_variant(rb_to_symbol(meth)), //FIXME: actually this needs to be a StringName not a Variant; it’s C++ magic even if it works
  argv,
  argc,
  ret_variant,
  r_error
); }
VALUE godot_rb_cVariant_i_godot_send(int argc, VALUE* argv, VALUE self) {
  VALUE meth, args;
  rb_scan_args(argc, argv, "1*", &meth, &args);
  GDExtensionVariantPtr ret_variant = godot_rb_variant_alloc();
  godot_rb_variant_call(
    (godot_rb_variant_call_function)godot_rb_cVariant_impl_godot_send,
    self,
    meth,
    args,
    (uintmax_t)ret_variant
  );
  return godot_rb_parse_variant(ret_variant);
}


VALUE godot_rb_cVariant_i_nonzero_(VALUE self) {
  return godot_rb_gdextension.variant_booleanize(godot_rb_cVariant_get_variant(self)) ? Qtrue : Qfalse;
}


void godot_rb_init_Variant() {
  godot_rb_require_relative(variant);
  godot_rb_cVariant = rb_const_get(godot_rb_mGodot, rb_intern("Variant"));
  rb_define_alloc_func(godot_rb_cVariant, godot_rb_cVariant_c_allocate);
  rb_define_private_method(godot_rb_cVariant, "initialize", godot_rb_cVariant_i_initialize, -2);
  rb_define_private_method(godot_rb_cVariant, "initialize_copy", godot_rb_cVariant_i_initialize_copy, 1);
  rb_define_method(godot_rb_cVariant, "godot_send", godot_rb_cVariant_i_godot_send, -1);
  rb_define_method(godot_rb_cVariant, "nonzero?", godot_rb_cVariant_i_nonzero_, 0);
}
