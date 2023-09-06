# frozen_string_literal: true

module Godot
  class Variant
    VARIANTS = Array.new 38 # VARIANT_MAX
    VARIANT_TYPE = 0
    
    # @param meth [interned] the method name or `nil` for {#initialize}
    private def __godot_send__!(meth, *args)
      ret, error, argument, expected = __godot_send__ meth, args
      raise case error
      when 0 then return ret # OK
      when 6 then FrozenError.new( # METHOD_NOT_CONST
        "can't modify #{inspect} owned by Godot Engine",
        receiver: self
      )
      when 5 then FrozenError.new( # INSTANCE_IS_NULL
        "#{inspect} is null (you might have freed it)",
        receiver: self
      )
      when 2 then TypeError.new( # INVALID_ARGUMENT
        "wrong argument type #{self.class} #{
          case expected
          when 24 then '' # OBJECT
          when 1 then ' (expected true|false)' # BOOL
          when 0 then ' (expected nil)' # NIL
          else " (expected #{VARIANTS[expected]})"
          end
        }"
      )
      when 1 then if meth # INVALID_METHOD
        NoMethodError.new(
          "undefined or invalid method `#{meth}'",
          meth, #: String # Squelch possibly-incorrect RBS
          args,
          receiver: self
        )
      else # Constructor
        NoMethodError.new(
          "don't know how to initialize with #{args.map(&:class)}",
          'initialize', args,
          receiver: self
        )
      end
      else ArgumentError.new( # TOO_MANY_ARGUMENTS, TOO_FEW_ARGUMENTS
        "wrong number of arguments (given #{args.size}, expected #{
          expected.zero? ? argument : expected # https://github.com/godotengine/godot/pull/80844
        })"
      )
      end
    end
    
    def initialize(...) = __godot_send__!(nil, ...)
    def godot_send(meth, ...)
      raise TypeError, 'nil is not a symbol nor a string' if meth.nil?
      __godot_send__!(meth, ...)
    end
    
    def to_godot = self
  end
end
