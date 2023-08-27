#include "setup.h"

static char* arg0 = "main";
bool godot_rb_setup_core(void) {
  // On Windows, the argc/v pointers are rather return vars as their original contents are discarded.
  // https://github.com/ruby/ruby/blob/v3_2_2/win32/win32.c#L923-L926
  int argc = 1;
  char** argv = &arg0;
  ruby_sysinit(&argc, &argv);
  int err = ruby_setup();
  if(err) {
    godot_rb_error("Ruby ran into a problem while starting.", __func__, __FILE__, __LINE__);
    return false;
  }
  ruby_script("godot_rb.c");
  return true;
}

bool (*godot_rb_setup_functions[GDEXTENSION_MAX_INITIALIZATION_LEVEL])(void) = {
  [GDEXTENSION_INITIALIZATION_CORE] = godot_rb_setup_core
};
// Ruby keeps a copy of the argc/v pointers’ contents, though it seems to only use `argv[0]` occasionally.
// https://github.com/ruby/ruby/blob/v3_2_2/ruby.c#L2783-L2784
void godot_rb_setup(__attribute__((unused)) void* userdata, GDExtensionInitializationLevel p_level) {
  bool (*func)(void) = godot_rb_setup_functions[p_level];
  if(func) {
    printf("setting up Godot.rb init level %u...\n", p_level);
    if(func()) {
      godot_rb_init_levels[p_level] = true;
      printf("Godot.rb init level %u set up.\n", p_level);
    }
  }
}
