# Libs
srcs = ['implot.cpp',
        'implot_items.cpp',
        'implot_demo.cpp']

implot_lib = library('gnuradio-blocklib-blocks', 
    srcs, 
    include_directories : ['.'],
    install : true,
    link_language: 'cpp',
    dependencies : imgui_dep)

implot_dep = declare_dependency(include_directories : ['.'],
					   link_with : implot_lib,
                       dependencies : imgui_dep)
# GR namespace tests
srcs = ['demo_imgui'
          ]
deps = [gnuradio_gr_dep,
                gnuradio_blocklib_blocks_dep,
                gnuradio_blocklib_streamops_dep,
                gnuradio_blocklib_math_dep,
                gnuradio_blocklib_imgui_dep,
                # gnuradio_scheduler_nbt_dep,
                imgui_dep,
                opengl_dep,
                glfw3_dep,
                # gl_dep,
                pybind11_dep,
                implot_dep
                ]

foreach app : srcs
    e = executable(app, 
        app + '.cc', 
        include_directories : incdir, 
        link_language : 'cpp',
        link_args : ['-lgnuradio-runtime', '-lgnuradio-blocklib-blocks', '-lgnuradio-blocklib-math', '-lgnuradio-blocklib-streamops'],
        dependencies: deps, 
        install : false)
endforeach

# e = executable('implot_demo', 
#     'implot_demo.cpp', 
#     include_directories : incdir, 
#     link_language : 'cpp',
#     dependencies: [imgui_dep, implot_dep],
#     install : false)