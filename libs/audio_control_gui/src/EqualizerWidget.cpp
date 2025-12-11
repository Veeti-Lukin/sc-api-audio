#include "EqualizerWidget.h"

#include "ui_EqualizerWidget.h"

namespace audio_control_gui {
EqualizerWidget::EqualizerWidget(QWidget* parent) : QWidget(parent), ui(new Ui::EqualizerWidget) { ui->setupUi(this); }

EqualizerWidget::~EqualizerWidget() { delete ui; }
}  // namespace audio_control_gui
