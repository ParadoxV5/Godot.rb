# frozen_string_literal: true
require 'mkmf'

find_header 'ruby.gdextension.h', 'include'
create_makefile 'ruby.gdextension', 'src'
