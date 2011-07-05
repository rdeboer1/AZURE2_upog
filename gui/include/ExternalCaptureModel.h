#ifndef EXTERNALCAPTUREMODEL_H
#define EXTERNALCAPTUREMODEL_H

#include <QAbstractTableModel>
#include <QList>

struct ExternalCaptureData {
  static const int SIZE = 5;
  int isActive;
  int exitPairIndex;
  double minJ;
  double maxJ;
  unsigned char multMask;
};

class PairsModel;

class ExternalCaptureModel : public QAbstractTableModel {
  Q_OBJECT

 public:
  ExternalCaptureModel(QObject *parent = 0);

  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
  bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
  bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
  Qt::ItemFlags flags(const QModelIndex &index) const;
  int isECLine(const ExternalCaptureData &line) const;
  QList<ExternalCaptureData> getLines() const {return ecLineList;};
  QString getSpinLabel(const ExternalCaptureData &line, int which) const;
  void setPairsModel(PairsModel* model);

 private:
  QString parseMultMask(unsigned char multMask) const;

  QList<ExternalCaptureData> ecLineList;  
  PairsModel* pairsModel;
};


#endif
