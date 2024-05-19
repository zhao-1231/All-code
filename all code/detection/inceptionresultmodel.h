#ifndef INCEPTIONRESULTMODEL_H
#define INCEPTIONRESULTMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>
#include <QMap>

//由图像处理线程管理

class InceptionResultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit InceptionResultModel(QObject *parent = nullptr);
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void UpdataDataContainer(const QMap<QString,float>& data);
private:
    QStringList m_headerDate;
    QVector<QMap<QString,float>> m_DataContainer;
private:
};

#endif // INCEPTIONRESULTMODEL_H
