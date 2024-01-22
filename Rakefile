# frozen_string_literal: true
require 'rake/extensiontask'

# Build modes and their compiler flags
# @return `{"debug" => ["flag", …], "release" => …}`
BUILD_FLAGS = {
  'debug' => %w[
  ],
  'release' => %w[
    -DNDEBUG
    -O2
  ]
}

# sh('gcc',
#    *flags,
#   …
# )

# C
Rake::ExtensionTask.new 'godot_rb' do|ext|
  # ext.config_options << '--with-foo'
  ext.source_pattern = '**/*.c'
  ext.no_native = true
end

# # Ruby #
#
# hash_files = %w[core editor].to_h do|api_type|
#   tsv = File.join BIN, "#{api_type}.hashes.tsv"
#   # Each task executes at most once
#   file tsv => :extension_api
#   [api_type, tsv]
# end
#
# extension_api = File.join 'include', 'godot', 'extension_api.json'
# # Database of method hashes
# # ```
# # Class
# # method␉12345
# # ```
# task :extension_api => [BIN, extension_api] do
#   require 'json'
#   json = JSON.load_file extension_api
#   files = hash_files.transform_values do|tsv|
#     File.open tsv, 'w'
#   rescue => e
#     warn e.full_message
#   end
#   begin
#     json['classes'].each do|klass_data|
#       files[klass_data['api_type']]&.then do|tsv|
#         tsv.puts klass_data['name']
#         klass_data['methods']&.each do|method_data|
#           method_data['hash']&.then { tsv.puts "#{method_data['name']}\t#{_1}" }
#         end
#       end
#     end
#   ensure
#     files.each_value { _1&.close }
#   end
# end


# Tasks #

# desc 'build Godot.rb for debug builds'
# task default: [BUILD_FLAGS.each_key.first, :additional_files]
# desc 'build Godot.rb for all build types'
# task all: (BUILD_FLAGS.keys << 'additional_files')
#
# multitask additional_files: hash_files.values
