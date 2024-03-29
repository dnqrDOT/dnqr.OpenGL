#include <stdio.h>
#include <stdlib.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SPEED 0.125

void readKeyboard(GLFWwindow *window, float *x_direction, float *y_direction);
void monitor_callback(GLFWmonitor* monitor, int event);

int main()
{
    printf("Hello, YouTube!\n");

    if(!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(420, 420, "dnqr.", NULL, NULL);
    if(!window) {
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGL(glfwGetProcAddress)) {
        exit(EXIT_FAILURE);
    }
    glfwSetMonitorCallback(monitor_callback);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // shaders
    const char* vShaderCode =
    "#version 410 core\n"
    "layout (location = 0) in vec2 vertex_position;\n"
    "uniform float x_dir;\n"
    "uniform float y_dir;\n"
    "out vec2 TexCoord;\n"
    "void main() {\n"
    "    gl_Position = vec4(vertex_position.x + x_dir - 0.5, vertex_position.y + y_dir - 0.5, 0.0, 1.0);\n"
    "    TexCoord = vec2(vertex_position.x, vertex_position.y);\n"
    "}\0";
    const char* fShaderCode =
    "#version 410 core\n"
    "uniform sampler2D imgTexture;\n"
    "uniform float uv_x;\n"
    "uniform float uv_y;\n"
    "uniform float nx_frames;\n"
    "uniform float ny_frames;\n"
    "in vec2 TexCoord;\n"
    "out vec4 frag_color;\n"
    "void main(){\n"
    "    float x = 1.0 / nx_frames;\n"
    "    float y = 1.0 / ny_frames;\n"
    "    frag_color = texture(imgTexture, vec2(TexCoord.x * x, TexCoord.y * y) + vec2(x * uv_x, y * uv_y));\n"
    //"    frag_color = texture(imgTexture, TexCoord);\n"
    //"    frag_color = vec4(0.1412, 0.1608, 0.1843, 1.0);\n"
    "}\0";

    unsigned int vShader, fShader;
    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShaderCode, NULL);
    glCompileShader(vShader);
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderCode, NULL);
    glCompileShader(fShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);

    // vertices & indices
    float vertices[] = {
        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(0);

    float x_dir = 0.0f, y_dir = 0.0f;

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(GL_TRUE);
    unsigned char *img_data = stbi_load("sprite_fire.png", &width, &height, &nrChannels, 0);
    if(!img_data) {
        fprintf(stderr, "Failed loading image!\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(img_data);

    float nx_frames = 8.0f, ny_frames = 4.0f;
    float uv_x = 0.0f, uv_y = 2.0f;


    double time_now, time_old, time_delta, frames_ps;
    frames_ps = 4.0f;
    time_now = time_old = glfwGetTime();


    // loop
    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.9451, 0.9451, 0.9451, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // read keyboard
        readKeyboard(window, &x_dir, &y_dir);

        time_now = glfwGetTime();
        time_delta = time_now - time_old;
        if(time_delta >= 1.0f / frames_ps) {
            time_old = time_now;
            time_delta = 0.0f;
            uv_x += 1.0f;
            if(uv_x >= nx_frames) {
                uv_x = 0.0f;
            }
        }

        // draw
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1f(glGetUniformLocation(shaderProgram, "x_dir"), x_dir);
        glUniform1f(glGetUniformLocation(shaderProgram, "y_dir"), y_dir);
        glUniform1f(glGetUniformLocation(shaderProgram, "uv_x"), uv_x);
        glUniform1f(glGetUniformLocation(shaderProgram, "uv_y"), uv_y);
        glUniform1f(glGetUniformLocation(shaderProgram, "nx_frames"), nx_frames);
        glUniform1f(glGetUniformLocation(shaderProgram, "ny_frames"), ny_frames);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void readKeyboard(GLFWwindow *window, float *x_direction, float *y_direction)
{
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        *y_direction += SPEED;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        *y_direction -= SPEED;
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        *x_direction -= SPEED;
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        *x_direction += SPEED;
    }
}

void monitor_callback(GLFWmonitor* monitor, int event)
{
    if (event == GLFW_CONNECTED)
    {
        // The monitor was connected
        float xscale, yscale;
        glfwGetMonitorContentScale(monitor, &xscale, &yscale);
        glViewport(0, 0, 420*xscale, 420*yscale);
    }
    else if (event == GLFW_DISCONNECTED)
    {
        // The monitor was disconnected
    }
}