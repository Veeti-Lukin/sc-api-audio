#ifndef SC_API_AUDIO_MAINWINDOW_H
#define SC_API_AUDIO_MAINWINDOW_H

#include <QMainWindow>

#include "utils/ThreadSafeRingBuffer.h"

namespace gui {

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(const utils::ThreadSafeRingBuffer<float>& capture_to_processing_buffer, size_t capture_sample_rate,
               QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow* ui;

    const utils::ThreadSafeRingBuffer<float>& capture_to_processing_buffer_;
    size_t                                    captrue_sample_rate_;
};

}  // namespace gui

#endif  // SC_API_AUDIO_MAINWINDOW_H
