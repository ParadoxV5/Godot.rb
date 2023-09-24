#include "variants.h"

VALUE godot_rb_cObject;

init(Object, OBJECT)
  godot_rb_cObject = cObject;
  rb_alias(cObject, rb_intern("call"), rb_intern("godot_send"));
}
