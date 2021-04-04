
                                Gem Drop Deluxe

                               by Bill Kendrick
                              New Breed Software
                           August & September, 1997
              Ported to C, July-October, 2015 & March-April 2021
                                   $VERSION

                           bill@newbreedsoftware.com
                   http://www.newbreedsoftware.com/gemdrop/
                https://github.com/billkendrick/gemdrop_deluxe

Welcome to "Gem Drop," a fast-paced puzzle game for the Atari 8-bit from
New Breed Software!

If you want to dive right in to the game, feel free to skim over the
controls listed below and try out the game!  It's simple enough to figure
out yourself!

TITLE/MENU CONTROLS:

 [START]  - Begin game
 Fire     - Begin game
 [SELECT] - Choose starting level
 [OPTION] - Toggle controller type
            (Atari (one-button joystick) or Sega (two-button support))
 [F]      - Toggle the in-game SuperIRG flicker effect
 [ESC]    - Quit to DOS


GAME CONTROLS:

Joystick:

  Left / Right - Move
  Down / Fire  - Grab gem/object
  Up           - Throw carried objects

Sega Genesis 3-button Controller:

  Left / Right    - Move
  Down / B-button - Grab gem/object
  Up   / C-butotn - Throw carried objects

Keyboard: (arrow keys, w/o [CONTROL])

  Left / Right - Move
  Down         - Grab gem/object
  Up           - Throw carried objects

Other key controls:

  START   - Abort
  SELECT  - Abort
  OPTION  - Abort
  [ESC]   - Abort
  [SPACE] - Pause
  [F]     - Toggle SuperIRG flicker effect on/off
            (also works during pause)


OBJECTIVE:

3-In-A-Row:

The point of the game is to grab gems (which are appearing at the top
of the screen) and then throw them back up to get three or more in a
vertical column.  When you grab a gem, you will also pick up any gems
of the same type that are directly above it.  This means you sometimes
can't take just one gem...

You can tell how many gems you are carrying by looking at the little guy
at the bottom of the screen.  If you carry none, you'll see none.  If you
carry 1 or 2, you'll see gems on one or both sides of him.  If you carry
3 or more, you'll just see a collection of gems.  (Knowing whether or
not you have more than 3 doesn't really matter, since it will definitely
be a match anyway.)

Getting a match of three or more gets rid of that column of matching gems,
as well as any matching gems adjacent to the column, and gems adjacent
to those gems, and so on.  This makes for neat chain reactions!

Scoring:

The more gems in a match, the higher score you get for that match.
Be warned... if you always match three in a row, you'll finish the level
more quickly, but... if you always match three in a row, you won't get
as high a score!  You'll have to decide when it's more important to get
rid of lots of GEMS or lots of MATCHES.

Winning:

When you get enough matches ("lines") you advance to the next level.
The number of lines you've made so far, and the number of lines you
need to make to complete the current level are listed on the left side
of the screen, below your current score.

There are 20 unique levels total.  Levels 15 through 20 become harder
because you have twice as many different gems to deal with, making
getting matches that much more difficult!  (You'll especially hate one
of the pieces I've created!)

Note: When you first start playing Gem Drop, only levels 1 through 5
are available.  As you advance through higher levels, you will then be
able to select them later.  (ie, if you've beaten EVERY level, you will
then be able to select any level.)

Note: The stand-alone XEX version does not do any disk I/O, so does not save
its state.  Use the "gemdropd.xex" version with your favorite DOS.
Or, use the "gemdrop.atr" disk image, which includes uDOS (ultra small DOS)
by Dietrich (see his ABBUC forum thread about it, found here:
http://www.abbuc.de/community/forum/viewtopic.php?f=3&t=10347)

Dying:

As time goes on, more gems appear at the top, pushing the rest down.
You'll know whe this is about to happen because the screen shakes and
you'll hear a rumbling noise.

If the screen fills up (the gems go beyond the bottom of the screen)
the game ends.

When you return to the title screen, the level you were just one will
be pre-selected, so you can keep trying.  But, of course, your score
will be reset to 0.

Special Objects:

There are also a number of non-gem objects.  To "activate" them, you
must throw two or more of the same gem onto them.

Note: You CANNOT grab and carry these special objects!

  Bomb - explodes, destroying any gems directly above, below, and to
         either side of it.

  Wildcard - it will match any gems next to it or above it, often causing
             nice chain-reactions!

  Clock - this freezes the game for a moment.  You still get to play,
          but no more new pieces will appear for a few seconds. (You'll
          hear a clicking sound until the clock stops.) This gives you
          some breathing room!

ABOUT GEM DROP:

Gem Drop is loosely based on the SNK Neo-Geo game "Magical Drop III."

Gem Drop was originally written in OSS's Action! programming language,
and 18 years later was ported to C and cross-compiled with CC65.
(Previous to that, shortly after it's initial release, it was ported to
C & libSDL and made available for Linux, Windows, Mac OS X, and a
variety of other platforms, as "Gem Drop X".)

Gem Drop uses a special software-based graphics mode to produce the high
amount of colors see on the screen: one 20x24 5-colored text mode with
one set of 5 colors and two character set fonts, which are toggled at
every vertical blank.  (This means NTSC machines (at 60 cycles) will
look better than PAL machines (at the slower 50 cycles).)

Press the [F] key to toggle this effect on & off at (almost) any time.

The fonts were created using my own simple font editor in BASIC.
(Amazingly, I was able to create these 4-pixel, 2-bits-per-pixel graphics
using an 8-pixel, 1-bpp editor!  It was pain-staking!)

ADDITIONAL CREDITS:

 * FIXME: RMT player
 * FIXME: Music

 * uDOS by Dietrich
   http://www.abbuc.de/community/forum/viewtopic.php?f=3&t=10347

 * Title / Help screen font based loosely on Ubuntu Mono
   https://fonts.google.com/specimen/Ubuntu+Mono

THE END!

Please tell me what you think of this game!  Thanks!

New Breed Software
c/o Bill Kendrick
bill@newbreedsoftware.com
http://www.newbreedsoftware.com/gemdrop/

