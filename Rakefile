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

# Output lib for debug builds
# @return `"bin/<architecture>/<platform>/debug.lib"`
# @see OUT_DIR
OUT_DEBUG = File.join(OUT_DIR, 'debug.lib')
# Output lib for release builds
# @return `"bin/<architecture>/<platform>/release.lib"`
# @see OUT_DIR
OUT_RELEASE = File.join(OUT_DIR, 'release.lib')

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
# @see LIBRUBY_FILE
LIBRUBY_DIR = RbConfig::CONFIG[RubyInstaller ? 'bindir' : 'libdir']
# @return `libruby.so`, the file name (dependant on platform and possibly architecture)
# @see LIBRUBY_DIR
LIBRUBY_FILE = RbConfig::CONFIG['LIBRUBY_SO']
# `libruby` & dependencies in the output directory and their original locations
# @return `{"bin/<architecture>/<platform>/<lib.so>" => "/<path>/<to>/<lib.so>", …}`
LIBS = {File.join(OUT_DIR, LIBRUBY_FILE) => File.join(LIBRUBY_DIR, LIBRUBY_FILE)}
if RubyInstaller
  %w[libgmp-10.dll].each do|lib|
    LIBS[File.join(OUT_DIR, lib)] =
      File.join(LIBRUBY_DIR, 'ruby_builtin_dlls', lib)
  end
end

desc "compile Godot.rb & symlink libs for <#{OS}>"
task default: %i[debug libruby]
desc "compile in both modes & symlink libs for <#{OS}>"
task all: [OUT_RELEASE, :default]
desc "compile `godot_rb.c` for <#{OS}>"
task debug: OUT_DEBUG

{
  OUT_DEBUG => %w[
  ],
  OUT_RELEASE => %w[
    -DNDEBUG
    -O2
  ]
}.each {|name, flags| file name do
  Rake::Task[:c].invoke(name, *flags)
end }

multitask :c, %i[name] => (O_C.keys << OUT_DIR) do|_, args|
  sh('gcc',
    *args.extras,
    "-o#{args.name}",
    '-shared',
    *O_C.keys,
    "-L#{LIBRUBY_DIR}",
    "-l:#{RbConfig::CONFIG['LIBRUBY_SO']}"
  )
end

O_C.each do|name, source|
  file name, %i[_] => [O_DIR, source] do|_, args|
    sh('gcc',
      *args.extras,
      "-o#{name}",
      '-c',
      '-Iinclude',
      "-I#{RbConfig::CONFIG['rubyhdrdir']}",
      "-I#{RbConfig::CONFIG['rubyarchhdrdir']}",
      source
    )
  end
end

desc "symlink `libruby` and dependencies for <#{OS}>"
multitask libruby: LIBS.keys

LIBS.each do|name, source|
  file(name => [OUT_DIR, source]) { File.symlink(source, name) }
end

require 'fileutils'

desc "delete the intermediate directory `#{O_DIR}`"
task :mostlyclean do
  FileUtils.remove_dir O_DIR
end

desc "delete out directories `#{O_DIR}` & `#{OUT_DIR}`"
task clean: :mostlyclean do
  FileUtils.remove_dir OUT_DIR
end
