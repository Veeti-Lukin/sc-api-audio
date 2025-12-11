#pragma once
#include <QMouseEvent>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QVector2D>
#include <algorithm>
#include <vector>

class EqCurveWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    explicit EqCurveWidget(QWidget* parent = nullptr);
    ~EqCurveWidget();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

private:
    // Data
    std::vector<QVector2D> points;

    // GL objects
    GLuint lineVAO  = 0;
    GLuint lineVBO  = 0;

    GLuint pointVAO = 0;
    GLuint pointVBO = 0;

    QOpenGLShaderProgram lineShader;
    QOpenGLShaderProgram pointShader;

    // Interaction
    bool  dragging     = false;
    int   draggedIndex = -1;
    float pickRadius   = 0.03f;  // domain radius

    // Internal helpers
    QVector2D mapPixelToDomain(const QPoint&) const;
    int       findPoint(const QVector2D&, float radius) const;

    void rebuildLineVBO();
    void rebuildPointVBO();
};
