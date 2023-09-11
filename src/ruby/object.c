#include "variants.h"

VALUE godot_rb_mGodot_m_const_defined_(VALUE name, VALUE kwargs) {
}
VALUE godot_rb_mGodot_m_const_missing(VALUE name) {
}

init(Object, OBJECT)
  rb_alias(cObject, rb_intern("call"), rb_intern("godot_send"));
  rb_define_singleton_method(godot_rb_mGodot, "const_defined?", godot_rb_mGodot_m_const_defined_, 2);
  rb_define_singleton_method(godot_rb_mGodot, "const_missing", godot_rb_mGodot_m_const_missing, 1);
}
