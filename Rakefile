# frozen_string_literal: true

warn 'only CRuby supported', category: :experimental if 'ruby' != RUBY_ENGINE

# @return `"<architecture>/<platform>"`
# @see RbConfig::CONFIG
OS = File.join(
  case target = RbConfig::CONFIG['target_cpu']
  when 'x64'
    'x86_64'
  else
    warn "target_cpu `#{target}` not supported", category: :experimental
    ".#{target}"
  end,
  case target = RbConfig::CONFIG['target_os']
  when 'mingw32'
    'windows'
  else
    warn "target_os `#{target}` not supported", category: :experimental
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
  warn 'non-RubyInstaller2 not supported', category: :experimental
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

puts "architecture/platform:\t#{OS}", '' unless $VERBOSE.nil?

desc 'compile Godot.rb for debug builds & symlink libs'
task default: %i[c libruby]
desc 'compile Godot.rb for both build types & symlink libs'
task all: [OUT_RELEASE, :default]
desc 'compile Godot.rb for debug builds'
task c: OUT_DEBUG

{
  OUT_DEBUG => %w[
  ],
  OUT_RELEASE => %w[
    -DNDEBUG
    -O2
  ]
}.each {|name, flags| file name do
  Rake::Task[:compile].invoke(name, *flags)
end }

l_libruby_dir = "-L#{LIBRUBY_DIR}"
l_libruby_file = "-l:#{LIBRUBY_FILE}"
multitask :compile, %i[name] => (O_C.keys << OUT_DIR) do|_, args|
  sh('gcc',
    *args.extras,
    "-o#{args.name}",
    '-shared',
    *O_C.keys,
    l_libruby_dir,
    l_libruby_file
  )
end

i_h_dir = "-I#{RbConfig::CONFIG['rubyhdrdir']}"
i_arch_h_dir = "-I#{RbConfig::CONFIG['rubyarchhdrdir']}"
O_C.each do|name, source|
  file name, %i[_] => [O_DIR, source] do|_, args|
    sh('gcc',
      *args.extras,
      "-o#{name}",
      '-c',
      '-Iinclude',
       i_h_dir,
       i_arch_h_dir,
      source
    )
  end
end

desc "symlink `libruby` and dependencies"
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
