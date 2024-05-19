#ifndef SIGNALTUNER_H
#define SIGNALTUNER_H
#pragma execution_character_set("utf-8")
#include <QDialog>
#include "camerabasic.h"
namespace Ui {
class SignalTuner;
}

class SignalTuner : public QDialog
{
    Q_OBJECT

public:
    explicit SignalTuner(QWidget *parent = nullptr);
    ~SignalTuner();
    void updateVal();
    void SetCameras(const QVector<CameraBasic*>& elems) {Cameras = elems;updateVal();}
private:
    void SetUpConnections();
    void SetSlidersRange();
private:
    Ui::SignalTuner *ui;
private slots:
    void G1FactorChanged(int);
    void G1DividerChanged(int);
    void G2FactorChanged(int);
    void G2DividerChanged(int);
    void ApplyCurrentSettingSlot();
    void SaveCurrentSettingSlot();
    void LoadExistingSettingSlot();
private:
    QVector<CameraBasic*> Cameras;
    int m_G1FactorVal{};
    int m_G1DividerVal{};
    int m_G2FactorVal{};
    int m_G2DividerVal{};
    int m_G1LinePerSignalVal{};
    int m_G2LinePerSignalVal{};
    int CamNumsPerGroup{4};
signals:
    void sendToLogger(QString,QColor);
};

#endif // SIGNALTUNER_H
