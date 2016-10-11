#
# Copyright (c) 2016 Pelagicore AB.
#
# Author: Zeeshan Ali <zeeshanak@gnome.org>
#
# This build file currently is not very useful, as is. There is currently no way
# in meson for the main project to pass any flags/options to subproject's build
# and hence it can't choose which components to build, which is what all users
# of libgd currently do.
#
# However, you can build libgd standalone with it:
#
# $ mkdir build
# $ cd build
# $ meson # choose components to build using -Doption=true paremeters
#         # Check meson.options for list of options
# $ ninja # ninja-build on Fedora
#

project('libgd', 'c')

gtk = dependency('gtk+-3.0')
cc = meson.get_compiler('c')
libm = cc.find_library('m', required: false)

sources = [ 'libgd/gd.h',
            'libgd/gd-types-catalog.c',
            'libgd/gd-types-catalog.h' ]

if get_option('libgd-gtk-hacks')
    sources += [ 'libgd/gd-entry-focus-hack.c',
                 'libgd/gd-entry-focus-hack.h',
                 'libgd/gd-icon-utils.c',
                 'libgd/gd-icon-utils.h' ]
endif

build_view_generic = (get_option('libgd-main-icon-view') or
                      get_option('libgd-main-list-view') or
                      get_option('libgd-main-view'))

if build_view_generic
    sources += [ 'libgd/gd-main-view-generic.c',
                 'libgd/gd-main-view-generic.h',
                 'libgd/gd-styled-text-renderer.c',
                 'libgd/gd-styled-text-renderer.h',
                 'libgd/gd-two-lines-renderer.c',
                 'libgd/gd-two-lines-renderer.h' ]
endif

build_icon_view = (get_option('libgd-main-icon-view') or
                   get_option('libgd-main-view'))
if build_icon_view
    sources += [ 'libgd/gd-main-icon-view.c',
                 'libgd/gd-main-icon-view.h',
                 'libgd/gd-toggle-pixbuf-renderer.c',
                 'libgd/gd-toggle-pixbuf-renderer.h' ]
endif

build_list_view = (get_option('libgd-main-list-view') or
                   get_option('libgd-main-view'))
if build_list_view
    sources += [ 'libgd/gd-main-list-view.c',
                 'libgd/gd-main-list-view.h' ]
endif

if get_option('libgd-main-view')
    sources += [ 'libgd/gd-main-view.c',
                 'libgd/gd-main-view.h' ]
endif

if get_option('libgd-main-toolbar')
    sources += [ 'libgd/gd-main-toolbar.c',
                 'libgd/gd-main-toolbar.h' ]
endif

if get_option('libgd-header-bar')
    sources += [ 'libgd/gd-header-bar.c',
                 'libgd/gd-header-bar.h' ]
endif

if get_option('libgd-header-button')
    sources += [ 'libgd/gd-header-button.c',
                 'libgd/gd-header-button.h' ]
endif

if get_option('libgd-margin-container')
    sources += [ 'libgd/gd-margin-container.c',
                 'libgd/gd-margin-container.h' ]
endif

if get_option('libgd-notification')
    sources += [ 'libgd/gd-notification.c',
                 'libgd/gd-notification.h' ]
endif

if get_option('libgd-revealer')
    sources += [ 'libgd/gd-revealer.c',
                 'libgd/gd-revealer.h' ]
endif

add_global_arguments('-I' + meson.source_root(), language: 'c')
add_global_arguments('-I' + meson.source_root() + '/libgd', language: 'c')

libgd = shared_library('gd', sources, dependencies: [gtk, libm])
if get_option('libgd-static')
    libgd_lib = static_library('gd', sources, dependencies: [gtk, libm])
else
    libgd_lib = libgd
endif

if get_option('libgd-gir') or get_option('libgd-vapi')
    gnome = import('gnome')
    gir = gnome.generate_gir(libgd,
                             sources : sources,
                             nsversion : '1.0',
                             namespace : 'Gd',
                             symbol_prefix : 'gd',
                             identifier_prefix : 'Gd',
                             includes : 'Gtk-3.0')

    if get_option('libgd-vapi')
        vapigen = find_program('vapigen')
        vapi = custom_target('gd-1.0.vapi',
                             output: 'gd-1.0.vapi',
                             input: gir,
                             command: [vapigen, '--library', 'gd-1.0',
                                                '--pkg', 'gtk+-3.0',
                                                '@INPUT@'],
                             install: false)
    endif
endif

include = include_directories(meson.source_root())

# If you use libgd as meson subproject in your project, you'll want to make use
# of libgd_dep variable
libgd_dep = declare_dependency(link_with: libgd_lib,
                               include_directories: include)
