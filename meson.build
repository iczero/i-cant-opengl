project(
    'opengl-test',
    'cpp',
    version: '1',
    default_options: ['buildtype=debug', 'warning_level=3']
)

cmake = import('cmake')
cc = meson.get_compiler('cpp')

glfw_options = cmake.subproject_options()
# please shut up about warnings i can't and won't do anything about them
glfw_options.set_override_option('warning_level', '1')
glfw_proj = cmake.subproject('glfw', options: glfw_options)
glfw = glfw_proj.dependency('glfw')
glad_proj = subproject('glad', default_options: ['warning_level=1'])
glad = glad_proj.get_variable('glad_dep')
glm_inc = include_directories('subprojects/glm')
glm = declare_dependency(include_directories: glm_inc)

dependencies = [
    glfw,
    glad,
    glm,
    cc.find_library('X11', required: false),
    cc.find_library('pthread', required: false)
]

include = include_directories('include')
common = [
    'src/shaders.cpp',
    'src/window.cpp',
    'src/graphics.cpp',
    'src/camera.cpp',
    'src/geometry.cpp',
    'src/NBody.cpp',
    'src/octree.cpp'
]

cxxflags = []
add_project_arguments(cxxflags, language: 'cpp')

executable('main', common + ['src/main.cpp'],
    include_directories: include,
    dependencies: dependencies)
