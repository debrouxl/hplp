hpcomm: embryonic code for communicating with recent HP calculators
===================================================================

As of 2013/10/20, hpcomm (still called hidtest, see below) is nothing more
than a proof of concept for communicating with a single HP Prime calculator
running "SDKV0.30" firmware version from mid-August 2013. It's not even
interactive.

However, it does successfully send and receive some packets, which means
that 1) the partial analysis was correct and 2) the packets sent to the
calculator are correct.

This early PoC is meant to become a possible foundation for a library dealing
with two-way communication with HP Prime calculators (and maybe 39gII, if
their protocol is similar ?) using USB HID class. This PoC does not aim at
duplicating / replacing code for older HP calculators.
The code leverages HIDAPI, and it was strongly inspired by the time-proven
concepts and APIs of the libti* family, which I (Lionel Debroux) am the
current maintainer of.
The libti* family forms the back-end for TILP, TIEmu, TilEm and titools
(mainly), and contains the only third-party FLOSS code with two-way
communication support for all three of TI-Z80, TI-68k and TI-Nspire
graphing calculators.
Putting together a minimal, incomplete framework strongly inspired by libti*
was a matter of several hours of work, but having such a foundation clearly
saves space in the longer term :)

Build process
-------------
For now, hidtest.cpp is meant to be dropped into an HIDAPI source tree
(from https://github.com/signal11/hidapi.git ) and built from there.
Yeah, this badly needs improvement, but it was quicker this way, and
actually enabled me to get something out of the door in several hours,
i.e. before the end of the week-end ;)

TODO list
---------
* hopefully attract contributors who have an actual HP Prime calculator
  (I don't) and have more free time than I do;
* make this a separate, autotools-based project;
* split the HP cable and calcs code across multiple files and headers;
* add comments to the code (they will look like those in libti*);
* somehow gain better knowledge of the protocol, so as to be able to...
* ... add more operations (several others are known, see commented code
  snippet in main). Finish the rpkt / vpkt code in the process.
* add support for Prime in MSD mode, for firmware upgrades ? I've already
  partially analyzed the protocol and published my notes at
  http://tiplanet.org/hpwiki/index.php?title=HP_Prime/Linking_Protocol
  but I'm not really confident about it ;)
* certainly more :-)

License
-------
Copyright (C) 2013 Lionel Debroux

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2, as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.

Changelog
---------
2013/10/20: First public version.
2013/10/19: Project started.

Thanks
------
* Xavier "critor" Andréani for testing on his real Prime and providing USB
  packet dumps (which I analyzed with Wireshark);
* the other TI-Planet ( http://tiplanet.org/ ) admins.
* the HP Prime developers at HP: some of its members spend time attending
  message boards, even in communities traditionally interested in TI calcs
  (Omnimaga, Cemetech, TI-Planet).
