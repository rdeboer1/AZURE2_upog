#include "SegmentsTestModel.h"
#include "PairsModel.h"

SegmentsTestModel::SegmentsTestModel(QObject *parent) : QAbstractTableModel(parent) {
}

int SegmentsTestModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return segTestLineList.size();
}

int SegmentsTestModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return SegmentsTestData::SIZE;
}

QVariant SegmentsTestModel::data(const QModelIndex &index, int role) const {
  if(!index.isValid()) return QVariant();

  if(index.row() >= segTestLineList.size() || index.row() < 0)  return QVariant();

  if (role == Qt::DisplayRole) {
    SegmentsTestData line = segTestLineList.at(index.row());
    if(index.column() == 1) {
      if(pairsModel->getPairs().size()>=line.entrancePairIndex) {
	PairsData pair=pairsModel->getPairs().at(line.entrancePairIndex-1);
	return pairsModel->getParticleLabel(pair);
      } else return QString("<center><font style='color:red;font-weight:bold'>UNDEFINED</font></center>");
    } else if(index.column() == 2) {
      if(pairsModel->getPairs().size()>=line.exitPairIndex) {
	PairsData pair=pairsModel->getPairs().at(line.exitPairIndex-1);
	return pairsModel->getParticleLabel(pair);
      } else return QString("<center><font style='color:red;font-weight:bold'>UNDEFINED</font></center>");
    } else if(index.column() == 3) {
      if(line.lowEnergy==line.highEnergy) return line.lowEnergy;
      else return QString("%1-%2").arg(line.lowEnergy).arg(line.highEnergy);
    } else if(index.column() == 4) return QVariant();
    else if(index.column() == 5) return line.energyStep;
    else if(index.column() == 6) {
      if(line.lowAngle==line.highAngle) return line.lowAngle;
      else return QString("%1-%2").arg(line.lowAngle).arg(line.highAngle);
    } else if(index.column() == 7) return QVariant();
    else if(index.column() == 8) return line.angleStep;
    else if(index.column() == 9) {
      if(line.dataType==2) return QString(tr("Phase Shift"));
      else if(line.dataType==1) return QString(tr("Differential"));
      else return QString(tr("Angle Integrated"));
    } 
    else if(index.column() == 10) return QVariant();
    else if(index.column() == 11) return QVariant();
  } else if (role == Qt::EditRole) {
    SegmentsTestData line = segTestLineList.at(index.row());
    if(index.column() == 1) return line.entrancePairIndex;
    else if(index.column() == 2) return line.exitPairIndex;
    else if(index.column() == 3) return line.lowEnergy;
    else if(index.column() == 4) return line.highEnergy;
    else if(index.column() == 5) return line.energyStep;
    else if(index.column() == 6) return line.lowAngle;
    else if(index.column() == 7) return line.highAngle;
    else if(index.column() == 8) return line.angleStep;
    else if(index.column() == 9) return line.dataType;
    else if(index.column() == 10) return line.phaseJ;
    else if(index.column() == 11) return line.phaseL;
  } else if (role==Qt::CheckStateRole && index.column()==0) {
    SegmentsTestData line = segTestLineList.at(index.row());
    if(line.isActive==1) return Qt::Checked;
    else return Qt::Unchecked;
  } else if(role == Qt::TextAlignmentRole) return Qt::AlignCenter;

  return QVariant();
}

QVariant SegmentsTestModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if(role!= Qt::DisplayRole) return QVariant();
  if(orientation == Qt::Horizontal) {
    switch(section) {
    case 0:
      return tr("");
    case 1:
      return tr("Entrance Pair");
    case 2:
      return tr("Exit Pair");
    case 3:
      return tr("Energy\nRange");
    case 4:
      return QVariant();
    case 5:
      return tr("Energy\nStep");
    case 6:
      return tr("Angle\nRange");
    case 7: 
      return QVariant();
    case 8: 
      return tr("Angle\nStep");
    case 9:
      return tr("Data Type");
    case 10:
      return QVariant();
    case 11:
      return QVariant();
    default: 
      return QVariant();
    }
  } else if(orientation == Qt::Vertical) {
    return section+1;
  }
  return QVariant();
}

bool SegmentsTestModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (index.isValid() && role == Qt::EditRole ) {
    int row = index.row();
    SegmentsTestData tempData = segTestLineList.value(row);
    if(index.column() == 0) tempData.isActive=value.toInt();
    else if(index.column() == 1) tempData.entrancePairIndex=value.toInt();
    else if(index.column() == 2) tempData.exitPairIndex=value.toInt();
    else if(index.column() == 3) tempData.lowEnergy=value.toDouble();
    else if(index.column() == 4) tempData.highEnergy=value.toDouble();
    else if(index.column() == 5) tempData.energyStep=value.toDouble();
    else if(index.column() == 6) tempData.lowAngle=value.toDouble();
    else if(index.column() == 7) tempData.highAngle=value.toDouble();
    else if(index.column() == 8) tempData.angleStep=value.toDouble();
    else if(index.column() == 9) tempData.dataType=value.toInt();
    else if(index.column() == 10) tempData.phaseJ=value.toDouble();
    else if(index.column() == 11) tempData.phaseL=value.toInt();
    else return false;

    segTestLineList.replace(row,tempData);
    emit(dataChanged(index,index));
    return true;
  } else if(role== Qt::CheckStateRole) {
    int row = index.row();
    SegmentsTestData tempData = segTestLineList.value(row);
    if(index.column()==0) {
      if(value==Qt::Checked) tempData.isActive=1;
      else tempData.isActive=0;
    } else return false;
    segTestLineList.replace(row,tempData);
    emit(dataChanged(index,index));
    return true;
  }
  return false;
}

bool SegmentsTestModel::insertRows(int position, int rows, const QModelIndex &index) {
  Q_UNUSED(index);
  if(rows>0) {
    beginInsertRows(QModelIndex(),position,position+rows-1);
    for(int row=0; row<rows; row++) {
      SegmentsTestData tempData;
      segTestLineList.insert(position,tempData);
    }
    endInsertRows();
  }
  return true;
}

bool SegmentsTestModel::removeRows(int position, int rows, const QModelIndex &index) {
  Q_UNUSED(index);
  if(rows>0) {
    beginRemoveRows(QModelIndex(),position,position+rows-1);
    for(int row=0; row<rows;++row) {
      segTestLineList.removeAt(position);
    }
    endRemoveRows();
  }
  return true;
}

Qt::ItemFlags SegmentsTestModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return Qt::ItemIsEnabled;
  if(index.column()==0) return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
  return QAbstractTableModel::flags(index);
}

int SegmentsTestModel::isSegTestLine(const SegmentsTestData &line) const {
  int foundLine=-1;
  for(int i=0;i<segTestLineList.size();i++) {
    SegmentsTestData tempLine=segTestLineList.value(i);
    if(tempLine.entrancePairIndex==line.entrancePairIndex&&
       tempLine.exitPairIndex==line.exitPairIndex&&
       tempLine.lowEnergy==line.lowEnergy&&
       tempLine.highEnergy==line.highEnergy&&
       tempLine.energyStep==line.energyStep&&
       tempLine.lowAngle==line.lowAngle&&
       tempLine.highAngle==line.highAngle&&
       tempLine.angleStep==line.angleStep&&
       tempLine.dataType==line.dataType&&
       tempLine.phaseJ==line.phaseJ&&
       tempLine.phaseL==line.phaseL) {
      foundLine=i;
      break;
    }
  }
  return foundLine;
}

void SegmentsTestModel::setPairsModel(PairsModel* model) {
  pairsModel=model;
}
