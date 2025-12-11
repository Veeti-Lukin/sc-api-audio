
#ifndef SC_API_AUDIO_AUDIOCONTROLWIDGET_H
#define SC_API_AUDIO_AUDIOCONTROLWIDGET_H

#include <QWidget>

namespace audio_control_gui {
QT_BEGIN_NAMESPACE
namespace Ui {
class AudioControlWidget;
}
QT_END_NAMESPACE

class AudioControlWidget : public QWidget {
    Q_OBJECT

public:
    explicit AudioControlWidget(QWidget* parent = nullptr);
    ~AudioControlWidget() override;

private:
    Ui::AudioControlWidget* ui;
};
}  // namespace audio_control_gui

#endif  // SC_API_AUDIO_AUDIOCONTROLWIDGET_H
