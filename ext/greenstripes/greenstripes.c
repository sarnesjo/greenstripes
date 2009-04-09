#include <ruby.h>
#include <spotify/api.h>

// modules and classes

static VALUE module_greenstripes;

static VALUE module_error;
static VALUE module_connection_state;
static VALUE module_link_type;

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
}
