#include "cleanup.h"

static void godot_rb_cleanup_core(void) {
  if(ruby_cleanup(EXIT_SUCCESS))
    godot_rb_error("Ruby ran into a problem while exiting.", __func__, __FILE__, __LINE__);
}

static void (* const godot_rb_cleanup_functions[GDEXTENSION_MAX_INITIALIZATION_LEVEL])(void) = {
  [GDEXTENSION_INITIALIZATION_CORE] = godot_rb_cleanup_core
};
void godot_rb_cleanup(__attribute__((unused)) void* userdata, GDExtensionInitializationLevel p_level) {
  void (*func)(void) = godot_rb_cleanup_functions[p_level];
  if(func) {
    printf("cleaning up Godot.rb init level %u...\n", p_level);
    func();
    printf("Godot.rb init level %u cleaned up.\n", p_level);
  }
  godot_rb_init_levels[p_level] = false;
}
