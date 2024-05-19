#ifndef GRASCALETUNERDIA_H
#define GRASCALETUNERDIA_H
#pragma execution_character_set("utf-8")
#include <QDialog>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QFileDialog>

namespace Ui {
class GrascaleTunerDia;
}

class GrascaleTunerDia : public QDialog
{
    Q_OBJECT

public:
    explicit GrascaleTunerDia(QWidget *parent = nullptr);
    void SetInitialPathState(QStringList& theElem,int mode);
    void SetInitialGainState(QVector<double>& theElem,int mode);
    void ApplyG1SettingsSlot();
    void ApplyG2SettingsSlot();
    QStringList GetFFtPathsSettings(int mode);
    QVector<double> GetGainValsSettings(int mode);
    ~GrascaleTunerDia();
public:
    bool G1Ok{false};
    bool G2OK{false};
private:
    QString GetPickedFlatFiledPath();
    void setUpConnections();
    void SetSlideValRange();
private slots:
    void PickG1CamAFFTSlot();
    void PickG1CamBFFTSlot();
    void PickG1CamCFFTSlot();
    void PickG1CamDFFTSlot();
    void PickG2CamAFFTSlot();
    void PickG2CamBFFTSlot();
    void PickG2CamCFFTSlot();
    void PickG2CamDFFTSlot();

    void G1CamAGainChangedSkot(int);
    void G1CamBGainChangedSkot(int);
    void G1CamCGainChangedSkot(int);
    void G1CamDGainChangedSkot(int);
    void G2CamAGainChangedSkot(int);
    void G2CamBGainChangedSkot(int);
    void G2CamCGainChangedSkot(int);
    void G2CamDGainChangedSkot(int);

    void SaveSettingSlot();
    void LoadSettingSlot();
private:
    QStringList Group1FFtPaths;
    QStringList Group2FFtPaths;
    QVector<double> Group1Gains;
    QVector<double> Group2Gains;
private:
    Ui::GrascaleTunerDia *ui;
signals:
    //void Save();
};

#endif // GRASCALETUNERDIA_H
