#include "EqCurveWidget.h"

EqCurveWidget::EqCurveWidget(QWidget* parent) : QOpenGLWidget(parent) {
    points.push_back({0.0f, 0.0f});
    points.push_back({1.0f, 0.0f});
}

EqCurveWidget::~EqCurveWidget() {
    makeCurrent();
    glDeleteBuffers(1, &lineVBO);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &pointVBO);
    glDeleteVertexArrays(1, &pointVAO);
    doneCurrent();
}

void EqCurveWidget::initializeGL() {
    initializeOpenGLFunctions();

    // --- Shaders ---
    lineShader.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                       "#version 330 core\n"
                                       "layout(location = 0) in vec2 pos;\n"
                                       "void main() {\n"
                                       "    gl_Position = vec4(pos * 2.0 - 1.0, 0.0, 1.0);\n"
                                       "}\n");

    lineShader.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                       "#version 330 core\n"
                                       "out vec4 frag;\n"
                                       "void main() { frag = vec4(0.2, 0.7, 1.0, 1.0); }\n");
    lineShader.link();

    pointShader.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                        "#version 330 core\n"
                                        "layout(location = 0) in vec2 pos;\n"
                                        "void main() {\n"
                                        "    gl_PointSize = 10.0;\n"
                                        "    gl_Position = vec4(pos * 2.0 - 1.0, 0.0, 1.0);\n"
                                        "}\n");

    pointShader.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                        "#version 330 core\n"
                                        "out vec4 frag;\n"
                                        "void main() { frag = vec4(1.0, 0.2, 0.2, 1.0); }\n");
    pointShader.link();

    // --- Buffers ---
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);

    rebuildLineVBO();
    rebuildPointVBO();
}

void EqCurveWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void EqCurveWidget::paintGL() {
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // --- Line ---
    lineShader.bind();
    glBindVertexArray(lineVAO);
    glDrawArrays(GL_LINE_STRIP, 0, points.size());

    // --- Points ---
    pointShader.bind();
    glBindVertexArray(pointVAO);
    glDrawArrays(GL_POINTS, 0, points.size());
}

QVector2D EqCurveWidget::mapPixelToDomain(const QPoint& p) const {
    float x = float(p.x()) / width();
    float y = 1.0f - float(p.y()) / height();
    return {x, y};
}

int EqCurveWidget::findPoint(const QVector2D& p, float radius) const {
    float r2 = radius * radius;
    for (int i = 0; i < points.size(); ++i) {
        if ((points[i] - p).lengthSquared() <= r2) return i;
    }
    return -1;
}

void EqCurveWidget::mousePressEvent(QMouseEvent* ev) {
    QVector2D p   = mapPixelToDomain(ev->pos());
    int       idx = findPoint(p, pickRadius);

    if (idx >= 0) {
        dragging     = true;
        draggedIndex = idx;
    } else {
        // insert new point
        points.push_back(p);
        std::sort(points.begin(), points.end(), [](auto& a, auto& b) { return a.x() < b.x(); });
        rebuildLineVBO();
        rebuildPointVBO();
    }

    update();
}

void EqCurveWidget::mouseMoveEvent(QMouseEvent* ev) {
    if (!dragging) return;

    QVector2D p          = mapPixelToDomain(ev->pos());
    points[draggedIndex] = p;

    std::sort(points.begin(), points.end(), [](auto& a, auto& b) { return a.x() < b.x(); });

    rebuildLineVBO();
    rebuildPointVBO();
    update();
}

void EqCurveWidget::mouseReleaseEvent(QMouseEvent*) {
    dragging     = false;
    draggedIndex = -1;
    update();
}

void EqCurveWidget::rebuildLineVBO() {
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);

    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(QVector2D), points.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QVector2D), nullptr);
    glEnableVertexAttribArray(0);
}

void EqCurveWidget::rebuildPointVBO() {
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);

    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(QVector2D), points.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QVector2D), nullptr);
    glEnableVertexAttribArray(0);
}
