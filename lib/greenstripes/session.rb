module GreenStripes
  class Session
    LOGGED_IN = Proc.new do |session, error|
      #puts "logged in: #{session.inspect}, #{error}"
      session = Session.instances[session.inspect]
      session.login_proc.call if session && session.login_proc
    end

    LOGGED_OUT = Proc.new do |session|
      #puts "logged out: #{session.inspect}"
      session = Session.instances[session.inspect]
      session.logout_proc.call if session && session.logout_proc
    end

    METADATA_UPDATED = Proc.new do |session|
      puts "metadata updated: #{session.inspect}"
    end

    CONNECTION_ERROR = Proc.new do |session, error|
      puts "connection error: #{session.inspect}, #{error.inspect}"
    end

    MESSAGE_TO_USER = Proc.new do |session, message|
      #puts "message to user: #{session.inspect}, #{message.inspect}"
      session = Session.instances[session.inspect]
      session.message_to_user_proc.call(message) if session && session.message_to_user_proc
    end

    NOTIFY_MAIN_THREAD = Proc.new do |session|
      puts "notify main thread: #{session.inspect}"
    end

    LOG_MESSAGE = Proc.new do |session, message|
      puts "log message: #{session.inspect}, #{message.inspect}"
    end

    def self.instances
      @@instances ||= {}
    end

    def initialize(args = {})
      args[:cache_location]     ||= 'tmp'
      args[:settings_location]  ||= 'tmp'
      args[:application_key]    ||= ''
      args[:user_agent]         ||= 'GreenStripes'

      callbacks = ::GreenStripes::FFI::SessionCallbacks.new
      callbacks[:logged_in]           = LOGGED_IN
      callbacks[:logged_out]          = LOGGED_OUT
      #callbacks[:metadata_updated]    = METADATA_UPDATED
      #callbacks[:connection_error]    = CONNECTION_ERROR
      callbacks[:message_to_user]     = MESSAGE_TO_USER
      #callbacks[:notify_main_thread]  = NOTIFY_MAIN_THREAD
      #callbacks[:log_message]         = LOG_MESSAGE

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

      Session.instances[@ptr.inspect] = self
      #puts self.inspect
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
      # TODO: return int pointed to by int_ptr
    end

    def connection_state
      ::GreenStripes::FFI.sp_session_connectionstate(@ptr)
    end

    attr_accessor :login_proc
    def on_login(&block)
      @login_proc = Proc.new(&block)
    end

    attr_accessor :logout_proc
    def on_logout(&block)
      @logout_proc = Proc.new(&block)
    end

    attr_accessor :message_to_user_proc
    def on_message_to_user(&block)
      @message_to_user_proc = Proc.new(&block)
    end
  end
end
