# frozen_string_literal: true
module Godot
  class RubyScript < ScriptExtension
    def self.base_class_name = StringName.new 'ScriptExtension'
    
    # The parsed class, which may be de-sync with {#source_code} until {#_reload}ed
    attr_reader :klass
    def initialize(klass = nil)
      #TODO check type of `klass`
      klass&.const_set :RUBY_SCRIPT, self
      @klass = klass
    end
    
    # The Godot Editor pass entire source code( references)s around to enable IDE capabilities independent of the disk
    attr_accessor :source_code
    def _get_source_code = source_code || String.new
    alias _set_source_code source_code=
    def _has_source_code = source_code.nonzero?
    
    def _reload(_keep_state)
      1 # TODO
    end
    
    def _get_language = Godot::RubyLanguage::INSTANCE
    # TODO: LSP integration
    def _is_tool = true
    
    def _get_global_name
      name = klass&.name #: ::String?
      name ? StringName.new(name) : StringName.new
    end
    def _is_valid = !!klass
    # *Cannot* instantiate if the script is non-existent (it shouldn’t);
    # *shouldn’t* instantiate if it’s invalid (fails static checks)
    alias _can_instantiate _is_valid
    def _instance_has(obj) = klass ? obj.instance_of?(klass) : false
    
    def _get_base_script
      klass.superclass.const_get :RUBY_SCRIPT if klass
    end
    def _get_instance_base_type = klass ? klass.base_class_name : StringName.new
    
    # !
    # void _update_exports ( ) virtual
    # 
    # ! Standard Reflection
    # Dictionary _get_constants ( ) virtual const
    # StringName[] _get_members ( ) virtual const
    # Dictionary _get_method_info ( StringName method ) virtual const
    # Dictionary[] _get_script_method_list ( ) virtual const
    # bool _has_method ( StringName method ) virtual const
    # bool _inherits_script ( Script script ) virtual const
    # 
    # ! Extended Reflection
    # Dictionary[] _get_script_property_list ( ) virtual const
    # bool _has_property_default_value ( StringName property ) virtual const
    # Variant _get_property_default_value ( StringName property ) virtual const
    # 
    # Extended Reflection
    # Dictionary[] _get_script_signal_list ( ) virtual const
    # bool _has_script_signal ( StringName signal ) virtual const
    # 
    # LSP integration
    # Dictionary[] _get_documentation ( ) virtual const
    # int _get_member_line ( StringName member ) virtual const
    # 
    # TODO: Ask the Godot community for a description on placeholders and fallbacks
    # bool _is_placeholder_fallback_enabled ( ) virtual const
    # void _placeholder_erased ( void* placeholder ) virtual
    # GDExtensionScriptInstancePtr _placeholder_instance_create ( Object for_object ) virtual const
    # 
    # ?
    # def _get_rpc_config: () -> Variant
    
    def new(...)
      # Temporary workaround until script initializing finds insight
      klass.new(...).tap { _1.set_script self }
    end
  end
end
