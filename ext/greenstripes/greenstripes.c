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

  /*
   * Search
   */
  class_search = rb_define_class_under(module_greenstripes, "Search", rb_cObject);

  /*
   * Artist
   */
  class_artist = rb_define_class_under(module_greenstripes, "Artist", rb_cObject);

  /*
   * Album
   */
  class_album = rb_define_class_under(module_greenstripes, "Album", rb_cObject);

  /*
   * Track
   */
  class_track = rb_define_class_under(module_greenstripes, "Track", rb_cObject);

  /*
   * Link
   */
  class_link = rb_define_class_under(module_greenstripes, "Link", rb_cObject);
  rb_define_singleton_method(class_link, "new", link_new, 1);
  rb_define_method(class_link, "link_type", link_link_type, 0);
  rb_define_method(class_link, "to_s", link_to_s, 0);
}
