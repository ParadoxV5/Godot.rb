# frozen_string_literal: true

module Godot
  class Variant
    #def self.allocate: () -> instance
    #def initialize: (*variant args) -> instance
    #def godot_send: (*variant args) -> variant
    
    def to_godot = self
    def nonzero? = !zero?
  end
end
