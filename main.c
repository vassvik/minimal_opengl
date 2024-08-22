#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <glad/glad.h>
#include <glad/glad.c>

#include <GLFW/glfw3.h>

GLuint load_compute_shader(const char* compute_file_path);

int main() {
    if (!glfwInit()) {
        printf("Could not initialize GLFW\n");
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // matches 330 in the shaders
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Minimal OpenGL Compute", 0, 0);
    if (!window) {
        printf("Could not open GLFW window\n");
        glfwTerminate();
        exit(-2);
    }
    glfwMakeContextCurrent(window); 
    glfwSwapInterval(1);

    if(!gladLoadGL()) {
        printf("Couldn't load OpenGL!\n");
        exit(-3);
    }

    GLuint program = load_compute_shader( "compute_shader.glsl");

    while ( !glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_TRUE) glfwSetWindowShouldClose(window, GLFW_TRUE);
        glfwSwapBuffers(window);
    }

    glDeleteProgram(program);

    glfwTerminate();

    return 0;
}

// Helpers
char* read_entire_file(const char* filename) {
    FILE *f = fopen(filename, "rb");

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = (char*)malloc(fsize + 1);
    fread(string, fsize, 1, f);
    string[fsize] = '\0';
    fclose(f);

    return string;
}

bool compile_shader(const char* file_path, GLuint shader_id) {
    char *shader_code = read_entire_file(file_path);

    glShaderSource(shader_id, 1, (const char**)&shader_code, NULL);
    glCompileShader(shader_id);

    free(shader_code);
    
    GLint result;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE){
        int info_log_length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);

        char message[9999];
        glGetShaderInfoLog(shader_id, info_log_length, NULL, message);
        printf("Error compiling \"%s\":\n", file_path);
        printf("%s\n", message);
        return false;
    }

    return true;
}

GLuint load_compute_shader(const char* compute_file_path){
    GLuint shader_id = glCreateShader(GL_COMPUTE_SHADER);
    if (!compile_shader(compute_file_path, shader_id)) {
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, shader_id);
    glLinkProgram(program);

    glDeleteShader(shader_id);

    GLint result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);

    if (result == GL_FALSE){
        int info_log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

        GLchar message[9999];
        glGetProgramInfoLog(program, info_log_length, NULL, &message[0]);
        printf("Error linking \"%s\":\n", compute_file_path);
        printf("%s\n", message);

        return 0;
    }

    return program;
}
