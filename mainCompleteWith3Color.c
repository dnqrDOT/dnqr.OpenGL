#include <stdio.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main(void) {
    /* code */
    int w_width  = 720;
    int w_height = 420;
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 8);
    
    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "dnqr.", NULL, NULL);
    if(!window) {
        fprintf(stderr, "%s\n", "ERROR: Failed creating GLFW window!");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "%s\n", "ERROR: Failed initialize GLAD!");
        exit(EXIT_FAILURE);
    }
    //glfwGetWindowSize(window, &w_width, &w_height);
    glViewport(0, 0, w_width, w_height);
    
    // shader
    const char* vShaderSource1 =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
    "}\n";
    const char* vShaderSource2 =
    "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
    "    ourColor = aColor;\n"
    "}\n";
    
    const char* fShaderSource =
    "#version 410 core\n"
    "in vec3 ourColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vec4(ourColor, 1.0);\n"
    "}\n";
    
    int success;
    char infoLog[512];
    
    unsigned int fShader;
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderSource, NULL);
    glCompileShader(fShader);
    
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fShader, 512, NULL, infoLog);
        fprintf(stderr, "%s%s\n", "ERROR: Fragment shader compilation failed! ", infoLog);
        exit(EXIT_FAILURE);
    }
    
    
    unsigned int vShader;
    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShaderSource2, NULL);
    glCompileShader(vShader);
    
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vShader, 512, NULL, infoLog);
        fprintf(stderr, "%s%s\n", "ERROR: Vertex shader compilation failed! ", infoLog);
        exit(EXIT_FAILURE);
    }
    
    
    unsigned programShader;
    programShader = glCreateProgram();
    glAttachShader(programShader, vShader);
    glAttachShader(programShader, fShader);
    glLinkProgram(programShader);
    
    glGetProgramiv(programShader, GL_LINK_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(programShader, 512, NULL, infoLog);
        fprintf(stderr, "%s%s\n", "ERROR: Linking shaders failed! ", infoLog);
        exit(EXIT_FAILURE);
    }
    
    glDeleteShader(fShader);
    glDeleteShader(vShader);
    
    // triangle data
    float vertices[] = {
        // position            // color
        -0.75f, -0.75f, 0.0f,  0.5f, 0.1f, 0.8f,  // left down
         0.75f, -0.75f, 0.0f,  0.1f, 0.8f, 0.5f,  // right down
         0.0f ,  0.75f, 0.0f,  0.8f, 0.5f, 0.1f   // middle top
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
        
        glUseProgram(programShader);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return EXIT_SUCCESS;
}
