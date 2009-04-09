# add ../lib to the load path
$:.unshift(File.join(File.dirname(__FILE__), '..', 'lib'))

# add . to the load path
$:.unshift(File.dirname(__FILE__))

require 'test/unit'
require 'greenstripes'
require 'config'

class TestGreenStripes < Test::Unit::TestCase
  def setup
    @session = GreenStripes::Session.new(APPLICATION_KEY, 'GreenStripes', 'tmp', 'tmp')
    @session.login(USERNAME, PASSWORD)
    @session.process_events until @session.connection_state == GreenStripes::ConnectionState::LOGGED_IN
  end

  def teardown
    @session.logout
    @session.process_events until @session.connection_state == GreenStripes::ConnectionState::LOGGED_OUT
  end

  def test_user
    user = @session.user
    @session.process_events until user.loaded?
    assert_not_nil(user.display_name)
    assert_not_nil(user.canonical_name)
  end

  def test_playlists
    playlist_container = @session.playlist_container

    # PlaylistContainer#num_playlists will return 0 just after login
    # this is a stupid workaround, since a user could have 0 playlists...
    @session.process_events
    assert_not_equal(0, playlist_container.num_playlists)

    playlist_container.num_playlists.times do |i|
      playlist = playlist_container.playlist(i)
      @session.process_events until playlist.loaded?
      assert_not_nil(playlist.name)
      assert_not_nil(playlist.owner)
      assert_not_nil(playlist.collaborative?)
      assert_not_nil(playlist.track(0)) if playlist.num_tracks > 0
    end
  end

  def test_search
    search = GreenStripes::Search.new(@session, 'a', 0, 1)
    @session.process_events until search.loaded?
    assert_not_nil(search.artists)
    assert_not_equal(0, search.artists.size)
    assert_not_nil(search.albums)
    assert_not_equal(0, search.albums.size)
    assert_not_nil(search.tracks)
    assert_not_equal(0, search.tracks.size)
  end

  def test_link_from_objects
    playlist = @session.playlist_container.playlist(0)
    track = playlist.track(0)
    search = GreenStripes::Search.new(@session, 'a', 0, 1)
    @session.process_events until track.loaded? and search.loaded?
    [track.artist(0), track.album, track, playlist, search].each do |obj|
      assert_kind_of(GreenStripes::Link, GreenStripes::Link.new(obj))
      assert_kind_of(GreenStripes::Link, obj.to_link)
    end
  end

  def test_link_from_strings
    %w{spotify:artist:3mvkWMe6swnknwscwvGCHO
       spotify:album:57SkIVhE1QfVnShjmvKw3O
       spotify:track:3DTrAmImiol2ugB5wsqFcx
       spotify:user:sarnesjo:playlist:3nCwOiwDiZtv9xsuEIFw4q
       spotify:search:a}.each do |str|
      assert_kind_of(GreenStripes::Link, GreenStripes::Link.new(str))
      assert_kind_of(GreenStripes::Link, str.to_link)
    end
  end
end
