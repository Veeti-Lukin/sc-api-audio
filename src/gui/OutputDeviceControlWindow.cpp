#include "gui/OutputDeviceControlWindow.h"

#include <utility>

#include "ui_OutputDeviceControlWindow.h"

namespace gui {
OutputDeviceControlWindow::OutputDeviceControlWindow(std::shared_ptr<OutputDevice> output_device, QWidget* parent)
    : QMainWindow(parent), ui(new Ui::OutputDeviceControlWindow), output_device_(std::move(output_device)) {
    ui->setupUi(this);

    mEqWidget_ = new mEqWidget(output_device_->getAudioProcessor(), this);
    ui->eqFrame->layout()->addWidget(mEqWidget_);

    ui->audioControlWidget->setAudioProcessor(output_device_->getAudioProcessor());
    connect(ui->leftRightHorizontalSlider, &QSlider::valueChanged, this, [this]() {
        float pan = ui->leftRightHorizontalSlider->value() / 100.0f;
        output_device_->setPan(pan);
    });

    QString window_title = QString::fromStdString(std::string(output_device_->getDeviceInfo()->getUid()));
    setWindowTitle(std::move(window_title));
}

OutputDeviceControlWindow::~OutputDeviceControlWindow() { delete ui; }
}  // namespace gui
