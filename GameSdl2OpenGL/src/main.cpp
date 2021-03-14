// python -m glad --out-path=build --api="gl=2.1" --extensions="" --generator="c"
// g++ example/c++/sdl.cpp build/src/glad.c -Ibuild/include -lSDL2 -ldl

#include <iostream>

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

int main(int argc, char** argv) {
    GLuint vao;
    GLuint vbo;

    /* geometry to use. these are 3 xyz points (9 floats total) to make a triangle */
    GLfloat points[] = {
      0.0f, 0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f
    };

    /* these are the strings of code for the shaders
    the vertex shader positions each vertex point */
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
    /* GL shader objects for vertex and fragment shader [components] */
    GLuint vert_shader;
    GLuint frag_shader;
    /* GL shader programme object [combined, to link] */
    GLuint shader_programme;



    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        spdlog::critical("SDL2 video subsystem couldn't be initialized. Error: ", SDL_GetError());
        exit(1);
    }

    /* Setting up OpenGL version and profile details for context creation */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);


    SDL_Window* window = SDL_CreateWindow("Glad Sample",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600, 
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr) {
        spdlog::critical("SDL2 Renderer couldn't be created. Error: {}", SDL_GetError());
        exit(1);
    }

    // Create a OpenGL context on SDL2
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    // Load GL extensions using glad
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        spdlog::critical("Failed to initialize the OpenGL context.");
        exit(1);
    }

    // Loaded OpenGL successfully.
    spdlog::info("OpenGL version loaded: {}.{}", GLVersion.major, GLVersion.minor);

    // Create an event handler
    SDL_Event event;
    // Loop condition
    bool running = true;


    //From antons opengl tutorial https://github.com/AntonioCS/antons_opengl_tutorials_book/tree/master/00_hello_triangle (adapted)
    /* tell GL to only draw onto a pixel if the shape is closer to the viewer
 than anything already drawn at that pixel */
    glEnable(GL_DEPTH_TEST); /* enable depth-testing */
    /* with LESS depth-testing interprets a smaller depth value as meaning "closer" */
    glDepthFunc(GL_LESS);

    /* a vertex buffer object (VBO) is created here. this stores an array of
    data on the graphics adapter's memory. in our case - the vertex points */
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), points, GL_STATIC_DRAW);

    /* the vertex array object (VAO) is a little descriptor that defines which
    data from vertex buffer objects should be used as input variables to vertex
    shaders. in our case - use our only VBO, and say 'every three floats is a
    variable' */
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    /* "attribute #0 should be enabled when this vao is bound" */
    glEnableVertexAttribArray(0);
    /* this VBO is already bound, but it's a good habit to explicitly specify which
    VBO's data the following vertex attribute pointer refers to */
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    /* "attribute #0 is created from every 3 variables in the above buffer, of type
    float (i.e. make me vec3s)" */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    /* here we copy the shader strings into GL shaders, and compile them. we
    then create an executable shader 'program' and attach both of the compiled
        shaders. we link this, which matches the outputs of the vertex shader to
    the inputs of the fragment shader, etc. and it is then ready to use */
    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vertex_shader, NULL);
    glCompileShader(vert_shader);
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &fragment_shader, NULL);
    glCompileShader(frag_shader);
    shader_programme = glCreateProgram();
    glAttachShader(shader_programme, frag_shader);
    glAttachShader(shader_programme, vert_shader);
    glLinkProgram(shader_programme);


    while (running) {
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
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_programme);
        glBindVertexArray(vao);
        /* draw points 0-3 from the currently bound VAO with current in-use shader */
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    // Destroy everything to not leak memory.
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
