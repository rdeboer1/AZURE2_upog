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
      if(line.dataType==4) {
	int i = 0;
	QList<PairsData> pairsList = pairsModel->getPairs();
	for(i=0;i<pairsList.size();i++) 
	  if(pairsList[i].pairType==10) break;
	if(pairsList.size()>=line.entrancePairIndex&&i<pairsList.size()) {
	  PairsData firstPair=pairsModel->getPairs().at(line.entrancePairIndex-1);
	  return QString("<center>%1</center>").arg(pairsModel->getReactionLabelTotalCapture(firstPair));
	} else return QString("<center><font style='color:red;font-weight:bold;'>UNDEFINED</font></center>");
      } else {
	if(pairsModel->getPairs().size()>=line.entrancePairIndex&&pairsModel->getPairs().size()>=line.exitPairIndex) {
	  PairsData firstPair=pairsModel->getPairs().at(line.entrancePairIndex-1);
	  PairsData secondPair=pairsModel->getPairs().at(line.exitPairIndex-1);
	  return QString("<center>%1</center>").arg(pairsModel->getReactionLabel(firstPair,secondPair));
	} else return QString("<center><font style='color:red;font-weight:bold'>UNDEFINED</font></center>");
      }
    } else if(index.column() == 2) return QVariant();
    else if(index.column() == 3) {
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
      if(line.dataType==3) return QString(tr("<center>Angular Distribution</center>"));
      else if(line.dataType==2) {
	QChar orbital;
	switch (line.phaseL) {
	case 0:
	  orbital='s';
	  break;
	case 1:
	  orbital='p';
	  break;
	case 2:
	  orbital='d';
	  break;
	case 3:
	  orbital='f';
	  break;
	case 4:
	  orbital='g';
	  break;
	case 5:
	  orbital='h';
	  break;
	case 6:
	  orbital='i';
	  break;
	default:
	  orbital='?';
	}
	QString tempSpin;
	if(((int)(line.phaseJ*2))%2!=0&&line.phaseJ!=0.) tempSpin=QString("%1/2").arg((int)(line.phaseJ*2));
	else tempSpin=QString("%1").arg(line.phaseJ);
	return QString("<center>Phase Shift [%1<sub>%2</sub>]</center>").arg(orbital).arg(tempSpin);
      } else if(line.dataType==1) return QString(tr("<center>Differential</center>"));
      else if(line.dataType==4) return QString(tr("<center>Total Capture</center>"));
      else return QString(tr("<center>Angle Integrated</center>"));
    } 
    else if(index.column() == 10) return QVariant();
    else if(index.column() == 11) return QVariant();
    else if(index.column() == 12) return QVariant();
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
    else if(index.column() == 12) return line.maxAngDistOrder;
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
      return tr("Reaction");
    case 2:
      return QVariant();
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
    case 12:
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
    else if(index.column() == 12) tempData.maxAngDistOrder=value.toInt();
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
       tempLine.phaseL==line.phaseL&&
       tempLine.maxAngDistOrder==line.maxAngDistOrder) {
      foundLine=i;
      break;
    }
  }
  return foundLine;
}

void SegmentsTestModel::setPairsModel(PairsModel* model) {
  pairsModel=model;
}

QString SegmentsTestModel::getReactionLabel(const QModelIndex &index) {
  SegmentsTestData line = segTestLineList.at(index.row());
  if(line.dataType==4) {
    int i = 0;
    QList<PairsData> pairsList = pairsModel->getPairs();
    for(i=0;i<pairsList.size();i++) 
      if(pairsList[i].pairType==10) break;
    if(pairsList.size()>=line.entrancePairIndex&&i<pairsList.size()) {
      PairsData firstPair=pairsModel->getPairs().at(line.entrancePairIndex-1);
      return pairsModel->getReactionLabelTotalCapture(firstPair);
    }
    return QString("<font style='color:red;font-weight:bold;'>UNDEFINED</font>");
  } else {
    int numPairs = pairsModel->getPairs().size();
    if(line.entrancePairIndex-1>=numPairs || 
       line.exitPairIndex-1>=numPairs) return QString("<font style='color:red;font-weight:bold'>UNDEFINED</font>");
    PairsData firstPair=pairsModel->getPairs().at(line.entrancePairIndex-1);
    PairsData secondPair=pairsModel->getPairs().at(line.exitPairIndex-1);
    return pairsModel->getReactionLabel(firstPair,secondPair);
  }
}
