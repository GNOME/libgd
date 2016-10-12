#
# Copyright (c) 2016 Pelagicore AB.
#
# Author: Zeeshan Ali <zeeshanak@gnome.org>
#
# libgd should be defined as a project itself to be used as meson subproject in
# other projects but that is currently not possible since meson doesn't
# currently provide a mechanism for the main project to pass any flags/options
# to subproject's build.
#
# For now, just treat libgd submodule repo like a normal subdir in your project
# and make use of 'libgd_dep' variable defined here to pass to 'dependencies'
# keywords.
#

# Uncomment following line (and remove this comment) to use libgd as meson
# subproject in future.
#project('libgd', 'c')

gtk = dependency('gtk+-3.0')
cc = meson.get_compiler('c')
libm = cc.find_library('m', required: false)

sources = [ 'libgd/gd.h',
            'libgd/gd-types-catalog.c',
            'libgd/gd-types-catalog.h' ]

c_args = []

if get_variable('libgd_gtk_hacks', false)
    sources += [ 'libgd/gd-entry-focus-hack.c',
                 'libgd/gd-entry-focus-hack.h',
                 'libgd/gd-icon-utils.c',
                 'libgd/gd-icon-utils.h' ]
    c_args += '-DLIBGD_GTK_HACKS=1'
endif

build_view_generic = (get_variable('libgd_main_icon_view', false) or
                      get_variable('libgd_main_list_view', false) or
                      get_variable('libgd_main_view', false))

if build_view_generic
    sources += [ 'libgd/gd-main-view-generic.c',
                 'libgd/gd-main-view-generic.h',
                 'libgd/gd-styled-text-renderer.c',
                 'libgd/gd-styled-text-renderer.h',
                 'libgd/gd-two-lines-renderer.c',
                 'libgd/gd-two-lines-renderer.h' ]
    c_args += '-DLIBGD__VIEW_COMMON=1'
endif

build_icon_view = (get_variable('libgd_main_icon_view', false) or
                   get_variable('libgd_main_view', false))
if build_icon_view
    sources += [ 'libgd/gd-main-icon-view.c',
                 'libgd/gd-main-icon-view.h',
                 'libgd/gd-toggle-pixbuf-renderer.c',
                 'libgd/gd-toggle-pixbuf-renderer.h' ]
    c_args += '-DLIBGD_MAIN_ICON_VIEW=1'
endif

build_list_view = (get_variable('libgd_main-list_view', false) or
                   get_variable('libgd_main_view', false))
if build_list_view
    sources += [ 'libgd/gd-main-list-view.c',
                 'libgd/gd-main-list-view.h' ]
    c_args += '-DLIBGD_MAIN_LIST_VIEW=1'
endif

if get_variable('libgd_main_view', false)
    sources += [ 'libgd/gd-main-view.c',
                 'libgd/gd-main-view.h' ]
    c_args += '-DLIBGD_MAIN_VIEW=1'
endif

if get_variable('libgd_main_toolbar', false)
    sources += [ 'libgd/gd-main-toolbar.c',
                 'libgd/gd-main-toolbar.h' ]
    c_args += '-DLIBGD_MAIN_TOOLBAR=1'
endif

if get_variable('libgd_header_bar', false)
    sources += [ 'libgd/gd-header-bar.c',
                 'libgd/gd-header-bar.h' ]
    c_args += '-DLIBGD_HEADER_BAR=1'
endif

if get_variable('libgd_header_button', false)
    sources += [ 'libgd/gd-header-button.c',
                 'libgd/gd-header-button.h' ]
    c_args += '-DLIBGD__HEADER_BUTTON=1'
endif

if get_variable('libgd_margin_container', false)
    sources += [ 'libgd/gd-margin-container.c',
                 'libgd/gd-margin-container.h' ]
    c_args += '-DLIBGD_MARGIN_CONTAINER=1'
endif

if get_variable('libgd_tagged_entry', false)
    sources += [ 'libgd/gd-tagged-entry.c',
                 'libgd/gd-tagged-entry.h' ]
    c_args += '-DLIBGD_TAGGED_ENTRY=1'
endif

if get_variable('libgd_notification', false)
    sources += [ 'libgd/gd-notification.c',
                 'libgd/gd-notification.h' ]
    c_args += '-DLIBGD_NOTIFICATION=1'
endif

if get_variable('libgd_revealer', false)
    sources += [ 'libgd/gd-revealer.c',
                 'libgd/gd-revealer.h' ]
    c_args += '-DLIBGD_REVEALER=1'
endif

if get_variable('libgd_stack', false)
    sources += [ 'libgd/gd-stack.c',
                 'libgd/gd-stack.h',
                 'libgd/gd-stack-switcher.c',
                 'libgd/gd-stack-switcher.h' ]
    c_args += '-DLIBGD_STACK=1'
endif

libgd = shared_library('gd',
                       sources,
                       dependencies: [gtk, libm],
                       c_args: c_args)
if get_variable('libgd_static', false)
    libgd_lib = static_library('gd',
                               sources,
                               dependencies: [gtk, libm],
                               c_args: c_args)
else
    libgd_lib = libgd
endif

built_sources = []

if get_variable('libgd_gir', false) or get_variable('libgd_vapi', false)
    gnome = import('gnome')
    gir = gnome.generate_gir(libgd,
                             sources : sources,
                             nsversion : '1.0',
                             namespace : 'Gd',
                             symbol_prefix : 'gd',
                             identifier_prefix : 'Gd',
                             includes : 'Gtk-3.0',
                             extra_args: ['-I' + meson.source_root(),
                                          '-I' + meson.current_source_dir(),
                                          '--c-include=libgd/gd.h'])
   built_sources += gir

    if get_variable('libgd_vapi', false)
        vapigen = find_program('vapigen')
        vapi = custom_target('gd-1.0.vapi',
                             output: 'gd-1.0.vapi',
                             input: gir,
                             command: [vapigen, '--library', 'gd-1.0',
                                                '--pkg', 'gtk+-3.0',
                                                '@INPUT@'],
                             install: false)
        built_sources += vapi
    endif
endif

include = include_directories(meson.source_root(),
                              meson.current_source_dir())

libgd_dep = declare_dependency(link_with: libgd_lib,
                               include_directories: include,
                               compile_args: c_args,
                               sources: built_sources)
