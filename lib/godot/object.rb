# frozen_string_literal: true
module Godot
  class Object < Variant
    VARIANT_TYPE = 24
    
    alias call godot_send
  end
end