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
GLUE_C = 'tmp/variant_types.c' # {Rake::ExtensionTask} defaults
GLUE_C_ERB = 'ext/godot_rb/variant_types.c.erb' # ditto
task 'compile:godot_rb': GLUE_C
Rake::ExtensionTask.new 'godot_rb' do|ext|
  ext.lib_dir = OUT
  ext.source_pattern = '**/*.c'
  ext.no_native = true
end

# Glue Code Entry Task
GLUE_RB = %w[core editor].to_h { [_1, File.join OUT, "#{_1}.rb"] }
glue = GLUE_RB.values.push File.join(OUT, 'godot.rbs'), GLUE_C
glue.each { file _1 => :extension_api } # Each task executes at most once
desc 'Generate glue code'
task(glue:)


# Common Glue Code Generator Task
EXTENSION_API = './ext/include/godot/extension_api_with_docs.json'
task :extension_api => [GLUE_C_ERB, EXTENSION_API, OUT] do
  require 'json'
  require 'erb'
  puts "> loading #{EXTENSION_API}"
  json = JSON.load_file EXTENSION_API
  N_IMMEDIATES = 4
  
  puts '| processing variant sizes'
  string_name_default_size = 0
  build_configurations = json.delete('builtin_class_sizes').to_h do|config|
    sizes = config.fetch('sizes').drop(N_IMMEDIATES).to_h(&:values)
    sizes['Nil'] = sizes.delete 'Variant' # repurpose slot as noted in C
    string_name_size = sizes.fetch 'StringName'
    string_name_default_size = string_name_size if string_name_size > string_name_default_size
    [
      config.fetch('build_configuration'),
      sizes.map {|name, size| <<-"C" }.join
        godot_rb_cVariant_sizes[GDEXTENSION_VARIANT_TYPE_#{name}] = #{size};
      C
    ]
  end
  
  puts '| processing variant classes'
  json.delete('builtin_classes')
    .drop(N_IMMEDIATES)
    .to_h { _1.values_at 'name', 'has_destructor' } => type_has_destructors
  
  json.delete('classes')
    .find { _1['name'] == 'OS' }
    .delete('methods')
    .find { _1['name'] == 'has_feature' }
    .fetch('hash') => hash_OS_has_feature
  
  puts "< generating #{GLUE_C}"
  File.write GLUE_C, ERB.new(File.read GLUE_C_ERB).result_with_hash(
    string_name_default_size:,
    build_configurations:,
    type_has_destructors:,
    hash_OS_has_feature:
  )
  
  # files = GLUE_RB.transform_values do|path|
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
end


# `Godot.rb.gdextension`
gdextension = File.join OUT, 'Godot.rb.gdextension'
task(gdextension:)
file gdextension => OUT do
  arch, plat, _ = RUBY_PLATFORM.split '-', 3
  # doublet reference: https://docs.godotengine.org/en/stable/tutorials/export/feature_tags.html#default-features
  puts 'Writing `.gdextenion`...' if verbose
    #TODO: set `compatibility_minimum` from JSON `header`
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
