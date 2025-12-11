// You may need to build the project (run Qt uic code generator) to get "ui_AudioControlWidget.h" resolved

#include "audio_control_gui/AudioControlWidget.h"

#include "ui_AudioControlWidget.h"

namespace audio_control_gui {
AudioControlWidget::AudioControlWidget(QWidget* parent) : QWidget(parent), ui(new Ui::AudioControlWidget) {
    ui->setupUi(this);
}

AudioControlWidget::~AudioControlWidget() { delete ui; }

void AudioControlWidget::setAudioProcessor(AudioProcessor* audio_processor) {
    audio_processor_ = audio_processor;
    connect(ui->gainDial, &QDial::valueChanged, this, [this]() {
        float gain = ui->gainDial->value() / 100.0f * 3.0f;
        audio_processor_->setGain(gain);
    });
}

}  // namespace audio_control_gui
