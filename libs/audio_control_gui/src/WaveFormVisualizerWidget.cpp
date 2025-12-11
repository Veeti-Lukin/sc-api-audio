#include "WaveFormVisualizerWidget.h"

#include <QColor>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <QTimer>

#include "WaveFormVisualizerWidget_fragment_shader.h"
#include "WaveFormVisualizerWidget_vertex_shader.h"

namespace audio_control_gui {

WaveFormVisualizerWidget::WaveFormVisualizerWidget(QWidget* parent) : QOpenGLWidget(parent) {
    QStyle*  fusion_style   = QStyleFactory::create("Fusion");
    QPalette palette        = fusion_style->standardPalette();
    QColor   color          = palette.color(QPalette::Dark);
    background_color_.red   = color.redF();
    background_color_.green = color.greenF();
    background_color_.blue  = color.blueF();
    background_color_.alpha = color.alphaF();

    // Initialize OpenGL Context
    // NOTE: by default each Qt OpenGL widget has its own context
    QSurfaceFormat format;
    format.setDepthBufferSize(24);   // TODO what is this
    format.setStencilBufferSize(8);  // TODO what is this
    format.setVersion(3, 3);         // Used OpenGL version
    // https://learnopengl.com/Getting-started/OpenGL # Core-profile vs Immediate mode
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);  // must be called before the widget or its parent window gets shown

    // Simple animation timer
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    timer->start(16);  // ~60 FPS
}

WaveFormVisualizerWidget::~WaveFormVisualizerWidget() { glDeleteProgram(shader_program_id_); }

void WaveFormVisualizerWidget::initializeGL() {
    initializeOpenGLFunctions();

    auto checkShaderCompilationSuccess = [this](GLuint shader_id, std::string_view shader_type) -> void {
        GLint success;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        if (!static_cast<bool>(success)) {
            char info_log[512] = {'\0'};
            glGetShaderInfoLog(shader_id, 512, nullptr, info_log);
            throw std::runtime_error(std::format("ERROR: SHADER COMPILATION FAILED (Shader type: {}): {}", shader_type,
                                                 std::string(info_log)));
        }
    };

    auto checkShaderProgramLinkSuccess = [this](GLuint program_id) -> void {
        GLint success;
        glGetProgramiv(program_id, GL_LINK_STATUS, &success);
        if (!static_cast<bool>(success)) {
            char info_log[512] = {'\0'};
            glGetProgramInfoLog(program_id, 512, nullptr, info_log);
            throw std::runtime_error(std::format("ERROR: SHADER PROGRAM LINKING FAILED: {}", std::string(info_log)));
        }
    };

    // CREATE VBO
    // Just like any object in OpenGL, this buffer has a unique ID corresponding to that buffer, so we can generate one
    // with a buffer ID using the glGenBuffers function:
    glGenBuffers(1, &VBO_id_);
    // OpenGL has many types of buffer objects and the buffer type of a vertex buffer object is
    // GL_ARRAY_BUFFER.OpenGL allows us to bind to several buffers at once as long as they have a different
    // buffer type.We can bind the newly created buffer to the GL_ARRAY_BUFFER target with the glBindBuffer function :
    glBindBuffer(GL_ARRAY_BUFFER, VBO_id_);

    // In order for OpenGL to use the shader it has to dynamically compile it at run-time from its source code. The
    // first thing we need to do is create a shader object, again referenced by an ID. So we store the vertex shader and
    // create the shader with glCreateShader:
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &K_VERTEX_SHADER_SOURCE, nullptr);
    glCompileShader(vertex_shader_id);
    checkShaderCompilationSuccess(vertex_shader_id, "VERTEX");

    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &K_FRAGMENT_SHADER_SOURCE, nullptr);
    glCompileShader(fragment_shader_id);
    checkShaderCompilationSuccess(fragment_shader_id, "FRAGMENT");

    // A shader program object is the final linked version of multiple shaders combined. To use the recently compiled
    // shaders we have to link them to a shader program object and then activate this shader program when rendering
    // objects. The activated shader program's shaders will be used when we issue render calls.
    shader_program_id_ = glCreateProgram();
    glAttachShader(shader_program_id_, vertex_shader_id);
    glAttachShader(shader_program_id_, fragment_shader_id);
    glLinkProgram(shader_program_id_);
    checkShaderProgramLinkSuccess(shader_program_id_);
    glUseProgram(shader_program_id_);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    //  We can clear the screen's color buffer using glClear where we pass in buffer bits to specify which buffer we
    //  would like to clear. The possible bits we can set are GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and
    //  GL_STENCIL_BUFFER_BIT. Right now we only care about the color values so we only clear the color buffer.
    glClearColor(background_color_.red, background_color_.green, background_color_.blue, background_color_.alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}
void WaveFormVisualizerWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    // TODO halutaan käyttää glBegin(GL_LINE_STRIP);
    float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};
    // From that point on any buffer calls we make (on the GL_ARRAY_BUFFER target) will be used to configure the
    // currently bound buffer, which is VBO. Then we can make a call to the glBufferData function that copies the
    // previously defined vertex data into the buffer's memory:
    //  usage type of GL_DYNAMIC_DRAW ensures the graphics card will place the data in memory that allows for faster
    //  writes.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                 GL_STATIC_DRAW);  // GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    // TODO should this use glBufferSubData which updates only the bytes that have changed, faster and more efficient?

    /*
    // Set line color
    glColor3f(0.0f, 1.0f, 0.0f);  // green

    // Draw a simple triangle using LINE_STRIP
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(-0.5f, -0.5f);  // close the triangle
    glEnd();*/
}
void WaveFormVisualizerWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);

    /*
    // TODO random ai code
    // Optional: simple orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();*/
}

}  // namespace audio_control_gui
