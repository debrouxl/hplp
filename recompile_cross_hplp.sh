#! /bin/sh

# Maintainer script for automating the cross-compilation and installation of hplp
# from a checkout of the complete hplp repository over at Github.
# Simplified from the tilp / gfm script.
#
# Copyright (C) 2010, 2011, 2012, 2013 Lionel Debroux, Benjamin Moody


# REMINDERS: don't forget to:
# 1) create AND chmod +x a "i686-w64-mingw32-pkg-config" script in the path:
# #!/bin/sh
# export PKG_CONFIG_LIBDIR=/usr/i686-w64-mingw32/lib
# exec pkg-config $@
# (this is for my Debian Wheezy/sid, at least)
#
# 2) have Wine registered through binfmt-support if it wasn't already done:
# update-binfmts --install wine <path_to_wine> --magic MZ

# The prefix where the binaries will be installed, e.g. $HOME, /usr, /usr/local.
PREFIX="$HOME/lpg/packages"

# Common flags and definitions.
CCFLAGS="-Os -g3 -Wall -W -Wno-unused-parameter -Wshadow -Wwrite-strings -Wredundant-decls -Werror=declaration-after-statement -Werror=implicit-function-declaration -D_FORTIFY_SOURCE=2"
CCPPFLAGS="-I$HOME/lpg/depsprefix/include"
CLDFLAGS="-I$HOME/lpg/depsprefix/lib -static-libgcc"
CHOST="i686-w64-mingw32"

export PKG_CONFIG_PATH=$HOME/lpg/packages/lib/pkgconfig:$HOME/lpg/depsprefix/lib/pkgconfig:$PKG_CONFIG_PATH
export PKG_CONFIG_LIBDIR=$HOME/lpg/pkgconfig

# Configure and build the given module
handle_one_module() {
  module_name="$1"
  shift # Kick the first argument, so as to be able to pass the rest to configure.

  cd "$module_name"
  echo "Configuring $module_name"
  rm -f config.cache
  ./configure --host="$CHOST" CPPFLAGS="$CCPPFLAGS" CFLAGS="$CCFLAGS" CXXFLAGS="$CCFLAGS" LDFLAGS="$CLDFLAGS" --prefix="$PREFIX" $@ || return 1
  echo "Building $module_name"
  make clean || return 1
  make || return 1
  echo "Installing $module_name"
  make check || return 1
  make install || return 1
  cd -
}


echo "=== AUTORECONF ==="
sh run_autoreconf.sh

echo "=== UPDATEPOT ==="
sh run_updatepot.sh


sed "s/+ _nl_msg.*$//" -i libhpcalcs/configure


echo "=== libhpcalcs ==="
handle_one_module libhpcalcs || exit 1
