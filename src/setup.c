#include "setup.h"

// Ruby keeps a copy of the argc/v pointers’ contents, though it seems to only use `argv[0]` occasionally.
// https://github.com/ruby/ruby/blob/v3_2_2/ruby.c#L2783-L2784
static char* arg0 = "main";
static bool godot_rb_setup_core(void) {
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

static bool godot_rb_setup_servers(void) {
  int state;
  const int line = __LINE__ + 1;
  rb_protect(rb_require_string, rb_str_new_cstr("./addons/Godot.rb/lib/godot.rb"), &state);
  if(state) { // Handle exception
    VALUE message = rb_funcall(rb_errinfo(), rb_intern("inspect"), 0);
    rb_set_errinfo(Qnil); // Clear exception
    godot_rb_error(StringValuePtr(message), __func__ , __FILE__, line);
    return false;
  }
  return true;
}

/** Dummy */
static bool godot_rb_setup_scene(void) {
  return true;
}

static bool (* const godot_rb_setup_functions[GDEXTENSION_MAX_INITIALIZATION_LEVEL])(void) = {
  godot_rb_setup_core,
  godot_rb_setup_servers,
  godot_rb_setup_scene
};
void godot_rb_setup(__attribute__((unused)) void* userdata, GDExtensionInitializationLevel p_level) {
  bool (*func)(void) = godot_rb_setup_functions[p_level];
  if(func) {
    if(p_level && !godot_rb_init_levels[p_level - 1])
      return;
    printf("setting up Godot.rb init level %u...\n", p_level);
    if(!func())
      return;
    printf("Godot.rb init level %u set up.\n", p_level);
  }
  godot_rb_init_levels[p_level] = true;
}
