#include "gui/MainWindow.h"

#include "ui/ui_MainWindow.h"

namespace gui {

MainWindow::MainWindow(utils::ThreadSafeRingBuffer<float>& capture_to_processing_buffer, size_t capture_sample_rate,
                       QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      capture_to_processing_buffer_(capture_to_processing_buffer),
      captrue_sample_rate_(capture_sample_rate) {
    ui->setupUi(this);

    ui->waveFormVizualiserWidget->init(&capture_to_processing_buffer);
}

MainWindow::~MainWindow() { delete ui; }

}  // namespace gui
