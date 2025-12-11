#ifndef SC_API_AUDIO_OUTPUTDEVICECONTROLWINDOW_H
#define SC_API_AUDIO_OUTPUTDEVICECONTROLWINDOW_H

#include <QMainWindow>

#include "audio_player/OutputDevice.h"

namespace gui {
QT_BEGIN_NAMESPACE
namespace Ui {
class OutputDeviceControlWindow;
}
QT_END_NAMESPACE

class OutputDeviceControlWindow : public QMainWindow {
    Q_OBJECT

public:
    OutputDeviceControlWindow(std::shared_ptr<OutputDevice> output_device, QWidget* parent = nullptr);
    ~OutputDeviceControlWindow() override;

private:
    Ui::OutputDeviceControlWindow* ui;

    std::shared_ptr<OutputDevice> output_device_;
};

}  // namespace gui

#endif  // SC_API_AUDIO_OUTPUTDEVICECONTROLWINDOW_H
