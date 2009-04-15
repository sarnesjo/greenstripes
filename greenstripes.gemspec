# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{greenstripes}
  s.version = "0.1.0"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Jesper S\303\244rnesj\303\266"]
  s.date = %q{2009-04-15}
  s.email = %q{jesper@sarnesjo.org}
  s.extensions = ["ext/greenstripes/extconf.rb"]
  s.extra_rdoc_files = ["README.markdown", "LICENSE"]
  s.files = ["ext/greenstripes/extconf.rb", "ext/greenstripes/greenstripes.c", "lib/greenstripes.rb", "test/config.rb.sample", "test/test_greenstripes.rb", "README.markdown", "LICENSE"]
  s.has_rdoc = true
  s.homepage = %q{http://github.com/sarnesjo/greenstripes}
  s.rdoc_options = ["--inline-source", "--charset=UTF-8"]
  s.require_paths = ["lib"]
  s.rubygems_version = %q{1.3.1}
  s.summary = %q{Ruby bindings for libspotify}
  s.test_files = ["test/config.rb.sample", "test/test_greenstripes.rb"]

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 2

    if Gem::Version.new(Gem::RubyGemsVersion) >= Gem::Version.new('1.2.0') then
    else
    end
  else
  end
end
