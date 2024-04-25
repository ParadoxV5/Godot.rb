#ifndef GODOT_RB_H
#define GODOT_RB_H
#include <ruby.h>
#include <godot/gdextension_interface.h>

extern GDExtensionInterfaceGetProcAddress godot_proc;


// Ruby Binding Constants //

/**
  This array repurposes elements corresponding to immediate values –
  which are converted on-the-spot – for storing general classes and modules
  * `GDEXTENSION_VARIANT_TYPE_NIL` ➡ {Godot}
  * `GDEXTENSION_VARIANT_TYPE_BOOL` ➡ {Godot::Variant}
  * `GDEXTENSION_VARIANT_TYPE_INT` ➡ *unassigned*
  * `GDEXTENSION_VARIANT_TYPE_FLOAT` ➡ *unassigned*
  * `GDEXTENSION_VARIANT_TYPE_STRING`.. ➡ corresponding subclass
*/
extern VALUE godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_VARIANT_MAX];
#define godot_rb_mGodot   godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_NIL   ]
#define godot_rb_cVariant godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_BOOL  ]
#define godot_rb_cObject  godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_OBJECT]

/**
  This repurposes the element for `Nil` (which is always 0 by definition) to store the size for `Variant`.
  Elements for other immediate types are reserved but not set as Godot.rb treat them individually.
*/
size_t godot_rb_cVariant_sizes[GDEXTENSION_VARIANT_TYPE_VARIANT_MAX];
#define godot_rb_cVariant_size godot_rb_cVariant_sizes[GDEXTENSION_VARIANT_TYPE_NIL]


// Variant Helpers //

/** @return
  the Ruby version of the immediate value or
  a GC-independent instance of the corresponding {Godot::Variant} class from godot_rb_cVariants
*/
VALUE godot_rb_type_ptr_to_ruby(GDExtensionVariantType type_id, GDExtensionConstTypePtr ptr);
/**
  @param self
    1. if `nil`, do nothing
    2. if {Godot::Variant}, call create a new, GC-independent copy of `self`’s data at `ret`
    3. if immediate-value variant, convert it and store the C version at `ret`
    4. else, UB
  @param ret return buffer
  @return `ret`
*/
GDExtensionTypePtr godot_rb_ruby_to_type_ptr(VALUE self, GDExtensionUninitializedTypePtr ret);

/** @see godot_rb_variant_call */
typedef void (* godot_rb_variant_call_function)(
  GDExtensionVariantPtr self_variant,
  VALUE func,
  long argc,
  GDExtensionConstVariantPtr const* argv,
  VALUE var,
  GDExtensionCallError* r_error
);
/**
  @param func
    a function name (RBS: `interned`);
    this is reported in the event of a {rb_eNoMethodError} ({GDEXTENSION_CALL_ERROR_INVALID_METHOD})
  @param var
    an extra arg directly passed to the function for your various purposes
  @return if no {GDExtensionCallError error} raised, otherwise it {rb_raise go-tos instead of returning}
*/
void godot_rb_variant_call(
  godot_rb_variant_call_function function,
  VALUE self,
  VALUE func,
  int argc, const VALUE* argv,
  VALUE var
);


// General Helpers //

#define godot_rb_require_relative(name) rb_require_string(rb_str_new_lit("./addons/Godot.rb/lib/godot/"#name".rb"))
#define godot_rb_get_module(name) rb_const_get_at(godot_rb_mGodot, rb_intern(#name))

#include <ruby/encoding.h>
/** (initialized at level `SERVERS`) */
extern rb_encoding* godot_rb_encoding_UTF32;

/** `begin`–`rescue`s the passed function.
  If an exception occurs, log a Script Error or, for non-{rb_eStandardError}s, a generic Error.
  Also immediately return if the current thread is found to be non-Ruby.
  (It is a CRuby limitation that the Ruby APIs segfault when non-Ruby threads call them.)
  
  @param ret may be `NULL`
  @note This assumes that init level `CORE` is set up.
*/
bool godot_rb_protect(VALUE* ret, VALUE (* function)(VALUE arg), VALUE arg);


/** Entry Function */
__attribute__((used)) GDExtensionBool Init_godot_rb(
  GDExtensionInterfaceGetProcAddress get_proc_address,
  GDExtensionClassLibraryPtr library,
  GDExtensionInitialization* r_initialization
);


#endif
