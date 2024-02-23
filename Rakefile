# frozen_string_literal: true
require 'rake/extensiontask'
begin
  require 'mingw-make'
rescue LoadError
  # ignore
end

# Output directory
OUT = 'generated'
CLOBBER << OUT

# C
Rake::ExtensionTask.new 'godot_rb' do|ext|
  ext.lib_dir = OUT
  ext.source_pattern = '**/*.c'
  ext.no_native = true
end

# Glue Code Entry Tasks
glue = %w[core editor].to_h do|api_type|
  rb = File.join OUT, "#{api_type}.rb"
  [api_type, rb]
end #: Hash[String|Symbol, String]
glue[:rbs] = File.join OUT, 'godot.rbs'
glue.each_value { file _1 => :extension_api } # Each task executes at most once
desc 'Generate glue code'
task glue: glue.values

# Common Glue Code Generator Task
extension_api = './ext/include/godot/extension_api_with_docs.json'
task :extension_api => [extension_api, OUT] do
  # require 'json'
  # json = JSON.load_file extension_api
  # files = glue.transform_values do|path|
  #   File.open path, 'w'
  # rescue => e
  #   warn e.full_message
  # end
  # begin
  #   json['classes'].each do|klass_data|
  #     files[klass_data['api_type']]&.then do|rb|
  #       klass_data['methods']&.each do|method_data|
  #         method_data['hash']&.then { _1 }
  #       end
  #     end
  #   end
  # ensure
  #   files.each_value { _1&.close }
  # end
  warn :extension_api
end

# `Godot.rb.gdextension`
gdextension = File.join OUT, 'Godot.rb.gdextension'
task(gdextension:)
file gdextension => OUT do
  arch, plat, _ = RUBY_PLATFORM.split '-', 3
  # doublet reference: https://docs.godotengine.org/en/stable/tutorials/export/feature_tags.html#default-features
  puts 'Writing `.gdextenion`...' if verbose
  File.write gdextension, <<~"INI"
    [configuration]
    entry_symbol = "Init_godot_rb"
    compatibility_minimum = 4.1
    
    [libraries]
    #{arch.eql?('x64') ? 'x86_64' : arch}.#{case plat
    when /mingw|mswin|cygwin/
      'windows'
    when 'darwin'
      'macos'
    when /bsd/
      'bsd'
    else # Linux & unknowns
      plat
    end} = "#{File.expand_path 'godot_rb.so', OUT}"
  INI
    # [dependencies]
    # arch.plat = {
    #   "relative/path/to/source/": "path/to/export/dir"
    # }
end

# Ruby DLL symlinks
# Windows RubyInstaller2 has it in `bindir` rather than `libdir`
so_dir = RbConfig::CONFIG[defined?(RubyInstaller) ? 'bindir' : 'libdir']
libruby_so = RbConfig::CONFIG['LIBRUBY_SO']
libs = {libruby_so => File.join(so_dir, libruby_so)}
if defined? RubyInstaller
  %w[libgmp-10.dll].each do|lib|
    libs[lib] = File.join so_dir, 'ruby_builtin_dlls', lib
  end
end
multitask dll_symlinks: libs.map {|name, lib_path|
  symlink_path = File.join OUT, name
  file symlink_path => [lib_path, OUT] do|t|
    symlink t.prerequisites.first, t.name, verbose:
  end
  symlink_path
}

# Entry Task
desc 'Build Godot.rb, complete with glue code, the `.gdextension` and Ruby DLL symlinks'
multitask default: %i[compile glue gdextension dll_symlinks]
