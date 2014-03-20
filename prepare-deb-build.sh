#!/bin/bash

if [ "$1" = "" ]; then
    echo "Dest dir required"
    exit 1
fi

if [ ! -e "$1" ]; then
    mkdir "$1"
fi

if [ ! -d "$1" ]; then
    echo "'$1' is not a directory"
    exit 1
fi

DESTDIR=$1
DEFDIR=$(pwd)
PRODNAME=dolphin-dropbox-plugin

echo $DEFDIR

# we need to prepare sources for build
rm -rf $DESTDIR/*

mkdir $DESTDIR/_tmp
cp -r * $DESTDIR/_tmp
cd $DESTDIR/_tmp
rm -rf prepare-deb-build.sh
mv debian ../_deb

# detect program version
cd $DEFDIR
DEB_VERSION=$(head -n1 debian/changelog | sed -e 's/^[^(]*(\([^)]*\)).*/\1/')
UP_VERSION=$(echo $DEB_VERSION | sed -e 's/-.*$//')

# prepare dirs
cd $DESTDIR

BASEDIR="${PRODNAME}-${UP_VERSION}"
mv _tmp "$BASEDIR"
tar czf "${PRODNAME}_${UP_VERSION}.orig.tar.gz" "$BASEDIR"
mv _deb "$BASEDIR"/debian

# ready to build
cd "$BASEDIR"
dpkg-buildpackage -rfakeroot
