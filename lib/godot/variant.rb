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
    def self.demodulized_name
      #StringName.new(*name&.[](7..)) # https://github.com/soutaro/steep/issues/926
      demodulized = name&.[](7..) # `Godot::` has 7 chars
      demodulized ? StringName.new(demodulized) : StringName.new
    end
    
    alias _init initialize
    
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
    
    # Same as {#has_key}, but returns `false` instead of raising {TypeError}
    def key?(key)
      has_key(key)
    rescue TypeError
      false
    end
    def respond_to_missing?(name, _include_all = false) # https://github.com/soutaro/steep/issues/913
      case name[-1] # again, Ruby suffixes are special
      when '=' then key? name[..-1]
      when '?' then has_method "is_#{name[..-1]}"
      else has_method name or key? name
      end or super
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
        no_method_error = NoMethodError #: _Exception # https://github.com/soutaro/steep/issues/919
    # First, check methods (if checking `attr_reader`s first, #[] can return a {Callable})
        begin
          return godot_send(name, *args) # `rescue` attaches `KeyError` cause if this raises
        rescue NoMethodError => no_method_error
          # fall-through, clear {$!} to prevent circular causes
        end
        # @ t ype var no_method_error NoMethodError[self]
        # Second, check `attr_reader`s
        if args.empty? # necessary condition for `attr_reader`
          begin
            return self[name]
          rescue => e # `attr_reader`s doesn’t work either: re-raise
            raise no_method_error, cause: e
          end
        else
          raise no_method_error
        end
      end
    end
    
    alias == eql?
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
