#include "setup.h"
#include "ruby/variants.h"

/**
  Ruby keeps a copy of the argc/v pointers’ contents, though it seems to only use `argv[0]` occasionally.
  https://github.com/ruby/ruby/blob/v3_2_2/ruby.c#L2783-L2784
*/
static char* arg0 = "godot_rb.c";
static bool core(void) {
  // On Windows, the argc/v pointers are rather return vars as their original contents are discarded.
  // https://github.com/ruby/ruby/blob/v3_2_2/win32/win32.c#L923-L926
  int argc = 1;
  char** argv = &arg0;
  ruby_sysinit(&argc, &argv);
  int error = ruby_setup();
  if RB_LIKELY(!error) {
    if RB_LIKELY(ruby_executable_node(ruby_options(1, (char*[]){"-e "}), &error)) {
      ruby_script(arg0);
      return true;
    }
  }
  godot_rb_error("Ruby ran into a problem while starting.");
  return false;
}

VALUE godot_rb_mGodot_m_init_level(RB_UNUSED_VAR(VALUE self)) {
  return INT2FIX(godot_rb_init_level_next - 1);
}
static void servers(RB_UNUSED_VAR(va_list* args)) {
  // Load {Godot}
  godot_rb_require_relative(version);
  godot_rb_mGodot = rb_const_get(rb_cObject, rb_intern("Godot"));
  rb_gc_register_address(&godot_rb_mGodot);
  rb_define_singleton_method(godot_rb_mGodot, "init_level", godot_rb_mGodot_m_init_level, 0);
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
  // Load Ruby Integration
  godot_rb_init_Mixins();
}

static void scene(RB_UNUSED_VAR(va_list* args)) {
  godot_rb_init_Objects(true);
  godot_rb_init_RubyScript();
}

static void editor(RB_UNUSED_VAR(va_list* args)) {
  godot_rb_init_Objects(false);
}

static void (* const unprotected_functions[])(RB_UNUSED_VAR(va_list* args)) = {NULL, servers, scene, editor};
void godot_rb_setup(RB_UNUSED_VAR(void* userdata), GDExtensionInitializationLevel init_level) {
  if RB_LIKELY(godot_rb_init_level_next == init_level) {
    printf("setting up Godot.rb init level %u...\n", init_level);
    if RB_LIKELY(RB_LIKELY(init_level) ? godot_rb_protect(unprotected_functions[init_level]) : core())
      printf("Godot.rb init level %u set up.\n", godot_rb_init_level_next++);
  }
}
