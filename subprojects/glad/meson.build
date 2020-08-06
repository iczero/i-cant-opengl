project('glad', 'c',
    version: '0.1.33',
    default_options: ['buildtype=release'])

cc = meson.get_compiler('c')

glad_deps = [
    cc.find_library('dl', required: false)
]
include = include_directories('include')
glad = static_library('glad', 'src/glad.c', include_directories: include, dependencies: glad_deps)

glad_dep = declare_dependency(include_directories: include, link_with: glad)
