require 'json'
require 'erb'

EXTENSION_API = File.expand_path '../../include/godot/extension_api_with_docs.json', __FILE__
immediate_skips = 4

puts "> loading #{EXTENSION_API}"
json = JSON.load_file EXTENSION_API

puts '| processing variant sizes'
string_name_default_size = 0
build_configurations = json.delete('builtin_class_sizes').to_h do|config|
  sizes = config.fetch('sizes').tap { _1.shift immediate_skips }.to_h(&:values)
  string_name_size = sizes.fetch 'StringName'
  string_name_default_size = string_name_size if string_name_size > string_name_default_size
  [
    config.fetch('build_configuration'),
    sizes.map do|name, size|
      "godot_rb_cVariant_sizes[gdvt#{name}] = #{size};"
    end.join("\n")
  ]
end

puts '| processing variant classes'
type_has_destructors = json.delete('builtin_classes').tap do|it|
  it.pop immediate_skips
end.to_h { _1.values_at 'name', 'has_destructor' }
type_has_destructors['Object'] = false # not included in `builtin_classes`

json.delete('classes')
  .find { _1['name'] == 'OS' }
  .delete('methods')
  .find { _1['name'] == 'has_feature' }
  .fetch('hash') => hash_OS_has_feature

variant_types_c = File.expand_path '../variant_types.c', __FILE__
puts "< generating #{variant_types_c}"
File.write(
  variant_types_c,
  ERB.new(File.read "#{variant_types_c}.erb").result_with_hash(
    string_name_default_size:,
    build_configurations:,
    type_has_destructors:,
    hash_OS_has_feature:
  )
)

# files = %w[core editor].to_h { [_1, File.join(OUT, "#{_1}.rb")] }.transform_values do|path|
#   File.open path, 'w'
# rescue => e
#   warn e.full_message
# end
# begin
#   json['classes'].each do|klass_data|
#     files[klass_data['api_type']]&.then do|rb|
#       klass_data['methods']&.each do|method_data|
#         method_data['hash']&.then { _1 }
#       end
#     end
#   end
# ensure
#   files.each_value { _1&.close }
# end
