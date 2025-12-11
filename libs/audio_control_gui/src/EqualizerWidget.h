#ifndef SC_API_AUDIO_EQUALIZERWIDGET_H
#define SC_API_AUDIO_EQUALIZERWIDGET_H

#include <QWidget>

namespace audio_control_gui {
QT_BEGIN_NAMESPACE
namespace Ui {
class EqualizerWidget;
}
QT_END_NAMESPACE

class EqualizerWidget : public QWidget {
    Q_OBJECT

public:
    explicit EqualizerWidget(QWidget* parent = nullptr);
    ~EqualizerWidget() override;

private:
    Ui::EqualizerWidget* ui;
};
}  // namespace audio_control_gui

#endif  // SC_API_AUDIO_EQUALIZERWIDGET_H
