#ifndef SC_API_AUDIO_WAVEFORMVISUALIZERWIDGET_H
#define SC_API_AUDIO_WAVEFORMVISUALIZERWIDGET_H

#include <QMutex>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <vector>

#include "utils/ThreadSafeRingBuffer.h"

namespace gui {

class WaveFormVisualizerWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    explicit WaveFormVisualizerWidget(QWidget* parent = nullptr);
    ~WaveFormVisualizerWidget() override;

    void init(utils::ThreadSafeRingBuffer<float>* ringBuffer, int maxSamples = 4096);
    void setAmplitudeScale(float amp);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    // External data source (not owned)
    utils::ThreadSafeRingBuffer<float>* ringBuffer_ = nullptr;

    // GL resources
    GLuint vao_                                     = 0;
    GLuint vbo_                                     = 0;
    GLuint shaderProgram_                           = 0;

    // CPU-side sliding buffer
    std::vector<float> slidingBuffer_;  // most recent samples
    std::vector<float> vertexBuffer_;   // interleaved (x,y)
    int                maxSamples_ = 0;

    float amplitudeScale_          = 1.0f;

    // Shader helpers
    GLuint compileShader(GLenum type, const char* src);
    GLuint linkProgram(GLuint vs, GLuint fs);

    // Data preparation
    void fetchSamples();
    void buildVertices();
};

}  // namespace gui

#endif
