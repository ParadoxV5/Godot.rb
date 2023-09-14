#include "setup.h"
#include "ruby/variants.h"

/** Load encodings – for some reason they are separate in `rubyarchhdrdir` */
VALUE Init_enc(__attribute__((unused)) VALUE var) {
  rb_require("enc/encdb.so"),
  rb_require("enc/trans/transdb.so");
  return var;
}

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
  if RB_UNLIKELY(ruby_setup()) {
    godot_rb_error("Ruby ran into a problem while starting.");
    return false;
  }
  ruby_script(arg0);
  ruby_init_loadpath();
  return godot_rb_protect(Init_enc, NULL);
}

static VALUE servers_unprotected(__attribute__((unused)) VALUE value) {
  VALUE ret = godot_rb_require_relative(version);
  godot_rb_mGodot = rb_const_get(rb_cObject, rb_intern("Godot"));
  rb_gc_register_mark_object(godot_rb_mGodot);
  godot_rb_init_Variant();
  godot_rb_init_String();
  godot_rb_init_StringName();
  godot_rb_require_relative(object);
  godot_rb_cVariants[GDEXTENSION_VARIANT_TYPE_OBJECT] = rb_const_get_at(godot_rb_mGodot, rb_intern("Object"));
  godot_rb_init_Mixins();
  godot_rb_init_Engine();
  // Skip NIL since it points to the same class as BOOL
  for(GDExtensionVariantType i = GDEXTENSION_VARIANT_TYPE_BOOL; i < GDEXTENSION_VARIANT_TYPE_VARIANT_MAX; ++i)
    rb_gc_register_mark_object(godot_rb_cVariants[i]);
  return ret;
}
static bool servers(void) { return godot_rb_protect(servers_unprotected, NULL); }

/*
static bool scene(void) {
}
*/

static bool (* const godot_rb_setup_functions[GDEXTENSION_MAX_INITIALIZATION_LEVEL])(void) = {core, servers/*, scene*/};
void godot_rb_setup(__attribute__((unused)) void* userdata, GDExtensionInitializationLevel p_level) {
  bool (*func)(void) = godot_rb_setup_functions[p_level];
  if RB_LIKELY(func) {
    if RB_UNLIKELY(p_level && !godot_rb_init_levels[p_level - 1])
      return;
    printf("setting up Godot.rb init level %u...\n", p_level);
    if RB_UNLIKELY(!func())
      return;
  }
  godot_rb_init_levels[p_level] = true;
  printf("Godot.rb init level %u set up.\n", p_level);
}
