gnome = import('gnome')

sources = [ 'gd.h',
            'gd-types-catalog.c' ]
built_sources = []
c_args = []

if get_variable('libgd_gtk_hacks', false)
    sources += [ 'gd-entry-focus-hack.c',
                 'gd-icon-utils.c' ]
    c_args += '-DLIBGD_GTK_HACKS=1'
endif

build_view_generic = (get_variable('libgd_main_icon_view', false) or
                      get_variable('libgd_main_list_view', false) or
                      get_variable('libgd_main_view', false))

if build_view_generic
    sources += [ 'gd-main-view-generic.c',
                 'gd-styled-text-renderer.c',
                 'gd-two-lines-renderer.c' ]
    c_args += '-DLIBGD__VIEW_COMMON=1'
endif

build_icon_view = (get_variable('libgd_main_icon_view', false) or
                   get_variable('libgd_main_view', false))
if build_icon_view
    sources += [ 'gd-main-icon-view.c',
                 'gd-toggle-pixbuf-renderer.c' ]
    c_args += '-DLIBGD_MAIN_ICON_VIEW=1'
endif

build_list_view = (get_variable('libgd_main-list_view', false) or
                   get_variable('libgd_main_view', false))
if build_list_view
    sources += [ 'gd-main-list-view.c' ]
    c_args += '-DLIBGD_MAIN_LIST_VIEW=1'
endif

if get_variable('libgd_main_view', false)
    sources += [ 'gd-main-view.c' ]
    c_args += '-DLIBGD_MAIN_VIEW=1'
endif

if get_variable('libgd_main_toolbar', false)
    sources += [ 'gd-main-toolbar.c' ]
    c_args += '-DLIBGD_MAIN_TOOLBAR=1'
endif

if get_variable('libgd_header_bar', false)
    sources += [ 'gd-header-bar.c' ]
    c_args += '-DLIBGD_HEADER_BAR=1'
endif

if get_variable('libgd_header_button', false)
    sources += [ 'gd-header-button.c' ]
    c_args += '-DLIBGD__HEADER_BUTTON=1'
endif

if get_variable('libgd_margin_container', false)
    sources += [ 'gd-margin-container.c' ]
    c_args += '-DLIBGD_MARGIN_CONTAINER=1'
endif

if get_variable('libgd_tagged_entry', false)
    gdres = gnome.compile_resources('gd-tagged-entry-resources',
                                    'gd-tagged-entry.gresource.xml',
                                    export : true,
                                    sources_dir : 'libgd')
    built_sources += [gdres]
    sources += [ gdres,
                 'gd-tagged-entry.c' ]
    c_args += '-DLIBGD_TAGGED_ENTRY=1'
endif

if get_variable('libgd_notification', false)
    sources += [ 'gd-notification.c' ]
    c_args += '-DLIBGD_NOTIFICATION=1'
endif

if get_variable('libgd_revealer', false)
    sources += [ 'gd-revealer.c' ]
    c_args += '-DLIBGD_REVEALER=1'
endif

if get_variable('libgd_stack', false)
    sources += [ 'gd-stack.c',
                 'gd-stack-switcher.c' ]
    c_args += '-DLIBGD_STACK=1'
endif

libgd = shared_library('gd',
                       sources,
                       include_directories: include,
                       dependencies: [gtk, libm],
                       c_args: c_args)
if get_variable('libgd_static', false)
    libgd_lib = static_library('gd',
                               sources,
                               include_directories: include,
                               dependencies: [gtk, libm],
                               c_args: c_args)
else
    libgd_lib = libgd
endif

if get_variable('libgd_gir', false) or get_variable('libgd_vapi', false)
    gir = gnome.generate_gir(libgd,
                             sources : sources,
                             nsversion : '1.0',
                             namespace : 'Gd',
                             symbol_prefix : 'gd',
                             identifier_prefix : 'Gd',
                             includes : 'Gtk-3.0')
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

libgd_dep = declare_dependency(link_with: libgd_lib,
                               include_directories: include,
                               compile_args: c_args,
                               sources: built_sources)
