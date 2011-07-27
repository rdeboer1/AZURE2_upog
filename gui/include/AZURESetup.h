#ifndef AZURESETUP_H
#define AZURESETUP_H

#include "PairsTab.h"
#include "LevelsTab.h"
#include "ExternalCaptureTab.h"
#include "SegmentsTab.h"
#include "TargetIntTab.h"
#include "Config.h"

class RunTab;
class AZUREMainThread;

QT_BEGIN_NAMESPACE

class QTabWidget;
class QMenu;
class QAction;
class QActionGroup;
class QTextEdit;

QT_END_NAMESPACE

class Directories {
 public:
 Directories() : outputDir(QString("")), checksDir(QString("")) {};
  QString outputDir;
  QString checksDir;
};

class AZURESetup : public QMainWindow {
  
 Q_OBJECT

 public:
  AZURESetup();
  Config& GetConfig();
  void open(QString filename);

 public slots:
  void SaveAndRun();
  void DeleteThread();

 private slots:
  void open();
  void openRecent();
  void clearRecent();
  void save();
  void saveAs();
  void matrixChanged(QAction* action);
  void editChecks();
  void editDirs(); 
  void editOptions();

 private:
  bool readFile(QString filename);
  bool readConfig(QTextStream& inStream);
  bool writeFile(QString filename);
  bool writeConfig(QTextStream& outStream, QString directory);
  bool readLastRun(QTextStream& inStream);
  bool writeLastRun(QTextStream& outStream);
  void createActions();
  void createMenus();
  void updateRecent();

  Config config;

  QAction* quitAction;
  QAction* openAction;
  QAction* saveAction;
  QAction* saveAsAction;
  QAction* editChecksAction;
  QAction* editDirsAction;
  QAction* copyAction;
  QAction* aMatrixAction;
  QAction* rMatrixAction;
  QAction* editOptionsAction;
  QAction* recentSeparator;
  QAction* clearRecentAction;
  enum { numRecent = 5 };
  QAction* recentFileActions[numRecent];

  QActionGroup* matrixActionGroup;

  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *configMenu;
  QMenu *formalismMenu;
  QMenu *recentFileMenu;
  QTabWidget *tabWidget;
  PairsTab *pairsTab;
  LevelsTab *levelsTab;
  ExternalCaptureTab *externalCaptureTab;
  SegmentsTab *segmentsTab;
  TargetIntTab *targetIntTab;
  RunTab *runTab;
  AZUREMainThread *azureMain;
};

#endif 
