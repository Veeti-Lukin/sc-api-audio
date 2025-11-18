#ifndef SC_API_AUDIO_WAVEFORMVISUALIZERWIDGET_H
#define SC_API_AUDIO_WAVEFORMVISUALIZERWIDGET_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

namespace gui {

class WaveFormVisualizerWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit WaveFormVisualizerWidget(QWidget* parent = nullptr);
    ~WaveFormVisualizerWidget() override;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
};

}  // namespace gui

#endif  // SC_API_AUDIO_WAVEFORMVISUALIZERWIDGET_H
