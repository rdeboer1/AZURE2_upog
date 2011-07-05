#include "ExternalCaptureModel.h"
#include "PairsModel.h"

ExternalCaptureModel::ExternalCaptureModel(QObject *parent) : QAbstractTableModel(parent) {
}

int ExternalCaptureModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return ecLineList.size();
}

int ExternalCaptureModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return ExternalCaptureData::SIZE;
}

QVariant ExternalCaptureModel::data(const QModelIndex &index, int role) const {
  if(!index.isValid()) return QVariant();

  if(index.row() >= ecLineList.size() || index.row() < 0)  return QVariant();

  if (role == Qt::DisplayRole) {
    ExternalCaptureData line = ecLineList.at(index.row());
    if(index.column() == 1) {
      if(pairsModel->getPairs().size()>=line.exitPairIndex) {
	PairsData pair=pairsModel->getPairs().at(line.exitPairIndex-1);
	return pairsModel->getParticleLabel(pair);
      } else return QString("<center><font style='color:red;font-weight:bold'>UNDEFINED</font></center>");
    } else if(index.column() == 2) return getSpinLabel(line,0);
    else if(index.column() == 3) return getSpinLabel(line,1);
    else if(index.column() == 4) return parseMultMask(line.multMask);
  } else if (role == Qt::EditRole) {
    ExternalCaptureData line = ecLineList.at(index.row());
    if(index.column() == 1) return line.exitPairIndex;
    else if(index.column() == 2) return line.minJ;
    else if(index.column() == 3) return line.maxJ;
    else if(index.column() == 4) return line.multMask;
  } else if (role==Qt::CheckStateRole && index.column()==0) {
    ExternalCaptureData line = ecLineList.at(index.row());
    if(line.isActive==1) return Qt::Checked;
    else return Qt::Unchecked;
  } else if(role == Qt::TextAlignmentRole) return Qt::AlignCenter;

  return QVariant();
}

QVariant ExternalCaptureModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if(role!= Qt::DisplayRole) return QVariant();
  if(orientation == Qt::Horizontal) {
    switch(section) {
    case 0:
      return tr("");
    case 1:
      return tr("Capture Pair");
    case 2:
      return tr("Minimum\nInitial J");
    case 3:
      return tr("Maximum\nInitial J");
    case 4:
      return tr("Multipolarities");
    default: 
      return QVariant();
    }
  } else if(orientation == Qt::Vertical) {
    return section+1;
  }
  return QVariant();
}

bool ExternalCaptureModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (index.isValid() && role == Qt::EditRole ) {
    int row = index.row();
    ExternalCaptureData tempData = ecLineList.value(row);
    if(index.column() == 0) tempData.isActive=value.toInt();
    else if(index.column() == 1) tempData.exitPairIndex=value.toInt();
    else if(index.column() == 2) tempData.minJ=value.toDouble();
    else if(index.column() == 3) tempData.maxJ=value.toDouble();
    else if(index.column() == 4) tempData.multMask=value.toInt();
    else return false;

    ecLineList.replace(row,tempData);
    emit(dataChanged(index,index));
    return true;
  } else if(role== Qt::CheckStateRole) {
    int row = index.row();
    ExternalCaptureData tempData = ecLineList.value(row);
    if(index.column()==0) {
      if(value==Qt::Checked) tempData.isActive=1;
      else tempData.isActive=0;
    } else return false;
    ecLineList.replace(row,tempData);
    emit(dataChanged(index,index));
    return true;
  }
  return false;
}

bool ExternalCaptureModel::insertRows(int position, int rows, const QModelIndex &index) {
  Q_UNUSED(index);
  if(rows>0) {
    beginInsertRows(QModelIndex(),position,position+rows-1);
    for(int row=0; row<rows; row++) {
      ExternalCaptureData tempData={0,0,0.,0.,0};
      ecLineList.insert(position,tempData);
    }
    endInsertRows();
  }
  return true;
}

bool ExternalCaptureModel::removeRows(int position, int rows, const QModelIndex &index) {
  Q_UNUSED(index);
  if(rows>0) {
    beginRemoveRows(QModelIndex(),position,position+rows-1);
    for(int row=0; row<rows;++row) {
      ecLineList.removeAt(position);
    }
    endRemoveRows();
  }
  return true;
}

Qt::ItemFlags ExternalCaptureModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return Qt::ItemIsEnabled;
  if(index.column()==0) return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
  return QAbstractTableModel::flags(index);
}

int ExternalCaptureModel::isECLine(const ExternalCaptureData &line) const {
  int foundLine=-1;
  for(int i=0;i<ecLineList.size();i++) {
    ExternalCaptureData tempLine=ecLineList.value(i);
    if(tempLine.exitPairIndex==line.exitPairIndex&&
       tempLine.minJ==line.minJ&&
       tempLine.maxJ==line.maxJ&&
       tempLine.multMask==line.multMask) {
      foundLine=i;
      break;
    }
  }
  return foundLine;
}

QString ExternalCaptureModel::getSpinLabel(const ExternalCaptureData &line, int which) const {
  double tempJ;
  if(which==0) tempJ=line.minJ;
  else tempJ=line.maxJ;

  if(((int)(tempJ*2))%2!=0&&tempJ!=0.) return QString("%1/2").arg((int)(tempJ*2));
  else return QString("%1").arg(tempJ);
}

void ExternalCaptureModel::setPairsModel(PairsModel* model) {
  pairsModel=model;
}

QString ExternalCaptureModel::parseMultMask(unsigned char multMask) const {
  QList<QString> mults;
  if(multMask&(1<<0)) mults.push_back(QString("E1"));
  if(multMask&(1<<1)) mults.push_back(QString("M1"));
  if(multMask&(1<<2)) mults.push_back(QString("E2"));

  QString label="";
  for(int i=0;i<mults.size();i++) {
    label+=mults.at(i);
    if(i!=mults.size()-1) label+=", ";
  }
  return label;
}
