#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCALL(x) clearGlErrors(); x; ASSERT(checkGlError(#x, __FILE__, __LINE__))

struct ShaderProgramSource {
    std::string vertexSource;
    std::string fragmentSource;
};

static ShaderProgramSource parseShader(const std::string &filePath) {
    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::ifstream stream(filePath);
    if(!stream) {
        std::cout << "Failed to open file: " << filePath << std::endl;
        return {"", ""};
    }
    ShaderType type = ShaderType::NONE;
    std::stringstream ss[2];

    std::string line;
    while(getline(stream, line)) {
        if(line.find("#define") != std::string::npos) {
            if(line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if(line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            if(type != ShaderType::NONE) {
                ss[(int) type] << line << '\n';
            }
        }
    }

    return {ss[0].str(), ss[1].str()};
}

static unsigned int compileShader(unsigned int shaderType, const std::string &source) {
    unsigned int shaderId = glCreateShader(shaderType);
    const char* src = source.c_str(); // pointer to first character of string
    glShaderSource(shaderId, 1, &src, nullptr);
    glCompileShader(shaderId);

    // Error handling
    int result;
    // i specifies integer, v specifies vector array
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char));
        glGetShaderInfoLog(shaderId, length, &length, message);
        std::cout << "Failed to compile " << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

static unsigned int createShader(const std::string &vertexShader, const std::string &fragmentShader) {
    unsigned int programId = glCreateProgram();
    unsigned int vertexShaderId = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fragmentShaderId = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // Attach the shaders to the programId
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);
    glValidateProgram(programId);

    // Delete the temporary shader objs
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    return programId;
}

static void clearGlErrors() {
    while(glGetError() != GL_NO_ERROR);
}

static bool checkGlError(const char* function, const char* file, int line) {
    while(GLenum error = glGetError()) {
        std::cout << "[OpenGL Error #" << error << "] " << function << " on line " << line << " in " << file << std::endl;
        return false;
    }
    return true;
}

int main() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Window hints
    glfwWindowHint(GLFW_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Sync frame rate to vsync
    glfwSwapInterval(1);

    // Initialize GLEW
    if(GLenum res = glewInit() != GLEW_OK) {
        std::cout << "Error: " << glewGetErrorString(res) << std::endl;
        return 1;
    }

    // Print OpenGL version
    std::cout << "OpenGL version [" << glGetString(GL_VERSION) << "]" << std::endl;

    float positions[] = {
        -0.5f, -0.5f,  // 0
         0.5f,  -0.5f, // 1
         0.5f, 0.5f,   // 2
         -0.5f, 0.5f   // 3
    };

    unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
    };

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), &positions, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    unsigned int ibo; // Index buffer object ID
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indices, GL_STATIC_DRAW);

    ShaderProgramSource source = parseShader("res/shaders/default.glsl");

    unsigned int shaderId = createShader(source.vertexSource, source.fragmentSource);
    glUseProgram(shaderId);

    // Upload uniform data to bound shader
    int location = glGetUniformLocation(shaderId, "uColour");
    glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f);

    // Unbinding everything
    glBindVertexArray(0);
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    float r = 0.0f;
    bool increment = true;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        if(r > 1.0f) {
            r = 1.0f;
            increment = false;
        } else if(r < 0.0f) {
            r = 0.0f;
            increment = true;
        }

        r = r + (increment ? 0.01f : -0.01f);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        positions[0] = 0.5f - r;
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), &positions, GL_STATIC_DRAW);

        glUseProgram(shaderId);
        glUniform4f(location, r, 0.3f, 0.8f, 1.0f);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        glUseProgram(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    //glDeleteProgram(shaderId);

    glfwTerminate();
    return 0;
}
