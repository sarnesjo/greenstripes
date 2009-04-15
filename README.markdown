# GreenStripes: Ruby bindings for libspotify

GreenStripes provides Ruby bindings for [libspotify][1], letting you write
programs like this one:

[1]: http://developer.spotify.com/libspotify/

    session = GreenStripes::Session.new(APPLICATION_KEY, 'GreenStripes', 'tmp', 'tmp')
    session.login(USERNAME, PASSWORD)
    session.process_events until session.connection_state == GreenStripes::ConnectionState::LOGGED_IN
    
    search = GreenStripes::Search.new(session, 'yakety sax', 0, 100)
    session.process_events until search.loaded?
    puts "found #{search.num_tracks} tracks"
    if search.num_tracks > 0
      track = search.track(0)
      session.process_events until track.loaded? and track.artist(0).loaded?
      puts "the first one is #{track.name} by #{track.artist(0).name}"
    end
    
    session.logout
    session.process_events until session.connection_state == GreenStripes::ConnectionState::LOGGED_OUT

## Installing GreenStripes

First off, make sure that you have libspotify installed and set up correctly,
and that you have a valid application key. An easy way of doing this is
compiling and running one of the example programs that come with libspotify.

The easiest way of installing GreenStripes is via RubyGems:

    $ sudo gem install sarnesjo-greenstripes --source http://gems.github.com

(If you've already added GitHub as a gem source, you can skip the `--source`
flag.)

If the installation fails with about a million errors, make sure you've
installed libspotify somewhere gcc will look, like `/usr/local`.

## Working with GreenStripes

GreenStripes is intended to provide a fairly straight mapping from C to Ruby.
Since libspotify is written in a rather object-oriented way, this isn't too
awkward.

For example, functions dealing with session management in libspotify have names
beginning with `sp_session_`, and take a `sp_session *` as their first argument.
In GreenStripes, these functions become methods in the `Session` class, and the
first argument becomes `self`.

Class and method naming is mostly intuitive. The libspotify function
`sp_session_process_events` becomes the method `Session#process_events`, etc.
Some name changes have been introduced to give a more Ruby-esque feel to the
API. For example, libspotify functions with names ending with `_is_loaded`
become `loaded?` methods.

There are a few exceptions to these rules, however. For details, refer to the
API docs.

### Application keys

The `Session#new` method expects as its first argument your libspotify
application key as a binary string. An easy way of creating this string, is
using `Array#pack`, like this:

    APPLICATION_KEY = [0x00, ..., 0x00].pack('C*')

## Caveats

GreenStripes is not stable or complete in any way. Please be aware that the API
may change rapidly, and that it currently lacks much of the functionality in
libspotify.

Currently, GreenStripes provides mapping for all libspotify datatypes, functions
and constants, except those dealing with audio or images. It also lacks callback
support of any kind. It probably leaks memory, it is not thread-safe, and its
test coverage is incomplete. Hopefully, all of these issues will be solved in
the near future.

## Copyright

Copyright (c) 2009 Jesper Särnesjö. GreenStripes is available under the terms of
the MIT license (see LICENSE for details). However, it's not good for much
without libspotify, which comes with a much less permissive license.
