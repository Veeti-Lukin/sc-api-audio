#include "audio_control_gui/mEqBandWidget.h"

#include "ui_mEqBandWidget.h"

mEqBandWidget::mEqBandWidget(QWidget* parent) : QWidget(parent), ui(new Ui::mEqBandWidget) {
    ui->setupUi(this);

    connect(ui->freq_doubleSpinBox, &QDoubleSpinBox::valueChanged, this, &mEqBandWidget::freqChanged);
    connect(ui->gain_doubleSpinBox, &QDoubleSpinBox::valueChanged, this, [this]() {
        qDebug() << "gain changed";
        emit gainChanged(getGain());
    });
    connect(ui->q_doubleSpinBox, &QDoubleSpinBox::valueChanged, this, &mEqBandWidget::qChanged);
}

mEqBandWidget::~mEqBandWidget() { delete ui; }

float mEqBandWidget::getFreq() const { return ui->freq_doubleSpinBox->value(); }

float mEqBandWidget::getGain() const { return ui->gain_doubleSpinBox->value(); }

float mEqBandWidget::getQ() const { return ui->q_doubleSpinBox->value(); }
