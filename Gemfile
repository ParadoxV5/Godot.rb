# frozen_string_literal: true
source 'https://rubygems.org'
ruby '~> 3.0', engine: 'ruby', engine_version: '~> 3.0'

gem 'rake', '~> 13.0.0'

# Development Apps
group :development do
  group :type_check do
    gem 'rbs', github: 'ruby/rbs', ref: '1b9c8a4', require: false
    gem 'steep', '~> 1.5.0', require: false
  end
  group :documentation do
    gem 'yard', '~> 0.9.0', require: false
    gem 'commonmarker', '~> 0.23.0', require: false
  end
end
