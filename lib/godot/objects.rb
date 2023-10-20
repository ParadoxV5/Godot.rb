# frozen_string_literal: true
module Godot
  
  #@type singleton_name: String
  Engine.get_singleton_list.each do|name|
    unless const_defined? name, false
      const_set name, Engine.get_singleton(name)
    end
  end
  
  GDEXTENSION_API_TYPES ||= [
    nil   ,  # CORE
    nil   ,  # SERVERS
    'core',  # SCENE
    'editor' # EDITOR
  ]
  
  #FIXME: **method redefined** when `load`ing this file the second time
  private_class_method def self.__const_get(name)
    if const_defined? name, false
      const_get name, false
    else
      const_set name, Class.new(__const_get(ClassDB.get_parent_class(name)))
    end
  end
  #TODO: do similar treatment with {Variant}. code reüse potential?
  GDEXTENSION_API_TYPES[init_level]&.then do|api_type|
    klass = self #: Module
    File.foreach "./addons/Godot.rb/build/#{api_type}.hashes.tsv" do|row|
      name, hash, _comments = row.split "\t", 3
      next unless name
      if hash # method
        unless klass.method_defined? name, false
          klass.__send__ :__godot_bind, name, hash.to_i
          # Prefix ➡️ Suffix
          prefix, n = name.split '_', 2
          if n # 'name'.split('_', 2)[1] #=> %w[name][1]
            case prefix
              when 'is', 'has' then '?'
              when 'get' then ''
              when 'set' then '='
              else nil
            end&.then {|suffix| klass.alias_method "#{n}#{suffix}", name }
          end
        end
      else # class
        klass = __const_get name
        klass = klass.singleton_class unless klass.is_a? Class
        #TODO: non-method members
      end
    end
  end
  
end
