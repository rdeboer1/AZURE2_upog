#ifndef EXTERNALCAPTURETAB_H
#define EXTERNALCAPTURETAB_H

#include <QWidget>
#include <QLineEdit>
#include <QTableView>
#include <QPushButton>
#include <QItemSelection>
#include "ExternalCaptureModel.h"

class PairsModel;

class ExternalCaptureTab : public QWidget {
  Q_OBJECT

 public:
  ExternalCaptureTab(QWidget *parent = 0);
  void addLine(ExternalCaptureData line);
  bool readExternalCaptureFile(QTextStream& inStream);
  bool writeExternalCaptureFile(QTextStream& outStream);
  void setPairsModel(PairsModel* model) {externalCaptureModel->setPairsModel(model);};

 public slots:
  void addLine();
  void editLine();
  void deleteLine();
  void updateButtons(const QItemSelection &selection);

 private:
  ExternalCaptureModel *externalCaptureModel;
  QTableView *externalCaptureView;
  QPushButton *addButton;
  QPushButton *deleteButton;
};


#endif
