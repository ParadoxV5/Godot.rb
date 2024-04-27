require 'mkmf'

INCLUDE = File.expand_path '../../include/', __FILE__
def find_header!(header)
  find_header header, INCLUDE or raise LoadError, "missing header:\t#{header}"
end
find_header! 'godot_rb.h'
find_header! 'godot/gdextension_interface.h'

require_relative 'extension_api'
create_makefile 'godot_rb'
