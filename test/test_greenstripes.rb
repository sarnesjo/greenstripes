# add ../lib to the load path
$:.unshift(File.join(File.dirname(__FILE__), '..', 'lib'))

# add . to the load path
$:.unshift(File.dirname(__FILE__))

require 'test/unit'
require 'greenstripes'
require 'config'

class TestGreenStripes < Test::Unit::TestCase
  def setup
    # TODO: it would actually be better if this was run just once, before all tests
    @session = GreenStripes::Session.new(APPLICATION_KEY, 'GreenStripes', 'tmp', 'tmp')
    @session.login(USERNAME, PASSWORD)
    @session.process_events until @session.connection_state == GreenStripes::ConnectionState::LOGGED_IN

    # TODO: it would be better if we created a designated test playlist here
    playlist_container = @session.playlist_container
    @session.process_events until playlist_container.num_playlists > 0
    @playlist = playlist_container.playlist(0)
    @session.process_events until @playlist.loaded?
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

  def test_playlist
    assert_not_nil(@playlist.name)
    assert_not_nil(@playlist.owner)
    assert_not_nil(@playlist.collaborative?)
    assert_not_equal(0, @playlist.num_tracks)
    assert_not_nil(@playlist.track(0))
  end

  def test_search
    query = 'a'
    search = GreenStripes::Search.new(@session, query, 0, 1)
    @session.process_events until search.loaded?
    assert_equal(GreenStripes::Error::OK, search.error)
    assert_equal(query, search.query)
    assert_not_nil(search.did_you_mean)
    assert_not_equal(0, search.num_artists)
    assert_not_nil(search.artist(0))
    assert_not_equal(0, search.num_albums)
    assert_not_nil(search.album(0))
    assert_not_equal(0, search.num_tracks)
    assert_not_nil(search.track(0))
  end

  def test_artist_browse
    track = @playlist.track(0)
    @session.process_events until track.loaded?
    artist_browse = GreenStripes::ArtistBrowse.new(@session, track.artist)
    @session.process_events until artist_browse.loaded?
    assert_equal(GreenStripes::Error::OK, artist_browse.error)
    assert_same(track.artist, artist_browse.artist)
    assert_not_equal(0, artist_browse.num_tracks)
    assert_not_nil(artist_browse.track(0))
    assert_not_equal(0, artist_browse.num_similar_artists)
    assert_not_nil(artist_browse.similar_artist(0))
    assert_not_nil(artist_browse.biography)
  end

  def test_album_browse
    track = @playlist.track(0)
    @session.process_events until track.loaded?
    album_browse = GreenStripes::AlbumBrowse.new(@session, track.album)
    @session.process_events until album_browse.loaded?
    assert_equal(GreenStripes::Error::OK, album_browse.error)
    assert_same(track.album, album_browse.album)
    assert_same(track.album.artist, album_browse.artist)
    assert_not_equal(0, album_browse.num_tracks)
    assert_not_nil(album_browse.track(0))
    assert_not_equal(0, album_browse.num_copyrights)
    assert_not_nil(album_browse.copyright(0))
    assert_not_nil(album_browse.review)
  end

  def test_link_from_objects
    track = @playlist.track(0)
    search = GreenStripes::Search.new(@session, 'a', 0, 1)
    @session.process_events until track.loaded? and search.loaded?
    [@playlist, search, track.artist(0), track.album, track].each do |obj|
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
