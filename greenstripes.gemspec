Gem::Specification.new do |s|
  s.name              = 'greenstripes'
  s.version           = '0.0.0'
  s.date              = '2010-01-31'

  s.summary           = 'Ruby bindings for libspotify'

  s.author            = "Jesper S\303\244rnesj\303\266"
  s.email             = 'jesper@sarnesjo.org'
  s.homepage          = 'http://github.com/sarnesjo/greenstripes'

  s.files             = %w{lib/greenstripes.rb} # TODO
  s.test_files        = %w{} # TODO
  s.extra_rdoc_files  = %w{} # TODO

  s.has_rdoc          = true

  s.add_dependency('ffi', '>= 0.5.4')
end
