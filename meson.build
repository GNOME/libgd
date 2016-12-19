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

include = include_directories('.')

subdir('libgd')
