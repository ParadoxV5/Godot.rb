# frozen_string_literal: true

module Godot
  class Object < Variant
    VARIANT_TYPE = 24 # OBJECT
    # The {RubyScript} that manages this subclass, or `nil` if it didn’t come from Godot.rb
    RUBY_SCRIPT = nil
    
    alias call godot_send
    
    private def __get_class
      script = get_script #: RubyScript[Object]?
      # TODO: could also be scripts from other languages…
      script&.klass or Godot.const_get get_class.to_sym, false
    end
  end
end
