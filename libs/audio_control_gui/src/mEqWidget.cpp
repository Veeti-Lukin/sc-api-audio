#include "audio_control_gui/mEqWidget.h"

#include "ui_mEqWidget.h"

mEqWidget::mEqWidget(AudioProcessor* audio_processor, QWidget* parent)
    : QWidget(parent), ui(new Ui::mEqWidget), audio_processor_(audio_processor) {
    ui->setupUi(this);

    for (int i = 0; i < 3; i++) {
        mEqBandWidget* widget = new mEqBandWidget(this);
        band_widgets_.push_back(widget);
        ui->bandFilterLayout->addWidget(widget);

        connect(widget, &mEqBandWidget::freqChanged, this, &mEqWidget::updateBandValues);
        connect(widget, &mEqBandWidget::gainChanged, this, &mEqWidget::updateBandValues);
        connect(widget, &mEqBandWidget::qChanged, this, &mEqWidget::updateBandValues);
    }

    connect(ui->highpass_doubleSpinBox, &QDoubleSpinBox::valueChanged, this,
            [&]() { audio_processor_->setEqHighPassCutoff(ui->highpass_doubleSpinBox->value()); });
    connect(ui->lowpass_doubleSpinBox, &QDoubleSpinBox::valueChanged, this,
            [&]() { audio_processor_->setEqLowPassCutoff(ui->lowpass_doubleSpinBox->value()); });
}

mEqWidget::~mEqWidget() { delete ui; }

void mEqWidget::updateBandValues() {
    for (int band = 0; band < 3; band++) {
        audio_processor_->setEqBand(band, band_widgets_[band]->getFreq(), band_widgets_[band]->getGain(),
                                    band_widgets_[band]->getQ());
    }
}
