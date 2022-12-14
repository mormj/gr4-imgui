// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable
// pipeline If you are new to dear imgui, see examples/README.txt and documentation at the
// top of imgui.cpp. (GLFW is a cross-platform general purpose library for handling
// windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <stdio.h>

#include <gnuradio/blocks/vector_source.h>
#include <gnuradio/flowgraph.h>
#include <gnuradio/imgui/line_sink.h>
#include <gnuradio/math/multiply_const.h>
#include <gnuradio/streamops/throttle.h>


// System includes
#include <ctype.h>                        // toupper
#include <limits.h>                       // INT_MIN, INT_MAX
#include <math.h>                         // sqrtf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>                        // vsnprintf, sscanf, printf
#include <stdlib.h>                       // NULL, malloc, free, atoi
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>                       // intptr_t
#else
#include <stdint.h> // intptr_t
#endif

// // About Desktop OpenGL function loaders:
// //  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL
// function pointers.
// //  Helper libraries are often used for this purpose! Here we are supporting a few
// common ones (gl3w, glew, glad).
// //  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose
// to manually implement your own. #if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W) #include
// <GL/gl3w.h>    // Initialize with gl3wInit() #elif
// defined(IMGUI_IMPL_OPENGL_LOADER_GLEW) #include <GL/glew.h>    // Initialize with
// glewInit() #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD) #include <glad/glad.h>  //
// Initialize with gladLoadGL() #else #include IMGUI_IMPL_OPENGL_LOADER_CUSTOM #endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize
// ease of testing and compatibility with old VS compilers. To link with VS2010-era
// libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do
// using this pragma. Your own project should not be affected, as you are likely to link
// with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// Helper to wire demo markers located in code to an interactive browser
typedef void (*ImGuiDemoMarkerCallback)(const char* file,
                                        int line,
                                        const char* section,
                                        void* user_data);
extern ImGuiDemoMarkerCallback GImGuiDemoMarkerCallback;
extern void* GImGuiDemoMarkerCallbackUserData;
ImGuiDemoMarkerCallback GImGuiDemoMarkerCallback = NULL;
void* GImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_DEMO_MARKER(section)                                              \
    do {                                                                        \
        if (GImGuiDemoMarkerCallback != NULL)                                   \
            GImGuiDemoMarkerCallback(                                           \
                __FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); \
    } while (0)


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{


    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;


    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only


    // Create window with graphics context
    GLFWwindow* window =
        glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely
                      // to requires some form of initialization.
#endif
    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard
    // Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable
    // Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load
    // multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to
    // select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those
    // errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a
    // texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which
    // ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal
    // you need to write a double backslash \\ !
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    // ImFont* font =
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL,
    // io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    std::vector<float> input_data{ 0.0, 0.1, 0.2, 0.3, 0.4, 0.5 };
    float samp_rate = 32000;

    static float mult_k = 1.0;

    auto src = gr::blocks::vector_source_f::make_cpu({ input_data, true });
    auto mult = gr::math::multiply_const_ff::make({ mult_k });
    auto throttle = gr::streamops::throttle::make({ samp_rate });
    auto snk = gr::imgui::line_sink_f::make({ 1024, 1 });

    auto fg = gr::flowgraph::make();
    fg->connect(src, 0, mult, 0);
    fg->connect(mult, 0, throttle, 0);
    fg->connect(throttle, 0, snk, 0);

    fg->start();


    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if
        // dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your
        // main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to
        // your main application. Generally you may always pass all inputs to dear imgui,
        // and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in
        // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
        // ImGui!). if (show_demo_window)
        //     ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to
        // created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and
                                           // append into it.

            ImGui::Text("This is some useful text."); // Display some text (you can use a
                                                      // format strings too)
            ImGui::Checkbox(
                "Demo Window",
                &show_demo_window); // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat(
                "float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color",
                              (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets
                                         // return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            ImGui::End();
        }

        {
            // ImGui::Begin("name of widget"); // Create a window called "Hello, world!"
            // and append into it.
            // static bool animate = true;
            // ImGui::Checkbox("Animate", &animate);

            // Plot as lines and plot as histogram
            // IMGUI_DEMO_MARKER("Widgets/Plotting/PlotLines, PlotHistogram");

            // Fill an array of contiguous float values to plot
            // Tip: If your float aren't contiguous but part of a structure, you can pass
            // a pointer to your first float and the sizeof() of your structure in the
            // "stride" parameter.
            // static float values[90] = {};
            // static int values_offset = 0;
            // static double refresh_time = 0.0;
            // if (!animate || refresh_time == 0.0)
            //     refresh_time = ImGui::GetTime();
            // while (refresh_time <
            //        ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
            // {
            //     static float phase = 0.0f;
            //     values[values_offset] = cosf(phase);
            //     values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
            //     phase += 0.10f * values_offset;
            //     refresh_time += 1.0f / 60.0f;
            // }

            static std::vector<std::vector<float>> buf;
            // if (animate)
            buf = snk->get_buf();
            auto npoints = snk->npoints();
            auto nplots = snk->nplots();
            // static double refresh_time = 0.0;
            // if (!animate || refresh_time == 0.0)
            //     refresh_time = ImGui::GetTime();
            // while (refresh_time <
            //        ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
            // {
            // if (animate) {
            // for (size_t idx = 0; idx < nplots; idx++) {
            //     ImGui::PlotLines("Lines",
            //                      buf[idx].data(),
            //                      npoints,
            //                      npoints,
            //                      "",
            //                      -1.0f,
            //                      1.0f,
            //                      ImVec2(800, 400.0f));
            // }
            // ImGui::End();

            ImGui::Begin("My Window");


            for (size_t idx = 0; idx < nplots; idx++) {
                if (ImPlot::BeginPlot("My Plot")) {
                    ImPlot::PlotLine("My Line Plot", buf[idx].data(), npoints);

                    ImPlot::EndPlot();
                }
            }

            if (ImGui::SliderFloat("float", &mult_k, 0.0f, 1.0f)) {
                mult->set_k(mult_k);
            }

            ImGui::End();
            // }

            // // Plots can display overlay texts
            // // (in this example, we will display an average value)
            // {
            //     float average = 0.0f;
            //     for (int n = 0; n < IM_ARRAYSIZE(values); n++)
            //         average += values[n];
            //     average /= (float)IM_ARRAYSIZE(values);
            //     char overlay[32];
            //     sprintf(overlay, "avg %f", average);
            //     ImGui::PlotLines("Lines",
            //                      values,
            //                      IM_ARRAYSIZE(values),
            //                      values_offset,
            //                      overlay,
            //                      -1.0f,
            //                      1.0f,
            //                      ImVec2(0, 80.0f));
            // }
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    fg->stop();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
