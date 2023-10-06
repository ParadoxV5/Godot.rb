# frozen_string_literal: true
module Godot
  class Object < Variant
    VARIANT_TYPE = 24
    RUBY_SCRIPT = nil
    
    def self.base_class_name
      if const_defined? :RUBY_SCRIPT, false # {RubyScript} class
        superclass&.base_class_name or StringName.new
      else # Godot native class
        # StringName.new(*name&.delete_prefix('Godot::')) # https://github.com/soutaro/steep/issues/926
        demodulized_name = name&.delete_prefix('Godot::')
        demodulized_name ? StringName.new(demodulized_name) : StringName.new
      end
    end
    
    # Stop `initialize(…)` from becoming `Object(…)` (rather than `Object.new(…)`, which calls {#_init})
    def initialize; end
    alias call godot_send
    
    # Temporarily disable this to hot fix infinite loop due to `super` still calls on `self`
    def method_missing(name, *args)
      raise NoMethodError.new(
        "undefined method `#{name}' for #{inspect}",
        name.to_s,
        args,
        receiver: self
      ) if name.start_with? '_'
      super
    end
  end
end
