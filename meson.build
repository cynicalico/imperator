project('imperator', 'c', 'cpp',
        version : '0.1',
        default_options : ['cpp_std=c++20'])

imp_include = include_directories('include')

imp_sources = files([
                        'src/imp/imp.cpp'
                    ])

argparse_proj = subproject('argparse')
fmt_proj = subproject('fmt')
glbinding_proj = subproject('glbinding')
glfw_proj = subproject('glfw')
glm_proj = subproject('glm')
spdlog_proj = subproject('spdlog')

imp_public_deps = [
    argparse_proj.get_variable('argparse_dep'),
    fmt_proj.get_variable('fmt_dep'),
    glbinding_proj.get_variable('glbinding_dep'),
    glfw_proj.get_variable('glfw_dep'),
    glm_proj.get_variable('glm_dep'),
    spdlog_proj.get_variable('spdlog_dep')]

imp_target = library(
    'imp',
    imp_sources,
    include_directories : imp_include,
    dependencies : imp_public_deps)

imp_dep = declare_dependency(
    include_directories : imp_include,
    dependencies : imp_public_deps,
    link_with : imp_target)

subdir('example')
