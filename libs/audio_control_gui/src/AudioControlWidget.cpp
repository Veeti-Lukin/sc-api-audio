// You may need to build the project (run Qt uic code generator) to get "ui_AudioControlWidget.h" resolved

#include "audio_control_gui/AudioControlWidget.h"

#include "ui_AudioControlWidget.h"

namespace audio_control_gui {
AudioControlWidget::AudioControlWidget(QWidget* parent) : QWidget(parent), ui(new Ui::AudioControlWidget) {
    ui->setupUi(this);
}

AudioControlWidget::~AudioControlWidget() { delete ui; }
}  // namespace audio_control_gui
