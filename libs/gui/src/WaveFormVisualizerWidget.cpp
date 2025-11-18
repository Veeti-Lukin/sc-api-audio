#include "WaveFormVisualizerWidget.h"

#include <QTimer>

namespace gui {

WaveFormVisualizerWidget::WaveFormVisualizerWidget(QWidget* parent) : QOpenGLWidget(parent) {
    // TODO what is this?
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);  // TODO change to Coreprofile
    setFormat(format);  // must be called before the widget or its parent window gets shown

    // Simple animation timer
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    timer->start(16);  // ~60 FPS
}

WaveFormVisualizerWidget::~WaveFormVisualizerWidget() {}

void WaveFormVisualizerWidget::initializeGL() {
    initializeOpenGLFunctions();
    // Fixed-function pipeline doesn't need shaders
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}
void WaveFormVisualizerWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Set line color
    glColor3f(0.0f, 1.0f, 0.0f);  // green

    // Draw a simple triangle using LINE_STRIP
    glBegin(GL_LINE_STRIP);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(-0.5f, -0.5f);  // close the triangle
    glEnd();
}
void WaveFormVisualizerWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);

    // TODO random ai code
    // Optional: simple orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

}  // namespace gui
