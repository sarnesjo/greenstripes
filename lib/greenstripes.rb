# GreenStripes

# add ../ext/greenstripes to load path
$:.unshift(File.join(File.dirname(__FILE__), '..', 'ext', 'greenstripes'))

# require C extension
require 'greenstripes'

# GreenStripes
module GreenStripes
  class FakeArray
    include Enumerable

    def initialize(target, objects)
      @target = target
      @num_objects_sym = "num_#{objects}".to_sym
      @object_sym = objects.to_s.chop.to_sym
    end

    def size
      @target.send(@num_objects_sym)
    end

    def [](index)
      @target.send(@object_sym, index)
    end

    def each
      self.size.times do |i|
        yield self[i]
      end
    end
  end

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

    def artists
      FakeArray.new(self, :artists)
    end

    def albums
      FakeArray.new(self, :albums)
    end

    def tracks
      FakeArray.new(self, :tracks)
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
