#include "WaveFormVisualizerWidget.h"

#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <QTimer>
#include <sstream>
#include <stdexcept>

namespace gui {

// Same shaders as before
static constexpr const char* kVertexShaderSrc   = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

static constexpr const char* kFragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 uColor;
void main() {
    FragColor = uColor;
}
)";

WaveFormVisualizerWidget::WaveFormVisualizerWidget(QWidget* parent) : QOpenGLWidget(parent) {}

WaveFormVisualizerWidget::~WaveFormVisualizerWidget() {
    makeCurrent();
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (shaderProgram_) glDeleteProgram(shaderProgram_);
    doneCurrent();
}
void WaveFormVisualizerWidget::init(utils::ThreadSafeRingBuffer<float>* ringBuffer, int maxSamples) {
    ringBuffer_ = ringBuffer;
    maxSamples_ = maxSamples;

    slidingBuffer_.reserve(maxSamples_);
    vertexBuffer_.reserve(maxSamples_ * 2);

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    timer->start(10);  // 100 fps
}

void WaveFormVisualizerWidget::setAmplitudeScale(float amp) { amplitudeScale_ = amp; }

GLuint WaveFormVisualizerWidget::compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetShaderInfoLog(shader, len, nullptr, log.data());
        glDeleteShader(shader);
        throw std::runtime_error("Shader error: " + log);
    }
    return shader;
}

GLuint WaveFormVisualizerWidget::linkProgram(GLuint vs, GLuint fs) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(prog, len, nullptr, log.data());
        glDeleteProgram(prog);
        throw std::runtime_error("Link error: " + log);
    }
    return prog;
}

void WaveFormVisualizerWidget::initializeGL() {
    initializeOpenGLFunctions();

    // Background
    QStyle*  fusion = QStyleFactory::create("Fusion");
    QPalette pal    = fusion->standardPalette();
    QColor   bg     = pal.color(QPalette::Dark);
    glClearColor(bg.redF(), bg.greenF(), bg.blueF(), bg.alphaF());

    GLuint vs      = compileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
    GLuint fs      = compileShader(GL_FRAGMENT_SHADER, kFragmentShaderSrc);
    shaderProgram_ = linkProgram(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, maxSamples_ * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(shaderProgram_);
    GLint colorLoc = glGetUniformLocation(shaderProgram_, "uColor");
    glUniform4f(colorLoc, 0.0f, 1.0f, 0.0f, 1.0f);
    glUseProgram(0);
}

void WaveFormVisualizerWidget::fetchSamples() {
    // read as many as fit
    float temp[512];
    while (true) {
        auto popped = ringBuffer_->pop(std::span<float>(temp, 512));
        if (popped.empty()) break;

        int i = 0;
        for (float v : popped) {
            i++;
            if (i % 2 == 0) continue;
            slidingBuffer_.push_back(v);
            if (slidingBuffer_.size() > maxSamples_) slidingBuffer_.erase(slidingBuffer_.begin());
        }
    }
}

void WaveFormVisualizerWidget::buildVertices() {
    size_t N = slidingBuffer_.size();
    vertexBuffer_.resize(N * 2);

    if (N == 0) return;

    for (size_t i = 0; i < N; ++i) {
        float sx = static_cast<float>(i) / (N - 1);
        float x  = sx * 2.0f - 1.0f;
        float y  = slidingBuffer_[i] * amplitudeScale_;
        if (y > 1.f) y = 1.f;
        if (y < -1.f) y = -1.f;

        vertexBuffer_[2 * i + 0] = x;
        vertexBuffer_[2 * i + 1] = y;
    }
}

void WaveFormVisualizerWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    fetchSamples();
    buildVertices();

    if (slidingBuffer_.empty()) return;

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBuffer_.size() * sizeof(float), vertexBuffer_.data());

    glUseProgram(shaderProgram_);
    glBindVertexArray(vao_);
    glDrawArrays(GL_LINE_STRIP, 0, slidingBuffer_.size());
    glBindVertexArray(0);
}

void WaveFormVisualizerWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

}  // namespace gui
