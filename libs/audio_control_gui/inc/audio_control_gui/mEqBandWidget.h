#ifndef MEQBANDWIDGET_H
#define MEQBANDWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class mEqBandWidget;
}
QT_END_NAMESPACE

class mEqBandWidget : public QWidget {
    Q_OBJECT

public:
    explicit mEqBandWidget(QWidget* parent = nullptr);
    ~mEqBandWidget() override;

    float getFreq() const;
    float getGain() const;
    float getQ() const;

signals:
    void freqChanged(float value);
    void gainChanged(float value);
    void qChanged(float value);

private:
    Ui::mEqBandWidget* ui;
};

#endif  // MEQBANDWIDGET_H
