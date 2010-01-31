# GreenStripes

## Status

* Spotify types & structs
* Error Handling
  * sp_error_message
* Session handling
  * sp_session_init
  * sp_session_login
  * sp_session_user
  * sp_session_logout
  * sp_session_connectionstate
  * sp_session_userdata
  * sp_session_process_events
  * sp_session_player_load
  * sp_session_player_seek
  * sp_session_player_play
  * sp_session_player_unload
  * sp_session_playlistcontainer
* Links
  * sp_link_create_from_string
  * sp_link_create_from_track
  * sp_link_create_from_album
  * sp_link_create_from_artist
  * sp_link_create_from_search
  * sp_link_create_from_playlist
  * sp_link_as_string
  * sp_link_type
  * sp_link_as_track
  * sp_link_as_track_and_offset
  * sp_link_as_album
  * sp_link_as_artist
  * sp_link_add_ref
  * sp_link_release
* Track subsystem
  * sp_track_is_loaded
  * sp_track_error
  * sp_track_is_available
  * sp_track_num_artists
  * sp_track_artist
  * sp_track_album
  * sp_track_name
  * sp_track_duration
  * sp_track_popularity
  * sp_track_disc
  * sp_track_index
  * sp_track_add_ref
  * sp_track_release
* Album subsystem
  * sp_album_is_loaded
  * sp_album_is_available
  * sp_album_artist
  * sp_album_cover
  * sp_album_name
  * sp_album_year
  * sp_album_type
  * sp_album_add_ref
  * sp_album_release
* Artist subsystem
  * sp_artist_name
  * sp_artist_is_loaded
  * sp_artist_add_ref
  * sp_artist_release
* Album browsing
  * sp_albumbrowse_create
  * sp_albumbrowse_is_loaded
  * sp_albumbrowse_error
  * sp_albumbrowse_album
  * sp_albumbrowse_artist
  * sp_albumbrowse_num_copyrights
  * sp_albumbrowse_copyright
  * sp_albumbrowse_num_tracks
  * sp_albumbrowse_track
  * sp_albumbrowse_review
  * sp_albumbrowse_add_ref
  * sp_albumbrowse_release
* Artist browsing
  * sp_artistbrowse_create
  * sp_artistbrowse_is_loaded
  * sp_artistbrowse_error
  * sp_artistbrowse_artist
  * sp_artistbrowse_num_portraits
  * sp_artistbrowse_portrait
  * sp_artistbrowse_num_tracks
  * sp_artistbrowse_track
  * sp_artistbrowse_num_albums
  * sp_artistbrowse_album
  * sp_artistbrowse_num_similar_artists
  * sp_artistbrowse_similar_artist
  * sp_artistbrowse_biography
  * sp_artistbrowse_add_ref
  * sp_artistbrowse_release
* Image handling
  * sp_image_create
  * sp_image_add_load_callback
  * sp_image_remove_load_callback
  * sp_image_is_loaded
  * sp_image_error
  * sp_image_format
  * sp_image_data
  * sp_image_image_id
  * sp_image_add_ref
  * sp_image_release
* Search subsystem
  * sp_search_create
  * sp_search_is_loaded
  * sp_search_error
  * sp_search_num_tracks
  * sp_search_track
  * sp_search_num_albums
  * sp_search_album
  * sp_search_num_artists
  * sp_search_artist
  * sp_search_query
  * sp_search_did_you_mean
  * sp_search_total_tracks
  * sp_search_add_ref
  * sp_search_release
* Playlist subsystem
  * sp_playlist_is_loaded
  * sp_playlist_add_callbacks
  * sp_playlist_remove_callbacks
  * sp_playlist_num_tracks
  * sp_playlist_track
  * sp_playlist_name
  * sp_playlist_rename
  * sp_playlist_owner
  * sp_playlist_is_collaborative
  * sp_playlist_set_collaborative
  * sp_playlist_has_pending_changes
  * sp_playlist_add_tracks
  * sp_playlist_remove_tracks
  * sp_playlist_reorder_tracks
  * sp_playlistcontainer_add_callbacks
  * sp_playlistcontainer_remove_callbacks
  * sp_playlistcontainer_num_playlists
  * sp_playlistcontainer_playlist
  * sp_playlistcontainer_add_new_playlist
  * sp_playlistcontainer_add_playlist
  * sp_playlistcontainer_remove_playlist
  * sp_playlistcontainer_move_playlist
* User handling
  * sp_user_canonical_name
  * sp_user_display_name
  * sp_user_is_loaded
* Toplist handling
  * sp_toplistbrowse_create
  * sp_toplistbrowse_is_loaded
  * sp_toplistbrowse_error
  * sp_toplistbrowse_add_ref
  * sp_toplistbrowse_release
  * sp_toplistbrowse_num_artists
  * sp_toplistbrowse_artist
  * sp_toplistbrowse_num_albums
  * sp_toplistbrowse_album
  * sp_toplistbrowse_num_tracks
  * sp_toplistbrowse_track
