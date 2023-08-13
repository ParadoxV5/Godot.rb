# frozen_string_literal: true
require_relative 'lib/mygem/version'

Gem::Specification.new do |spec|
  spec.name = 'mygem'
  spec.summary = ''
  spec.version = MyGem::VERSION
  spec.author = 'ParadoxV5'
  spec.license = ''
  
  github_account = spec.author
  github = File.join 'https://github.com', github_account, spec.name
  spec.homepage = github
  spec.metadata = {
    'homepage_uri'      => spec.homepage,
    'source_code_uri'   => github,
    'changelog_uri'     => File.join(github, 'releases'),
    'bug_tracker_uri'   => File.join(github, 'issues'),
    'wiki_uri'          => File.join(github, 'wiki'),
    'funding_uri'       => File.join('https://github.com/sponsors', github_account),
    'documentation_uri' => File.join('https://rubydoc.info/gems', spec.name)
  }

  spec.files = Dir['**/*']
  
  spec.required_ruby_version = '>= 3'
  #spec.add_dependency 'dependent_gem', '~> 2.1.0'
end
