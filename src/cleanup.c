#include "cleanup.h"

static void core(void) {
  if(ruby_cleanup(EXIT_SUCCESS))
    godot_rb_error("Ruby ran into a problem while exiting.");
}

static void (* const godot_rb_cleanup_functions[GDEXTENSION_MAX_INITIALIZATION_LEVEL])(void) = {core};
void godot_rb_cleanup(__attribute__((unused)) void* userdata, GDExtensionInitializationLevel p_level) {
  void (*func)(void) = godot_rb_cleanup_functions[p_level];
  if(func) {
    printf("cleaning up Godot.rb init level %u...\n", p_level);
    func();
    godot_rb_init_levels[p_level] = false;
    printf("Godot.rb init level %u cleaned up.\n", p_level);
  }
}
