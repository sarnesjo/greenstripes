# add ../ext/greenstripes to load path
$:.unshift(File.join(File.dirname(__FILE__), '..', 'ext', 'greenstripes'))

# require C extension
require 'greenstripes'

# Module
class Module
  def has_link
    module_eval <<-code
    def to_link
      ::GreenStripes::Link.new(self)
    end
    code
  end
end

# GreenStripes
module GreenStripes
  class Session
    def initialize(*args) # :nodoc:
    end
  end

  class Playlist
    has_link

    def ==(other)
      self.to_link == other.to_link
    end
  end

  class Search
    has_link

    def initialize(*args) # :nodoc:
    end

    def ==(other)
      self.to_link == other.to_link
    end
  end

  class ArtistBrowse
    def initialize(*args) # :nodoc:
    end
  end

  class AlbumBrowse
    def initialize(*args) # :nodoc:
    end
  end

  class Artist
    has_link

    def ==(other)
      self.to_link == other.to_link
    end
  end

  class Album
    has_link

    def ==(other)
      self.to_link == other.to_link
    end
  end

  class Track
    has_link

    def ==(other)
      self.to_link == other.to_link
    end
  end

  class Link
    def initialize(*args) # :nodoc:
    end

    def ==(other)
      self.to_s == other.to_s
    end
  end
end

class String
  has_link
end
