About the plugin
================

Changes and versions
--------------------

**Version 1.4, released Jul 18 2013**

In this version one bug in folder context menu has been resolved. Now this menu is displayed 
properly with all items that dropbox provides.

**Version 1.3, released Jul 7 2013**

Since this version it's not possible to generate links to albums, modern Dropbox versions
could generate "album" for literally any folder inside your Dropbox account, so use 
context menu option "Share folder and copy link".

Also starting with version 1.3 plugin doesn't require web browser to generate "shared" link,
it's copied to clipboard just after you select "Share folder/file and copy link" option
in the context menu.

Important notice
----------------

This plugin does **not** include Dropbox client itself, it's just a plugin for Dolphin File Manager.

Also pay attention that plugin usually is working correctly only with the recent versions of dropbox
applications, so very old version won't work properly.

You have to install Dropbox application separately: either by downloading archive and unpacking to some directory or by using installation package provided for your system. For example, for Debian you may want to install package `nautilus-dropbox`, it will automatically download and install Dropbox application.

How to use
----------

You need Dolphin installed, of course. Then just install dolphin-dropbox-plugin, open Dolphin, go to menu *Settings* → *Configure Dolphin...*, then go to page *Services* and turn on checkbox *Dropbox* in the services list (usually it's enabled by default).

After that you'll get context menu for files and folders in Dropbox directory. Plugin provides basic dropbox services:

* generating link to file from directory Public;
* generating link to any file or folder from Dropbox folder (so called “shareable link”);
* viewing file modifications history;
* and some more less important feautures.

Development notes
=================

To compile project you first need to install kde development packages:

    sudo apt-get install kdelibs5-dev cmake

Compilation:

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/usr ..

How to build debian package:

    sh prepare-deb-build.sh _deb

After that you'll find just built package in the directory _deb.
