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
  /*bool readExternalCaptureFile(QString filename);*/
  bool readExternalCaptureFile(QTextStream& inStream);
  /*bool writeExternalCaptureFile(QString filename);*/
  bool writeExternalCaptureFile(QTextStream& outStream);
  /*QLineEdit *getFileText() const {return fileText;};*/
  void setPairsModel(PairsModel* model) {externalCaptureModel->setPairsModel(model);};

 public slots:
  void addLine();
  void editLine();
  void deleteLine();
  void updateButtons(const QItemSelection &selection);
  /*void openFile();
  void openFile(QString filename);
  void saveFile();
  void saveAsFile();*/

 private:
  ExternalCaptureModel *externalCaptureModel;
  /*QLineEdit *fileText;*/
  QTableView *externalCaptureView;
  QPushButton *addButton;
  //QPushButton *editButton;
  QPushButton *deleteButton;
};


#endif
