# frozen_string_literal: true
module Godot
  class Variant
    class AbstractArray < Variant
      include Enumerable
      
      def self.from(enum)
        array = new
        size = enum.size
        if size
          array.resize(size)
          enum.each_with_index {|elem, index| array[index] = elem }
        else
          # Stop at {StopIteration}
          loop { array[index] = enum.next }
        end
        array
      end
      
      def self.[](*elems) = from(elems)
      
      def each(&blk)
        return to_enum { size } unless blk
        size.times { blk.(self[_1]) }
        self
      end
    end
  end
  
  class Array < Variant::AbstractArray
    VARIANT_TYPE = 28
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
