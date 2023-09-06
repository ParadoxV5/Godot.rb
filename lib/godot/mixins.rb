# frozen_string_literal: true
module Godot
  module Mixins 
    %i[
      NilClass
      TrueClass
      FalseClass
    ].each do|const|
      toplevel_mod = const_get(const) #: Module
      toplevel_mod.include const_set(const, Module.new)
    end
  end
end
