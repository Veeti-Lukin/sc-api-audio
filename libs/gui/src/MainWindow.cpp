#include "gui/MainWindow.h"

#include "ui/ui_MainWindow.h"

namespace gui {

MainWindow::MainWindow(const utils::ThreadSafeRingBuffer<float>& original_audio_samples,
                       size_t capture_sample_rate, QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      original_audio_samples_(original_audio_samples),
      captrue_sample_rate_(capture_sample_rate) {
    ui->setupUi(this);

}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::addOutputDevice(const std::string& device_name) {
        ui->deviceListWidget->addItem(device_name.c_str());

}

}  // namespace gui
