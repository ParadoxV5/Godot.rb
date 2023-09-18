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
VALUE godot_rb_cVariant_m_allocate(VALUE self) {
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
      if RB_LIKELY(godot_rb_gdextension.variant_booleanize(variant)) { // Non-null check
        // “`godot_rb_parse_object`”
        GDExtensionObjectPtr object;
        godot_rb_gdextension.object_ptr_from_variant(&object, variant);
        GDExtensionStringName class_name_str;
        godot_rb_gdextension.object_get_class_name(object, godot_rb_library, &class_name_str);
        godot_rb_gdextension.object_destroy(object);
        VALUE class_name = godot_rb_sym_from_string_name(&class_name_str);
        godot_rb_gdextension.string_name_destroy(class_name_str);
        // “`godot_rb_wrap_object`”
        return TypedData_Wrap_Struct(
          rb_const_get_at(godot_rb_mGodot, SYM2ID(class_name)), // calls {Godot#const_missing} as needed
          &godot_rb_cVariant_type,
          variant
        );
      }
      
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


#define a(get_value_variant, meth, ret) { \
  GDExtensionVariantPtr value_variant = get_value_variant; \
  GDExtensionBool success = false; \
  godot_rb_gdextension.meth( \
    godot_rb_obj_get_variant(self), \
    godot_rb_obj_get_variant(key), \
    value_variant, \
    &success \
  ); \
  if RB_LIKELY(success) \
    return ret; \
  VALUE kwargs = rb_hash_new_capa(2); \
  rb_hash_aset(kwargs, rb_intern("receiver"), self); \
  rb_hash_aset(kwargs, rb_intern("key"), key); \
  rb_exc_raise(rb_class_new_instance_kw( \
    2, \
    (VALUE[]){rb_sprintf("key not found: %+"PRIsVALUE, key), kwargs}, \
    rb_eKeyError, \
    RB_PASS_KEYWORDS) \
  ); \
}

/*TODO: docs
> The bracket syntax can be used to access properties of any Object, not just Dictionaries.
> Keep in mind it will cause a script error when attempting to index a non-existing property.
> To avoid this, use the [Object.get()](https://docs.godotengine.org/en/stable/classes/class_object.html#id1)
> and [Object.set()](https://docs.godotengine.org/en/stable/classes/class_object.html#id6) methods instead.
⸺ https://docs.godotengine.org/en/stable/tutorials/scripting/gdscript/gdscript_basics.html#dictionary */
__attribute__((used)) VALUE godot_rb_cVariant_i__aref(VALUE self, VALUE key)
  a(godot_rb_variant_alloc(), variant_get, godot_rb_parse_variant(value_variant))
__attribute__((used)) VALUE godot_rb_cVariant_i__aset(VALUE self, VALUE key, VALUE value)
  a(godot_rb_obj_get_variant(value), variant_set, value)


void godot_rb_variant_call(godot_rb_variant_call_function function, VALUE self, VALUE func, VALUE args, VALUE var) {
  
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
      rb_exc_raise(rb_class_new_instance_kw( 4, (VALUE[]){
        rb_sprintf("undefined method `%"PRIsVALUE"', or cannot call it with %+"PRIsVALUE, func, args),
        func,
        args,
        kwargs
      }, rb_eNoMethodError, RB_PASS_KEYWORDS) );
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
) {
  VALUE variant_type_value = rb_const_get_from(CLASS_OF(self), godot_rb_idVARIANT_TYPE);
  unsigned int variant_type = FIX2INT(variant_type_value);
  if RB_UNLIKELY(variant_type >= GDEXTENSION_VARIANT_TYPE_VARIANT_MAX)
    rb_raise(rb_eRangeError,
      "`VARIANT_TYPE' %"PRIsVALUE" outside of valid range: 0...%ud (one does not simply `initialize' a Variant)",
      variant_type_value,
      GDEXTENSION_VARIANT_TYPE_VARIANT_MAX
    );
  godot_rb_gdextension.variant_construct(variant_type, self_variant, argv, argc, r_error);
}
VALUE godot_rb_cVariant_i_initialize(VALUE self, VALUE args) {
  godot_rb_variant_call(godot_rb_cVariant_impl_initialize, self, rb_intern("initialize"), args, self);
  return args;
}

static void godot_rb_cVariant_impl_godot_send(
  GDExtensionVariantPtr self_variant,
  VALUE meth,
  long argc,
  GDExtensionConstVariantPtr* argv,
  GDExtensionVariantPtr r_return,
  GDExtensionCallError* r_error
) {
  GDExtensionStringName meth_string_name = godot_rb_obj_to_string_name(meth);
  godot_rb_gdextension.variant_call(self_variant, &meth_string_name, argv, argc, r_return, r_error);
  godot_rb_gdextension.string_name_destroy(&meth_string_name);
}
__attribute__((used)) VALUE godot_rb_cVariant_i_godot_send(int argc, VALUE* argv, VALUE self) {
  VALUE meth, args;
  rb_scan_args(argc, argv, "1*", &meth, &args);
  GDExtensionVariantPtr ret_variant = godot_rb_variant_alloc();
  // {VALUE} ↔️ {void*} casts are exact
  godot_rb_variant_call(
    (godot_rb_variant_call_function)godot_rb_cVariant_impl_godot_send,
    self,
    meth,
    args,
    (VALUE)ret_variant
  );
  return godot_rb_parse_variant(ret_variant);
}


GDExtensionInterfaceVariantHasMethod variant_has_method;
__attribute__((used)) VALUE godot_rb_cVariant_i_has_method(VALUE self, VALUE name) {
  GDExtensionStringName string_name = godot_rb_obj_to_string_name(name);
  GDExtensionBool ret = variant_has_method(godot_rb_obj_get_variant(self), &string_name);
  godot_rb_gdextension.string_name_destroy(&string_name);
  return ret ? Qtrue : Qfalse;
}

__attribute__((used)) VALUE godot_rb_cVariant_i_nonzero_(VALUE self) {
  return godot_rb_gdextension.variant_booleanize(godot_rb_cVariant_get_variant(self)) ? Qtrue : Qfalse;
}


void godot_rb_init_Variant() {
  variant_has_method = (GDExtensionInterfaceVariantHasMethod)godot_rb_get_proc("variant_has_method");
  godot_rb_require_relative(variant);
  godot_rb_cVariant = rb_const_get(godot_rb_mGodot, rb_intern("Variant"));
  variant_to_bool   = godot_rb_gdextension.get_variant_to_type_constructor(GDEXTENSION_VARIANT_TYPE_BOOL  );
  rb_define_alloc_func(godot_rb_cVariant, godot_rb_cVariant_m_allocate);
  rb_define_private_method(godot_rb_cVariant, "initialize", godot_rb_cVariant_i_initialize, -2);
  rb_define_private_method(godot_rb_cVariant, "initialize_copy", godot_rb_cVariant_i_initialize_copy, 1);
  #define m(meth, func, arity) rb_define_method(godot_rb_cVariant, #meth, godot_rb_cVariant_i_##func, arity);
  m([]        , _aref     ,  1)
  m([]=       , _aset     ,  2)
  m(godot_send, godot_send, -1)
  m(has_method, has_method,  1)
  m(nonzero?  , nonzero_  ,  0)
}
