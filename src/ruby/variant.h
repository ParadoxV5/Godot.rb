#ifndef GODOT_RB_VARIANT_H
#define GODOT_RB_VARIANT_H
#include <godot_rb.h>

extern rb_data_type_t godot_rb_cVariant_type;

__attribute__((unused)) VALUE godot_rb_init_Variant(__attribute__((unused)) VALUE klass);

GDExtensionVariantPtr godot_rb_cVariant_to_variant(VALUE self);
// Return is `#initialize`d (usable)
VALUE godot_rb_cVariant_from_variant(GDExtensionConstVariantPtr variant);

#endif
