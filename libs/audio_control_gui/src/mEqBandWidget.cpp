#include "audio_control_gui/mEqBandWidget.h"

#include "ui_mEqBandWidget.h"

mEqBandWidget::mEqBandWidget(QWidget* parent) : QWidget(parent), ui(new Ui::mEqBandWidget) {
    ui->setupUi(this);

    ui->freq_dial->setValue(ui->freq_doubleSpinBox->value() * 10000.0f / ui->freq_doubleSpinBox->maximum());
    ui->gain_dial->setValue(ui->gain_doubleSpinBox->value() * 10000.0f / ui->gain_doubleSpinBox->maximum());
    ui->q_dial->setValue(ui->q_doubleSpinBox->value() * 10000.0f / ui->q_doubleSpinBox->maximum());

    connect(ui->freq_dial, &QDial::valueChanged, this, [this]() {
        ui->freq_doubleSpinBox->setValue(ui->freq_dial->value() / 10000.0f * ui->freq_doubleSpinBox->maximum());
    });

    connect(ui->gain_dial, &QDial::valueChanged, this, [this]() {
        ui->gain_doubleSpinBox->setValue(ui->gain_dial->value() / 10000.0f * ui->gain_doubleSpinBox->maximum());
    });

    connect(ui->q_dial, &QDial::valueChanged, this, [this]() {
        ui->q_doubleSpinBox->setValue(ui->q_dial->value() / 10000.0f * ui->q_doubleSpinBox->maximum());
    });

    connect(ui->freq_doubleSpinBox, &QDoubleSpinBox::valueChanged, this, &mEqBandWidget::freqChanged);
    connect(ui->gain_doubleSpinBox, &QDoubleSpinBox::valueChanged, this, &mEqBandWidget::gainChanged);
    connect(ui->q_doubleSpinBox, &QDoubleSpinBox::valueChanged, this, &mEqBandWidget::qChanged);
}

mEqBandWidget::~mEqBandWidget() { delete ui; }

float mEqBandWidget::getFreq() const { return ui->freq_doubleSpinBox->value(); }

float mEqBandWidget::getGain() const { return ui->gain_doubleSpinBox->value(); }

float mEqBandWidget::getQ() const { return ui->q_doubleSpinBox->value(); }
