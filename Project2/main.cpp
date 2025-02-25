/**
* Author: [Haoxuan Wang]
* Assignment: Pong Clone
* Date due: 2025-3-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION 

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h" 

//Global constants
enum AppStatus { RUNNING, TERMINATED };

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr int NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

GLuint g_player_texture_id;

constexpr int WINDOW_WIDTH = 640 * 2,
WINDOW_HEIGHT = 480 * 2;

constexpr float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr int TRIANGLE_RED = 1.0,
TRIANGLE_BLUE = 0.4,
TRIANGLE_GREEN = 0.4,
TRIANGLE_OPACITY = 1.0;

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix,
g_model_matrix,
g_projection_matrix;

//Global constants end

//Intial setup

glm::mat4 g_paddle_matrix;
glm::mat4 g_paddle2_matrix;
glm::mat4 g_pad_matrix;
glm::mat4 g_pad2_matrix;
glm::mat4 g_pad3_matrix;
glm::mat4 g_left_matrix;
glm::mat4 g_right_matrix;

GLuint g_paddle_texture_id;
GLuint g_paddle2_texture_id;
GLuint g_pad_texture_id;
GLuint g_pad2_texture_id;
GLuint g_pad3_texture_id;
GLuint g_left_texture_id;
GLuint g_right_texture_id;

constexpr char PADDLE_SPRITE[] = "paddle.jpg";
constexpr char PADDLE_SPRITE2[] = "paddle2.jpg";
constexpr char PAD_SPRITE[] = "pad.jpg";
constexpr char WINL[] = "leftwin.png";
constexpr char WINR[] = "rightwin.png";

constexpr glm::vec3 INIT_POS_paddle = glm::vec3(-4.5f, 0.0f, 0.0f);
constexpr glm::vec3 INIT_POS_paddle2 = glm::vec3(4.5f, 0.0f, 0.0f);
constexpr glm::vec3 INIT_POS_pad = glm::vec3(0.0f, 0.0f, 0.0f);
constexpr glm::vec3 INIT_POS_pad2 = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 INIT_POS_pad3 = glm::vec3(0.0f, -1.0f, 0.0f);
constexpr glm::vec3 INIT_POS_win = glm::vec3(0.0f, 0.0f, 0.0f);

constexpr glm::vec3 INIT_SCALE_paddle = glm::vec3(1.0f, 1.0f, 0.0f);
constexpr glm::vec3 INIT_SCALE_paddle2 = glm::vec3(1.0f, 1.0f, 0.0f);
constexpr glm::vec3 INIT_SCALE_pad = glm::vec3(1.0f, 1.0f, 0.0f);
constexpr glm::vec3 INIT_SCALE_win = glm::vec3(8.0f, 1.0f, 0.0f); 

float g_previous_ticks = 0;
float paddle2_prev_move = 1.5f;

glm::vec3 g_paddle_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_paddle_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_paddle2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_paddle2_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_pad_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_pad2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_pad3_position = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_pad_movement = glm::vec3(0.6f, 0.6f, 0.0f);
glm::vec3 g_pad2_movement = glm::vec3(0.6f, 0.6f, 0.0f);
glm::vec3 g_pad3_movement = glm::vec3(0.6f, 0.6f, 0.0f);

glm::vec3 g_win_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_win_movement = glm::vec3(0.0f, 0.0f, 0.0f);

bool playertwo = true;
int victorypage = 0;
int padnum = 1;


//Initial setup ends

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Transformation Exercise",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_model_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_paddle_matrix = glm::mat4(1.0f);
    g_paddle2_matrix = glm::mat4(1.0f);
    g_pad_matrix = glm::mat4(1.0f);
    g_pad2_matrix = glm::mat4(1.0f);
    g_pad3_matrix = glm::mat4(1.0f);
    g_left_matrix = glm::mat4(1.0f);
    g_right_matrix = glm::mat4(1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    g_shader_program.set_colour(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_paddle_texture_id = load_texture(PADDLE_SPRITE);
    g_paddle2_texture_id = load_texture(PADDLE_SPRITE2);
    g_pad_texture_id = load_texture(PAD_SPRITE);
    g_pad2_texture_id = load_texture(PAD_SPRITE);
    g_pad3_texture_id = load_texture(PAD_SPRITE);
    g_left_texture_id = load_texture(WINL);
    g_right_texture_id = load_texture(WINR);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_paddle_movement.y = 0.0f;
    g_paddle2_movement.y = 0.0f;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
            
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_q: g_app_status = TERMINATED; break;

                case SDLK_t: playertwo = !playertwo; break;

                case SDLK_1: padnum = 1; break;

                case SDLK_2: padnum = 2; break;

                case SDLK_3: padnum = 3; break;

                default: break;
                }

        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if (key_state[SDL_SCANCODE_W])
    {
        if (g_paddle_position.y < 3.25f)
            g_paddle_movement.y = 1.5f;
    }
    else if (key_state[SDL_SCANCODE_S])
    {
        if (g_paddle_position.y > -3.25f)
            g_paddle_movement.y = -1.5f;
    }

    if (playertwo == true)
    {

        if (key_state[SDL_SCANCODE_UP])
        {
            if(g_paddle2_position.y < 3.25f)
                g_paddle2_movement.y = 1.5f;
        }
            
        else if (key_state[SDL_SCANCODE_DOWN])
        {
            if(g_paddle2_position.y > -3.25f)
                g_paddle2_movement.y = -1.5f;
        }
            
    }
}

void update()
{   
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    if (victorypage > 0)
    {
        if (victorypage == 1)
        {
            g_left_matrix = glm::mat4(1.0f);
            g_left_matrix = glm::scale(g_left_matrix, INIT_SCALE_win);
        }
        else
        {
            g_right_matrix = glm::mat4(1.0f);
            g_right_matrix = glm::scale(g_right_matrix, INIT_SCALE_win);
        }

    }

    if (playertwo == false)
    {
        if (g_paddle2_position.y > 3.0f)
        {
            g_paddle2_movement.y = -1.0f;
        }
        else if (g_paddle2_position.y < -3.0f)
        {
            g_paddle2_movement.y = 1.0f;
        }
        else
        {
            g_paddle2_movement.y = paddle2_prev_move;
        }
        
        paddle2_prev_move = g_paddle2_movement.y; 
    }

    g_paddle_position += g_paddle_movement * delta_time;
    g_paddle2_position += g_paddle2_movement * delta_time;
    g_pad_position += g_pad_movement * delta_time;

    //Reset matrix
    g_paddle_matrix = glm::mat4(1.0f);
    g_paddle2_matrix = glm::mat4(1.0f);
    g_pad_matrix = glm::mat4(1.0f);

    g_paddle_matrix = glm::translate(g_paddle_matrix, INIT_POS_paddle);
    g_paddle2_matrix = glm::translate(g_paddle2_matrix, INIT_POS_paddle2);
    g_pad_matrix = glm::translate(g_pad_matrix, INIT_POS_pad);

    //Add current movement
    g_paddle_matrix = glm::translate(g_paddle_matrix, g_paddle_position);
    g_paddle2_matrix = glm::translate(g_paddle2_matrix, g_paddle2_position);
    g_pad_matrix = glm::translate(g_pad_matrix, g_pad_position);

    //Collision check To write more collision check
    float x_distance_p1 = fabs(g_pad_position.x + INIT_POS_pad.x - g_paddle_position.x - INIT_POS_paddle.x) - ((INIT_SCALE_paddle.x + INIT_SCALE_pad.x) / 2.0f);
    float y_distance_p1 = fabs(g_pad_position.y + INIT_POS_pad.y - g_paddle_position.y - INIT_POS_paddle.y) - ((INIT_SCALE_paddle.y + INIT_SCALE_pad.y) / 2.0f);

    float x_distance_p2 = fabs(g_pad_position.x + INIT_POS_pad.x - g_paddle2_position.x - INIT_POS_paddle2.x) - ((INIT_SCALE_paddle2.x + INIT_SCALE_pad.x) / 2.0f);
    float y_distance_p2 = fabs(g_pad_position.y + INIT_POS_pad.y - g_paddle2_position.y - INIT_POS_paddle2.y) - ((INIT_SCALE_paddle2.y + INIT_SCALE_pad.y) / 2.0f);

    if (x_distance_p1 < 0 && y_distance_p1 < 0)
    {
        g_pad_movement.x = 0.6;
    }

    if (x_distance_p2 < 0 && y_distance_p2 < 0)
    {
        g_pad_movement.x = -0.6;
    }

    if (g_pad_position.y >= 3.25f)
    {
        g_pad_movement.y = -0.6;
    }

    if (g_pad_position.y <= -3.25f)
    {
        g_pad_movement.y = 0.6;
    }


    if (padnum > 1) // two balls
    {
        g_pad2_position += g_pad2_movement * delta_time;
        g_pad2_matrix = glm::mat4(1.0f);
        g_pad2_matrix = glm::translate(g_pad2_matrix, INIT_POS_pad2);
        g_pad2_matrix = glm::translate(g_pad2_matrix, g_pad2_position);

        float x_distance_p1 = fabs(g_pad2_position.x + INIT_POS_pad2.x - g_paddle_position.x - INIT_POS_paddle.x) - ((INIT_SCALE_paddle.x + INIT_SCALE_pad.x) / 2.0f);
        float y_distance_p1 = fabs(g_pad2_position.y + INIT_POS_pad2.y - g_paddle_position.y - INIT_POS_paddle.y) - ((INIT_SCALE_paddle.y + INIT_SCALE_pad.y) / 2.0f);

        float x_distance_p2 = fabs(g_pad2_position.x + INIT_POS_pad2.x - g_paddle2_position.x - INIT_POS_paddle2.x) - ((INIT_SCALE_paddle2.x + INIT_SCALE_pad.x) / 2.0f);
        float y_distance_p2 = fabs(g_pad2_position.y + INIT_POS_pad2.y - g_paddle2_position.y - INIT_POS_paddle2.y) - ((INIT_SCALE_paddle2.y + INIT_SCALE_pad.y) / 2.0f);

        if (x_distance_p1 < 0 && y_distance_p1 < 0)
        {
            g_pad2_movement.x = 0.6;
        }

        if (x_distance_p2 < 0 && y_distance_p2 < 0)
        {
            g_pad2_movement.x= -0.6;
        }

        if (g_pad2_position.y >= 2.25f)
        {
            g_pad2_movement.y = -0.6;
        }

        if (g_pad2_position.y <= -4.25f)
        {
            g_pad2_movement.y = 0.6;
        }

    }

    if (padnum >2) // three balls
    {
        g_pad3_position += g_pad3_movement * delta_time;
        g_pad3_matrix = glm::mat4(1.0f);
        g_pad3_matrix = glm::translate(g_pad3_matrix, INIT_POS_pad3);
        g_pad3_matrix = glm::translate(g_pad3_matrix, g_pad3_position);

        float x_distance_p1 = fabs(g_pad3_position.x + INIT_POS_pad3.x - g_paddle_position.x - INIT_POS_paddle.x) - ((INIT_SCALE_paddle.x + INIT_SCALE_pad.x) / 2.0f);
        float y_distance_p1 = fabs(g_pad3_position.y + INIT_POS_pad3.y - g_paddle_position.y - INIT_POS_paddle.y) - ((INIT_SCALE_paddle.y + INIT_SCALE_pad.y) / 2.0f);

        float x_distance_p2 = fabs(g_pad3_position.x + INIT_POS_pad3.x - g_paddle2_position.x - INIT_POS_paddle2.x) - ((INIT_SCALE_paddle2.x + INIT_SCALE_pad.x) / 2.0f);
        float y_distance_p2 = fabs(g_pad3_position.y + INIT_POS_pad3.y - g_paddle2_position.y - INIT_POS_paddle2.y) - ((INIT_SCALE_paddle2.y + INIT_SCALE_pad.y) / 2.0f);

        if (x_distance_p1 < 0 && y_distance_p1 < 0)
        {
            g_pad3_movement.x = 0.6;
        }

        if (x_distance_p2 < 0 && y_distance_p2 < 0)
        {
            g_pad3_movement.x = -0.6;
        }

        if (g_pad3_position.y >= 4.25f)
        {
            g_pad3_movement.y = -0.6;
        }

        if (g_pad3_position.y <= -2.25f)
        {
            g_pad3_movement.y = 0.6;
        }
    }

    //Reset pads if not visible
    if (padnum < 3)
    {
      g_pad3_position = glm::vec3(0.0f, 0.0f, 0.0f);
      if (padnum < 2)
      {
          g_pad2_position = glm::vec3(0.0f, 0.0f, 0.0f);
      }
    }
    
    //Game ending check
    if (g_pad_position.x >= 4.5f || g_pad2_position.x >= 4.5f || g_pad3_position.x >= 4.5f)
    {
        victorypage = 1;
        //g_app_status = TERMINATED;
    }

    else if (g_pad_position.x <= -4.5f || g_pad2_position.x <= -4.5f || g_pad3_position.x <= -4.5f)
    {
        victorypage = 2;
        //g_app_status = TERMINATED;
    }

    else
    {}

}

void draw_object(glm::mat4& g_object_model_matrix, GLuint& g_object_texture_id)
{
    g_shader_program.set_model_matrix(g_object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, g_object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    draw_object(g_paddle_matrix, g_paddle_texture_id);
    draw_object(g_paddle2_matrix, g_paddle2_texture_id);
    draw_object(g_pad_matrix, g_pad_texture_id);

    if (victorypage > 0)
    {
        if (victorypage == 1)
            draw_object(g_left_matrix, g_left_texture_id);
        if (victorypage == 2)
            draw_object(g_right_matrix, g_right_texture_id);
    }

    if (padnum > 1)
    {
        draw_object(g_pad2_matrix, g_pad2_texture_id);  
    }
    if (padnum > 2)
    {
        draw_object(g_pad3_matrix, g_pad3_texture_id);
    }

    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }

int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}