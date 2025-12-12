#ifndef SC_API_AUDIO_MAINWINDOW_H
#define SC_API_AUDIO_MAINWINDOW_H

#include <QMainWindow>

#include "audio_control_gui/AudioControlWidget.h"
#include "audio_player/AudioProcessor.h"
#include "gui/OutputDeviceControlWindow.h"
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
    MainWindow(const utils::ThreadSafeRingBuffer<float>& original_audio_samples, size_t capture_sample_rate,
               AudioProcessor& master_audio_processor, QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void updateDeviceList();

    Ui::MainWindow* ui;

    const utils::ThreadSafeRingBuffer<float>& original_audio_samples_;
    size_t                                    captrue_sample_rate_;

    AudioProcessor& master_audio_processor_;

    std::map<std::string_view, OutputDeviceControlWindow*> device_windows_;
};

}  // namespace gui

#endif  // SC_API_AUDIO_MAINWINDOW_H
