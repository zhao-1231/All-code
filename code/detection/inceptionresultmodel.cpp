#include "inceptionresultmodel.h"
#include <QDebug>

InceptionResultModel::InceptionResultModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headerDate<<"Name"<<"Width(AB)"<<"Height(BC)"<<"Width(CD)"<<"Height(AD)"<<"Diag(AC)"<<"Diag(BD)"<<"Angle(A)"<<"Angle(B)"<<"Angle(C)"<<"Angle(D)";

}

QVariant InceptionResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return m_headerDate[section];
    return QAbstractTableModel::headerData(section,orientation,role);
}

int InceptionResultModel::rowCount(const QModelIndex &parent) const
{
    return 50;  //最多显示50条信息,可以提供查询
}

int InceptionResultModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 11;
}

QVariant InceptionResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(role == Qt::DisplayRole){
        if(index.row() > (m_DataContainer.size() - 1))
            return QVariant{};
        QMap<QString,float> curData = m_DataContainer.at(index.row());
        if(index.column() != 0){
            return curData[m_headerDate[index.column()]];
        }else{//name
            int GlassIndex = curData["Name"];
            //qDebug()<<"GlassIndex: "<<GlassIndex;
            QString str,glassName;
            str.setNum(GlassIndex);
            glassName = "Glass_" + str;
            return glassName;
        }
    }
    return QVariant();
}

void InceptionResultModel::UpdataDataContainer(const QMap<QString,float>& data)
{
    m_DataContainer.push_front(data);
}





















