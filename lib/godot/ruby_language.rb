# frozen_string_literal: true

module Godot
  module Ruby::Autoloads
    public_class_method :remove_const
    Godot.include self
  end
  
  # Singleton
  class << RubyLanguage = ScriptLanguageExtension.new
    
    # TODO: don’t generate a new {Variant} on every call
    
    def _get_name = 'Ruby'
    #TODO: can this be `'Godot::Ruby'`?
    def _get_type = 'Ruby'
    def _get_extension = 'rb'
    def _get_recognized_extensions = PackedStringArray.from %w[rb rbw gemspec]
    def _get_comment_delimiters = PackedStringArray['#'] # What `=begin`-`=end`?
    # for syntax-highlighting
    def _get_string_delimiters = PackedStringArray.from %w[" ' / `].map { "#{_1} #{_1}" }
    def _can_inherit_from_file = true
    # Whether or not users get to name classes themselves – no for both GDScript and C#
    def _has_named_classes = false
    
    # Same _as {add_named_global_constant}, but does not set if the value is falsy. This is because
    # [Godot Engine preloads `nil`s](https://github.com/godotengine/godot/blob/4.1.1-stable/main/main.cpp#L3009)
    # in _case the autoload fails to load.
    # Here _in Ruby, we’ll just let {NameError} fail fast instead of leaving `nil` error risks.
    def _add_global_constant(name, value)
      value and add_named_global_constant(name, value)
    end
    def _add_named_global_constant(name, value) = Ruby::Autoloads.const_set(name, value)
    def _remove_named_global_constant(name) = Ruby::Autoloads.remove_const(name)
    
    def _create_script = Ruby.new
    # !
    # Dictionary _get_global_class_name ( String path ) virtual const
    # 
    # LSP integration
    # String _auto_indent_code ( String code, int from_line, int to_line ) virtual const
    # Dictionary _complete_code ( String code, String path, Object owner ) virtual const
    # int _find_function ( String class_name, String function_name ) virtual const
    # Dictionary[] _get_built_in_templates ( StringName object ) virtual const
    # PackedStringArray _get_reserved_words ( ) virtual const
    # bool _is_control_flow_keyword ( String keyword ) virtual const
    # bool _overrides_external_editor ( ) virtual
    # Dictionary _lookup_code ( String code, String symbol, String path, Object owner ) virtual const
    # Dictionary _validate ( String script, String path, bool validate_functions, bool validate_errors, bool validate_warnings, bool validate_safe_lines ) virtual const
    # 
    # Debugger integration
    # Dictionary[] _debug_get_current_stack_info ( ) virtual
    # String _debug_get_error ( ) virtual const
    # Dictionary _debug_get_globals ( int max_subitems, int max_depth ) virtual
    # int _debug_get_stack_level_count ( ) virtual const
    # String _debug_get_stack_level_function ( int level ) virtual const
    # void* debug_get_stack_level_instance ( int level ) virtual
    # int _debug_get_stack_level_line ( int level ) virtual const
    # Dictionary _debug_get_stack_level_locals ( int level, int max_subitems, int max_depth ) virtual
    # Dictionary _debug_get_stack_level_members ( int level, int max_subitems, int max_depth ) virtual
    # String _debug_parse_stack_level_expression ( int level, String expression, int max_subitems, int max_depth ) virtual
    # 
    # Profiling integration
    # int _profiling_get_accumulated_data ( ScriptLanguageExtensionProfilingInfo* info_array, int info_max ) virtual
    # int _profiling_get_frame_data ( ScriptLanguageExtensionProfilingInfo* info_array, int info_max ) virtual
    # void _profiling_start ( ) virtual
    # void _profiling_stop ( ) virtual
    # 
    # !
    # void _reload_all_scripts ( ) virtual
    # void _reload_tool_script ( Script script, bool soft_reload ) virtual
    # bool _supports_documentation ( ) virtual const
    # void _thread_enter ( ) virtual
    # void _thread_exit ( ) virtual
    # String _validate_path ( String path ) virtual const
    # 
    # ?
    # void _finish ( ) virtual
    # void _frame ( ) virtual
    # Dictionary[] _get_public_annotations ( ) virtual const
    # Dictionary _get_public_constants ( ) virtual const
    # Dictionary[] _get_public_functions ( ) virtual const
    # bool _handles_global_class_type ( String type ) virtual const
    # bool _is_using_templates ( ) virtual
    # String _make_function ( String class_name, String function_name, PackedStringArray function_args ) virtual const
    # Script _make_template ( String template, String class_name, String base_class_name ) virtual const
    # Error _open_in_external_editor ( ScriptExtension script, int line, int column ) virtual
    # bool _supports_builtin_mode ( ) virtual const
    
  end
end
