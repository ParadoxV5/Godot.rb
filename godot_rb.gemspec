# frozen_string_literal: true
require_relative 'lib/godot_rb/version'

Gem::Specification.new do|spec|
  spec.name = 'Godot.rb'
  spec.summary = 'CRuby in Godot Engine; see Homepage/README'
  spec.version = Godot::VERSION
  spec.author = 'ParadoxV5'
  spec.license = 'Apache-2.0'
  
  github_account = spec.author
  github = "https://github.com/#{github_account}/#{spec.name}"
  spec.metadata = {
    'homepage_uri'      => spec.homepage = github,
    'changelog_uri'     => File.join(github, 'releases'),
    'bug_tracker_uri'   => File.join(github, 'issues'),
    'funding_uri'       => "https://github.com/sponsors/#{github_account}",
    'documentation_uri' => "https://rubydoc.info/gems/#{spec.name}"
  }
  
  spec.files = Dir['**/*'].grep_v Regexp.union *%w[
    rbs_collection.yaml
    Steepfile
  ]
  spec.extensions = %w[Rakefile]
  
  spec.required_ruby_version = '~> 3.3.0'
  spec.add_runtime_dependency 'json', '~> 2.7'
  spec.add_development_dependency 'rake-compiler', '~> 1.2.0'
  if RUBY_PLATFORM.eql? 'x64-mingw-ucrt'
    spec.add_development_dependency 'mingw-make', '~> 1.0'
  end
  spec.requirements.push(
    'CRuby (not JRuby, etc.)',
    'Godot Engine ~> 4.2'
  )
end
