#include "setup.h"
#include "ruby/variant.h"

// Ruby keeps a copy of the argc/v pointers’ contents, though it seems to only use `argv[0]` occasionally.
// https://github.com/ruby/ruby/blob/v3_2_2/ruby.c#L2783-L2784
static char* arg0 = "main";
static bool core(void) {
  // On Windows, the argc/v pointers are rather return vars as their original contents are discarded.
  // https://github.com/ruby/ruby/blob/v3_2_2/win32/win32.c#L923-L926
  int argc = 1;
  char** argv = &arg0;
  ruby_sysinit(&argc, &argv);
  if(ruby_setup()) {
    godot_rb_error("Ruby ran into a problem while starting.", __func__, __FILE__, __LINE__);
    return false;
  }
  ruby_script("godot_rb.c");
  return true;
}

static VALUE godot_rb_get_mGodot(__attribute__((unused)) VALUE name) {
  return godot_rb_mGodot = rb_const_get(rb_cObject, rb_intern("Godot"));
}
static bool servers(void) {
  if( godot_rb_protect(
    rb_require_string, rb_str_new_lit("./addons/Godot.rb/lib/godot.rb"), "rb_require_string", __FILE__, __LINE__
  ) && godot_rb_protect(
    godot_rb_get_mGodot, Qnil, "rb_const_get", __FILE__, __LINE__
  ) ) {
    rb_gc_register_mark_object(godot_rb_mGodot);
    return true;
  }
  return false;
}

static bool scene(void) {
  #define i(klass) godot_rb_protect(godot_rb_init_##klass, Qnil, "godot_rb_init_"#klass, __FILE__, __LINE__) &&
  return
    i(Variant)
  true;
}

static bool (* const godot_rb_setup_functions[GDEXTENSION_MAX_INITIALIZATION_LEVEL])(void) = {core, servers, scene};
void godot_rb_setup(__attribute__((unused)) void* userdata, GDExtensionInitializationLevel p_level) {
  bool (*func)(void) = godot_rb_setup_functions[p_level];
  if(func) {
    if(p_level && !godot_rb_init_levels[p_level - 1])
      return;
    printf("setting up Godot.rb init level %u...\n", p_level);
    if(!func())
      return;
  }
  godot_rb_init_levels[p_level] = true;
  printf("Godot.rb init level %u set up.\n", p_level);
}
