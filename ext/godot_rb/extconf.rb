require 'mkmf'

# Detect (Windows) stand-alone MinGW without MSYS2 (AKA DevKit)
unless CROSS_COMPILING or RbConfig::CONFIG['build_os'] != 'mingw32' or find_executable 'msys2'
  # Monkeypatch {MakeMakefile#find_header} to prevent translation to MSYS `/C/some/path/`
  def mkintpath(path) = path
end

include = File.expand_path '../../include/', __FILE__
find_header 'godot_rb.h', include
find_header 'godot/gdextension_interface.h', include

create_makefile 'godot_rb'
