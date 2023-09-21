#include "variants.h"

void godot_rb_init_RubyLanguage(void) {
  godot_rb_require_relative(ruby);
  godot_rb_require_relative(ruby_language);
}
