#ifndef LEVELSTAB_H
#define LEVELSTAB_H

#include <QWidget>
#include <QItemSelection>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTableView>
#include <QSortFilterProxyModel>
#include "PairsModel.h"
#include "LevelsModel.h"
#include "ChannelsModel.h"
#include "ChannelDetails.h"

class LevelsTab : public QWidget {
  Q_OBJECT

 public:
  LevelsTab(QWidget *parent = 0);
  void setPairsModel(PairsModel*);
  void updateChannelsLevelAdded(int levelIndex);
  void updateChannelsLevelDeleted(int levelIndex);
  void updateChannelsLevelEdited(int levelIndex);
  QList<ChannelsData> calculateChannels(int levelIndex);
  bool writeNuclearFile(QTextStream& outStream);
  bool readNuclearFile(QTextStream& inStream);
  void reset();

 public slots:
  void addLevel();
  void addLevel(LevelsData level, bool fromFile);
  void removeLevel();
  void editLevel();
  void updateButtons(const QItemSelection &selection);
  void updateFilter(const QItemSelection &selection);
  void updateChannelsPairAddedEdited();
  void updateChannelsPairRemoved(int pairIndex);
  void updateDetails(const QItemSelection &selection);
  void updateReducedWidth(const QString &string);

 signals:
  void readNewPair(PairsData,int,bool);
  void readExistingPair(PairsData,int,bool);

 private:  
  QSpinBox *maxLSpin;
  QSpinBox *maxMultSpin;
  QSpinBox *maxNumMultSpin;
  QPushButton *addLevelButton;
  QPushButton *removeLevelButton;
  PairsModel *pairsModel;
  LevelsModel *levelsModel;
  ChannelsModel *channelsModel;
  QTableView *levelsView;
  QTableView *channelsView;
  QSortFilterProxyModel *levelsModelProxy;
  QSortFilterProxyModel *proxyModel;
  ChannelDetails *channelDetails;
};


#endif
