# add ../ext/greenstripes to load path
$:.unshift(File.join(File.dirname(__FILE__), '..', 'ext', 'greenstripes'))

# require C extension
require 'greenstripes'

# GreenStripes
module GreenStripes
end
