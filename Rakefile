# frozen_string_literal: true

warn 'only CRuby supported' unless 'ruby' == RUBY_ENGINE

# @return `"<architecture>/<platform>"`
# @see RbConfig::CONFIG
OS = File.join(
  case target = RbConfig::CONFIG['target_cpu']
  when 'x64'
    'x86_64'
  else
    warn "target_cpu `#{target}` not supported"
    ".#{target}"
  end,
  case target = RbConfig::CONFIG['target_os']
  when 'mingw32'
    'windows'
  else
    warn "target_os `#{target}` not supported"
    ".#{target}"
  end
)

# Output root directory
# @return `"bin"`
BIN_DIR = 'bin'
# Output directory
# @return `"bin/<architecture>/<platform>/"`
# @see OS
directory OUT_DIR = File.join(BIN_DIR, OS, '')
# Intermediate output directory for `.o`s
# @return `"bin/.o/"`
directory O_DIR = File.join(BIN_DIR, '.o', '')

# C sources directory
# @return `"src"`
SRC_DIR = 'src'
src_dir_size = SRC_DIR.size
# C intermediate `.o`s and their corresponding sources
# @return `{"bin/.o/*.o" => "src/*.c"}`
O_C = Dir[File.join(SRC_DIR, '**', '*.c')].to_h {|source| [
  File.join(O_DIR, source[src_dir_size..]).tap { _1[-1] = 'o' },
  source
] }

begin
  require 'ruby_installer'
rescue LoadError
  warn 'non-RubyInstaller2 not supported'
  # RubyInstaller has its stuff in its own self-contained folder
  RubyInstaller = nil
end

# @return `libruby.so`’s directory: `"/<path>/<to>"`
LIBRUBY_DIR = RbConfig::CONFIG[RubyInstaller ? 'bindir' : 'libdir']
# @return `libruby.so`, the file name (dependant on platform and possibly architecture)
LIBRUBY_SO = RbConfig::CONFIG['LIBRUBY_SO']
# `libruby` & dependencies in the output directory and their original locations
# @return `{"bin/<architecture>/<platform>/<lib.so>" => "/<path>/<to>/<lib.so>", …}`
LIBS = {File.join(OUT_DIR, LIBRUBY_SO) => File.join(LIBRUBY_DIR, LIBRUBY_SO)}
if RubyInstaller
  %w[libgmp-10.dll].each do|lib|
    LIBS[File.join(OUT_DIR, lib)] =
      File.join(LIBRUBY_DIR, 'ruby_builtin_dlls', lib)
  end
end

# Build modes and their compiler flags
# @return `{'debug' => ["flag", …], 'release' => …}`
BUILD_FLAGS = {
  'debug' => %w[],
  'release' => %w[
    -DNDEBUG
    -O2
  ]
}
#TODO: release mode builds

desc "[WIP] compile Godot.rb & symlink libs for <#{OS}>"
task default: %i[c libruby]

desc "compile `godot_rb.c` for <#{OS}>"
multitask c: (O_C.keys << OUT_DIR) do
  sh(
    'gcc',
    "-o#{OUT_DIR}debug.lib",
    '-shared',
    *O_C.keys,
    "-L#{LIBRUBY_DIR}",
    "-l:#{RbConfig::CONFIG['LIBRUBY_SO']}"
  )
end

O_C.each do|name, source|
  file name => [O_DIR, source] do
    sh *%W[
      gcc
      -o#{name}
      -c
      -Iinclude
      -I#{RbConfig::CONFIG['rubyhdrdir']}
      -I#{RbConfig::CONFIG['rubyarchhdrdir']}
      #{source}
    ]
  end
end

desc "[TODO] symlink `libruby` and dependencies for <#{OS}>"
multitask libruby: LIBS.keys

LIBS.each do|name, source|
  file(name => [OUT_DIR, source]) { File.symlink(source, name) }
end

desc "[TODO] delete the intermediate directory `#{O_DIR}`"
task :mostlyclean

desc "[TODO] delete the output directories `#{O_DIR}` & `#{OUT_DIR}`"
task clean: :mostlyclean
