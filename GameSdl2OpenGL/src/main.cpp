
#include <iostream>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_GLContext gl_context = nullptr;
SDL_Event event;

void processEvents(bool& running)
{
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            running = false;
        break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                }
        
        break;
        default: //do nothing
            ;
    }
}


void initSDLWithOpenGL()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      spdlog::critical("SDL2 video subsystem couldn't be initialized. Error: ", SDL_GetError());
      exit(1);
  }

  /* Setting up OpenGL version and profile details for context creation */
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  window = SDL_CreateWindow("Hello Triangle",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      800, 600,
      SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
  );

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (renderer == nullptr) {
      spdlog::critical("SDL2 Renderer couldn't be created. Error: {}", SDL_GetError());
      exit(1);
  }

  // Create a OpenGL context on SDL2
  gl_context = SDL_GL_CreateContext(window);

  SDL_GL_MakeCurrent(window, gl_context);
  SDL_GL_SetSwapInterval(1); // Enable vsync

  // Load GL extensions using glad
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
      spdlog::critical("Failed to initialize the OpenGL context.");
      exit(1);
  }

  // Loaded OpenGL successfully.
  spdlog::info("OpenGL version loaded: {}.{}", GLVersion.major, GLVersion.minor);
}


void terminateSDL()
{
    // Destroy everything to not leak memory.
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

const char* vertex_shader = R"(
    #version 410
    in vec3 vp;
    void main () {
      gl_Position = vec4(vp, 1.0);
    }
)";

const char* fragment_shader = R"(
    #version 410
    out vec4 frag_colour;
    void main () {
      frag_colour = vec4(0.5, 0.0, 0.5, 1.0);
    }
)";

int main(int argc, char** argv) {
    GLuint vao;
    GLuint vbo;

    /* geometry to use. these are 3 xyz points (9 floats total) to make a triangle */
    GLfloat points[] = {
      0.0f, 0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f
    };

    initSDLWithOpenGL();

    const char* glsl_version = "#version 130";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window = true;
    bool show_another_window = false;

    bool running = true;


    //From antons opengl tutorial https://github.com/AntonioCS/antons_opengl_tutorials_book/tree/master/00_hello_triangle (adapted)

    glEnable(GL_DEPTH_TEST); /* enable depth-testing */  
    glDepthFunc(GL_LESS);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), points, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    /* here we copy the shader strings into GL shaders, and compile them. we
    then create an executable shader 'program' and attach both of the compiled
        shaders. we link this, which matches the outputs of the vertex shader to
    the inputs of the fragment shader, etc. and it is then ready to use */
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vertex_shader, nullptr);
    glCompileShader(vert_shader);

    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &fragment_shader, NULL);
    glCompileShader(frag_shader);

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, frag_shader);
    glAttachShader(shader_programme, vert_shader);
    glLinkProgram(shader_programme);


    while (running) {
        processEvents(running);
        ImGui_ImplSDL2_ProcessEvent(&event);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }


        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        ImGui::Render();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_programme);
        glBindVertexArray(vao);
        /* draw points 0-3 from the currently bound VAO with current in-use shader */
        glDrawArrays(GL_TRIANGLES, 0, 3);


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    terminateSDL();
    return 0;
}
