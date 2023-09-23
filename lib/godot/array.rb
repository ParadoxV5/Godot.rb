module Godot
  class Variant
    class AbstractArray < Variant
      include Enumerable
      
      def self.from(enum, **kwargs)
        array = new(**kwargs)
        enum = enum.to_enum
        size = enum.size || 0
        array.resize(size)
        begin
          size.times { array[_1] = enum.next }
        rescue StopIteration
          # ignore
        else
          loop { array[index] = enum.next }
        end
        array
      end
      
      def self.[](*elems, **kwargs)
        array = new(**kwargs)
        array.resize(elems.size)
        elems.each_with_index do|elem, index|
          #@type var elem: untyped
          array[index] = elem
        end
        array
      end
      
      def each(&blk)
        return to_enum { size } unless blk
        size.times { blk.(self[_1]) }
        self
      end
    end
  end
  
  %w[
    Byte
    Int32
    Int64
    Float32
    Float64
    String
    Vector2
    Vector3
    Color
  ].each_with_index do|elem_type, index|
    klass = Class.new(Variant::AbstractArray)
    klass::VARIANT_TYPE = 29 + index # PACKED_BYTE_ARRAY + offset
    const_set "Packed#{elem_type}Array", klass
  end
end