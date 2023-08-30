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

# C sources directory
# @return `"src"`
SRC = 'src'
# Output root directory
# @return `"bin"`
BIN = 'bin'
# Output directory
# @return `"bin/<architecture>/<platform>/"`
# @see OS
directory OUT = File.join(BIN, OS, '')

begin
  require 'ruby_installer'
rescue LoadError
  warn 'non-RubyInstaller2 not supported', category: :experimental
  # RubyInstaller has its stuff in its own self-contained folder
  # @return the `RubyInstaller` module for Windows RubyInstaller, or `nil` for non-RubyInstaller
  RubyInstaller = nil
end

# @return `libruby.so`’s directory: `"/<path>/<to>"`
# @see LIBRUBY_NAME
LIBRUBY_DIR = RbConfig::CONFIG[RubyInstaller ? 'bindir' : 'libdir']
# @return `libruby.so`, the file name (dependant on platform and possibly architecture)
# @see LIBRUBY_DIR
LIBRUBY_NAME = RbConfig::CONFIG['LIBRUBY_SO']
i_h_dir = "-I#{RbConfig::CONFIG['rubyhdrdir']}"
i_arch_h_dir = "-I#{RbConfig::CONFIG['rubyarchhdrdir']}"
l_libruby_dir = "-L#{LIBRUBY_DIR}"
l_libruby_name = "-l:#{LIBRUBY_NAME}"

# symlinks of `libruby` & dependencies in the output directory and their originals
# @return `{"bin/<architecture>/<platform>/<lib.so>" => "/<path>/<to>/<lib.so>", …}`
LIBS = {File.join(OUT, LIBRUBY_NAME) => File.join(LIBRUBY_DIR, LIBRUBY_NAME)}
if RubyInstaller
  %w[libgmp-10.dll].each do|lib|
    LIBS[File.join(OUT, lib)] =
      File.join(LIBRUBY_DIR, 'ruby_builtin_dlls', lib)
  end
end

rake_output_message "<architecture/platform>\t#{OS}"

src_dir_size = SRC.size
# C sources and file names of their corresponding  intermediate `.o`s
# @return `{"src/*.c" => "*.o"}`
# @see SRC
C__O_NAMES = Dir[File.join(SRC, '**', '*.c')].to_h {|source| [
  source,
  source[src_dir_size..].tap { _1[-1] = 'o' }
] }

# Build modes and their compiler flags
# @return `{'debug' => ["flag", …], 'release' => …}`
BUILD_FLAGS = {
  'debug' => %w[
  ],
  'release' => %w[
    -DNDEBUG
    -O2
  ]
}
BUILD_FLAGS.to_h do|build, flags|
  out = File.join(OUT, "#{build}.lib")
  o_dir_name = ".#{build}.o"
  directory o_dir = File.join(OUT, o_dir_name)
  
  # Compile Object Binaries (`.o`)
  C__O_NAMES.map {|source, o_name| File.join(o_dir, o_name).tap do|o0|
    file o0 => o_dir do
      sh('gcc',
         *flags,
         '-c',
         "-o#{o0}",
         '-fPIC',
         '-Iinclude',
         i_h_dir,
         i_arch_h_dir,
         source
      )
    end
  end } => o
  
  # Link Shared Library (`.so`)
  file out => o_dir_name
  multitask o_dir_name => o do
    sh('gcc',
       *flags,
       '-shared',
       '-fPIC',
       "-o#{out}",
       *o,
       l_libruby_dir,
       l_libruby_name
    )
  end
  
  [o_dir, out]
end => o_dir__out # {"…/.debug.o" => "…/debug.lib", …}`

desc 'compile Godot.rb for debug builds & symlink libs'
task default: [o_dir__out.each_value.first, :libruby]
desc 'compile Godot.rb for all build types & symlink libs'
task all: %i[c libruby]
desc 'compile Godot.rb for all build types'
multitask c: o_dir__out.values

desc "symlink `libruby` and dependencies"
multitask libruby: LIBS.keys
LIBS.each {|symlink, lib| file symlink => OUT do
  ln_sf lib, symlink
end }

desc "delete the intermediate directories such as `#{o_dir__out.each_key.first}`"
task :mostlyclean do
  rm_rf o_dir__out.keys
end
desc "delete the output directory `#{OUT}`"
task :clean do
  rm_rf OUT
end
