#include "AZURESetup.h"
#include "EditChecksDialog.h"
#include "EditDirsDialog.h"
#include "RunTab.h"
#include "Config.h"
#include "EditOptionsDialog.h"
#include "AZUREMainThread.h"
#include <iostream>

struct SegPairs {int firstPair; int secondPair;};
extern bool readSegmentFile(const Config& configure,std::vector<SegPairs>& segPairs);
extern bool checkExternalCapture(Config& configure, const std::vector<SegPairs>& segPairs);

AZURESetup::AZURESetup() : config(std::cout) {
  tabWidget=new QTabWidget();
  
  tabWidget->setMinimumSize(1000,640);

  pairsTab=new PairsTab;

  levelsTab = new LevelsTab;
  levelsTab->setPairsModel(pairsTab->getPairsModel());
  connect(pairsTab,SIGNAL(pairAdded(int)),levelsTab,SLOT(updateChannelsPairAddedEdited()));
  connect(pairsTab,SIGNAL(pairEdited(int)),levelsTab,SLOT(updateChannelsPairAddedEdited()));
  connect(pairsTab,SIGNAL(pairRemoved(int)),levelsTab,SLOT(updateChannelsPairRemoved(int)));
  connect(levelsTab,SIGNAL(readNewPair(PairsData,int,bool)),pairsTab,SLOT(addPair(PairsData,int,bool)));
  connect(levelsTab,SIGNAL(readExistingPair(PairsData,int,bool)),pairsTab,SLOT(editPair(PairsData,int,bool)));

  externalCaptureTab = new ExternalCaptureTab;
  externalCaptureTab->setPairsModel(pairsTab->getPairsModel());

  segmentsTab = new SegmentsTab;
  segmentsTab->setPairsModel(pairsTab->getPairsModel());

  targetIntTab=new TargetIntTab;

  runTab = new RunTab();
  connect(runTab->calcButton,SIGNAL(clicked()),this,SLOT(SaveAndRun()));

  tabWidget->addTab(pairsTab,tr("Particle Pairs (R)"));
  tabWidget->addTab(levelsTab,tr("Levels and Channels"));
  tabWidget->addTab(externalCaptureTab,tr("External Capture"));
  tabWidget->addTab(segmentsTab,tr("Data Segments"));
  tabWidget->addTab(targetIntTab,tr("Target Effects"));
  tabWidget->addTab(runTab,tr("Run AZURE2"));

  setCentralWidget(tabWidget);
  
  createActions();
  createMenus();

  setWindowTitle(tr("AZURESetup2 -- untitled"));
 
}

Config& AZURESetup::GetConfig() {
  return config;
}

void AZURESetup::createActions() {
  openAction = new QAction(tr("&Open..."),this);
  openAction->setShortcuts(QKeySequence::Open);
  connect(openAction,SIGNAL(triggered()),this,SLOT(open()));

  saveAction = new QAction(tr("&Save"),this);
  saveAction->setShortcuts(QKeySequence::Save);
  connect(saveAction,SIGNAL(triggered()),this,SLOT(save()));

  saveAsAction = new QAction(tr("Save &As..."),this);
  saveAsAction->setShortcuts(QKeySequence::SaveAs);
  connect(saveAsAction,SIGNAL(triggered()),this,SLOT(saveAs()));

  copyAction = new QAction(tr("&Copy"),this);
  copyAction->setShortcuts(QKeySequence::Copy);

  matrixActionGroup = new QActionGroup(this);
  aMatrixAction = new QAction(tr("&A-Matrix"),this);
  aMatrixAction->setCheckable(true);
  matrixActionGroup->addAction(aMatrixAction);
  rMatrixAction = new QAction(tr("&R-Matrix"),this);
  rMatrixAction->setCheckable(true);
  matrixActionGroup->addAction(rMatrixAction);
  aMatrixAction->setChecked(true);
  connect(matrixActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(matrixChanged(QAction*)));

  editChecksAction = new QAction(tr("&Checks..."),this);
  connect(editChecksAction,SIGNAL(triggered()),this,SLOT(editChecks()));

  editDirsAction = new QAction(tr("&Directories..."),this);
  connect(editDirsAction,SIGNAL(triggered()),this,SLOT(editDirs()));
  
  editOptionsAction = new QAction(tr("&Options..."),this);
  connect(editOptionsAction,SIGNAL(triggered()),this,SLOT(editOptions()));
}

void AZURESetup::createMenus() {
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAction);
  fileMenu->addAction(saveAction);
  fileMenu->addAction(saveAsAction);
  fileMenu->addSeparator();
  
  configMenu = menuBar()->addMenu(tr("&Configure"));
  formalismMenu = configMenu->addMenu(tr("&Formalism"));
  formalismMenu->addAction(aMatrixAction);
  formalismMenu->addAction(rMatrixAction);
  configMenu->addAction(editChecksAction);
  configMenu->addAction(editDirsAction);
  configMenu->addAction(editOptionsAction);
}

void AZURESetup::open() {
  QString filename = QFileDialog::getOpenFileName(this);
  if(!filename.isEmpty()) {
    if(!this->readFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("An error was encountered while reading the file."));
  }
}

void AZURESetup::open(QString filename) {
  if(!filename.isEmpty()) {
    if(!this->readFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("An error was encountered while reading the file."));
  }
}

bool AZURESetup::readFile(QString filename) {
  QFile file(filename);
  if(!file.open(QIODevice::ReadOnly)) return false;
  QFileInfo info(file);
  QString directory=info.absolutePath();

  QTextStream in(&file);
  QString line("");
  while(line.trimmed()!=QString("<config>")&&!in.atEnd()) line = in.readLine();
  if(in.atEnd()) return false;
  if(!this->readConfig(in)) return false;

  line=QString("");
  while(line.trimmed()!=QString("<levels>")&&!in.atEnd()) line = in.readLine();
  if(in.atEnd()) return false;
  if(!levelsTab->readNuclearFile(in)) return false;
  
  line=QString("");
  while(line.trimmed()!=QString("<externalCapture>")&&!in.atEnd()) line = in.readLine();
  if(in.atEnd()) return false;
  if(!externalCaptureTab->readExternalCaptureFile(in)) return false;

  line=QString("");
  while(line.trimmed()!=QString("<segmentsData>")&&!in.atEnd()) line = in.readLine();
  if(in.atEnd()) return false;
  if(!segmentsTab->readSegDataFile(in)) return false;

  line=QString("");
  while(line.trimmed()!=QString("<segmentsTest>")&&!in.atEnd()) line = in.readLine();
  if(in.atEnd()) return false;
  if(!segmentsTab->readSegTestFile(in)) return false;

  line=QString("");
  while(line.trimmed()!=QString("<targetInt>")&&!in.atEnd()) line = in.readLine();
  if(in.atEnd()) return false;
  if(!targetIntTab->readFile(in)) return false;

  GetConfig().configfile=info.absoluteFilePath().toStdString();
  setWindowTitle(QString("AZURESetup2 -- %1").arg(QString::fromStdString(GetConfig().configfile)));
  QDir::setCurrent(directory);

  file.close();
  return true;
}

bool AZURESetup::readConfig(QTextStream& inStream) {
  GetConfig().screenCheckMask=0;
  GetConfig().fileCheckMask=0;
  GetConfig().outputdir="";
  GetConfig().checkdir="";
  
  QString isAMatrix;
  QString outputDirectory;
  QString checksDirectory;
  QString compoundCheck;
  QString boundaryCheck;
  QString dataCheck;
  QString lMatrixCheck;
  QString legendreCheck;
  QString coulAmpCheck;
  QString pathwaysCheck;
  QString angDistsCheck;
  QString dummyString;

  inStream >> isAMatrix;dummyString=inStream.readLine();
  inStream >> outputDirectory;dummyString=inStream.readLine();
  inStream >> checksDirectory;dummyString=inStream.readLine();
  inStream >> compoundCheck;dummyString=inStream.readLine();
  inStream >> boundaryCheck;dummyString=inStream.readLine();
  inStream >> dataCheck;dummyString=inStream.readLine();
  inStream >> lMatrixCheck;dummyString=inStream.readLine();
  inStream >> legendreCheck;dummyString=inStream.readLine();
  inStream >> coulAmpCheck;dummyString=inStream.readLine();
  inStream >> pathwaysCheck;dummyString=inStream.readLine();
  inStream >> angDistsCheck;dummyString=inStream.readLine();
 
  QString line("");
  while(line.trimmed()!=QString("</config>")&&!inStream.atEnd()) 
    line=inStream.readLine();
  if(line.trimmed()!=QString("</config>")) return false;

  if(isAMatrix=="false") rMatrixAction->activate(QAction::Trigger);
  else aMatrixAction->activate(QAction::Trigger);
  GetConfig().outputdir=outputDirectory.toStdString();
  GetConfig().checkdir=checksDirectory.toStdString();
  if(compoundCheck=="file") GetConfig().fileCheckMask |= Config::CHECK_COMPOUND_NUCLEUS;
  else if(compoundCheck=="screen") GetConfig().screenCheckMask |= Config::CHECK_COMPOUND_NUCLEUS;
  if(boundaryCheck=="file") GetConfig().fileCheckMask |= Config::CHECK_BOUNDARY_CONDITIONS;
  else if(boundaryCheck=="screen")  GetConfig().screenCheckMask |= Config::CHECK_BOUNDARY_CONDITIONS;
  if(dataCheck=="file")  GetConfig().fileCheckMask |= Config::CHECK_DATA;
  else if(dataCheck=="screen") GetConfig().screenCheckMask |= Config::CHECK_DATA;
  if(lMatrixCheck=="file") GetConfig().fileCheckMask |= Config::CHECK_ENERGY_DEP;
  else if(lMatrixCheck=="screen") GetConfig().screenCheckMask |= Config::CHECK_ENERGY_DEP;
  if(legendreCheck=="file") GetConfig().fileCheckMask |= Config::CHECK_LEGENDRE;
  else if(legendreCheck=="screen") GetConfig().screenCheckMask |= Config::CHECK_LEGENDRE;
  if(coulAmpCheck=="file")  GetConfig().fileCheckMask |= Config::CHECK_COUL_AMPLITUDES;
  else if(coulAmpCheck=="screen")  GetConfig().screenCheckMask |= Config::CHECK_COUL_AMPLITUDES;
  if(pathwaysCheck=="file") GetConfig().fileCheckMask |= Config::CHECK_PATHWAYS;
  else if(pathwaysCheck=="screen") GetConfig().screenCheckMask |= Config::CHECK_PATHWAYS;
  if(angDistsCheck=="file") GetConfig().fileCheckMask |= Config::CHECK_ANGULAR_DISTS;
  else if(angDistsCheck=="screen") GetConfig().screenCheckMask |= Config::CHECK_ANGULAR_DISTS;

  return true;
}

void AZURESetup::save() {
  if(!GetConfig().configfile.empty()) {
    if(!this->writeFile(QString::fromStdString(GetConfig().configfile))) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("An error occured while writing the file."));
  } else saveAs();
}

void AZURESetup::saveAs() {
  QString filename = QFileDialog::getSaveFileName(this);
  if(!filename.isEmpty()) {
    if(!this->writeFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("An error occured while writing the file."));
  }
}

bool AZURESetup::writeFile(QString filename) {
  QFile file(filename);
  if(!file.open(QIODevice::WriteOnly)) return false;
  QFileInfo info(file);
  QString directory=info.absolutePath();

  QTextStream out(&file);
  out << "<config>" << endl;
  if(!this->writeConfig(out,directory)) return false;
  out << "</config>" << endl;
  
  out << "<levels>" << endl;
  if(!levelsTab->writeNuclearFile(out)) return false;
  out << "</levels>" << endl;

  out << "<externalCapture>" << endl;
  if(!externalCaptureTab->writeExternalCaptureFile(out)) return false;
  out << "</externalCapture>" << endl;

  out << "<segmentsData>" << endl;
  if(!segmentsTab->writeSegDataFile(out)) return false;
  out << "</segmentsData>" << endl;

  out << "<segmentsTest>" << endl;
  if(!segmentsTab->writeSegTestFile(out)) return false;
  out << "</segmentsTest>" << endl;

  out << "<targetInt>" << endl;
  if(!targetIntTab->writeFile(out)) return false;
  out << "</targetInt>" << endl;  
 
  GetConfig().configfile=info.absoluteFilePath().toStdString();
  setWindowTitle(QString("AZURESetup2 -- %1").arg(QString::fromStdString(GetConfig().configfile)));
  QDir::setCurrent(directory);

  out.flush();
  file.close();
  return true;
}

bool AZURESetup::writeConfig(QTextStream& outStream, QString directory) {
  QString isAMatrix;
  QString outputDirectory;
  QString checksDirectory;
  QString compoundCheck;
  QString boundaryCheck;
  QString dataCheck;
  QString lMatrixCheck;
  QString legendreCheck;
  QString coulAmpCheck;
  QString pathwaysCheck;
  QString angDistsCheck;

  if(GetConfig().paramMask & Config::USE_AMATRIX) isAMatrix="true";
  else isAMatrix="false";
  if(!GetConfig().outputdir.empty()) 
    outputDirectory=QString::fromStdString(GetConfig().outputdir);
  else outputDirectory=directory+'/';
  if(!GetConfig().checkdir.empty()) 
    checksDirectory=QString::fromStdString(GetConfig().checkdir);
  else checksDirectory=directory+'/';
  if(GetConfig().fileCheckMask & Config::CHECK_COMPOUND_NUCLEUS) compoundCheck="file";
  else if(GetConfig().screenCheckMask & Config::CHECK_COMPOUND_NUCLEUS) compoundCheck="screen";
  else compoundCheck="none";
  if(GetConfig().fileCheckMask & Config::CHECK_BOUNDARY_CONDITIONS) boundaryCheck="file";
  else if(GetConfig().screenCheckMask & Config::CHECK_BOUNDARY_CONDITIONS) boundaryCheck="screen";
  else boundaryCheck="none";
  if(GetConfig().fileCheckMask & Config::CHECK_DATA) dataCheck="file";
  else if(GetConfig().screenCheckMask & Config::CHECK_DATA) dataCheck="screen";
  else dataCheck="none";
  if(GetConfig().fileCheckMask & Config::CHECK_ENERGY_DEP) lMatrixCheck="file";
  else if(GetConfig().screenCheckMask & Config::CHECK_ENERGY_DEP) lMatrixCheck="screen";
  else lMatrixCheck="none";
  if(GetConfig().fileCheckMask & Config::CHECK_LEGENDRE) legendreCheck="file";
  else if(GetConfig().screenCheckMask & Config::CHECK_LEGENDRE) legendreCheck="screen";
  else legendreCheck="none";
  if(GetConfig().fileCheckMask & Config::CHECK_COUL_AMPLITUDES) coulAmpCheck="file";
  else if(GetConfig().screenCheckMask & Config::CHECK_COUL_AMPLITUDES) coulAmpCheck="screen";
  else coulAmpCheck="none";
  if(GetConfig().fileCheckMask & Config::CHECK_PATHWAYS) pathwaysCheck="file";
  else if(GetConfig().screenCheckMask & Config::CHECK_PATHWAYS) pathwaysCheck="screen";
  else pathwaysCheck="none";
  if(GetConfig().fileCheckMask & Config::CHECK_ANGULAR_DISTS) angDistsCheck="file";
  else if(GetConfig().screenCheckMask & Config::CHECK_ANGULAR_DISTS) angDistsCheck="screen";
  else angDistsCheck="none";

  outStream.setFieldAlignment(QTextStream::AlignLeft);
  outStream << qSetFieldWidth(100) << isAMatrix << qSetFieldWidth(0) << "#Perform A-Matrix Calculation" << endl;
  outStream << qSetFieldWidth(100) << outputDirectory << qSetFieldWidth(0) << "#Full Path to Output Directory" << endl;
  outStream << qSetFieldWidth(100) << checksDirectory << qSetFieldWidth(0) << "#Full Path to Checks Directory" << endl;
  outStream << qSetFieldWidth(100) << compoundCheck << qSetFieldWidth(0) << "#Compond Nucleus Check" << endl;
  outStream << qSetFieldWidth(100) << boundaryCheck << qSetFieldWidth(0) << "#Boundary Condition Check" << endl;
  outStream << qSetFieldWidth(100) << dataCheck << qSetFieldWidth(0) << "#Data Check" << endl;
  outStream << qSetFieldWidth(100) << lMatrixCheck << qSetFieldWidth(0) << "#Lo-Matrix and Penetrability Check" << endl;
  outStream << qSetFieldWidth(100) << legendreCheck << qSetFieldWidth(0) << "#Legendre Polynomial Check" << endl;
  outStream << qSetFieldWidth(100) << coulAmpCheck << qSetFieldWidth(0) << "#Coulomb Amplitudes Check" << endl;
  outStream << qSetFieldWidth(100) << pathwaysCheck << qSetFieldWidth(0) << "#Reaction Pathway Check" << endl;
  outStream << qSetFieldWidth(100) << angDistsCheck << qSetFieldWidth(0) << "#Angular Distributions Check" << endl;

  return true;
}

void AZURESetup::matrixChanged(QAction *action) {
  if(action==aMatrixAction) GetConfig().paramMask |= Config::USE_AMATRIX;
  else GetConfig().paramMask &= ~Config::USE_AMATRIX;
}

void AZURESetup::editChecks() {
  EditChecksDialog aDialog;
  if(GetConfig().fileCheckMask & Config::CHECK_COMPOUND_NUCLEUS) 
    aDialog.compoundCheckCombo->setCurrentIndex(2);
  else if(GetConfig().screenCheckMask & Config::CHECK_COMPOUND_NUCLEUS)
    aDialog.compoundCheckCombo->setCurrentIndex(1);
  else aDialog.compoundCheckCombo->setCurrentIndex(0);

  if(GetConfig().fileCheckMask & Config::CHECK_BOUNDARY_CONDITIONS) 
    aDialog.boundaryCheckCombo->setCurrentIndex(2);
  else if(GetConfig().screenCheckMask & Config::CHECK_BOUNDARY_CONDITIONS)
    aDialog.boundaryCheckCombo->setCurrentIndex(1);
  else aDialog.boundaryCheckCombo->setCurrentIndex(0);

  if(GetConfig().fileCheckMask & Config::CHECK_DATA) 
    aDialog.dataCheckCombo->setCurrentIndex(2);
  else if(GetConfig().screenCheckMask & Config::CHECK_DATA)
    aDialog.dataCheckCombo->setCurrentIndex(1);
  else aDialog.dataCheckCombo->setCurrentIndex(0);

  if(GetConfig().fileCheckMask & Config::CHECK_ENERGY_DEP) 
    aDialog.lMatrixCheckCombo->setCurrentIndex(2);
  else if(GetConfig().screenCheckMask & Config::CHECK_ENERGY_DEP)
    aDialog.lMatrixCheckCombo->setCurrentIndex(1);
  else aDialog.lMatrixCheckCombo->setCurrentIndex(0);

  if(GetConfig().fileCheckMask & Config::CHECK_LEGENDRE) 
    aDialog.legendreCheckCombo->setCurrentIndex(2);
  else if(GetConfig().screenCheckMask & Config::CHECK_LEGENDRE)
    aDialog.legendreCheckCombo->setCurrentIndex(1);
  else aDialog.legendreCheckCombo->setCurrentIndex(0);

  if(GetConfig().fileCheckMask & Config::CHECK_COUL_AMPLITUDES)
    aDialog.coulAmpCheckCombo->setCurrentIndex(2);
  else if(GetConfig().screenCheckMask & Config::CHECK_COUL_AMPLITUDES)
    aDialog.coulAmpCheckCombo->setCurrentIndex(1);
  else aDialog.coulAmpCheckCombo->setCurrentIndex(0);

  if(GetConfig().fileCheckMask & Config::CHECK_PATHWAYS) 
    aDialog.pathwaysCheckCombo->setCurrentIndex(2);
  else if(GetConfig().screenCheckMask & Config::CHECK_PATHWAYS)
    aDialog.pathwaysCheckCombo->setCurrentIndex(1);
  else aDialog.pathwaysCheckCombo->setCurrentIndex(0);

  if(GetConfig().fileCheckMask & Config::CHECK_ANGULAR_DISTS) 
    aDialog.angDistsCheckCombo->setCurrentIndex(2);
  else if(GetConfig().screenCheckMask & Config::CHECK_ANGULAR_DISTS)
    aDialog.angDistsCheckCombo->setCurrentIndex(1);
  else aDialog.angDistsCheckCombo->setCurrentIndex(0);

  if(aDialog.exec()) {
    GetConfig().fileCheckMask=0;
    GetConfig().screenCheckMask=0;
    if(aDialog.compoundCheckCombo->currentIndex()==1) GetConfig().screenCheckMask |= Config::CHECK_COMPOUND_NUCLEUS;
    else if(aDialog.compoundCheckCombo->currentIndex()==2) GetConfig().fileCheckMask |= Config::CHECK_COMPOUND_NUCLEUS;
    if(aDialog.boundaryCheckCombo->currentIndex()==1) GetConfig().screenCheckMask |= Config::CHECK_BOUNDARY_CONDITIONS;
    else if(aDialog.boundaryCheckCombo->currentIndex()==2)  GetConfig().fileCheckMask |= Config::CHECK_BOUNDARY_CONDITIONS;
    if(aDialog.dataCheckCombo->currentIndex()==1) GetConfig().screenCheckMask |= Config::CHECK_DATA;
    else if(aDialog.dataCheckCombo->currentIndex()==2) GetConfig().fileCheckMask |= Config::CHECK_DATA;
    if(aDialog.lMatrixCheckCombo->currentIndex()==1) GetConfig().screenCheckMask |= Config::CHECK_ENERGY_DEP;
    else if(aDialog.lMatrixCheckCombo->currentIndex()==2) GetConfig().fileCheckMask |= Config::CHECK_ENERGY_DEP;
    if(aDialog.legendreCheckCombo->currentIndex()==1) GetConfig().screenCheckMask |= Config::CHECK_LEGENDRE;
    else if(aDialog.legendreCheckCombo->currentIndex()==2) GetConfig().fileCheckMask |= Config::CHECK_LEGENDRE;
    if(aDialog.coulAmpCheckCombo->currentIndex()==1) GetConfig().screenCheckMask |= Config::CHECK_COUL_AMPLITUDES;
    else if(aDialog.coulAmpCheckCombo->currentIndex()==2) GetConfig().fileCheckMask |= Config::CHECK_COUL_AMPLITUDES;
    if(aDialog.pathwaysCheckCombo->currentIndex()==1) GetConfig().screenCheckMask |= Config::CHECK_PATHWAYS;
    else if(aDialog.pathwaysCheckCombo->currentIndex()==2) GetConfig().fileCheckMask |= Config::CHECK_PATHWAYS;
    if(aDialog.angDistsCheckCombo->currentIndex()==1) GetConfig().screenCheckMask |= Config::CHECK_ANGULAR_DISTS;
    else if(aDialog.angDistsCheckCombo->currentIndex()==2) GetConfig().fileCheckMask |= Config::CHECK_ANGULAR_DISTS;
  }
}

void AZURESetup::editDirs() {
  EditDirsDialog aDialog;

  aDialog.outputDirectoryText->setText(QString::fromStdString(GetConfig().outputdir));
  aDialog.checksDirectoryText->setText(QString::fromStdString(GetConfig().checkdir));

  if(aDialog.exec()) {
    GetConfig().outputdir=aDialog.outputDirectoryText->text().toStdString();
    GetConfig().checkdir=aDialog.checksDirectoryText->text().toStdString();
  }
}

void AZURESetup::editOptions() {
  EditOptionsDialog aDialog;

  if(GetConfig().paramMask & Config::USE_BRUNE_FORMALISM) aDialog.useBruneCheck->setChecked(true);
  else aDialog.useBruneCheck->setChecked(false);
  
  if(GetConfig().paramMask & Config::IGNORE_ZERO_WIDTHS) aDialog.ignoreExternalsCheck->setChecked(true);
  else aDialog.ignoreExternalsCheck->setChecked(false);

  if(GetConfig().paramMask & Config::USE_RMC_FORMALISM) aDialog.useRMCCheck->setChecked(true);
  else aDialog.useRMCCheck->setChecked(false);

  if(!(GetConfig().paramMask & Config::TRANSFORM_PARAMETERS)) aDialog.noTransformCheck->setChecked(true);
  else aDialog.noTransformCheck->setChecked(false);

  if(aDialog.exec()) {
    if(aDialog.useBruneCheck->isChecked()) GetConfig().paramMask |= Config::USE_BRUNE_FORMALISM;
    else GetConfig().paramMask &= ~Config::USE_BRUNE_FORMALISM;

    if(aDialog.ignoreExternalsCheck->isChecked()) GetConfig().paramMask |= Config::IGNORE_ZERO_WIDTHS;
    else  GetConfig().paramMask &= ~Config::IGNORE_ZERO_WIDTHS;

    if(aDialog.useRMCCheck->isChecked()) GetConfig().paramMask |= Config::USE_RMC_FORMALISM;
    else GetConfig().paramMask &= ~Config::USE_RMC_FORMALISM;

    if(aDialog.noTransformCheck->isChecked()) GetConfig().paramMask &= ~Config::TRANSFORM_PARAMETERS;
    else GetConfig().paramMask |= Config::TRANSFORM_PARAMETERS;

  }
}

void AZURESetup::SaveAndRun() {
  save();
  if(GetConfig().configfile.empty()) return;
  runTab->runtimeText->clear();
  QFile file(QString::fromStdString(GetConfig().configfile));
  QFileInfo info(file);
  QString directory=info.absolutePath();
  if(GetConfig().outputdir.empty()) GetConfig().outputdir=directory.toStdString()+'/';
  if(GetConfig().checkdir.empty()) GetConfig().checkdir=directory.toStdString()+'/';
  
  GetConfig().chiVariance=runTab->chiVarianceText->text().toDouble();

  if(runTab->calcType->currentIndex()==0 ||
     runTab->calcType->currentIndex()==3) GetConfig().paramMask |= Config::PERFORM_FIT;
  else GetConfig().paramMask &= ~Config::PERFORM_FIT;
  if(runTab->calcType->currentIndex()==2||
     runTab->calcType->currentIndex()==4) GetConfig().paramMask &= ~Config::CALCULATE_WITH_DATA;
  else GetConfig().paramMask |= Config::CALCULATE_WITH_DATA;
  if(runTab->calcType->currentIndex()==3) GetConfig().paramMask |= Config::PERFORM_ERROR_ANALYSIS;
  else GetConfig().paramMask &= ~Config::PERFORM_ERROR_ANALYSIS;
  if(runTab->calcType->currentIndex()==4) GetConfig().paramMask |= Config::CALCULATE_REACTION_RATE;
  else GetConfig().paramMask &= ~Config::CALCULATE_REACTION_RATE;

  if(runTab->oldParamFileButton->isChecked()) {
    GetConfig().paramMask |= Config::USE_PREVIOUS_PARAMETERS;
    GetConfig().paramfile=runTab->paramFileText->text().toStdString();
  } else GetConfig().paramMask &= ~Config::USE_PREVIOUS_PARAMETERS;

  std::vector<SegPairs> segPairs;
  if(!(GetConfig().paramMask & Config::CALCULATE_REACTION_RATE)) {
    if(!readSegmentFile(GetConfig(),segPairs)) return;
  } else {
    GetConfig().rateParams.entrancePair=runTab->rateEntranceKey->text().toInt();
    GetConfig().rateParams.exitPair=runTab->rateExitKey->text().toInt();
    if(!runTab->fileTempButton->isChecked()) {
      GetConfig().rateParams.useFile=false;
      GetConfig().rateParams.minTemp = runTab->minTempText->text().toDouble();
      GetConfig().rateParams.maxTemp = runTab->maxTempText->text().toDouble();
      GetConfig().rateParams.tempStep = runTab->tempStepText->text().toDouble();
    } else {
      GetConfig().rateParams.useFile=true;
      GetConfig().rateParams.temperatureFile = runTab->fileTempText->text().toStdString();
    }
    SegPairs tempPair = {runTab->rateEntranceKey->text().toInt(),
			 runTab->rateExitKey->text().toInt()}; 
    segPairs.push_back(tempPair);
  }
  if(segPairs.size()==0) {
    QMessageBox::information(this,tr("Empty Segments"),tr("No active segments have been found."));
    return;
  }
  int maxPairs=pairsTab->getPairsModel()->getPairs().size();
  for(std::vector<SegPairs>::const_iterator it = segPairs.begin();it<segPairs.end();it++) {
    if(it->firstPair>maxPairs||it->secondPair>maxPairs) {
      QMessageBox::information(this,tr("Undefined Key"),tr("An undefined pair key is specified."));
      return;
    }
  }

  GetConfig().paramMask &= ~Config::USE_EXTERNAL_CAPTURE;
  if(!checkExternalCapture(GetConfig(),segPairs)) return;
  if(GetConfig().paramMask &Config::USE_EXTERNAL_CAPTURE) {
    if(runTab->oldIntegralsFileButton->isChecked() && 
       !(GetConfig().paramMask & Config::CALCULATE_REACTION_RATE)) {
      GetConfig().paramMask |= Config::USE_PREVIOUS_INTEGRALS;
      GetConfig().integralsfile=runTab->integralsFileText->text().toStdString();
    } else GetConfig().paramMask &= ~Config::USE_PREVIOUS_INTEGRALS;
  }

  azureMain = new AZUREMainThread(runTab,GetConfig());
  connect(azureMain,SIGNAL(finished()),this,SLOT(DeleteThread()));
  runTab->calcButton->setEnabled(false);
  runTab->runtimeText->SetMouseFiltered(true);
  azureMain->start();
}

void AZURESetup::DeleteThread() {
  runTab->calcButton->setEnabled(true);
  runTab->runtimeText->SetMouseFiltered(false);
  delete azureMain;
}
