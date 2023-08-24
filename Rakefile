# frozen_string_literal: true

raise NotImplementedError, 'only CRuby supported' unless 'ruby' == RUBY_ENGINE

# @return "<architecture>/<platform>"
# @see RbConfig::CONFIG
OS = File.join(
  case host = RbConfig::CONFIG['host_cpu']
  when /x64/
    'x86_64'
  else
    raise NotImplementedError, "host_cpu `#{host}` not supported"
  end,
  case host = RbConfig::CONFIG['host_os']
  when /mingw32/
    'windows'
  else
    raise NotImplementedError, "host_os `#{host}` not supported"
  end
)
# Output directory
# @return "bin/<architecture>/<platform>"
# @see OS
OUT_DIR = File.join('bin', OS)

desc "[TODO] compile Godot.rb & symlink libs for <#{OS}>"
task :default

desc "[TODO] compile `godot_rb.c` for <#{OS}>"
task :main

#TODO: Task for symlinking each dep lib

desc "[TODO] delete the directory `#{OUT_DIR}`"
task :clean
