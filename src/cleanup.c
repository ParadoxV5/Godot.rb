#include "cleanup.h"

void godot_rb_cleanup_core() {
  if(godot_rb_init_levels[GDEXTENSION_INITIALIZATION_CORE]) {
    int err = ruby_cleanup(EXIT_SUCCESS);
    if(err)
      printf("Error %i", err);//TODO: Error
  } else
    puts("Godot.rb\tthis level wasn't set up.");
  //TODO: replace select if not all `stdio` printing statements with Godot functions
}

void (*godot_rb_cleanup_functions[GDEXTENSION_MAX_INITIALIZATION_LEVEL])(void) = {
  [GDEXTENSION_INITIALIZATION_CORE] = godot_rb_cleanup_core
};
void godot_rb_cleanup(__attribute__((unused)) void* userdata, GDExtensionInitializationLevel p_level) {
  void (*func)(void) = godot_rb_cleanup_functions[p_level];
  if(func) {
    printf("Godot.rb\tcleaning up level %u...\n", p_level);
    func();
    godot_rb_init_levels[p_level] = false;
    printf("Godot.rb\tlevel %u cleaned up.\n", p_level);
  }
}
