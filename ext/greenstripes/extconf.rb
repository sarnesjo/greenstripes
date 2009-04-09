require 'mkmf'
dir_config('libspotify')
have_library('spotify', 'sp_session_init')
create_makefile("greenstripes")
