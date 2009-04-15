# GreenStripes

# add ../ext/greenstripes to load path
$:.unshift(File.join(File.dirname(__FILE__), '..', 'ext', 'greenstripes'))

# require C extension
require 'greenstripes'

# GreenStripes
module GreenStripes
  class Session
    def initialize(*args) # :nodoc:
    end

    def to_link
      Link.new(self)
    end
  end

  class Playlist
    def to_link
      Link.new(self)
    end

    def ==(other)
      self.to_link == other.to_link
    end
  end

  class Search
    def initialize(*args) # :nodoc:
    end

    def to_link
      Link.new(self)
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
    def to_link
      Link.new(self)
    end

    def ==(other)
      self.to_link == other.to_link
    end
  end

  class Album
    def to_link
      Link.new(self)
    end

    def ==(other)
      self.to_link == other.to_link
    end
  end

  class Track
    def to_link
      Link.new(self)
    end

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
  def to_link
    ::GreenStripes::Link.new(self)
  end
end
