require 'rubygems'
require 'rake'

begin
  require 'jeweler'
  Jeweler::Tasks.new do |gem|
    gem.name = "greenstripes"
    gem.summary = "Ruby bindings for libspotify"
    gem.email = "jesper@sarnesjo.org"
    gem.homepage = "http://github.com/sarnesjo/greenstripes"
    gem.authors = ["Jesper Särnesjö"]

    gem.files = %w{ext/greenstripes/extconf.rb ext/greenstripes/greenstripes.c lib/greenstripes.rb}
    gem.test_files = %w{test/config.rb.sample test/test_greenstripes.rb}
    gem.extensions = %w{ext/greenstripes/extconf.rb}
    gem.extra_rdoc_files = %w{ext/greenstripes/greenstripes.c}

    # gem is a Gem::Specification... see http://www.rubygems.org/read/chapter/20 for additional settings
  end
rescue LoadError
  puts "Jeweler not available. Install it with: sudo gem install technicalpickles-jeweler -s http://gems.github.com"
end

require 'rake/testtask'
Rake::TestTask.new(:test) do |test|
  test.libs << 'lib' << 'test'
  test.pattern = 'test/**/test_*.rb'
  test.verbose = false
end

task :default => :test
