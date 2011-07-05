#ifndef PAIRSTAB_H
#define PAIRSTAB_H

#include <QWidget>
#include <QItemSelection>
#include "PairsModel.h"
#include "AddPairDialog.h"

QT_BEGIN_NAMESPACE

class QPushButton;

QT_END_NAMESPACE

class PairsTab : public QWidget {
  Q_OBJECT

 public:
  PairsTab(QWidget *parent = 0);
  PairsModel *getPairsModel();

 public slots:
  void addPair();
  void addPair(PairsData pair,int pairIndex,bool fromFile);
  void editPair();
  void editPair(PairsData pair,int pairIndex,bool fromFile);
  void removePair();
  void updateButtons(const QItemSelection &selection);

 signals:
  void pairAdded(int);
  void pairRemoved(int);
  void pairEdited(int);

 private:
  PairsModel *pairsModel;
  QTableView *pairsView;
  QPushButton *addButton;
  //QPushButton *editButton;
  QPushButton *deleteButton;
};

#endif
