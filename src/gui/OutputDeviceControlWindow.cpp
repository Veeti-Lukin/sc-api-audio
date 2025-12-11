// You may need to build the project (run Qt uic code generator) to get "ui_OutputDeviceControlWindow.h" resolved

#include "gui/OutputDeviceControlWindow.h"

#include <utility>

#include "ui_OutputDeviceControlWindow.h"

namespace gui {
OutputDeviceControlWindow::OutputDeviceControlWindow(std::shared_ptr<OutputDevice> output_device, QWidget* parent)
    : QMainWindow(parent), ui(new Ui::OutputDeviceControlWindow), output_device_(std::move(output_device)) {
    ui->setupUi(this);

    QString window_title = QString::fromStdString(std::string(output_device_->getDeviceInfo()->getUid()));
    setWindowTitle(std::move(window_title));
}

OutputDeviceControlWindow::~OutputDeviceControlWindow() { delete ui; }
}  // namespace gui
