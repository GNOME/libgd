if get_option('with-tagged-entry')
  foreach t : ['test-tagged-entry', 'test-tagged-entry-2']
    executable(t, t + '.c', dependencies : libgd_dep)
  endforeach
endif

if (get_option('with-main-icon-view') or
    get_option('with-main-list-view') or
    get_option('with-main-view'))
  executable('test-styled-text-renderer',
    'test-styled-text-renderer.c',
    dependencies: libgd_dep)
endif
