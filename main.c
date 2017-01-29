#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <glad/glad.c>

#include <GLFW/glfw3.h>

GLFWwindow* window;
double resx = 1600, resy = 900;

char *readFile(const char *filename);
void CompileShader(const char * file_path, GLuint ShaderID);
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);


int main() {
    /////////////////////////////////////////////////////////////////////////////////
    // initialize GLFW
    if (!glfwInit()) {
        printf("Could not initialize\n");
        exit(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////
    // create window with the right profile
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // matches 330 in the shaders
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(resx, resy, "test", 0, 0);
    if (!window) {
        printf("Could not open glfw window\n");
        glfwTerminate();
        exit(-2);
    }
    glfwMakeContextCurrent(window); 

    glfwSwapInterval(1);

    /////////////////////////////////////////////////////////////////////////////////
    // initialize GLAD
    if(!gladLoadGL()) {
        printf("Something went wrong!\n");
        exit(-1);
    }


    /////////////////////////////////////////////////////////////////////////////////
    // load shaders, i.e. programs on the gpu
    //
    // the "vertex shader" is a program that is called once per vertex
    // it describes how you want to transform vertices
    //
    // the "fragment shader" is a program that runs once per fragment (pixels, really)
    // it describes how you wan to color the pixels
    GLuint programID = LoadShaders( "vertex_shader.vs", "fragment_shader.fs" );


    /////////////////////////////////////////////////////////////////////////////////
    // triangle data
    GLfloat vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f,
    };

    /////////////////////////////////////////////////////////////////////////////////
    // create a vertex buffer array to attach our vertex buffer objects
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    /////////////////////////////////////////////////////////////////////////////////
    // create vertex buffer object, where we hold our triangle vertex position data
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);  // copy to gpu

    /////////////////////////////////////////////////////////////////////////////////
    // tell the program the properties of the vbo
    glEnableVertexAttribArray(0);                            // matches shader layout specifier
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // specify layout (vec3, float)

    /////////////////////////////////////////////////////////////////////////////////
    // main drawing routine
    glClearColor(3.0/255, 72/255.0, 133/255.0, 0.0f);
    
    double t1 = glfwGetTime();

    double dt_avg = 0.0;  // first moment
    double dt_avg2 = 0.0; // second moment

    int frames_to_avg = 100;
    int frame_ctr = 0;
    glfwSwapInterval(1);
    while ( !glfwWindowShouldClose(window)) {
        double t2 = glfwGetTime();
        double dt = t2 - t1;
        t1 = t2;

        dt_avg += dt;
        dt_avg2 += dt*dt;
        frame_ctr++;

        if (frame_ctr == frames_to_avg) {
            dt_avg /= frames_to_avg;
            dt_avg2 /= frames_to_avg;
            double dt_ste = sqrt(dt_avg2 - dt_avg*dt_avg)/sqrt(frames_to_avg);

            char str[64];
            sprintf(str, "time frame = %.3fms +/- %.4fms, fps = %.1f, %d frames", 
                         1000.0*dt_avg, 1000.0*dt_ste, 1.0/dt_avg, frames_to_avg);
            glfwSetWindowTitle(window, str);

            frames_to_avg = (int)(1.0/dt_avg); // this should make it update approximitely once per second

            dt_avg = 0.0;
            dt_avg2 = 0.0;
            frame_ctr = 0;
        }


        /////////////////////////////////////////////////////////////////////////////////
        // listen for input
        glfwPollEvents();

        /////////////////////////////////////////////////////////////////////////////////
        // clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        /////////////////////////////////////////////////////////////////////////////////
        // actual drawing
        //
        // instead of a bunch of calls to glBegin and glEnd, we setup our vertex information beforehand (identified by "VertexArrayID")
        // and use that directly to drow a triangle
        // we use a *program* (i.e. a pair of shaders) to determine what will happen with the vertex data, and how to color the pixels
        glUseProgram(programID);
        glBindVertexArray(VertexArrayID); // tells the program which vertex data we want to use (like glVertex, but referencing all at once)

        glUniform1f(glGetUniformLocation(programID, "time"), glfwGetTime());
        glUniform2f(glGetUniformLocation(programID, "resolution"), resx, resy);

        glDrawArrays(GL_TRIANGLES, 0, 3); // start at index 0, and draw 3 vertices, interpreted as GL_TRIANGLES
        
        /////////////////////////////////////////////////////////////////////////////////
        // swap, i.e. display the latest changes
        glfwSwapBuffers(window);
    }

    /////////////////////////////////////////////////////////////////////////////////
    // cleanup
    glDisableVertexAttribArray(0);
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);

    glfwTerminate();

    return 0;
}


/////////////////////////////////////////////////////////////////////////////////
// Code used to load the shaders, don't need to understand how they work yet
char *readFile(const char *filename) {
    FILE *f = fopen(filename, "rb");

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    printf("Reading %s, Filesize = %d\n", filename, (int)fsize);

    char *string = (char*)malloc(fsize + 1);
    fread(string, fsize, 1, f);
    string[fsize] = '\0';
    fclose(f);

    return string;
}

void CompileShader(const char * file_path, GLuint ShaderID) {
    GLint Result = GL_FALSE;
    int InfoLogLength;

    char *ShaderCode   = readFile(file_path);

    // Compile Shader
    printf("Compiling shader : %s\n", file_path);
    glShaderSource(ShaderID, 1, (const char**)&ShaderCode , NULL);
    glCompileShader(ShaderID);

    // Check Shader
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( Result == GL_FALSE ){
        char ShaderErrorMessage[9999];
        glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, ShaderErrorMessage);
        printf("%s", ShaderErrorMessage);
    }
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
    printf("Creating shaders\n");
    GLuint VertexShaderID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    CompileShader(vertex_file_path, VertexShaderID);
    CompileShader(fragment_file_path, FragmentShaderID);


    printf("Create and linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    GLint Result = GL_FALSE;
    int InfoLogLength;

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( InfoLogLength > 0 ){
        GLchar ProgramErrorMessage[9999];
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    fflush(stdout);

    return ProgramID;
}
