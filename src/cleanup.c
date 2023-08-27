#include "cleanup.h"

void godot_rb_cleanup_core() {
  if(godot_rb_init_levels[GDEXTENSION_INITIALIZATION_CORE]) {
    int err = ruby_cleanup(EXIT_SUCCESS);
    if(err)
      godot_rb_error("Ruby ran into a problem while exiting.", __func__, __FILE__, __LINE__);
  } else
    godot_rb_warn("Godot.rb hasn't set this init level up.", __func__, __FILE__, __LINE__);
}

void (*godot_rb_cleanup_functions[GDEXTENSION_MAX_INITIALIZATION_LEVEL])(void) = {
  [GDEXTENSION_INITIALIZATION_CORE] = godot_rb_cleanup_core
};
void godot_rb_cleanup(__attribute__((unused)) void* userdata, GDExtensionInitializationLevel p_level) {
  void (*func)(void) = godot_rb_cleanup_functions[p_level];
  if(func) {
    printf("cleaning up Godot.rb init level %u...\n", p_level);
    func();
    godot_rb_init_levels[p_level] = false;
    printf("Godot.rb init level %u cleaned up.\n", p_level);
  }
}
