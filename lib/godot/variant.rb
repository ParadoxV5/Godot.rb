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
    include Godot
    VARIANT_TYPE = -1 # “one does not simply `initialize' a Variant”
    
    def get(...)
      self.[](...)
    rescue KeyError => e
      warn "#{e.backtrace&.first}: #{e.detailed_message}"
    end
    def set(...)
      self.[]=(...)
    rescue KeyError => e
      warn "#{e.backtrace&.first}: #{e.detailed_message}"
    end
    
    def method_missing(name, *args)
    # Zeroth, Ruby suffixes are special
      case name[-1]
      when '='
        public_send(:[]=, # https://github.com/soutaro/steep/issues/914
          name[..-1], #: ::String
          *args
        )
      when '?'
        godot_send("is_#{name[..-1]}", *args)
      else
    # First, check `attr_reader`s (Note: Godot Engine expects the two have mutually exclusive names)
        if args.empty? # necessary condition for `attr_reader`
          begin
            self[name]
          rescue KeyError
    # Second, check methods
            godot_send(name, *args) # `rescue` attaches `KeyError` cause if this raises
          end
        else
          godot_send(name, *args) # Same as in the `rescue KeyError` block, just without any `KeyError` causes
        end
      end
    end
    def respond_to_missing?(name, _include_all = false) # https://github.com/soutaro/steep/issues/913
    # Zeroth, Ruby suffixes are special
      case name[-1]
      when '='
        begin
          self[
            name[..-1] #: ::String
          ]
          true
        rescue KeyError
          super
        end
      when '?'
        has_method("is_#{name[..-1]}") or super
      else # method or `attr_reader` (Note ditto)
    # First, check `attr_reader`s
        begin
          self[name]
          true
        rescue KeyError
    # Second, check methods
          has_method(name) or super
        end
      end
    end
    
    def to_godot = self
  end
  
  def self.const_missing(name) = const_set(name,
    if Engine.has_singleton(name) # First, check Singletons
      Engine.get_singleton(name) #: Object
    elsif ClassDB.class_exists(name) # Second, check classes
      Class.new const_get(ClassDB.get_parent_class(name)) #: singleton(Object)
    else
      super # raise {NameError}
    end
  )
end
