#include "testcommon.h"

/* Window resolution */
#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

/* Window title */
#define WINDOW_TITLE "vao core profile test"

int main( /* int argc , char* args[] */ )
{
    /* The window */
    SDL_Window* window = NULL;

    /* The window surface */
    SDL_Surface* screen = NULL;

    /* The event structure */
    SDL_Event event;

    /* The game loop flag */
    _Bool running = true;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL2 could not initialize! SDL2_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(
            WINDOW_TITLE,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (window == NULL) {
        printf("unable to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext maincontext; /* Our opengl context handle */
    maincontext = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    GLint level5Pixels[] = { 0xFF00FFFF, 0xFF00FF00, 0xFFFF0000, 0xFF000000 };
    GLint level6Pixels[] = { 0xFFFFFFFF };

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 5);
    glTexImage2D(GL_TEXTURE_2D,
            5,  
            GL_RGBA,
            2, 2,
            0,  
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            level5Pixels);
    glTexImage2D(GL_TEXTURE_2D,
            6,  
            GL_RGBA,
            1, 1,
            0,  
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            level6Pixels);

    static const GLfloat quadVerts[] = {
        -0.8f,  0.8f, 0.0f, 1.0f,
         0.8f,  0.8f, 0.0f, 1.0f,
        -0.8f, -0.8f, 0.0f, 1.0f,
         0.8f, -0.8f, 0.0f, 1.0f,
    };
 
    const GLchar* vshadercode = "#version 330 core\n"\
    "layout(location = 0) in vec4 a_position\n;"\
    "layout(location = 1) in vec2 a_texcoord\n;"\
    "out vec2 v_texcoord\n;"\
    "void main(void)\n"\
    "{\n"\
    "    gl_Position = a_position;\n"\
    "    v_texcoord = a_texcoord;\n"\
    "}";

    const GLchar* fshadercode = "#version 330 core\n"\
    "in vec2 v_texcoord;\n"\
    "uniform sampler2D sTexture;\n"\
    "out vec4 o_color;\n"\
    "void main(){\n"\
    "    vec2 tc = v_texcoord.xy*0.5 + 0.5;\n"\
    "    o_color = texture( sTexture, tc );\n"\
    "}";

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(vshader, 1, &vshadercode, NULL);
    glShaderSource(fshader, 1, &fshadercode, NULL);

    CompileShader(vshader, "Vertex");
    CompileShader(fshader, "Fragment");

    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);

    LinkProgram(program, "");

    glUseProgram(program);

    GLuint texLoc = glGetUniformLocation(program, "sTexture");
    GLuint matViewProjLoc = glGetUniformLocation(program, "u_matViewProjection");
    GLuint posLoc = 0;
    GLuint texcoordLoc = 1;

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    GLuint quadVB = 0;
    glGenBuffers(1, &quadVB);
    glBindBuffer(GL_ARRAY_BUFFER, quadVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, 0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(vao);

    while( running )
    {
        while( SDL_PollEvent( &event ) != 0 )
        {
            if( event.type == SDL_QUIT )
            {
                running = false;
            }
        }

        glClearColor ( 1.0, 0.0, 0.0, 1.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        

        SDL_GL_SwapWindow(window);
    }


    glDeleteProgram(program);
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    glDeleteTextures(1, &texture);

    glDeleteBuffers(1, &quadVB);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(maincontext);
    SDL_DestroyWindow( window );
    SDL_Quit();

    return 0;
}
