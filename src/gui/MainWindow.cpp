#include "gui/MainWindow.h"

#include <ScApiContext.h>
#include <gui/OutputDeviceControlWindow.h>

#include <QListWidgetItem>
#include <QTimer>

#include "audio_player/OutputDevice.h"
#include "ui/ui_MainWindow.h"

namespace gui {

MainWindow::MainWindow(const utils::ThreadSafeRingBuffer<float>& original_audio_samples, size_t capture_sample_rate,
                       AudioProcessor& master_audio_processor, QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      original_audio_samples_(original_audio_samples),
      captrue_sample_rate_(capture_sample_rate),
      master_audio_processor_(master_audio_processor) {
    ui->setupUi(this);
    setWindowTitle(QApplication::applicationName());

    ui->audioControlWidget->setAudioProcessor(&master_audio_processor_);

    QObject::connect(ui->deviceListWidget, &QListWidget::itemClicked, this, [parent = this](QListWidgetItem* item) {
        // Get the device data stored in the item
        auto device                       = item->data(Qt::UserRole).value<std::shared_ptr<OutputDevice>>();

        OutputDeviceControlWindow* window = new OutputDeviceControlWindow(device, parent);
        window->show();
    });

    updateDeviceList();
    QTimer* device_update_timer = new QTimer(this);
    connect(device_update_timer, &QTimer::timeout, this, &MainWindow::updateDeviceList);
    device_update_timer->start(2500);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::updateDeviceList() {
    ui->deviceListWidget->clear();

    for (std::shared_ptr device : ScApiContext::getInstance()->getConnectedDevices()) {
        QString          item_text = QString::fromStdString(std::string(device->getDeviceInfo()->getUid()));
        QListWidgetItem* list_item = new QListWidgetItem(item_text, ui->deviceListWidget);
        list_item->setData(Qt::UserRole, QVariant::fromValue(device));
        ui->deviceListWidget->addItem(list_item);
    }
}

}  // namespace gui
