#include <iostream>
#include <fstream>
#include <OpenGL/gl.h>
#include "shaders.h"

using namespace std;

static const GLchar* ReadShader( const char* filename ) {
    ifstream file(filename, ios::binary|ios::ate);
    if ( !file ) {
        cerr << "Unable to open " << filename << std::endl;
        return NULL;
    }

    int length = file.tellg();
    GLchar* data = new GLchar[length + 1];
    file.seekg(0, ios::beg);
    file.read(data, length);
    data[length] = 0;

    return data;
}

void cleanup(std::vector<ShaderInfo>& shaders) {
    // Delete the shaders we've already generated
    for (ShaderInfo& entry : shaders) {
        glDeleteShader( entry.shader );
        entry.shader = 0;
    }
}

GLuint LoadShaders( std::vector<ShaderInfo>& shaders ) {
    if ( shaders.empty() ) return 0;

    GLuint program = glCreateProgram();

    for (ShaderInfo& entry : shaders) {
        GLuint shader = glCreateShader(entry.type);
        entry.shader = shader;

        // Read the shader file
        const GLchar* source = ReadShader(entry.filename);
        if (!source) {
            cerr << "Error reading " << entry.filename << endl;
            cleanup(shaders);
            return 0;
        }

        // Compile the shader
        glShaderSource(shader, 1, &source, NULL);
        delete [] source;
        glCompileShader(shader);

        // Check for compilation errors
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            // Retrieve the compile log
            GLsizei length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            GLchar* buffer = new GLchar[length+1];
            glGetShaderInfoLog(shader, length, &length, buffer);
            cerr << "Failed to compile " << entry.filename << ": " << buffer << endl;

            delete [] buffer;
            cleanup(shaders);
            return 0;
        }

        // Add the shader to the program
        glAttachShader( program, shader );
    }

    // Link the program
    glLinkProgram( program );
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLsizei length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        GLchar* buffer = new GLchar[length+1];
        glGetProgramInfoLog(program, length, &length, buffer);
        cerr << "Shader linking failed: " << buffer << endl;

        cleanup(shaders);
        delete [] buffer;
        return 0;
    }

    return program;
}
