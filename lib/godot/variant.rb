# frozen_string_literal: true

module Godot
  # `Variant` is the atomic type in GDScript; correspondingly,
  # the `Godot::Variant` Ruby class wraps `Variant` data and provides universal APIs for it and its descendant classes.
  # Methods use the implicit conversion {#to_godot} to convert your {Mixins everyday Ruby objects} to `Godot::Variant`s.
  # 
  # Internally, `Variant`s are gap-bridging wrappers of references of various data structures
  # (internal to Godot Engine) or (for the simplest data such as {Godot::Integer int}s) immediate values.
  # As such, both Godot Engine and Ruby `Variant`s are pint-sized, passed-by-value and expendable.
  # Ruby programmers should treat them similar to {Rational}s – unlike immediate values such as {Integer}s,
  # two `Variant`s may be distinct Ruby instances even if they reference the same Godot-internal data.
  class Variant
    VARIANT_TYPE = 0
    
    def get(...)
      self.[](...)
    rescue KeyError => e
      warn e.full_message
    end
    def set(...)
      self.[]=(...)
    rescue KeyError => e
      warn e.full_message
    end
    
    def to_godot = self
  end
end
