module GreenStripes
  class Session
    def initialize(args = {})
      args[:cache_location]     ||= 'tmp'
      args[:settings_location]  ||= 'tmp'
      args[:application_key]    ||= ''
      args[:user_agent]         ||= 'GreenStripes'

      callbacks = ::GreenStripes::FFI::SessionCallbacks.new
      callbacks[:logged_in]           = args[:logged_in_callback]
      callbacks[:logged_out]          = args[:logged_out_callback]
      callbacks[:metadata_updated]    = args[:metadata_updated_callback]
      callbacks[:connection_error]    = args[:connection_error_callback]
      callbacks[:message_to_user]     = args[:message_to_user_callback]
      callbacks[:notify_main_thread]  = args[:notify_main_thread_callback]
      callbacks[:log_message]         = args[:log_message_callback]

      config = ::GreenStripes::FFI::SessionConfig.new
      config[:api_version]          = 3
      config[:cache_location]       = ::FFI::MemoryPointer.from_string(args[:cache_location])
      config[:settings_location]    = ::FFI::MemoryPointer.from_string(args[:settings_location])
      config[:application_key]      = ::FFI::MemoryPointer.from_string(args[:application_key])
      config[:application_key_size] = args[:application_key].size
      config[:user_agent]           = ::FFI::MemoryPointer.from_string(args[:user_agent])
      config[:callbacks]            = callbacks

      out = ::FFI::MemoryPointer.new(:pointer)
      error = ::GreenStripes::FFI.sp_session_init(config, out)
      raise ::GreenStripes::FFI.sp_error_message(error) unless error == :sp_error_ok
      @ptr = out.get_pointer(0)

      self
    end

    def login(username, password)
      error = ::GreenStripes::FFI.sp_session_login(@ptr, username, password)
      raise ::GreenStripes::FFI.sp_error_message(error) unless error == :sp_error_ok
    end

    def logout
      error = ::GreenStripes::FFI.sp_session_logout(@ptr)
      raise ::GreenStripes::FFI.sp_error_message(error) unless error == :sp_error_ok
    end

    def process_events
      int_ptr = ::FFI::MemoryPointer.new(:int)
      ::GreenStripes::FFI.sp_session_process_events(@ptr, int_ptr)
      int_ptr.get_pointer(0)
    end

    def connection_state
      ::GreenStripes::FFI.sp_session_connectionstate(@ptr)
    end
  end
end
