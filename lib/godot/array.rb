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
        elems.each_with_index {|elem, index| array[index] = elem }
        array
      end
      
      def each(&blk)
        return to_enum unless blk
        size.times { blk.(self[_1]) }
        self
      end
    end
  end
  
  end
end
