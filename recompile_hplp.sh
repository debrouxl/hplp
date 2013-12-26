#! /bin/sh

# Maintainer script for automating the compilation and installation of hplp
# from a checkout of the complete hplp repository over at Github.
# Simplified from the tilp / gfm script (recompile_tilp.sh).
#
# Copyright (C) 2010, 2011, 2012, 2013 Lionel Debroux

# The prefix where the binaries will be installed, e.g. $HOME, /usr, /usr/local.
PREFIX="$HOME"

# Common flags and definitions.
CCFLAGS="-fstack-protector-all --param=ssp-buffer-size=1 -fsanitize=address"
CCPPFLAGS="-D_FORTIFY_SOURCE=2"

# Configure and build the given module
handle_one_module() {
  module_name="$1"
  shift # Kick the first argument, so as to be able to pass the rest to configure.

  cd "$module_name"
  echo "Configuring $module_name"
  rm -f config.cache
  ./configure CPPFLAGS="$CCPPFLAGS" CFLAGS="$CCFLAGS $CFLAGS" CXXFLAGS="$CCFLAGS $CFLAGS" --prefix="$PREFIX" $@ || return 1
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


echo "=== libhpcalcs ==="
handle_one_module libhpcalcs || exit 1
