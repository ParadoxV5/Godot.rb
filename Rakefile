# frozen_string_literal: true

raise NotImplementedError, 'only CRuby supported' unless 'ruby' == RUBY_ENGINE

# @return `"<architecture>/<platform>"`
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
# @return `{"src/*.c" => "bin/.o/*.o"}`
O_C = Dir[File.join SRC_DIR, '**', '*.c'].to_h {|source| [
  File.join(O_DIR, source[src_dir_size..]).tap { _1[-1] = 'o' },
  source
] }

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
    *%W[
      gcc
      -shared
      -o#{OUT_DIR}debug.lib
    ],
    *O_C.keys,
    *%W[
      -L#{RbConfig::CONFIG['libdir']}
      -L#{
        RbConfig::CONFIG['bindir'] # RubyInstaller
      }
      -l#{RbConfig::CONFIG['LIBRUBY_SO']}
    ]
  )
end

O_C.each do|name, source|
  file name => [source, O_DIR] do
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

desc "[TODO] symlink “libruby” and dependencies for <#{OS}>"
task :libruby
#TODO: Task for symlinking each dep lib

desc "[TODO] delete the intermediate directory `#{O_DIR}`"
task :mostlyclean

desc "[TODO] delete the output directories `#{O_DIR}` & `#{OUT_DIR}`"
task clean: :mostlyclean
