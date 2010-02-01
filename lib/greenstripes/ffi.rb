require 'ffi'

module GreenStripes
  module FFI
    extend ::FFI::Library

    ffi_lib 'spotify'

    enum :sp_error,
      [ :sp_error_ok, 0,
        :sp_error_bad_api_version, 1,
        :sp_error_api_initialization_failed, 2,
        :sp_error_track_not_playable, 3,
        :sp_error_resource_not_loaded, 4,
        :sp_error_bad_application_key, 5,
        :sp_error_bad_username_or_password, 6,
        :sp_error_user_banned, 7,
        :sp_error_unable_to_contact_server, 8,
        :sp_error_client_too_old, 9,
        :sp_error_other_permanent, 10,
        :sp_error_bad_user_agent, 11,
        :sp_error_missing_callback, 12,
        :sp_error_invalid_indata, 13,
        :sp_error_index_out_of_range, 14,
        :sp_error_user_needs_premium, 15,
        :sp_error_other_transient, 16,
        :sp_error_is_loading, 17 ]

    enum :sp_connectionstate,
      [ :sp_connection_state_logged_out, 0,
        :sp_connection_state_logged_in, 1,
        :sp_connection_state_disconnected, 2,
        :sp_connection_state_undefined, 3 ]

    # session callbacks
    callback :logged_in, [ :pointer, :sp_error ], :void
    callback :logged_out, [ :pointer ], :void
    callback :metadata_updated, [ :pointer ], :void
    callback :connection_error, [ :pointer, :sp_error ], :void
    callback :message_to_user, [ :pointer, :string ], :void
    callback :notify_main_thread, [ :pointer ], :void
    #callback :music_delivery # TODO
    #callback :play_token_lost # TODO
    callback :log_message, [ :pointer, :string ], :void
    #callback :end_of_track # TODO

    attach_function :sp_error_message, [ :sp_error ], :string

    attach_function :sp_session_init, [ :pointer, :pointer ], :sp_error
    attach_function :sp_session_login, [ :pointer, :string, :string ], :sp_error
    attach_function :sp_session_logout, [ :pointer ], :sp_error
    attach_function :sp_session_process_events, [ :pointer, :pointer ], :void
    attach_function :sp_session_connectionstate, [ :pointer ], :sp_connectionstate

    class SessionCallbacks < ::FFI::Struct
      layout  :logged_in,           :logged_in,
              :logged_out,          :logged_out,
              :metadata_updated,    :metadata_updated,
              :connection_error,    :connection_error,
              :message_to_user,     :message_to_user,
              :notify_main_thread,  :notify_main_thread,
              :music_delivery,      :pointer, # TODO
              :play_token_lost,     :pointer, # TODO
              :log_message,         :log_message,
              :end_of_track,        :pointer # TODO
    end

    class SessionConfig < ::FFI::Struct
      layout  :api_version,           :int,
              :cache_location,        :pointer,
              :settings_location,     :pointer,
              :application_key,       :pointer,
              :application_key_size,  :int, # TODO: size_t?
              :user_agent,            :pointer,
              :callbacks,             :pointer,
              :userdata,              :pointer
    end
  end
end
