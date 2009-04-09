#include <ruby.h>
#include <spotify/api.h>

// modules and classes

static VALUE module_greenstripes;

static VALUE module_error;
static VALUE module_connection_state;
static VALUE module_link_type;

static VALUE class_session;

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
}
