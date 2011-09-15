#ifndef TARGETINTTAB_H
#define TARGETINTTAB_H

#include <QWidget>
#include <QItemSelection>
#include <QTableView>
#include "TargetIntModel.h"
#include "AddTargetIntDialog.h"

QT_BEGIN_NAMESPACE

class QPushButton;
class QLineEdit;

QT_END_NAMESPACE

class TargetIntTab : public QWidget {
  Q_OBJECT

 public:
  TargetIntTab(QWidget *parent = 0);
  TargetIntModel* getTargetIntModel();
  bool writeFile(QTextStream& outStream);
  bool readFile(QTextStream& inStream);

 public slots:
  void addLine();
  void addLine(TargetIntData line);
  void editLine();
  void deleteLine();
  void updateButtons(const QItemSelection &selection);

 private:
  TargetIntModel *targetIntModel;
  QTableView *targetIntView;
  QPushButton *addButton;
  QPushButton *deleteButton;
};

#endif
