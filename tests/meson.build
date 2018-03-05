if get_option('with-tagged-entry')
  foreach t : ['test-tagged-entry', 'test-tagged-entry-2']
    executable(t, t + '.c', dependencies : libgd_dep)
  endforeach
endif

if (get_option('with-main-icon-view') or
    get_option('with-main-list-view') or
    get_option('with-main-view'))
  foreach t : ['test-styled-text-renderer', 'test-two-lines-renderer']
    executable(t, t + '.c', dependencies: libgd_dep)
  endforeach
endif

if get_option('with-margin-container')
  executable('test-margin-container',
    'test-margin-container.c',
    dependencies: libgd_dep)
endif
