imgui_sources += []
imgui_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep, python3_embed_dep, json_dep]

block_cpp_args = ['-DHAVE_CPU']

# if IMPLEMENT_CUDA
#     block_cpp_args += '-DHAVE_CUDA'

#     gnuradio_blocklib_imgui_cu = library('gnuradio-blocklib-imgui-cu', 
#         imgui_cu_sources, 
#         include_directories : incdir, 
#         install : true, 
#         dependencies : [cuda_dep])

#     gnuradio_blocklib_imgui_cu_dep = declare_dependency(include_directories : incdir,
#                         link_with : gnuradio_blocklib_imgui_cu,
#                         dependencies : cuda_dep)

#     imgui_deps += [gnuradio_blocklib_imgui_cu_dep, cuda_dep]

# endif

incdir = include_directories(['../include/gnuradio/imgui','../include'])
gnuradio_blocklib_imgui_lib = library('gnuradio-blocklib-imgui', 
    imgui_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : imgui_deps,
    link_args : ['-lgnuradio-runtime'],
    cpp_args : block_cpp_args)

gnuradio_blocklib_imgui_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_imgui_lib,
                       dependencies : imgui_deps)
