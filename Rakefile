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

# RubyInstaller has its stuff in its own self-contained folder
# @return [boolish] the `RubyInstaller` module for Windows RubyInstaller, or `nil` for non-RubyInstaller
RubyInstaller ||= nil
warn 'non-RubyInstaller2 not supported', category: :experimental unless RubyInstaller

# C sources directory
# @return `"src"`
SRC = 'src'
# Output root directory
# @return `"build"`
BIN = 'build'
# Output directory
# @return `"build/<architecture>/<platform>/"`
# @see OS
OUT = File.join(BIN, OS, '')

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

rubyarchdir = RbConfig::CONFIG['rubyarchdir']
# Assume `rubyarchdir` always starts with `prefix`
directory out_archdir =
  File.join(OUT, rubyarchdir[RbConfig::CONFIG['prefix'].size..] || '')
out_enc_dir = File.join(out_archdir, 'enc')
file_create out_enc_dir => out_archdir

# symlinks of `libruby` & dependencies in the output directory and their originals
# @return `{"build/<architecture>/<platform>/<lib.so>" => "/<path>/<to>/<lib.so>", …}`
LIBS = {
  File.join(OUT, LIBRUBY_NAME) => File.join(LIBRUBY_DIR, LIBRUBY_NAME),
  out_enc_dir => File.join(rubyarchdir, 'enc')
}
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
C__O_NAMES = Dir[File.join(SRC, '**', '*.c'), sort: false].to_h {|source| [
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
BUILD_FLAGS.map do|build, flags|
  out = File.join(OUT, "#{build}.lib")
  o_dir_name = ".#{build}.o"
  directory o_dir = File.join(OUT, o_dir_name)
  
  # Compile Object Binaries (`.o`)
  C__O_NAMES.map do|source, o_name|
    o0 = File.join(o_dir, o_name)
    directory o0_dir = File.dirname(o0)
    file o0 => [source, o0_dir] do
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
    o0
  end => o
  
  # Link Shared Library (`.so`)
  file out => o do
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
  multitask build => [*o, out]
  
  o_dir
end => o_dirs # ["…/.debug.o", …]`

desc 'compile Godot.rb for debug builds & symlink libs'
task default: [BUILD_FLAGS.each_key.first, :libruby]
desc 'compile Godot.rb for all build types & symlink libs'
task all: (BUILD_FLAGS.keys << 'libruby')

desc "symlink `libruby` and dependencies"
multitask libruby: LIBS.keys
LIBS.each {|symlink, lib| file_create symlink => OUT do
  ln_sf lib, symlink
end }

desc "delete the intermediate directories such as `#{o_dirs.first}`"
task :mostlyclean do
  rm_rf o_dirs
end
desc "delete the output directory `#{OUT}`"
task :clean do
  rm_rf OUT
end
