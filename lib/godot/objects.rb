# frozen_string_literal: true
module Godot
  
  #@type singleton_name: String
  Engine.get_singleton_list.each do|name|
    name_str = name.to_s
    unless const_defined? name_str, false
      const_set name_str, Engine.get_singleton(name)
    end
  end
  
  GDEXTENSION_API_TYPES ||= [
    nil   ,  # SERVERS
    'core',  # SCENE
    'editor' # EDITOR
  ]
  # https://github.com/godotengine/godot/pull/80852
  UNEXPOSED_CLASSES ||= %w[
    FramebufferCacheRD
    GDScriptEditorTranslationParserPlugin
    GDScriptNativeClass
    GLTFDocumentExtensionPhysics
    GLTFDocumentExtensionTextureWebP
    GodotPhysicsServer2D
    GodotPhysicsServer3D
    IPUnix
    JNISingleton
    JavaClass
    JavaClassWrapper
    JavaScriptBridge
    JavaScriptObject
    MovieWriterMJPEG
    MovieWriterPNGWAV
    ResourceFormatImporterSaver
    ThemeDB
    UniformSetCacheRD
  ]
  
  #FIXME: **method redefined** when `load`ing this file the second time
  private_class_method def self.__const_get(name)
    if const_defined? name, false
      const_get name, false
    else
      const_set(
        name,
        Class.new(__const_get(ClassDB.get_parent_class(name).to_s))
      )
    end
  end
  #TODO: do similar treatment with {Variant}. code reüse potential?
  GDEXTENSION_API_TYPES[init_level]&.then do|api_type|
    klass = self #: Module?
    File.foreach(
      "./addons/Godot.rb/build/#{api_type}.hashes.tsv",
      chomp: true
    ) do|row|
      name, hash, _comments = row.split "\t", 3
      next unless name
      if hash # method
        # https://github.com/godotengine/godot/pull/80852
        unless klass.nil? or klass.method_defined? name, false
          klass.__send__ :__godot_bind, name, hash.to_i
          # Prefix ➡️ Suffix
          prefix, n = name.split '_', 2
          if n # 'name'.split('_', 2)[1] #=> %w[name][1]
            suffix = case prefix
              when 'is', 'has' then '?'
              when 'get' then ''
              when 'set' then '='
              else nil
            end
            klass.alias_method "#{n}#{suffix}", name if suffix
          end
        end
      else # class
        # https://github.com/godotengine/godot/pull/80852
        if UNEXPOSED_CLASSES.bsearch &name.method(:<=>)
          klass = nil
          next
        end
        klass = __const_get name
        klass = klass.singleton_class unless klass.is_a? Class
        klass.define_singleton_method(:base_class_name) { name }
        #TODO: non-method members
      end
    end
  end
  
end
