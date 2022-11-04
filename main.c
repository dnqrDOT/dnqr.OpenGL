#include <stdlib.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main(int argc, char** argv) {
    /* code */
    printf("%s\n", "Hello YouTube!");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* window;
    window = glfwCreateWindow(720, 480, "dnqr.", NULL, NULL);
    if(!window) {
        fprintf(stderr, "%s\n", "ERROR: Failed creating GLFW window!");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "%s\n", "ERROR: Failed initialize GLAD!");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glViewport(0, 0, 720, 480);

    // shaders
    const char* vShaderCode =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 bColor;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
    "    bColor = aColor;\n"
    "}\n";

    const char* fShaderCode =
    "#version 410 core\n"
    "out vec4 FragColor;\n"
    "in vec3 bColor;\n"
    "void main() {\n"
    "    FragColor = vec4(bColor, 1.0);\n"
    "}\n";

    unsigned int fShader;
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderCode, NULL);
    glCompileShader(fShader);

    unsigned int vShader;
    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShaderCode, NULL);
    glCompileShader(vShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fShader);
    glAttachShader(shaderProgram, vShader);
    glLinkProgram(shaderProgram);

    // triangle data
    float vertices[] = {
        // position             // color
        -0.75f, -0.75f, 0.00f,  1.0f, 0.0f, 0.0f,
         0.75f, -0.75f, 0.00f,  0.0f, 1.0f, 0.0f,
         0.00f,  0.75f, 0.00f,  0.0f, 0.0f, 1.0f
    };

    unsigned int VBO;
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.13f, 0.13f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw triangle
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
