#include "ruby/variants.h"

static void core(void) {
  if RB_UNLIKELY(ruby_cleanup(EXIT_SUCCESS))
    godot_rb_error("Ruby ran into a problem while exiting.");
}

static void (* const godot_rb_cleanup_functions[GDEXTENSION_MAX_INITIALIZATION_LEVEL])(void) = {
  core,
  NULL,
  godot_rb_destroy_RubyLanguage
};
void godot_rb_cleanup(RB_UNUSED_VAR(void* userdata), GDExtensionInitializationLevel init_level) {
  void (*func)(void) = godot_rb_cleanup_functions[init_level];
  if RB_UNLIKELY(func) {
    printf("cleaning up Godot.rb init level %u...\n", init_level);
    func();
    godot_rb_init_levels[init_level] = false;
    printf("Godot.rb init level %u cleaned up.\n", init_level);
  }
}
