#include <ruby.h>
#include <spotify/api.h>

// modules and classes

static VALUE module_greenstripes;

static VALUE module_error;
static VALUE module_connection_state;
static VALUE module_link_type;

static VALUE class_session;
static VALUE class_user;
static VALUE class_playlist_container;
static VALUE class_playlist;
static VALUE class_search;
static VALUE class_artist_browse;
static VALUE class_album_browse;
static VALUE class_artist;
static VALUE class_album;
static VALUE class_track;
static VALUE class_link;

// callbacks

static void logged_in_callback(sp_session *session, sp_error error)
{
  //fprintf(stderr, "logged_in_callback: %s\n", sp_error_message(error));
}

static void logged_out_callback(sp_session *session)
{
  //fprintf(stderr, "logged_out_callback\n");
}

static void metadata_updated_callback(sp_session *session)
{
  //fprintf(stderr, "metadata_updated_callback\n");
}

static void connection_error_callback(sp_session *session, sp_error error)
{
  //fprintf(stderr, "connection_error_callback: %s\n", sp_error_message(error));
}

static void message_to_user_callback(sp_session *session, const char *message)
{
  //fprintf(stderr, "message_to_user_callback: %s\n", message);
}

static void notify_main_thread_callback(sp_session *session)
{
  //fprintf(stderr, "notify_main_thread_callback\n");
}

static void log_message_callback(sp_session *session, const char *data)
{
  //fprintf(stderr, "log_message_callback: %s\n", data);
}

static void search_complete_callback(sp_search *result, void *userdata)
{
  //fprintf(stderr, "search_complete_callback\n");
}

static void artist_browse_complete_callback(sp_artistbrowse *result, void *userdata)
{
  //fprintf(stderr, "artist_browse_complete_callback\n");
}

static void album_browse_complete_callback(sp_albumbrowse *result, void *userdata)
{
  //fprintf(stderr, "album_browse_complete_callback\n");
}

/*
 * call-seq: Session.new(application_key, user_agent, cache_location, settings_location) -> session or nil
 *
 * Returns a new session.
 */
static VALUE session_new(VALUE klass, VALUE application_key, VALUE user_agent, VALUE cache_location, VALUE settings_location)
{
  // TODO: session callbacks should not be hardcoded
  sp_session_callbacks callbacks =
  {
    logged_in_callback,
    logged_out_callback,
    metadata_updated_callback,
    connection_error_callback,
    message_to_user_callback,
    notify_main_thread_callback,
    NULL, // music_delivery
    NULL, // play_token_lost
    log_message_callback
  };

  sp_session_config config =
  {
    SPOTIFY_API_VERSION,
    StringValuePtr(cache_location),
    StringValuePtr(settings_location),
    RSTRING_PTR(application_key),
    RSTRING_LEN(application_key),
    StringValuePtr(user_agent),
    &callbacks,
    NULL
  };

  sp_session *session = NULL;
  sp_error error = sp_session_init(&config, &session);

  // TODO: handle error in a better way?
  if(error != SP_ERROR_OK)
  {
    fprintf(stderr, "error: %s\n", sp_error_message(error));
    return Qnil;
  }

  VALUE session_value = Data_Wrap_Struct(class_session, NULL, NULL, session);
  VALUE argv[4] = {application_key, user_agent, cache_location, settings_location};
  rb_obj_call_init(session_value, 4, argv);
  return session_value;
}

/*
 * call-seq: session.login(username, password) -> error
 *
 * Attempts to log in with the given username and password. Returns one of the
 * constants defined in Error.
 */
static VALUE session_login(VALUE self, VALUE username, VALUE password)
{
  sp_session *session;
  Data_Get_Struct(self, sp_session, session);
  sp_error error = sp_session_login(session, StringValuePtr(username), StringValuePtr(password));
  // TODO: throw an exception instead?
  return INT2FIX(error);
}

/*
 * call-seq: session.logout -> error
 *
 * Attempts to log out. Returns one of the constants defined in Error.
 */
static VALUE session_logout(VALUE self)
{
  sp_session *session;
  Data_Get_Struct(self, sp_session, session);
  sp_error error = sp_session_logout(session);
  // TODO: throw an exception instead?
  return INT2FIX(error);
}

/*
 * call-seq: session.connection_state -> state
 *
 * Returns one of the constants defined in ConnectionState.
 */
static VALUE session_connection_state(VALUE self)
{
  sp_session *session;
  Data_Get_Struct(self, sp_session, session);
  int state = sp_session_connectionstate(session);
  return INT2FIX(state);
}

/*
 * call-seq: session.process_events -> fixnum
 *
 * Makes the session process any pending events. Returns the number of
 * milliseconds until you should call this method again.
 */
static VALUE session_process_events(VALUE self)
{
  sp_session *session;
  Data_Get_Struct(self, sp_session, session);
  int next_timeout;
  sp_session_process_events(session, &next_timeout);
  return INT2FIX(next_timeout);
}

/*
 * call-seq: session.user -> user or nil
 *
 * Returns the user for the session.
 */
static VALUE session_user(VALUE self)
{
  sp_session *s;
  Data_Get_Struct(self, sp_session, s);
  sp_user *user = NULL;
  user = sp_session_user(s);
  return user ? Data_Wrap_Struct(class_user, NULL, NULL, user) : Qnil;
}

/*
 * call-seq: session.playlist_container -> playlist_container or nil
 *
 * Returns the playlist container for the session.
 */
static VALUE session_playlist_container(VALUE self)
{
  sp_session *s;
  Data_Get_Struct(self, sp_session, s);
  sp_playlistcontainer *pc = NULL;
  pc = sp_session_playlistcontainer(s);
  return pc ? Data_Wrap_Struct(class_playlist_container, NULL, NULL, pc) : Qnil;
}

/*
 * call-seq: user.loaded? -> true or false
 *
 * Returns true if the user is loaded, false otherwise.
 */
static VALUE user_loaded(VALUE self)
{
  sp_user *user;
  Data_Get_Struct(self, sp_user, user);
  return sp_user_is_loaded(user) ? Qtrue : Qfalse;
}

/*
 * call-seq: user.canonical_name -> string or nil
 *
 * Returns the user's canonical name.
 */
static VALUE user_canonical_name(VALUE self)
{
  sp_user *user;
  Data_Get_Struct(self, sp_user, user);
  const char *name = sp_user_canonical_name(user);
  return name ? rb_str_new2(name) : Qnil;
}

/*
 * call-seq: user.display_name -> string or nil
 *
 * Returns the user's display name, or the canonical name if the display name is
 * not known.
 */
static VALUE user_display_name(VALUE self)
{
  sp_user *user;
  Data_Get_Struct(self, sp_user, user);
  const char *name = sp_user_display_name(user);
  return name ? rb_str_new2(name) : Qnil;
}

/*
 * call-seq: playlist_container.num_playlists -> fixnum or nil
 *
 */
static VALUE playlist_container_num_playlists(VALUE self)
{
  sp_playlistcontainer *playlistcontainer;
  Data_Get_Struct(self, sp_playlistcontainer, playlistcontainer);
  int n = sp_playlistcontainer_num_playlists(playlistcontainer);
  return (n >= 0) ? INT2FIX(n) : Qnil;
}

/*
 * call-seq: playlist_container.playlist(index) -> playlist or nil
 *
 */
static VALUE playlist_container_playlist(VALUE self, VALUE index)
{
  sp_playlistcontainer *playlistcontainer;
  Data_Get_Struct(self, sp_playlistcontainer, playlistcontainer);
  int i = FIX2INT(index);
  sp_playlist *playlist = NULL;
  playlist = sp_playlistcontainer_playlist(playlistcontainer, i);
  return playlist ? Data_Wrap_Struct(class_playlist, NULL, NULL, playlist) : Qnil;
}

/*
 * call-seq: playlist_container.add_new_playlist(name) -> error
 *
 */
static VALUE playlist_container_add_new_playlist(VALUE self, VALUE name)
{
  sp_playlistcontainer *pc;
  Data_Get_Struct(self, sp_playlistcontainer, pc);
  sp_playlist *p = sp_playlistcontainer_add_new_playlist(pc, StringValuePtr(name));
  return p ? Data_Wrap_Struct(class_playlist, NULL, NULL, p) : Qnil;
}

/*
 * call-seq: playlist_container.add_playlist(link) -> error
 *
 */
static VALUE playlist_container_add_playlist(VALUE self, VALUE link)
{
  sp_playlistcontainer *pc;
  Data_Get_Struct(self, sp_playlistcontainer, pc);
  sp_link *l;
  Data_Get_Struct(link, sp_link, l);
  sp_playlist *p = sp_playlistcontainer_add_playlist(pc, l);
  return p ? Data_Wrap_Struct(class_playlist, NULL, NULL, p) : Qnil;
}

/*
 * call-seq: playlist_container.remove_playlist(index) -> error
 *
 */
static VALUE playlist_container_remove_playlist(VALUE self, VALUE index)
{
  sp_playlistcontainer *pc;
  Data_Get_Struct(self, sp_playlistcontainer, pc);
  sp_error e = sp_playlistcontainer_remove_playlist(pc, FIX2INT(index));
  return INT2FIX(e);
}

/*
 * call-seq: playlist_container.move_playlist(index, new_position) -> error
 *
 */
static VALUE playlist_container_move_playlist(VALUE self, VALUE indices, VALUE new_position)
{
  sp_playlistcontainer *pc;
  Data_Get_Struct(self, sp_playlistcontainer, pc);
  sp_error e = sp_playlistcontainer_move_playlist(pc, FIX2INT(index), FIX2INT(new_position));
  return INT2FIX(e);
}

/*
 * call-seq: playlist.loaded? -> true or false
 *
 * Returns true if the playlist is loaded, false otherwise.
 */
static VALUE playlist_loaded(VALUE self)
{
  sp_playlist *playlist;
  Data_Get_Struct(self, sp_playlist, playlist);
  return sp_playlist_is_loaded(playlist) ? Qtrue : Qfalse;
}

/*
 * call-seq: playlist.name -> string or nil
 *
 * Returns the playlist's name.
 */
static VALUE playlist_name(VALUE self)
{
  sp_playlist *playlist;
  Data_Get_Struct(self, sp_playlist, playlist);
  const char *name = sp_playlist_name(playlist);
  return name ? rb_str_new2(name) : Qnil;
}

/*
 * call-seq: playlist.name = new_name -> string or nil
 *
 * Sets the playlist's name to the value of new_name.
 */
static VALUE playlist_name_set(VALUE self, VALUE new_name)
{
  sp_playlist *playlist;
  Data_Get_Struct(self, sp_playlist, playlist);

  sp_error error = sp_playlist_rename(playlist, StringValuePtr(new_name));

  // TODO: throw exception instead?
  if(error != SP_ERROR_OK)
    return Qnil;

  return new_name;
}

/*
 * call-seq: playlist.owner -> user or nil
 *
 * Returns the playlist's owner.
 */
static VALUE playlist_owner(VALUE self)
{
  sp_playlist *playlist;
  Data_Get_Struct(self, sp_playlist, playlist);
  sp_user *user = NULL;
  user = sp_playlist_owner(playlist);
  return user ? Data_Wrap_Struct(class_user, NULL, NULL, user) : Qnil;
}

/*
 * call-seq: playlist.collaborative? -> true or false
 *
 * Returns true if the playlist is collaborative, false otherwise.
 */
static VALUE playlist_collaborative(VALUE self)
{
  sp_playlist *playlist;
  Data_Get_Struct(self, sp_playlist, playlist);
  return sp_playlist_is_collaborative(playlist) ? Qtrue : Qfalse;
}

/*
 * call-seq: playlist.collaborative = true or false -> true or false
 *
 * Sets the collaborative status of the playlist.
 */
static VALUE playlist_collaborative_set(VALUE self, VALUE collaborative)
{
  sp_playlist *playlist;
  Data_Get_Struct(self, sp_playlist, playlist);
  sp_playlist_set_collaborative(playlist, collaborative != Qnil && collaborative != Qfalse);
  return collaborative;
}

/*
 * call-seq: playlist.num_tracks -> fixnum or nil
 *
 */
static VALUE playlist_num_tracks(VALUE self)
{
  sp_playlist *playlist;
  Data_Get_Struct(self, sp_playlist, playlist);
  int n = sp_playlist_num_tracks(playlist);
  return (n >= 0) ? INT2FIX(n) : Qnil;
}

/*
 * call-seq: playlist.track(index) -> track or nil
 *
 */
static VALUE playlist_track(VALUE self, VALUE index)
{
  sp_playlist *playlist;
  Data_Get_Struct(self, sp_playlist, playlist);
  int i = FIX2INT(index);
  sp_track *track = NULL;
  track = sp_playlist_track(playlist, i);
  return track ? Data_Wrap_Struct(class_track, NULL, NULL, track) : Qnil;
}

/*
 * call-seq: playlist.pending_changes? -> true or false
 *
 * Returns true if the playlist has local changes that have not yet been
 * acknowledged by the server, false otherwise.
 */
static VALUE playlist_pending_changes(VALUE self)
{
  sp_playlist *playlist;
  Data_Get_Struct(self, sp_playlist, playlist);
  return sp_playlist_has_pending_changes(playlist) ? Qtrue : Qfalse;
}

/*
 * call-seq: playlist.add_tracks(tracks, position) -> error
 *
 * Add tracks at position in the playlist.
 */
static VALUE playlist_add_tracks(VALUE self, VALUE tracks, VALUE position)
{
  sp_playlist *p;
  Data_Get_Struct(self, sp_playlist, p);
  int n = rb_ary_length(tracks);
  const sp_track **ts = malloc(n*sizeof(sp_track *));
  int i;
  for(i = 0; i < n; ++i)
  {
    sp_track *t;
    Data_Get_Struct(rb_ary_entry(tracks, i), sp_track, t);
    ts[i] = t;
  }
  sp_error e = sp_playlist_add_tracks(p, ts, n, FIX2INT(position));
  free(ts);
  return INT2FIX(e);
}

/*
 * call-seq: playlist.remove_tracks(indices) -> error
 *
 * Remove the tracks at indices from the playlist.
 */
static VALUE playlist_remove_tracks(VALUE self, VALUE indices)
{
  sp_playlist *p;
  Data_Get_Struct(self, sp_playlist, p);
  int n = rb_ary_length(indices);
  int *is = malloc(n*sizeof(int));
  int i;
  for(i = 0; i < n; ++i)
    is[i] = FIX2INT(rb_ary_entry(indices, i));
  sp_error e = sp_playlist_remove_tracks(p, is, n);
  free(is);
  return INT2FIX(e);
}

/*
 * call-seq: playlist.reorder_tracks(indices, new_position) -> error
 *
 * Move the tracks at indices to new_position in the playlist.
 */
static VALUE playlist_reorder_tracks(VALUE self, VALUE indices, VALUE new_position)
{
  sp_playlist *p;
  Data_Get_Struct(self, sp_playlist, p);
  int n = rb_ary_length(indices);
  int *is = malloc(n*sizeof(int));
  int i;
  for(i = 0; i < n; ++i)
    is[i] = FIX2INT(rb_ary_entry(indices, i));
  sp_error e = sp_playlist_reorder_tracks(p, is, n, FIX2INT(new_position));
  free(is);
  return INT2FIX(e);
}

static void search_free(void *search)
{
  sp_search_release(search);
}

/*
 * call-seq: Search.new(session, query, offset, count) -> search or nil
 *
 * Returns a new search object.
 */
static VALUE search_new(VALUE klass, VALUE session, VALUE query, VALUE offset, VALUE count)
{
  // TODO: search callback should not be hardcoded

  sp_session *sess;
  Data_Get_Struct(session, sp_session, sess);

  sp_search *search = NULL;
  search = sp_search_create(sess,
                            StringValuePtr(query),
                            FIX2INT(offset),
                            FIX2INT(count),
                            search_complete_callback,
                            NULL);

  if(!search)
    return Qnil;

  VALUE search_value = Data_Wrap_Struct(class_search, NULL, search_free, search);
  VALUE argv[4] = {session, query, offset, count};
  rb_obj_call_init(search_value, 4, argv);
  return search_value;
}

/*
 * call-seq: search.loaded? -> true or false
 *
 * Returns true if the search object is loaded, false otherwise.
 */
static VALUE search_loaded(VALUE self)
{
  sp_search *search;
  Data_Get_Struct(self, sp_search, search);
  return sp_search_is_loaded(search) ? Qtrue : Qfalse;
}

/*
 * call-seq: search.error -> error
 *
 * Returns one of the constants defined in Error.
 */
static VALUE search_error(VALUE self)
{
  sp_search *search;
  Data_Get_Struct(self, sp_search, search);
  sp_error error = sp_search_error(search);
  return INT2FIX(error);
}

/*
 * call-seq: search.num_artists -> fixnum or nil
 *
 */
static VALUE search_num_artists(VALUE self)
{
  sp_search *search;
  Data_Get_Struct(self, sp_search, search);
  int n = sp_search_num_artists(search);
  return (n >= 0) ? INT2FIX(n) : Qnil;
}

/*
 * call-seq: search.artist(index) -> artist or nil
 *
 */
static VALUE search_artist(VALUE self, VALUE index)
{
  sp_search *search;
  Data_Get_Struct(self, sp_search, search);
  int i = FIX2INT(index);
  sp_artist *artist = NULL;
  artist = sp_search_artist(search, i);
  return artist ? Data_Wrap_Struct(class_artist, NULL, NULL, artist) : Qnil;
}

/*
 * call-seq: search.num_albums -> fixnum or nil
 *
 */
static VALUE search_num_albums(VALUE self)
{
  sp_search *search;
  Data_Get_Struct(self, sp_search, search);
  int n = sp_search_num_albums(search);
  return (n >= 0) ? INT2FIX(n) : Qnil;
}

/*
 * call-seq: search.album(index) -> album or nil
 *
 */
static VALUE search_album(VALUE self, VALUE index)
{
  sp_search *search;
  Data_Get_Struct(self, sp_search, search);
  int i = FIX2INT(index);
  sp_album *album = NULL;
  album = sp_search_album(search, i);
  return album ? Data_Wrap_Struct(class_album, NULL, NULL, album) : Qnil;
}

/*
 * call-seq: search.num_tracks -> fixnum or nil
 *
 */
static VALUE search_num_tracks(VALUE self)
{
  sp_search *search;
  Data_Get_Struct(self, sp_search, search);
  int n = sp_search_num_tracks(search);
  return (n >= 0) ? INT2FIX(n) : Qnil;
}

/*
 * call-seq: search.track(index) -> track or nil
 *
 */
static VALUE search_track(VALUE self, VALUE index)
{
  sp_search *search;
  Data_Get_Struct(self, sp_search, search);
  int i = FIX2INT(index);
  sp_track *track = NULL;
  track = sp_search_track(search, i);
  return track ? Data_Wrap_Struct(class_track, NULL, NULL, track) : Qnil;
}

/*
 * call-seq: search.total_tracks -> fixnum or nil
 *
 * Returns the total number of track hits for the search query.
 */
static VALUE search_total_tracks(VALUE self)
{
  sp_search *search;
  Data_Get_Struct(self, sp_search, search);
  int n = sp_search_total_tracks(search);
  return (n >= 0) ? INT2FIX(n) : Qnil;
}

/*
 * call-seq: search.query -> string or nil
 *
 * Returns the query for the search object.
 */
static VALUE search_query(VALUE self)
{
  sp_search *search;
  Data_Get_Struct(self, sp_search, search);
  const char *q = sp_search_query(search);
  return q ? rb_str_new2(q) : Qnil;
}

/*
 * call-seq: search.did_you_mean -> string or nil
 *
 * Returns the "did you mean" suggestion for the search object, or nil if no
 * suggestion is available.
 */
static VALUE search_did_you_mean(VALUE self)
{
  sp_search *search;
  Data_Get_Struct(self, sp_search, search);
  const char *d = sp_search_did_you_mean(search);
  return d ? rb_str_new2(d) : Qnil;
}

static void artist_browse_free(void *ab)
{
  sp_artistbrowse_release(ab);
}

/*
 * call-seq: ArtistBrowse.new(session, artist) -> artist_browse or nil
 *
 * Returns a new artist browse object.
 */
static VALUE artist_browse_new(VALUE klass, VALUE session, VALUE artist)
{
  // TODO: artist browse callback should not be hardcoded

  sp_session *s;
  Data_Get_Struct(session, sp_session, s);

  sp_artist *a;
  Data_Get_Struct(artist, sp_artist, a);

  sp_artistbrowse *artistbrowse = NULL;
  artistbrowse = sp_artistbrowse_create(s, a, artist_browse_complete_callback, NULL);

  if(!artistbrowse)
    return Qnil;

  VALUE ab_value = Data_Wrap_Struct(class_artist_browse, NULL, artist_browse_free, artistbrowse);
  VALUE argv[2] = {session, artist};
  rb_obj_call_init(ab_value, 2, argv);
  return ab_value;
}

/*
 * call-seq: artist_browse.loaded? -> true or false
 *
 * Returns true if the artist browse object is loaded, false otherwise.
 */
static VALUE artist_browse_loaded(VALUE self)
{
  sp_artistbrowse *artistbrowse;
  Data_Get_Struct(self, sp_artistbrowse, artistbrowse);
  return sp_artistbrowse_is_loaded(artistbrowse) ? Qtrue : Qfalse;
}

/*
 * call-seq: artist_browse.error -> error
 *
 * Returns one of the constants defined in Error.
 */
static VALUE artist_browse_error(VALUE self)
{
  sp_artistbrowse *artistbrowse;
  Data_Get_Struct(self, sp_artistbrowse, artistbrowse);
  sp_error error = sp_artistbrowse_error(artistbrowse);
  return INT2FIX(error);
}

/*
 * call-seq: artist_browse.artist -> artist or nil
 *
 * Returns the artist for the artist browse object.
 */
static VALUE artist_browse_artist(VALUE self)
{
  sp_artistbrowse *artistbrowse;
  Data_Get_Struct(self, sp_artistbrowse, artistbrowse);
  sp_artist *artist = sp_artistbrowse_artist(artistbrowse);
  return artist ? Data_Wrap_Struct(class_artist, NULL, NULL, artist) : Qnil;
}

/*
 * call-seq: artist_browse.num_tracks -> fixnum or nil
 *
 */
static VALUE artist_browse_num_tracks(VALUE self)
{
  sp_artistbrowse *artistbrowse;
  Data_Get_Struct(self, sp_artistbrowse, artistbrowse);
  int n = sp_artistbrowse_num_tracks(artistbrowse);
  return (n >= 0) ? INT2FIX(n) : Qnil;
}

/*
 * call-seq: artist_browse.track(index) -> track or nil
 *
 */
static VALUE artist_browse_track(VALUE self, VALUE index)
{
  sp_artistbrowse *artistbrowse;
  Data_Get_Struct(self, sp_artistbrowse, artistbrowse);
  int i = FIX2INT(index);
  sp_track *track = NULL;
  track = sp_artistbrowse_track(artistbrowse, i);
  return track ? Data_Wrap_Struct(class_track, NULL, NULL, track) : Qnil;
}

/*
 * call-seq: artist_browse.num_similar_artists -> fixnum or nil
 *
 */
static VALUE artist_browse_num_similar_artists(VALUE self)
{
  sp_artistbrowse *artistbrowse;
  Data_Get_Struct(self, sp_artistbrowse, artistbrowse);
  int n = sp_artistbrowse_num_similar_artists(artistbrowse);
  return (n >= 0) ? INT2FIX(n) : Qnil;
}

/*
 * call-seq: artist_browse.similar_artist(index) -> artist or nil
 *
 */
static VALUE artist_browse_similar_artist(VALUE self, VALUE index)
{
  sp_artistbrowse *artistbrowse;
  Data_Get_Struct(self, sp_artistbrowse, artistbrowse);
  int i = FIX2INT(index);
  sp_artist *artist = NULL;
  artist = sp_artistbrowse_similar_artist(artistbrowse, i);
  return artist ? Data_Wrap_Struct(class_artist, NULL, NULL, artist) : Qnil;
}

/*
 * call-seq: artist_browse.biography -> string or nil
 *
 * Returns the biography for the artist browse object, or nil if no biography is
 * available.
 */
static VALUE artist_browse_biography(VALUE self)
{
  sp_artistbrowse *artistbrowse;
  Data_Get_Struct(self, sp_artistbrowse, artistbrowse);
  const char *bio = sp_artistbrowse_biography(artistbrowse);
  return bio ? rb_str_new2(bio) : Qnil;
}

static void album_browse_free(void *ab)
{
  sp_albumbrowse_release(ab);
}

/*
 * call-seq: AlbumBrowse.new(session, album) -> album_browse or nil
 *
 * Returns a new album browse object.
 */
static VALUE album_browse_new(VALUE klass, VALUE session, VALUE album)
{
  // TODO: album browse callback should not be hardcoded

  sp_session *s;
  Data_Get_Struct(session, sp_session, s);

  sp_album *a;
  Data_Get_Struct(album, sp_album, a);

  sp_albumbrowse *albumbrowse = NULL;
  albumbrowse = sp_albumbrowse_create(s, a, album_browse_complete_callback, NULL);

  if(!albumbrowse)
    return Qnil;

  VALUE ab_value = Data_Wrap_Struct(class_album_browse, NULL, album_browse_free, albumbrowse);
  VALUE argv[2] = {session, album};
  rb_obj_call_init(ab_value, 2, argv);
  return ab_value;
}

/*
 * call-seq: album_browse.loaded? -> true or false
 *
 * Returns true if the album browse object is loaded, false otherwise.
 */
static VALUE album_browse_loaded(VALUE self)
{
  sp_albumbrowse *albumbrowse;
  Data_Get_Struct(self, sp_albumbrowse, albumbrowse);
  return sp_albumbrowse_is_loaded(albumbrowse) ? Qtrue : Qfalse;
}

/*
 * call-seq: album_browse.error -> error
 *
 * Returns one of the constants defined in Error.
 */
static VALUE album_browse_error(VALUE self)
{
  sp_albumbrowse *albumbrowse;
  Data_Get_Struct(self, sp_albumbrowse, albumbrowse);
  sp_error error = sp_albumbrowse_error(albumbrowse);
  return INT2FIX(error);
}

/*
 * call-seq: album_browse.artist -> artist or nil
 *
 * Returns the artist for the album browse object.
 */
static VALUE album_browse_artist(VALUE self)
{
  sp_albumbrowse *albumbrowse;
  Data_Get_Struct(self, sp_albumbrowse, albumbrowse);
  sp_artist *artist = sp_albumbrowse_artist(albumbrowse);
  return artist ? Data_Wrap_Struct(class_artist, NULL, NULL, artist) : Qnil;
}

/*
 * call-seq: album_browse.album -> album or nil
 *
 * Returns the album for the album browse object.
 */
static VALUE album_browse_album(VALUE self)
{
  sp_albumbrowse *albumbrowse;
  Data_Get_Struct(self, sp_albumbrowse, albumbrowse);
  sp_album *album = sp_albumbrowse_album(albumbrowse);
  return album ? Data_Wrap_Struct(class_album, NULL, NULL, album) : Qnil;
}

/*
 * call-seq: album_browse.num_tracks -> fixnum or nil
 *
 */
static VALUE album_browse_num_tracks(VALUE self)
{
  sp_albumbrowse *albumbrowse;
  Data_Get_Struct(self, sp_albumbrowse, albumbrowse);
  int n = sp_albumbrowse_num_tracks(albumbrowse);
  return (n >= 0) ? INT2FIX(n) : Qnil;
}

/*
 * call-seq: album_browse_track(index) -> track or nil
 *
 */
static VALUE album_browse_track(VALUE self, VALUE index)
{
  sp_albumbrowse *albumbrowse;
  Data_Get_Struct(self, sp_albumbrowse, albumbrowse);
  int i = FIX2INT(index);
  sp_track *track = NULL;
  track = sp_albumbrowse_track(albumbrowse, i);
  return track ? Data_Wrap_Struct(class_track, NULL, NULL, track) : Qnil;
}

/*
 * call-seq: album_browse.num_copyrights -> fixnum or nil
 *
 */
static VALUE album_browse_num_copyrights(VALUE self)
{
  sp_albumbrowse *albumbrowse;
  Data_Get_Struct(self, sp_albumbrowse, albumbrowse);
  int n = sp_albumbrowse_num_copyrights(albumbrowse);
  return (n >= 0) ? INT2FIX(n) : Qnil;
}

/*
 * call-seq: album_browse.copyright(index) -> string or nil
 *
 */
static VALUE album_browse_copyright(VALUE self, VALUE index)
{
  sp_albumbrowse *albumbrowse;
  Data_Get_Struct(self, sp_albumbrowse, albumbrowse);
  int i = FIX2INT(index);
  const char *c = sp_albumbrowse_copyright(albumbrowse, i);
  return c ? rb_str_new2(c) : Qnil;
}

/*
 * call-seq: album_browse.review -> string or nil
 *
 * Returns the review for the album browse object, or nil if no review is
 * available.
 */
static VALUE album_browse_review(VALUE self)
{
  sp_albumbrowse *albumbrowse;
  Data_Get_Struct(self, sp_albumbrowse, albumbrowse);
  const char *r = sp_albumbrowse_review(albumbrowse);
  return r ? rb_str_new2(r) : Qnil;
}

/*
 * call-seq: artist.loaded? -> true or false
 *
 * Returns true if the artist is loaded, false otherwise.
 */
static VALUE artist_loaded(VALUE self)
{
  sp_artist *artist;
  Data_Get_Struct(self, sp_artist, artist);
  return sp_artist_is_loaded(artist) ? Qtrue : Qfalse;
}

/*
 * call-seq: artist.name -> string or nil
 *
 * Returns the artist's name.
 */
static VALUE artist_name(VALUE self)
{
  sp_artist *artist;
  Data_Get_Struct(self, sp_artist, artist);
  const char *name = sp_artist_name(artist);
  return name ? rb_str_new2(name) : Qnil;
}

/*
 * call-seq: album.loaded? -> true or false
 *
 * Returns true if the album is loaded, false otherwise.
 */
static VALUE album_loaded(VALUE self)
{
  sp_album *album;
  Data_Get_Struct(self, sp_album, album);
  return sp_album_is_loaded(album) ? Qtrue : Qfalse;
}

/*
 * call-seq: album.artist -> artist or nil
 *
 * Returns the album's artist.
 */
static VALUE album_artist(VALUE self)
{
  sp_album *album;
  Data_Get_Struct(self, sp_album, album);
  sp_artist *artist = sp_album_artist(album);
  return artist ? Data_Wrap_Struct(class_artist, NULL, NULL, artist) : Qnil;
}

/*
 * call-seq: album.name -> string or nil
 *
 * Returns the album's name.
 */
static VALUE album_name(VALUE self)
{
  sp_album *album;
  Data_Get_Struct(self, sp_album, album);
  const char *name = sp_album_name(album);
  return name ? rb_str_new2(name) : Qnil;
}

/*
 * call-seq: album.year -> fixnum or nil
 *
 * Returns the year the album was released.
 */
static VALUE album_year(VALUE self)
{
  sp_album *album;
  Data_Get_Struct(self, sp_album, album);
  int year = sp_album_year(album);
  return year ? INF2FIX(year) : Qnil;
}

/*
 * call-seq: track.loaded? -> true or false
 *
 * Returns true if the track is loaded, false otherwise.
 */
static VALUE track_loaded(VALUE self)
{
  sp_track *track;
  Data_Get_Struct(self, sp_track, track);
  return sp_track_is_loaded(track) ? Qtrue : Qfalse;
}

/*
 * call-seq: track.error -> error
 *
 * Returns one of the constants defined in Error.
 */
static VALUE track_error(VALUE self)
{
  sp_track *track;
  Data_Get_Struct(self, sp_track, track);
  sp_error error = sp_track_error(track);
  return INT2FIX(error);
}

/*
 * call-seq: track.num_artists -> fixnum or nil
 *
 */
static VALUE track_num_artists(VALUE self)
{
  sp_track *track;
  Data_Get_Struct(self, sp_track, track);
  int n = sp_track_num_artists(track);
  return n ? INT2FIX(n) : Qnil;
}

/*
 * call-seq: track.artist(index) -> artist or nil
 *
 */
static VALUE track_artist(VALUE self, VALUE index)
{
  sp_track *track;
  Data_Get_Struct(self, sp_track, track);
  int i = FIX2INT(index);
  sp_artist *artist = sp_track_artist(track, i);
  return artist ? Data_Wrap_Struct(class_artist, NULL, NULL, artist) : Qnil;
}

/*
 * call-seq: track.album -> album or nil
 *
 * Returns the track's album.
 */
static VALUE track_album(VALUE self)
{
  sp_track *track;
  Data_Get_Struct(self, sp_track, track);
  sp_album *album = sp_track_album(track);
  return album ? Data_Wrap_Struct(class_album, NULL, NULL, album) : Qnil;
}

/*
 * call-seq: track.name -> string or nil
 *
 * Returns the track's name.
 */
static VALUE track_name(VALUE self)
{
  sp_track *track;
  Data_Get_Struct(self, sp_track, track);
  const char *name = sp_track_name(track);
  return name ? rb_str_new2(name) : Qnil;
}

/*
 * call-seq: track.duration -> fixnum
 *
 * Returns the track's duration in milliseconds.
 */
static VALUE track_duration(VALUE self)
{
  sp_track *track;
  Data_Get_Struct(self, sp_track, track);
  int d = sp_track_duration(track);
  return INT2FIX(d);
}

/*
 * call-seq: track.popularity -> fixnum
 *
 * Returns the track's popularity as a number in the range [0, 100].
 */
static VALUE track_popularity(VALUE self)
{
  sp_track *track;
  Data_Get_Struct(self, sp_track, track);
  int p = sp_track_popularity(track);
  return INT2FIX(p);
}

/*
 * call-seq: track.disc -> fixnum or nil
 *
 * Returns the index of the disc the track is on, counting from 1.
 */
static VALUE track_disc(VALUE self)
{
  sp_track *track;
  Data_Get_Struct(self, sp_track, track);
  int d = sp_track_disc(track);
  return d ? INT2FIX(d) : Qnil;
}

/*
 * call-seq: track.index -> fixnum or nil
 *
 * Returns the index of the track on its disc, counting from 1.
 */
static VALUE track_index(VALUE self)
{
  sp_track *track;
  Data_Get_Struct(self, sp_track, track);
  int i = sp_track_index(track);
  return i ? INT2FIX(i) : Qnil;
}

static void link_free(void *link)
{
  sp_link_release(link);
}

/*
 * call-seq:
 *  Link.new(artist)   -> link
 *  Link.new(album)    -> link
 *  Link.new(track)    -> link
 *  Link.new(playlist) -> link
 *  Link.new(search)   -> link
 *  Link.new(string)   -> link or nil
 *
 * Returns a new link pointing to the given resource.
 */
static VALUE link_new(VALUE klass, VALUE object)
{
  sp_link *link = NULL;

  if(rb_obj_is_kind_of(object, class_artist))
  {
    sp_artist *a;
    Data_Get_Struct(object, sp_artist, a);
    link = sp_link_create_from_artist(a);
  }
  else if(rb_obj_is_kind_of(object, class_album))
  {
    sp_album *a;
    Data_Get_Struct(object, sp_album, a);
    link = sp_link_create_from_album(a);
  }
  else if(rb_obj_is_kind_of(object, class_track))
  {
    sp_track *t;
    Data_Get_Struct(object, sp_track, t);
    link = sp_link_create_from_track(t, 0); // TODO: offset
  }
  else if(rb_obj_is_kind_of(object, class_playlist))
  {
    sp_playlist *p;
    Data_Get_Struct(object, sp_playlist, p);
    link = sp_link_create_from_playlist(p);
  }
  else if(rb_obj_is_kind_of(object, class_search))
  {
    sp_search *s;
    Data_Get_Struct(object, sp_search, s);
    link = sp_link_create_from_search(s);
  }
  else
  {
    link = sp_link_create_from_string(StringValuePtr(object));
  }

  if(!link)
    return Qnil;

  VALUE link_value = Data_Wrap_Struct(class_link, NULL, link_free, link);
  VALUE argv[1] = {object};
  rb_obj_call_init(link_value, 1, argv);
  return link_value;
}

/*
 * call-seq: link.link_type -> type
 *
 * Returns the link type. Named link_type instead of just type to avoid
 * overriding a method in Object.
 */
static VALUE link_link_type(VALUE self)
{
  sp_link *link;
  Data_Get_Struct(self, sp_link, link);
  int type = sp_link_type(link);
  return INT2FIX(type);
}

/*
 * call-seq: link.to_s -> string or nil
 *
 * Returns a string representation of the link.
 */
static VALUE link_to_s(VALUE self)
{
  sp_link *link;
  Data_Get_Struct(self, sp_link, link);
  char buffer[100];
  int n = sp_link_as_string(link, buffer, 100);
  return (n < 100) ? rb_str_new2(buffer) : Qnil;
}

/*
 * call-seq: link.to_artist -> artist or nil
 *
 */
static VALUE link_to_artist(VALUE self)
{
  sp_link *l;
  Data_Get_Struct(self, sp_link, l);
  sp_artist *a = sp_link_as_artist(l);
  return a ? Data_Wrap_Struct(class_artist, NULL, NULL, a) : Qnil;
}

/*
 * call-seq: link.to_album -> album or nil
 *
 */
static VALUE link_to_album(VALUE self)
{
  sp_link *l;
  Data_Get_Struct(self, sp_link, l);
  sp_album *a = sp_link_as_album(l);
  return a ? Data_Wrap_Struct(class_album, NULL, NULL, a) : Qnil;
}

/*
 * call-seq: link.to_track -> track or nil
 *
 */
static VALUE link_to_track(VALUE self)
{
  sp_link *l;
  Data_Get_Struct(self, sp_link, l);
  sp_track *t = sp_link_as_track(l);
  return t ? Data_Wrap_Struct(class_track, NULL, NULL, t) : Qnil;
}

// init

void Init_greenstripes()
{
  module_greenstripes = rb_define_module("GreenStripes");

  /*
   * The Error module defines constants for error codes returned by various
   * methods.
   */
  module_error = rb_define_module_under(module_greenstripes, "Error");
  rb_define_const(module_error, "OK", INT2FIX(SP_ERROR_OK));
  rb_define_const(module_error, "BAD_API_VERSION", INT2FIX(SP_ERROR_BAD_API_VERSION));
  rb_define_const(module_error, "API_INITIALIZATION_FAILED", INT2FIX(SP_ERROR_API_INITIALIZATION_FAILED));
  rb_define_const(module_error, "TRACK_NOT_PLAYABLE", INT2FIX(SP_ERROR_TRACK_NOT_PLAYABLE));
  rb_define_const(module_error, "RESOURCE_NOT_LOADED", INT2FIX(SP_ERROR_RESOURCE_NOT_LOADED));
  rb_define_const(module_error, "BAD_APPLICATION_KEY", INT2FIX(SP_ERROR_BAD_APPLICATION_KEY));
  rb_define_const(module_error, "BAD_USERNAME_OR_PASSWORD", INT2FIX(SP_ERROR_BAD_USERNAME_OR_PASSWORD));
  rb_define_const(module_error, "USER_BANNED", INT2FIX(SP_ERROR_USER_BANNED));
  rb_define_const(module_error, "UNABLE_TO_CONTACT_SERVER", INT2FIX(SP_ERROR_UNABLE_TO_CONTACT_SERVER));
  rb_define_const(module_error, "CLIENT_TOO_OLD", INT2FIX(SP_ERROR_CLIENT_TOO_OLD));
  rb_define_const(module_error, "OTHER_PERMAMENT", INT2FIX(SP_ERROR_OTHER_PERMAMENT));
  rb_define_const(module_error, "BAD_USER_AGENT", INT2FIX(SP_ERROR_BAD_USER_AGENT));
  rb_define_const(module_error, "MISSING_CALLBACK", INT2FIX(SP_ERROR_MISSING_CALLBACK));
  rb_define_const(module_error, "INVALID_INDATA", INT2FIX(SP_ERROR_INVALID_INDATA));
  rb_define_const(module_error, "INDEX_OUT_OF_RANGE", INT2FIX(SP_ERROR_INDEX_OUT_OF_RANGE));
  rb_define_const(module_error, "USER_NEEDS_PREMIUM", INT2FIX(SP_ERROR_USER_NEEDS_PREMIUM));
  rb_define_const(module_error, "OTHER_TRANSIENT", INT2FIX(SP_ERROR_OTHER_TRANSIENT));
  rb_define_const(module_error, "IS_LOADING", INT2FIX(SP_ERROR_IS_LOADING));

  /*
   * The ConnectionState module defines constants describing the current state
   * of a connection.
   */
  module_connection_state = rb_define_module_under(module_greenstripes, "ConnectionState");
  rb_define_const(module_connection_state, "LOGGED_OUT", INT2FIX(SP_CONNECTION_STATE_LOGGED_OUT));
  rb_define_const(module_connection_state, "LOGGED_IN", INT2FIX(SP_CONNECTION_STATE_LOGGED_IN));
  rb_define_const(module_connection_state, "DISCONNECTED", INT2FIX(SP_CONNECTION_STATE_DISCONNECTED));
  rb_define_const(module_connection_state, "UNDEFINED", INT2FIX(SP_CONNECTION_STATE_UNDEFINED));

  /*
   * The LinkType module defines constants for link types.
   */
  module_link_type = rb_define_module_under(module_greenstripes, "LinkType");
  rb_define_const(module_link_type, "INVALID", INT2FIX(SP_LINKTYPE_INVALID));
  rb_define_const(module_link_type, "TRACK", INT2FIX(SP_LINKTYPE_TRACK));
  rb_define_const(module_link_type, "ALBUM", INT2FIX(SP_LINKTYPE_ALBUM));
  rb_define_const(module_link_type, "ARTIST", INT2FIX(SP_LINKTYPE_ARTIST));
  rb_define_const(module_link_type, "SEARCH", INT2FIX(SP_LINKTYPE_SEARCH));
  rb_define_const(module_link_type, "PLAYLIST", INT2FIX(SP_LINKTYPE_PLAYLIST));

  /*
   * Session
   */
  class_session = rb_define_class_under(module_greenstripes, "Session", rb_cObject);
  rb_define_singleton_method(class_session, "new", session_new, 4);
  rb_define_method(class_session, "login", session_login, 2);
  rb_define_method(class_session, "logout", session_logout, 0);
  rb_define_method(class_session, "connection_state", session_connection_state, 0);
  rb_define_method(class_session, "process_events", session_process_events, 0);
  rb_define_method(class_session, "user", session_user, 0);
  rb_define_method(class_session, "playlist_container", session_playlist_container, 0);

  /*
   * User
   */
  class_user = rb_define_class_under(module_greenstripes, "User", rb_cObject);
  rb_define_method(class_user, "loaded?", user_loaded, 0);
  rb_define_method(class_user, "canonical_name", user_canonical_name, 0);
  rb_define_method(class_user, "display_name", user_display_name, 0);

  /*
   * PlaylistContainer
   */
  class_playlist_container = rb_define_class_under(module_greenstripes, "PlaylistContainer", rb_cObject);
  rb_define_method(class_playlist_container, "num_playlists", playlist_container_num_playlists, 0);
  rb_define_method(class_playlist_container, "playlist", playlist_container_playlist, 1);
  rb_define_method(class_playlist_container, "add_new_playlist", playlist_container_add_new_playlist, 1);
  rb_define_method(class_playlist_container, "add_playlist", playlist_container_add_playlist, 1);
  rb_define_method(class_playlist_container, "remove_playlist", playlist_container_remove_playlist, 1);
  rb_define_method(class_playlist_container, "move_playlist", playlist_container_move_playlist, 2);

  /*
   * Playlist
   */
  class_playlist = rb_define_class_under(module_greenstripes, "Playlist", rb_cObject);
  rb_define_method(class_playlist, "loaded?", playlist_loaded, 0);
  rb_define_method(class_playlist, "name", playlist_name, 0);
  rb_define_method(class_playlist, "name=", playlist_name_set, 1);
  rb_define_method(class_playlist, "owner", playlist_owner, 0);
  rb_define_method(class_playlist, "collaborative?", playlist_collaborative, 0);
  rb_define_method(class_playlist, "collaborative=", playlist_collaborative_set, 1);
  rb_define_method(class_playlist, "num_tracks", playlist_num_tracks, 0);
  rb_define_method(class_playlist, "track", playlist_track, 1);
  rb_define_method(class_playlist, "pending_changes?", playlist_pending_changes, 0);
  rb_define_method(class_playlist, "add_tracks", playlist_add_tracks, 2);
  rb_define_method(class_playlist, "remove_tracks", playlist_remove_tracks, 1);
  rb_define_method(class_playlist, "reorder_tracks", playlist_reorder_tracks, 2);

  /*
   * Search
   */
  class_search = rb_define_class_under(module_greenstripes, "Search", rb_cObject);
  rb_define_singleton_method(class_search, "new", search_new, 4);
  rb_define_method(class_search, "loaded?", search_loaded, 0);
  rb_define_method(class_search, "error", search_error, 0);
  rb_define_method(class_search, "num_artists", search_num_artists, 0);
  rb_define_method(class_search, "artist", search_artist, 1);
  rb_define_method(class_search, "num_albums", search_num_albums, 0);
  rb_define_method(class_search, "album", search_album, 1);
  rb_define_method(class_search, "num_tracks", search_num_tracks, 0);
  rb_define_method(class_search, "track", search_track, 1);
  rb_define_method(class_search, "total_tracks", search_total_tracks, 0);
  rb_define_method(class_search, "query", search_query, 0);
  rb_define_method(class_search, "did_you_mean", search_did_you_mean, 0);

  /*
   * ArtistBrowse
   */
  class_artist_browse = rb_define_class_under(module_greenstripes, "ArtistBrowse", rb_cObject);
  rb_define_singleton_method(class_artist_browse, "new", artist_browse_new, 2);
  rb_define_method(class_artist_browse, "loaded?", artist_browse_loaded, 0);
  rb_define_method(class_artist_browse, "error", artist_browse_error, 0);
  rb_define_method(class_artist_browse, "artist", artist_browse_artist, 0);
  rb_define_method(class_artist_browse, "num_tracks", artist_browse_num_tracks, 0);
  rb_define_method(class_artist_browse, "track", artist_browse_track, 1);
  rb_define_method(class_artist_browse, "num_similar_artists", artist_browse_num_similar_artists, 0);
  rb_define_method(class_artist_browse, "similar_artist", artist_browse_similar_artist, 1);
  rb_define_method(class_artist_browse, "biography", artist_browse_biography, 0);

  /*
   * AlbumBrowse
   */
  class_album_browse = rb_define_class_under(module_greenstripes, "AlbumBrowse", rb_cObject);
  rb_define_singleton_method(class_album_browse, "new", album_browse_new, 2);
  rb_define_method(class_album_browse, "loaded?", album_browse_loaded, 0);
  rb_define_method(class_album_browse, "error", album_browse_error, 0);
  rb_define_method(class_album_browse, "artist", album_browse_artist, 0);
  rb_define_method(class_album_browse, "album", album_browse_album, 0);
  rb_define_method(class_album_browse, "num_tracks", album_browse_num_tracks, 0);
  rb_define_method(class_album_browse, "track", album_browse_track, 1);
  rb_define_method(class_album_browse, "num_copyrights", album_browse_num_copyrights, 0);
  rb_define_method(class_album_browse, "copyright", album_browse_copyright, 1);
  rb_define_method(class_album_browse, "review", album_browse_review, 0);

  /*
   * Artist
   */
  class_artist = rb_define_class_under(module_greenstripes, "Artist", rb_cObject);
  rb_define_method(class_artist, "loaded?", artist_loaded, 0);
  rb_define_method(class_artist, "name", artist_name, 0);

  /*
   * Album
   */
  class_album = rb_define_class_under(module_greenstripes, "Album", rb_cObject);
  rb_define_method(class_album, "loaded?", album_loaded, 0);
  rb_define_method(class_album, "artist", album_artist, 0);
  rb_define_method(class_album, "name", album_name, 0);
  rb_define_method(class_album, "year", album_year, 0);

  /*
   * Track
   */
  class_track = rb_define_class_under(module_greenstripes, "Track", rb_cObject);
  rb_define_method(class_track, "loaded?", track_loaded, 0);
  rb_define_method(class_track, "error", track_error, 0);
  rb_define_method(class_track, "num_artists", track_num_artists, 0);
  rb_define_method(class_track, "artist", track_artist, 1);
  rb_define_method(class_track, "album", track_album, 0);
  rb_define_method(class_track, "name", track_name, 0);
  rb_define_method(class_track, "duration", track_duration, 0);
  rb_define_method(class_track, "popularity", track_popularity, 0);
  rb_define_method(class_track, "disc", track_disc, 0);
  rb_define_method(class_track, "index", track_index, 0);

  /*
   * Link
   */
  class_link = rb_define_class_under(module_greenstripes, "Link", rb_cObject);
  rb_define_singleton_method(class_link, "new", link_new, 1);
  rb_define_method(class_link, "link_type", link_link_type, 0);
  rb_define_method(class_link, "to_s", link_to_s, 0);
  rb_define_method(class_link, "to_artist", link_to_artist, 0);
  rb_define_method(class_link, "to_album", link_to_album, 0);
  rb_define_method(class_link, "to_track", link_to_track, 0);
}
