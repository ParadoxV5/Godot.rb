#include "cleanup.h"

static void core(void) {
  if RB_UNLIKELY(ruby_cleanup(EXIT_SUCCESS))
    godot_rb_error("Ruby ran into a problem while exiting.");
}

static void (* const godot_rb_cleanup_functions[GDEXTENSION_MAX_INITIALIZATION_LEVEL])(void) = {core};
void godot_rb_cleanup(RB_UNUSED_VAR(void* userdata), GDExtensionInitializationLevel p_level) {
  void (*func)(void) = godot_rb_cleanup_functions[p_level];
  if RB_UNLIKELY(func) {
    printf("cleaning up Godot.rb init level %u...\n", p_level);
    func();
    godot_rb_init_levels[p_level] = false;
    printf("Godot.rb init level %u cleaned up.\n", p_level);
  }
}
