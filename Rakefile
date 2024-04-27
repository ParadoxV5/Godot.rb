# frozen_string_literal: true
require 'rake/extensiontask'
begin
  require 'mingw-make'
rescue LoadError
  # ignore
end


# Output directory
CLOBBER << OUT = 'generated'

# C
Rake::ExtensionTask.new 'godot_rb' do|ext|
  ext.lib_dir = OUT
  ext.source_pattern = '*.{c,h}{,.erb}'
  ext.extra_sources.add File.join(ext.ext_dir, '*.rb'), 'ext/include/**/*.h', 'ext/include/godot/extension_api_with_docs.json'
  ext.no_native = true
  CLEAN << File.join(ext.ext_dir, 'variant_types.c')
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
    end} = "#{File.join OUT, "godot_rb.#{RbConfig::CONFIG.fetch 'DLEXT'}"}"
  INI
    # [dependencies]
    # arch.plat = {
    #   "relative/path/to/source/": "path/to/export/dir"
    # }
end

# Ruby DLL symlinks
# Windows RubyInstaller2 has it in `bindir` rather than `libdir`
so_dir = RbConfig::CONFIG.fetch(defined?(RubyInstaller) ? 'bindir' : 'libdir')
libruby_so = RbConfig::CONFIG.fetch 'LIBRUBY_SO'
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
desc 'Build Godot.rb, including symlinks for Ruby shared libraries'
multitask default: %i[compile gdextension dll_symlinks]
