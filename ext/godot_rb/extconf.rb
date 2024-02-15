require 'mkmf'

include = File.expand_path '../../include/', __FILE__
find_header 'godot_rb.h', include
find_header 'godot/gdextension_interface.h', include

create_makefile 'godot_rb'
