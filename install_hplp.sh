#! /bin/bash

# This script, aimed at users, automates the compilation and installation of hplp
# from the Git repository.
# Simplified from the tilp / gfm script (install_tilp.sh).
#
# **********
# IMPORTANT:
# **********
#     * please read below for prerequisites (build dependencies) or peculiarities (e.g. 64-bit Fedora).
#     * you should remove equivalent packages, if any, before running this script.
#
# Copyright (C) Lionel Debroux 2009, 2010, 2011, 2012, 2013


# ********************************************************
# MANDATORY dependencies for compiling and running libhp*:
# ********************************************************
# (Debian and Fedora package names are given as examples, install respectively with `apt-get install ...` and `yum install ...`)
# * Git (git, git)
# * Suitable C compiler + C++ compiler with decent C11 support:
#      * GCC + G++: (gcc + g++, gcc + gcc-c++), version 4.6 or later, 4.7 or later preferred
#      * Clang (clang, clang), version 3.1 and later.
# * GNU make (make, make). BSD make might work.
#   (on Debian, you can install "build-essential" to get gcc, g++ and make)
# * pkg-config (pkg-config, pkgconfig)
# * GNU autoconf (autoconf, autoconf)
# * GNU automake (automake, automake)
# * GNU libtool (libtool, libtool)
# * hidapi development files (*at least on Debian, you need to compile them yourself*)
# * GNU gettext (gettext, gettext)
# * XDG utils (xdg-utils, xdg-utils)
# * intltool (intltool)


# ******************************************************************************
# Default prefix where the binaries will be installed, e.g.
# $HOME, /usr, /usr/local, /opt/hplp.
# Note that you can set the value of PREFIX interactively through e.g.:
# $ PREFIX="$HOME" <path>/install_hplp.sh
# ******************************************************************************

# IMPORTANT NOTES:
# ----------------
# * for compilation to succeed, you may have to execute
# $ export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:[{$PREFIX}]/lib/pkgconfig
# (where [{$PREFIX}] is the contents of the PREFIX line below, without the quotes).
# The main cause for having to execute this line is installing to e.g. PREFIX=$HOME or /usr/local,
# but it may be necessary when installing to PREFIX=/usr, if your distro doesn't store
# libraries into the standard /usr/lib path.
#
# * after successful installation, you may have to add $PREFIX/bin to $PATH,
# and $PREFIX/lib to $LD_LIBRARY_PATH, for the SVN versions of libhp*
# to get picked up.
if [ "x$PREFIX" = "x" ]; then
    PREFIX="/usr"
fi
echo Will use "PREFIX=$PREFIX"

# ******************************************************************************
# Default place where the sources will be stored, if it's not de
# Note that you can set the value of SRCDIR thusly:
# $ SRCDIR="/opt/src" <path>/install_hplp.sh
# ******************************************************************************
if [ "x$SRCDIR" = "x" ]; then
    SRCDIR="$HOME/lpg"
fi
echo Will use "SRCDIR=$SRCDIR"


# ******************************************************************************
# Default values for the C and C++ compilers, if these variables are not set
# in the environment or the command-line (before the invocation of install_hplp.sh).
# ******************************************************************************
if [ "x$CC" = "x" ]; then
    #CC=clang
    CC=gcc
    CFLAGS="-std=c99 -Os -g3 -Wall -W -Wno-unused-parameter -Wshadow -Wwrite-strings"
fi
if [ "x$CXX" = "x" ]; then
    #CXX=clang++
    CXX=g++
    CFLAGS="-std=c99 -Os -g3 -Wall -W -Wno-unused-parameter -Wshadow -Wwrite-strings"
fi

# No special linker flags, by default.
LDFLAGS=""


# Subroutine: clone/update repository copies.
handle_repository_copies() {
  module_name="$1"
  if [ -d "$module_name" -a -d "$module_name/.git" ]; then
    echo "Updating $module_name"
    cd "$module_name"
    git pull || return 1
    cd ..
  else
    echo "Checking out $module_name"
    git clone "https://github.com/debrouxl/$module_name" "$module_name" || return 1
  fi
}

# Subroutine: checkout/update, `configure`, `make` and `make install` the given module
handle_one_module() {
  module_name="$1"
  shift # Kick the first argument, so as to be able to pass the rest to configure.

  cd "$module_name"
  echo "Configuring $module_name"
  # Add --libdir=/usr/lib64 on e.g. 64-bit Fedora 14, which insists on searching for 64-bit libs in /usr/lib64.
  # Or modify PKG_CONFIG_PATH as described above.
  autoreconf -i -v -f
  ./configure "--prefix=$PREFIX" "CC=$CC" "CXX=$CXX" "CPPFLAGS=$CPPFLAGS" "CFLAGS=$CFLAGS" "CXXFLAGS=$CXXFLAGS" "LDFLAGS=$LDFLAGS" $@ || return 1
  echo "Building $module_name"
  make || return 1
  echo "Checking $module_name"
  make check || return 1
  echo "Installing $module_name"
  make install || return 1
  cd -
}

# Subroutine: perform quick rough sanity check on compilers and PREFIX.
rough_sanity_checks() {
  echo "Performing a quick rough sanity check on compilers"
  # Test CC, which also checks whether the user can write to SRCDIR
  cat << EOF > "$SRCDIR/hplp/hello.c"
#include <stdio.h>

int main(int argc, char * argv[]) {
    printf("Hello World !\n");
    return 0;
}
EOF

  "$CC" "$SRCDIR/hplp/hello.c" -o "$SRCDIR/hplp/hello" || exit 1
  "$SRCDIR/hplp/hello" || exit 1
  echo "CC=$CC exists"
  # Test CXX, which also checks whether the user can write to SRCDIR
  cat << EOF > "$SRCDIR/hplp/hello.cc"
#include <cstdio>

int main(int argc, char * argv[]) {
    printf("Hello World !\n");
    return 0;
}
EOF

  "$CXX" "$SRCDIR/hplp/hello.cc" -o "$SRCDIR/hplp/hello" || exit 1
  "$SRCDIR/hplp/hello" || exit 1
  echo "CXX=$CXX exists"

  echo "Checking whether $PREFIX can be written to"
  TEMPFILE=`mktemp $PREFIX/XXXXXXXXXXX`
  if [ "$?" -ne 0 ]; then
    echo -e "\033[1mNo, cannot write to $PREFIX. Perhaps you need to run the script as root ?\nAborting.\033[m"
    exit 1
  fi
  cat << EOF > "$TEMPFILE"
This is a test file
EOF
  if [ "$?" -ne 0 ]; then
    echo -e "\033[1mNo, cannot write to $PREFIX. Perhaps you need to run the script as root ?\nAborting.\033[m"
    exit 1
  fi
  rm "$TEMPFILE"
}

# The main part of the script starts here.
echo -e "\033[4mBefore proceeding further, make sure that you're ready to go (look inside the install script):\033[m"
echo -e "1) configured \033[1mPREFIX\033[m and \033[1mSRCDIR\033[m the way you wish"
echo -e "   (as well as \033[1mCC\033[m and \033[1mCXX\033[m if you're into using non-GCC compilers);"
echo -e "2a) if you're using \033[1m64-bit Fedora\033[m (or any distro which installs libraries to non-standard paths), added --libdir=/usr/lib64 to the marked line, or..."
echo -e "2b) configured \033[1mPKG_CONFIG_PATH\033[m if necessary"
echo    "3) installed the build dependencies listed in the script."
echo    "      - For instance, on Debian and derivatives, you would run:"
echo    "        (sudo) apt-get install build-essential git autoconf automake libtool gettext xdg-utils"
echo -e "        \033[1mthen you would proceed to compile hidapi yourself, as Debian does not package it\033[m"
echo    "      - On MacOS X, for instance:"
echo    '        ruby -e "$(curl -fsSL https://raw.github.com/mxcl/homebrew/go)" # to obtain brew if you do not have it yet'
echo    "        brew install git autoconf automake libtool gettext hidapi && brew link --force gettext"
echo -e "        \033[1mthen you will need to adjust some CFLAGS and LDFLAGS due to e.g. gettext\033[m"
echo -e "\033[4mOtherwise, the build will fail!\033[m."
echo -e "\033[1mENTER to proceed, CTRL + C to abort\033[m."
read

echo "Creating output folder if necessary"
mkdir -p "$SRCDIR/hplp" || exit 1

rough_sanity_checks

cd "$SRCDIR/hplp"
echo "=== Downloading hplibs ==="
handle_repository_copies hplp

# Use --with-kde if you want to use the native KDE file dialogs (it defaults to disabled because it requires a slew of development package dependencies).
echo "=== libhpcalcs ==="
handle_one_module hplp/libhpcalcs || exit 1

echo "==================================================="
echo "========== libhp* installed successfully =========="
echo "==================================================="
echo -e "\033[1mUnder Linux, if you want to use test_hpcalcs without being root, don't forget to create udev definitions (as described in the \033[4mREADME\033[m\033[1m) !\033[m"
