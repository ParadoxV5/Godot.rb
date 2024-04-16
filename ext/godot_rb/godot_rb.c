#include "godot_rb.h"

GDExtensionInterfaceGetProcAddress godot_proc;
GDExtensionClassLibraryPtr godot_rb_lib_p;
VALUE godot_rb_mGodot;

GDExtensionInterfacePrintErrorWithMessage godot_print_error_with_message;
GDExtensionInterfacePrintScriptErrorWithMessage godot_print_script_error_with_message;
#define godot_rb_error(func, message) godot_print_error_with_message(message, message, func, __FILE__, __LINE__, false)


bool godot_rb_protect(VALUE* ret, VALUE (* function)(VALUE arg), VALUE arg) {
  if RB_LIKELY(ruby_native_thread_p()) {
    int state;
    VALUE ret_bak = rb_protect(function, arg, &state);
    if RB_UNLIKELY(state) { // Handle exception
      VALUE error = rb_errinfo();
      rb_set_errinfo(Qnil); // Clear exception
      VALUE full_message = rb_funcall(error, rb_intern("full_message"), 0);
      (
        RB_LIKELY(rb_obj_is_kind_of(error, rb_eStandardError))
        ? godot_print_script_error_with_message : godot_print_error_with_message
      )(
        StringValueCStr(full_message), StringValueCStr(error),
        "", "Godot.rb", 0, // {Exception#full_message} (should) include {Exception#backtrace} printout
        false
      );
    } else {
      if(ret)
        *ret = ret_bak;
      return true;
    }
  } else
    godot_rb_error("ruby_native_thread_p", "Ruby does not support Godot threads");
  return false;
}


/**TODO new variant spec & change to generated glue */
static VALUE godot_rb_initialize_api(RB_UNUSED_VAR(VALUE arg)) {
  // Load {Godot}
  godot_rb_require_relative(version);
  godot_rb_mGodot = rb_const_get(rb_cObject, rb_intern("Godot"));
  rb_gc_register_address(&godot_rb_mGodot);
  // Load Variants
  godot_rb_init_Variant();
  godot_rb_init_String();
  godot_rb_init_StringName();
  godot_rb_init_Object();
  godot_rb_require_relative(enumerable);
  godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_DICTIONARY] = godot_rb_get_module(Dictionary);
  godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_ARRAY] = godot_rb_get_module(Array);
  GDExtensionVariantType i = GDEXTENSION_VARIANT_TYPE_PACKED_BYTE_ARRAY;
  #define a(type) godot_rb_cVariants[i++] = godot_rb_get_module(Packed##type##Array);
  a(Byte)
  a(Int32)
  a(Int64)
  a(Float32)
  a(Float64)
  a(String)
  a(Vector2)
  a(Vector3)
  a(Color)
  // Skip the first four since they don’t have unique classes
  for(i = GDEXTENSION_VARIANT_TYPE_STRING; i < GDEXTENSION_VARIANT_TYPE_VARIANT_MAX; ++i)
    rb_gc_register_address(&godot_rb_cVariants[i]);
  godot_rb_init_Object(true);
  godot_rb_init_Mixins();
  godot_rb_init_RubyScript();
  return 0;
}

bool godot_rb_initialized = false;
void godot_rb_initialize(RB_UNUSED_VAR(void* userdata), GDExtensionInitializationLevel init_level) {
  switch(init_level) {
    case GDEXTENSION_INITIALIZATION_SCENE:
      puts("initializing Godot.rb...");
      godot_proc_get(PrintErrorWithMessage, print_error_with_message);
      /**
        Ruby keeps a copy of the argc/v pointers’ contents.
        https://github.com/ruby/ruby/blob/v3_3_0/ruby.c#L3060-L3061
      */
      static char* arg0 = "godot_rb.c";
      // On Windows, the argc/v pointers are rather return vars as their original contents are discarded.
      // https://github.com/ruby/ruby/blob/v3_3_0/win32/win32.c#L926-L929
      char** argv = &arg0;
      int argc = 1;
      ruby_sysinit(&argc, &argv);
      int error = ruby_setup();
      if RB_LIKELY(!error) {
        if RB_LIKELY(ruby_executable_node(ruby_options(2, (char*[]){"ruby", "-e "}), &error)) {
          ruby_script(arg0);
          // Initialize {Godot}
          godot_proc_get(PrintScriptErrorWithMessage, print_script_error_with_message);
          godot_rb_initialized = godot_rb_protect(NULL, godot_rb_initialize_api, 0);
          if RB_LIKELY(godot_rb_initialized)
            puts("Godot.rb initialized.");
          return;
        }
      }
      godot_rb_error(__func__, "Ruby ran into a problem while starting.");
    return;
    
    case GDEXTENSION_INITIALIZATION_EDITOR:
      if RB_LIKELY(godot_rb_initialized) {
        puts("initializing Godot.rb editor...");
        if RB_LIKELY(godot_rb_protect(NULL, godot_rb_init_Objects, false)) //TODO change to generated glue
          puts("Godot.rb editor initialized.");
      }
    return;
  }
}

void godot_rb_deinitialize(RB_UNUSED_VAR(void* userdata), GDExtensionInitializationLevel init_level) {
  if(RB_UNLIKELY(init_level == GDEXTENSION_INITIALIZATION_SCENE) && RB_LIKELY(godot_rb_initialized)) {
    puts("deinitializing Godot.rb...");
    godot_rb_protect(NULL, godot_rb_destroy_RubyLanguage, 0);
    if RB_UNLIKELY(ruby_cleanup(EXIT_SUCCESS))
      godot_rb_error(__func__, "Ruby ran into a problem while exiting.");
  }
}


GDExtensionBool Init_godot_rb(
  GDExtensionInterfaceGetProcAddress get_proc_address,
  GDExtensionClassLibraryPtr library,
  GDExtensionInitialization* r_initialization
) {
  r_initialization->minimum_initialization_level = GDEXTENSION_INITIALIZATION_CORE;
  r_initialization->initialize = godot_rb_initialize;
  r_initialization->deinitialize = godot_rb_deinitialize;
  godot_proc = get_proc_address;
  godot_rb_lib_p = library;
  return true;
}
