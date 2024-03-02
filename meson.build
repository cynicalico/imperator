project('imp', 'c', 'cpp',
        version : '0.1',
        default_options : ['cpp_std=c++20'])

imp_inc = include_directories('include')

imp_srcs = [
    'src/imp/imp.cpp'
]

fmt_proj = subproject('fmt')
glm_proj = subproject('glm')
spdlog_proj = subproject('spdlog')

imp_deps = [
    fmt_proj.get_variable('fmt_dep'),
    glm_proj.get_variable('glm_dep'),
    spdlog_proj.get_variable('spdlog_dep')]

imp_target = library(
    'imp',
    imp_srcs,
    include_directories : imp_inc,
    dependencies : imp_deps)

imp_dep = declare_dependency(
    include_directories : imp_inc,
    link_with : imp_target)

subdir('example')
