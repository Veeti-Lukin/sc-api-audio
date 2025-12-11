#ifndef MEQWIDGET_H
#define MEQWIDGET_H

#include "audio_control_gui/mEqBandWidget.h"
#include "audio_player/AudioProcessor.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class mEqWidget;
}
QT_END_NAMESPACE

class mEqWidget : public QWidget {
    Q_OBJECT

public:
    explicit mEqWidget(AudioProcessor* audio_processor, QWidget* parent = nullptr);
    ~mEqWidget() override;

private:
    void updateBandValues();

    Ui::mEqWidget* ui;

    std::vector<mEqBandWidget*> band_widgets_;

    AudioProcessor* audio_processor_ = nullptr;
};

#endif  // MEQWIDGET_H
