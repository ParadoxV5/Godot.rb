#include "variants.h"

VALUE godot_rb_mGodot_m_const_missing(__attribute__((unused)) VALUE self, VALUE name) {
  return rb_call_super(1, &name);
}

init(Object, OBJECT)
  rb_alias(cObject, rb_intern("call"), rb_intern("godot_send"));
  rb_define_singleton_method(godot_rb_mGodot, "const_missing", godot_rb_mGodot_m_const_missing, 1);
}
