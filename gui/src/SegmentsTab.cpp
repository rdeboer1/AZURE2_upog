#include <QtGui>
#include "SegmentsTab.h"
#include "RichTextDelegate.h"

SegmentsTab::SegmentsTab(QWidget *parent) : QWidget(parent) {
  segmentsDataModel = new SegmentsDataModel;
  segmentsDataView = new QTableView;
  segmentsDataView->setModel(segmentsDataModel);
  RichTextDelegate *rt = new RichTextDelegate();
  segmentsDataView->setItemDelegateForColumn(1,rt);
  segmentsDataView->setColumnHidden(2,true);
  segmentsDataView->setColumnHidden(4,true);
  segmentsDataView->setColumnHidden(6,true);
  segmentsDataView->horizontalHeader()->setStretchLastSection(true);
  segmentsDataView->horizontalHeader()->setHighlightSections(false);
  segmentsDataView->setSelectionBehavior(QAbstractItemView::SelectRows);
  segmentsDataView->setSelectionMode(QAbstractItemView::SingleSelection);
  segmentsDataView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  segmentsDataView->setShowGrid(false);
  segmentsDataView->setColumnWidth(0,27);
  segmentsDataView->horizontalHeader()->setResizeMode(0,QHeaderView::Fixed);
  segmentsDataView->horizontalHeader()->setResizeMode(1,QHeaderView::Fixed);
  segmentsDataView->horizontalHeader()->setResizeMode(2,QHeaderView::Fixed);
  for(int i = 1; i<3;i++) segmentsDataView->setColumnWidth(i,200);
  for(int i = 3; i<7;i++) segmentsDataView->setColumnWidth(i,120);
  segmentsDataView->setColumnWidth(7,140);
  segmentsDataView->setColumnWidth(8,220);
  segmentsDataView->setItemDelegateForColumn(9,rt);
  segmentsDataView->setColumnHidden(10,true);
  segmentsDataView->setColumnHidden(11,true);
  segmentsDataView->setColumnHidden(12,true);
  connect(segmentsDataView->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(updateSegDataButtons(QItemSelection)));
  connect(segmentsDataView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(editSegDataLine()));

  segmentsTestModel = new SegmentsTestModel;
  segmentsTestView = new QTableView;
  segmentsTestView->setModel(segmentsTestModel);
  segmentsTestView->setItemDelegateForColumn(1,rt);
  segmentsTestView->setColumnHidden(2,true);
  segmentsTestView->setColumnHidden(4,true);
  segmentsTestView->setColumnHidden(7,true);
  segmentsTestView->horizontalHeader()->setStretchLastSection(true);
  segmentsTestView->horizontalHeader()->setHighlightSections(false);
  segmentsTestView->setSelectionBehavior(QAbstractItemView::SelectRows);
  segmentsTestView->setSelectionMode(QAbstractItemView::SingleSelection);
  segmentsTestView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  segmentsTestView->setShowGrid(false);
  segmentsTestView->setColumnWidth(0,27);
  segmentsTestView->horizontalHeader()->setResizeMode(0,QHeaderView::Fixed);
  segmentsTestView->horizontalHeader()->setResizeMode(1,QHeaderView::Fixed);
  segmentsTestView->horizontalHeader()->setResizeMode(2,QHeaderView::Fixed);
  for(int i = 1; i<3;i++) segmentsTestView->setColumnWidth(i,200);
  segmentsTestView->setColumnWidth(3,120);
  segmentsTestView->setColumnWidth(5,90);
  segmentsTestView->setColumnWidth(6,120);
  for(int i = 8; i<SegmentsTestData::SIZE;i++) segmentsTestView->setColumnWidth(i,90);
  segmentsTestView->setColumnHidden(10,true);
  segmentsTestView->setColumnHidden(11,true);
  segmentsTestView->setColumnHidden(12,true);
  connect(segmentsTestView->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(updateSegTestButtons(QItemSelection)));
  connect(segmentsTestView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(editSegTestLine()));

  //segDataAddButton=new QPushButton(tr("Add Line"));
  //segDataDeleteButton = new QPushButton(tr("Delete Line"));
  segDataAddButton=new QPushButton(tr("+"));
  segDataAddButton->setMaximumSize(28,28);
  segDataDeleteButton = new QPushButton(tr("-"));
  segDataDeleteButton->setEnabled(false);
  segDataDeleteButton->setMaximumSize(28,28);
  segDataUpButton = new QPushButton;
  segDataUpButton->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
  segDataUpButton->setEnabled(false);
  segDataUpButton->setMaximumSize(28,28);
  segDataDownButton = new QPushButton;
  segDataDownButton->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
  segDataDownButton->setEnabled(false);
  segDataDownButton->setMaximumSize(28,28);
   //segDataEditButton = new QPushButton(tr("Edit Line"));
  //segDataEditButton->setEnabled(false);
  connect(segDataAddButton,SIGNAL(clicked()),this,SLOT(addSegDataLine()));
  //connect(segDataEditButton,SIGNAL(clicked()),this,SLOT(editSegDataLine()));
  connect(segDataDeleteButton,SIGNAL(clicked()),this,SLOT(deleteSegDataLine()));
  connect(segDataUpButton,SIGNAL(clicked()),this,SLOT(moveSegDataLineUp()));
  connect(segDataDownButton,SIGNAL(clicked()),this,SLOT(moveSegDataLineDown()));

  //segTestAddButton=new QPushButton(tr("Add Line"));
  //segTestDeleteButton = new QPushButton(tr("Delete Line"));
  segTestAddButton=new QPushButton(tr("+"));
  segTestAddButton->setMaximumSize(28,28);
  segTestDeleteButton = new QPushButton(tr("-"));
  segTestDeleteButton->setMaximumSize(28,28);
  segTestDeleteButton->setEnabled(false);
  segTestUpButton = new QPushButton;
  segTestUpButton->setIcon(style()->standardIcon(QStyle::SP_ArrowUp));
  segTestUpButton->setEnabled(false);
  segTestUpButton->setMaximumSize(28,28);
  segTestDownButton = new QPushButton;
  segTestDownButton->setIcon(style()->standardIcon(QStyle::SP_ArrowDown));
  segTestDownButton->setEnabled(false);
  segTestDownButton->setMaximumSize(28,28);
  //segTestEditButton = new QPushButton(tr("Edit Line"));
  //segTestEditButton->setEnabled(false);
  connect(segTestAddButton,SIGNAL(clicked()),this,SLOT(addSegTestLine()));
  //connect(segTestEditButton,SIGNAL(clicked()),this,SLOT(editSegTestLine()));
  connect(segTestDeleteButton,SIGNAL(clicked()),this,SLOT(deleteSegTestLine()));
  connect(segTestUpButton,SIGNAL(clicked()),this,SLOT(moveSegTestLineUp()));
  connect(segTestDownButton,SIGNAL(clicked()),this,SLOT(moveSegTestLineDown()));

  QGroupBox *segDataBox = new QGroupBox(tr("Data Segments"));
  /*QGridLayout *segDatafileLayout = new QGridLayout;
  segDatafileLayout->addWidget(segDataFileText,0,0);
  segDatafileLayout->addWidget(segDataLoadButton,0,1);
  segDatafileLayout->addWidget(segDataSaveAsButton,0,2);
  segDatafileLayout->addWidget(segDataSaveButton,0,3);
  segDatafileLayout->setColumnStretch(0,4);
  segDatafileLayout->setColumnStretch(1,1);
  segDatafileLayout->setColumnStretch(2,1);
  segDatafileLayout->setColumnStretch(3,1);*/
  QGridLayout *segDataLayout = new QGridLayout;
  /*segDataLayout->addLayout(segDatafileLayout,0,0);
    segDataLayout->addWidget(segmentsDataView,1,0);*/
  segDataLayout->addWidget(segmentsDataView,0,0);
  QGridLayout *segDataButtonBox = new QGridLayout;
  segDataButtonBox->addWidget(segDataAddButton,0,0);
  //segDataButtonBox->addWidget(segDataEditButton);
  segDataButtonBox->addWidget(segDataDeleteButton,0,1);
  segDataButtonBox->addItem(new QSpacerItem(28,28),0,2);
  segDataButtonBox->addWidget(segDataUpButton,0,3);
  segDataButtonBox->addWidget(segDataDownButton,0,4);
  segDataButtonBox->setColumnStretch(0,0);
  segDataButtonBox->setColumnStretch(1,0);
  segDataButtonBox->setColumnStretch(2,1);
  segDataButtonBox->setColumnStretch(3,0);
  segDataButtonBox->setColumnStretch(4,0);
 /*segDataLayout->addLayout(segDataButtonBox,2,0);*/
#ifdef MACX_SPACING
  segDataButtonBox->setHorizontalSpacing(12);
#else 
  segDataButtonBox->setHorizontalSpacing(0);
#endif
  segDataLayout->addLayout(segDataButtonBox,1,0);
  segDataBox->setLayout(segDataLayout);

  QGroupBox *segTestBox = new QGroupBox(tr("Extrapolation Segments"));
  /*QGridLayout *segTestFileLayout = new QGridLayout;
  segTestFileLayout->addWidget(segTestFileText,0,0);
  segTestFileLayout->addWidget(segTestLoadButton,0,1);
  segTestFileLayout->addWidget(segTestSaveAsButton,0,2);
  segTestFileLayout->addWidget(segTestSaveButton,0,3);
  segTestFileLayout->setColumnStretch(0,4);
  segTestFileLayout->setColumnStretch(1,1);
  segTestFileLayout->setColumnStretch(2,1);
  segTestFileLayout->setColumnStretch(3,1);*/
  QGridLayout *segTestLayout = new QGridLayout;
  /*segTestLayout->addLayout(segTestFileLayout,0,0);
    segTestLayout->addWidget(segmentsTestView,1,0);*/
  segTestLayout->addWidget(segmentsTestView,0,0);
  QGridLayout *segTestButtonBox = new QGridLayout;
  segTestButtonBox->addWidget(segTestAddButton,0,0);
  //segTestButtonBox->addWidget(segTestEditButton);
  segTestButtonBox->addWidget(segTestDeleteButton,0,1);
  segTestButtonBox->addItem(new QSpacerItem(28,28),0,2);
  segTestButtonBox->addWidget(segTestUpButton,0,3);
  segTestButtonBox->addWidget(segTestDownButton,0,4);
  segTestButtonBox->setColumnStretch(0,0);
  segTestButtonBox->setColumnStretch(1,0);
  segTestButtonBox->setColumnStretch(2,1);
  segTestButtonBox->setColumnStretch(3,0);
  segTestButtonBox->setColumnStretch(4,0);
  /*segTestLayout->addLayout(segTestButtonBox,2,0);*/
#ifdef MACX_SPACING
  segTestButtonBox->setHorizontalSpacing(12);
#else 
  segTestButtonBox->setHorizontalSpacing(0);
#endif
  segTestLayout->addLayout(segTestButtonBox,1,0);
  segTestBox->setLayout(segTestLayout);

  QGridLayout *mainLayout= new QGridLayout;
  mainLayout->addWidget(segDataBox,0,0);
  mainLayout->addWidget(segTestBox,1,0);
  
  setLayout(mainLayout);
}

void SegmentsTab::deleteSegDataLine() {
  QItemSelectionModel *selectionModel = segmentsDataView->selectionModel();
  QModelIndexList indexes = selectionModel->selectedRows();
  QModelIndex index=indexes.at(0);
  
  segmentsDataModel->removeRows(index.row(),1,QModelIndex());
  updateSegDataButtons(selectionModel->selection());
}

void SegmentsTab::deleteSegTestLine() {
  QItemSelectionModel *selectionModel = segmentsTestView->selectionModel();
  QModelIndexList indexes = selectionModel->selectedRows();
  QModelIndex index=indexes.at(0);
  
  segmentsTestModel->removeRows(index.row(),1,QModelIndex());
  updateSegTestButtons(selectionModel->selection());
}

void SegmentsTab::addSegDataLine() {
  AddSegDataDialog aDialog;
  if(aDialog.exec()) {
    SegmentsDataData newLine;
    newLine.isActive=1;
    newLine.entrancePairIndex=aDialog.entrancePairIndexSpin->value();
    newLine.exitPairIndex=aDialog.exitPairIndexSpin->value();
    newLine.lowEnergy=aDialog.lowEnergyText->text().toDouble();
    newLine.highEnergy=aDialog.highEnergyText->text().toDouble();
    newLine.lowAngle=aDialog.lowAngleText->text().toDouble();
    newLine.highAngle=aDialog.highAngleText->text().toDouble();
    if(aDialog.dataTypeCombo->currentIndex()==2) newLine.dataType=2;
    else if(aDialog.dataTypeCombo->currentIndex()==1) newLine.dataType=1;
    else newLine.dataType=0;
    newLine.dataFile=aDialog.dataFileText->text();
    newLine.dataNorm=aDialog.dataNormText->text().toDouble();
    if(aDialog.varyNormCheck->isChecked()) newLine.varyNorm=1;
    else newLine.varyNorm=0;
    newLine.phaseJ=aDialog.phaseJValueText->text().toDouble();
    newLine.phaseL=aDialog.phaseLValueText->text().toInt();
    addSegDataLine(newLine);
  }
}

void SegmentsTab::addSegDataLine(SegmentsDataData line) {
  QList<SegmentsDataData> lines = segmentsDataModel->getLines();
  if(segmentsDataModel->isSegDataLine(line)==-1) {
    segmentsDataModel->insertRows(lines.size(),1,QModelIndex());
    QModelIndex index = segmentsDataModel->index(lines.size(),0,QModelIndex());
    segmentsDataModel->setData(index,line.isActive,Qt::EditRole);
    index = segmentsDataModel->index(lines.size(),1,QModelIndex());
    segmentsDataModel->setData(index,line.entrancePairIndex,Qt::EditRole);
    index = segmentsDataModel->index(lines.size(),2,QModelIndex());
    segmentsDataModel->setData(index,line.exitPairIndex,Qt::EditRole);
    index = segmentsDataModel->index(lines.size(),3,QModelIndex());
    segmentsDataModel->setData(index,line.lowEnergy,Qt::EditRole);
    index = segmentsDataModel->index(lines.size(),4,QModelIndex());
    segmentsDataModel->setData(index,line.highEnergy,Qt::EditRole);
    index = segmentsDataModel->index(lines.size(),5,QModelIndex());
    segmentsDataModel->setData(index,line.lowAngle,Qt::EditRole);
    index = segmentsDataModel->index(lines.size(),6,QModelIndex());
    segmentsDataModel->setData(index,line.highAngle,Qt::EditRole);
    index = segmentsDataModel->index(lines.size(),7,QModelIndex());
    segmentsDataModel->setData(index,line.dataType,Qt::EditRole);
    index = segmentsDataModel->index(lines.size(),8,QModelIndex());
    segmentsDataModel->setData(index,line.dataFile,Qt::EditRole);
    index = segmentsDataModel->index(lines.size(),9,QModelIndex());
    segmentsDataModel->setData(index,line.dataNorm,Qt::EditRole);
    index = segmentsDataModel->index(lines.size(),10,QModelIndex());
    segmentsDataModel->setData(index,line.varyNorm,Qt::EditRole);
    index = segmentsDataModel->index(lines.size(),11,QModelIndex());
    segmentsDataModel->setData(index,line.phaseJ,Qt::EditRole);
    index = segmentsDataModel->index(lines.size(),12,QModelIndex());
    segmentsDataModel->setData(index,line.phaseL,Qt::EditRole);
    segmentsDataView->resizeRowToContents(lines.size());
    updateSegDataButtons(segmentsDataView->selectionModel()->selection());
  } else {
    QMessageBox::information(this,tr("Duplicate Line"),tr("This line already exists."));
  }
}

void SegmentsTab::addSegTestLine() {
  AddSegTestDialog aDialog;
  if(aDialog.exec()) {
    SegmentsTestData newLine;
    newLine.isActive=1;
    newLine.entrancePairIndex=aDialog.entrancePairIndexSpin->value();
    newLine.exitPairIndex=aDialog.exitPairIndexSpin->value();
    newLine.lowEnergy=aDialog.lowEnergyText->text().toDouble();
    newLine.highEnergy=aDialog.highEnergyText->text().toDouble();
    newLine.energyStep=aDialog.energyStepText->text().toDouble();
    newLine.lowAngle=aDialog.lowAngleText->text().toDouble();
    newLine.highAngle=aDialog.highAngleText->text().toDouble();
    newLine.angleStep=aDialog.angleStepText->text().toDouble();
    if(aDialog.dataTypeCombo->currentIndex()==3) newLine.dataType=3;
    else if(aDialog.dataTypeCombo->currentIndex()==2) newLine.dataType=2;
    else if(aDialog.dataTypeCombo->currentIndex()==1) newLine.dataType=1;
    else newLine.dataType=0;
    newLine.phaseJ=aDialog.phaseJValueText->text().toDouble();
    newLine.phaseL=aDialog.phaseLValueText->text().toInt();
    newLine.maxAngDistOrder=aDialog.angDistSpin->value();
    addSegTestLine(newLine);
   }
}

void SegmentsTab::addSegTestLine(SegmentsTestData line) {
  QList<SegmentsTestData> lines = segmentsTestModel->getLines();
  if(segmentsTestModel->isSegTestLine(line)==-1) {
    segmentsTestModel->insertRows(lines.size(),1,QModelIndex());
    QModelIndex index = segmentsTestModel->index(lines.size(),0,QModelIndex());
    segmentsTestModel->setData(index,line.isActive,Qt::EditRole);
    index = segmentsTestModel->index(lines.size(),1,QModelIndex());
    segmentsTestModel->setData(index,line.entrancePairIndex,Qt::EditRole);
    index = segmentsTestModel->index(lines.size(),2,QModelIndex());
    segmentsTestModel->setData(index,line.exitPairIndex,Qt::EditRole);
    index = segmentsTestModel->index(lines.size(),3,QModelIndex());
    segmentsTestModel->setData(index,line.lowEnergy,Qt::EditRole);
    index = segmentsTestModel->index(lines.size(),4,QModelIndex());
    segmentsTestModel->setData(index,line.highEnergy,Qt::EditRole);
    index = segmentsTestModel->index(lines.size(),5,QModelIndex());
    segmentsTestModel->setData(index,line.energyStep,Qt::EditRole);
    index = segmentsTestModel->index(lines.size(),6,QModelIndex());
    segmentsTestModel->setData(index,line.lowAngle,Qt::EditRole);
    index = segmentsTestModel->index(lines.size(),7,QModelIndex());
    segmentsTestModel->setData(index,line.highAngle,Qt::EditRole);
    index = segmentsTestModel->index(lines.size(),8,QModelIndex());
    segmentsTestModel->setData(index,line.angleStep,Qt::EditRole);
    index = segmentsTestModel->index(lines.size(),9,QModelIndex());
    segmentsTestModel->setData(index,line.dataType,Qt::EditRole);
    index = segmentsTestModel->index(lines.size(),10,QModelIndex());
    segmentsTestModel->setData(index,line.phaseJ,Qt::EditRole);
    index = segmentsTestModel->index(lines.size(),11,QModelIndex());
    segmentsTestModel->setData(index,line.phaseL,Qt::EditRole);
    index = segmentsTestModel->index(lines.size(),12,QModelIndex());
    segmentsTestModel->setData(index,line.maxAngDistOrder,Qt::EditRole);
    segmentsTestView->resizeRowToContents(lines.size());
    updateSegTestButtons(segmentsTestView->selectionModel()->selection());
  } else {
    QMessageBox::information(this,tr("Duplicate Line"),tr("This line already exists."));
  }
}

void SegmentsTab::editSegDataLine() {
  QItemSelectionModel *selectionModel = segmentsDataView->selectionModel();
  QModelIndexList indexes = selectionModel->selectedRows();
  QModelIndex index=indexes[0];
  
  QModelIndex i=segmentsDataModel->index(index.row(),1,QModelIndex());
  QVariant var=segmentsDataModel->data(i,Qt::EditRole);
  int entrancePairIndex=var.toInt();
  i=segmentsDataModel->index(index.row(),2,QModelIndex());
  var=segmentsDataModel->data(i,Qt::EditRole);
  int exitPairIndex=var.toInt();
  i=segmentsDataModel->index(index.row(),3,QModelIndex());
  var=segmentsDataModel->data(i,Qt::EditRole);
  QString lowEnergy=var.toString();
  i=segmentsDataModel->index(index.row(),4,QModelIndex());
  var=segmentsDataModel->data(i,Qt::EditRole);
  QString highEnergy=var.toString();
  i=segmentsDataModel->index(index.row(),5,QModelIndex());
  var=segmentsDataModel->data(i,Qt::EditRole);
  QString lowAngle=var.toString();
  i=segmentsDataModel->index(index.row(),6,QModelIndex());
  var=segmentsDataModel->data(i,Qt::EditRole);
  QString highAngle=var.toString();
  i=segmentsDataModel->index(index.row(),7,QModelIndex());
  var=segmentsDataModel->data(i,Qt::EditRole);
  int dataType=var.toInt();
  i=segmentsDataModel->index(index.row(),8,QModelIndex());
  var=segmentsDataModel->data(i,Qt::EditRole);
  QString dataFile=var.toString();
  i=segmentsDataModel->index(index.row(),9,QModelIndex());
  var=segmentsDataModel->data(i,Qt::EditRole);
  QString dataNorm=var.toString();
  i=segmentsDataModel->index(index.row(),10,QModelIndex());
  var=segmentsDataModel->data(i,Qt::EditRole);
  int varyNorm=var.toInt();
  i=segmentsDataModel->index(index.row(),11,QModelIndex());
  var=segmentsDataModel->data(i,Qt::EditRole);
  QString phaseJ=var.toString();
  i=segmentsDataModel->index(index.row(),12,QModelIndex());
  var=segmentsDataModel->data(i,Qt::EditRole);
  QString phaseL=var.toString();


  AddSegDataDialog aDialog;
  aDialog.setWindowTitle(tr("Edit A Data Segment Line"));
  aDialog.entrancePairIndexSpin->setValue(entrancePairIndex);
  aDialog.exitPairIndexSpin->setValue(exitPairIndex);
  aDialog.lowEnergyText->setText(lowEnergy);
  aDialog.highEnergyText->setText(highEnergy);
  aDialog.lowAngleText->setText(lowAngle);
  aDialog.highAngleText->setText(highAngle);
  if(dataType==2) aDialog.dataTypeCombo->setCurrentIndex(2);
  else if(dataType==1) aDialog.dataTypeCombo->setCurrentIndex(1);
  else aDialog.dataTypeCombo->setCurrentIndex(0);
  aDialog.dataFileText->setText(dataFile);
  aDialog.dataNormText->setText(dataNorm);
  if(varyNorm==1) aDialog.varyNormCheck->setChecked(true);
  else aDialog.varyNormCheck->setChecked(false);
  aDialog.phaseJValueText->setText(phaseJ);
  aDialog.phaseLValueText->setText(phaseL);

  if(aDialog.exec()) {
    int newEntrancePairIndex=aDialog.entrancePairIndexSpin->value();
    if(newEntrancePairIndex!=entrancePairIndex) {
      i=segmentsDataModel->index(index.row(),1,QModelIndex());
      segmentsDataModel->setData(i,newEntrancePairIndex,Qt::EditRole);
    }
    int newExitPairIndex=aDialog.exitPairIndexSpin->value();
    if(newExitPairIndex!=exitPairIndex) {
      i=segmentsDataModel->index(index.row(),2,QModelIndex());
      segmentsDataModel->setData(i,newExitPairIndex,Qt::EditRole);
    }
    QString newLowEnergy=aDialog.lowEnergyText->text();
    if(newLowEnergy!=lowEnergy) {
      i=segmentsDataModel->index(index.row(),3,QModelIndex());
      segmentsDataModel->setData(i,newLowEnergy,Qt::EditRole);
    }
    QString newHighEnergy=aDialog.highEnergyText->text();
    if(newHighEnergy!=highEnergy) {
      i=segmentsDataModel->index(index.row(),4,QModelIndex());
      segmentsDataModel->setData(i,newHighEnergy,Qt::EditRole);
    }
    QString newLowAngle=aDialog.lowAngleText->text();
    if(newLowAngle!=lowAngle) {
      i=segmentsDataModel->index(index.row(),5,QModelIndex());
      segmentsDataModel->setData(i,newLowAngle,Qt::EditRole);
    }
    QString newHighAngle=aDialog.highAngleText->text();
    if(newHighAngle!=highAngle) {
      i=segmentsDataModel->index(index.row(),6,QModelIndex());
      segmentsDataModel->setData(i,newHighAngle,Qt::EditRole);
    }
    int newDataType;
    if(aDialog.dataTypeCombo->currentIndex()==2) newDataType=2;
    else if(aDialog.dataTypeCombo->currentIndex()==1) newDataType=1;
    else newDataType=0;
    if(newDataType!=dataType) {
      i=segmentsDataModel->index(index.row(),7,QModelIndex());
      segmentsDataModel->setData(i,newDataType,Qt::EditRole);
    }
    QString newDataFile=aDialog.dataFileText->text();
    if(newDataFile!=dataFile) {
      i=segmentsDataModel->index(index.row(),8,QModelIndex());
      segmentsDataModel->setData(i,newDataFile,Qt::EditRole);
    }
    QString newDataNorm=aDialog.dataNormText->text();
    if(newDataNorm!=dataNorm) {
      i=segmentsDataModel->index(index.row(),9,QModelIndex());
      segmentsDataModel->setData(i,newDataNorm,Qt::EditRole);
    }
    int newVaryNorm=0;
    if(aDialog.varyNormCheck->isChecked()) newVaryNorm=1;
    if(newVaryNorm!=varyNorm) {
      i=segmentsDataModel->index(index.row(),10,QModelIndex());
      segmentsDataModel->setData(i,newVaryNorm,Qt::EditRole);
    }    
    QString newPhaseJ=aDialog.phaseJValueText->text();
    if(newPhaseJ!=phaseJ) {
      i=segmentsDataModel->index(index.row(),11,QModelIndex());
      segmentsDataModel->setData(i,newPhaseJ,Qt::EditRole);
    }
    QString newPhaseL=aDialog.phaseLValueText->text();
    if(newPhaseL!=phaseL) {
      i=segmentsDataModel->index(index.row(),12,QModelIndex());
      segmentsDataModel->setData(i,newPhaseL,Qt::EditRole);
    }
  }
}

void SegmentsTab::editSegTestLine() {
  QItemSelectionModel *selectionModel = segmentsTestView->selectionModel();
  QModelIndexList indexes = selectionModel->selectedRows();
  QModelIndex index=indexes[0];
  
  QModelIndex i=segmentsTestModel->index(index.row(),1,QModelIndex());
  QVariant var=segmentsTestModel->data(i,Qt::EditRole);
  int entrancePairIndex=var.toInt();
  i=segmentsTestModel->index(index.row(),2,QModelIndex());
  var=segmentsTestModel->data(i,Qt::EditRole);
  int exitPairIndex=var.toInt();
  i=segmentsTestModel->index(index.row(),3,QModelIndex());
  var=segmentsTestModel->data(i,Qt::EditRole);
  QString lowEnergy=var.toString();
  i=segmentsTestModel->index(index.row(),4,QModelIndex());
  var=segmentsTestModel->data(i,Qt::EditRole);
  QString highEnergy=var.toString();
  i=segmentsTestModel->index(index.row(),5,QModelIndex());
  var=segmentsTestModel->data(i,Qt::EditRole);
  QString energyStep=var.toString();
  i=segmentsTestModel->index(index.row(),6,QModelIndex());
  var=segmentsTestModel->data(i,Qt::EditRole);
  QString lowAngle=var.toString();
  i=segmentsTestModel->index(index.row(),7,QModelIndex());
  var=segmentsTestModel->data(i,Qt::EditRole);
  QString highAngle=var.toString();
  i=segmentsTestModel->index(index.row(),8,QModelIndex());
  var=segmentsTestModel->data(i,Qt::EditRole);
  QString angleStep=var.toString();
  i=segmentsTestModel->index(index.row(),9,QModelIndex());
  var=segmentsTestModel->data(i,Qt::EditRole);
  int dataType=var.toInt();
  i=segmentsTestModel->index(index.row(),10,QModelIndex());
  var=segmentsTestModel->data(i,Qt::EditRole);
  QString phaseJ=var.toString();
  i=segmentsTestModel->index(index.row(),11,QModelIndex());
  var=segmentsTestModel->data(i,Qt::EditRole);
  QString phaseL=var.toString();
  i=segmentsTestModel->index(index.row(),12,QModelIndex());
  var=segmentsTestModel->data(i,Qt::EditRole);
  int maxAngDistOrder=var.toInt();

  AddSegTestDialog aDialog;
  aDialog.setWindowTitle(tr("Edit A Extrapolation Segment Line"));
  aDialog.entrancePairIndexSpin->setValue(entrancePairIndex);
  aDialog.exitPairIndexSpin->setValue(exitPairIndex);
  aDialog.lowEnergyText->setText(lowEnergy);
  aDialog.highEnergyText->setText(highEnergy);
  aDialog.energyStepText->setText(energyStep);
  aDialog.lowAngleText->setText(lowAngle);
  aDialog.highAngleText->setText(highAngle);
  aDialog.angleStepText->setText(angleStep);
  if(dataType==3) aDialog.dataTypeCombo->setCurrentIndex(3);
  else if(dataType==2) aDialog.dataTypeCombo->setCurrentIndex(2);
  else if(dataType==1) aDialog.dataTypeCombo->setCurrentIndex(1);
  else aDialog.dataTypeCombo->setCurrentIndex(0);
  aDialog.phaseJValueText->setText(phaseJ);
  aDialog.phaseLValueText->setText(phaseL);
  aDialog.angDistSpin->setValue(maxAngDistOrder);
 
 
  if(aDialog.exec()) {
    int newEntrancePairIndex=aDialog.entrancePairIndexSpin->value();
    if(newEntrancePairIndex!=entrancePairIndex) {
      i=segmentsTestModel->index(index.row(),1,QModelIndex());
      segmentsTestModel->setData(i,newEntrancePairIndex,Qt::EditRole);
    }
    int newExitPairIndex=aDialog.exitPairIndexSpin->value();
    if(newExitPairIndex!=exitPairIndex) {
      i=segmentsTestModel->index(index.row(),2,QModelIndex());
      segmentsTestModel->setData(i,newExitPairIndex,Qt::EditRole);
    }
    QString newLowEnergy=aDialog.lowEnergyText->text();
    if(newLowEnergy!=lowEnergy) {
      i=segmentsTestModel->index(index.row(),3,QModelIndex());
      segmentsTestModel->setData(i,newLowEnergy,Qt::EditRole);
    }
    QString newHighEnergy=aDialog.highEnergyText->text();
    if(newHighEnergy!=highEnergy) {
      i=segmentsTestModel->index(index.row(),4,QModelIndex());
      segmentsTestModel->setData(i,newHighEnergy,Qt::EditRole);
    }
    QString newEnergyStep=aDialog.energyStepText->text();
    if(newEnergyStep!=energyStep) {
      i=segmentsTestModel->index(index.row(),5,QModelIndex());
      segmentsTestModel->setData(i,newEnergyStep,Qt::EditRole);
    }
    QString newLowAngle=aDialog.lowAngleText->text();
    if(newLowAngle!=lowAngle) {
      i=segmentsTestModel->index(index.row(),6,QModelIndex());
      segmentsTestModel->setData(i,newLowAngle,Qt::EditRole);
    }
    QString newHighAngle=aDialog.highAngleText->text();
    if(newHighAngle!=highAngle) {
      i=segmentsTestModel->index(index.row(),7,QModelIndex());
      segmentsTestModel->setData(i,newHighAngle,Qt::EditRole);
    }
    QString newAngleStep=aDialog.angleStepText->text();
    if(newAngleStep!=angleStep) {
      i=segmentsTestModel->index(index.row(),8,QModelIndex());
      segmentsTestModel->setData(i,newAngleStep,Qt::EditRole);
    }
    int newDataType;
    if(aDialog.dataTypeCombo->currentIndex()==3) newDataType=3;
    else if(aDialog.dataTypeCombo->currentIndex()==2) newDataType=2;
    else if(aDialog.dataTypeCombo->currentIndex()==1) newDataType=1;
    else newDataType=0;
    if(newDataType!=dataType) {
      i=segmentsTestModel->index(index.row(),9,QModelIndex());
      segmentsTestModel->setData(i,newDataType,Qt::EditRole);
    }
    QString newPhaseJ=aDialog.phaseJValueText->text();
    if(newPhaseJ!=phaseJ) {
      i=segmentsTestModel->index(index.row(),10,QModelIndex());
      segmentsTestModel->setData(i,newPhaseJ,Qt::EditRole);
    }
    QString newPhaseL=aDialog.phaseLValueText->text();
    if(newPhaseL!=phaseL) {
      i=segmentsTestModel->index(index.row(),11,QModelIndex());
      segmentsTestModel->setData(i,newPhaseL,Qt::EditRole);
    }
    int newMaxAngDistOrder=aDialog.angDistSpin->value();
    if(newMaxAngDistOrder!=maxAngDistOrder) {
      i=segmentsTestModel->index(index.row(),12,QModelIndex());
      segmentsTestModel->setData(i,newMaxAngDistOrder,Qt::EditRole);      
    }
  }
}

void SegmentsTab::moveSegDataLineUp() {
  moveSegDataLine(1);
}

void SegmentsTab::moveSegDataLineDown() {
  moveSegDataLine(0);
}

void SegmentsTab::moveSegDataLine(unsigned int upDown) {
  QItemSelectionModel *selectionModel = segmentsDataView->selectionModel();
  QModelIndexList selectionList = selectionModel->selectedRows();
  QModelIndex selectionIndex=selectionList.at(0);
  
  int previous = selectionIndex.row();
  int future;
  if(upDown==0) future = previous+1;
  else future = previous-1;
  SegmentsDataData line = segmentsDataModel->getLines().at(previous);
  segmentsDataModel->removeRows(previous,1,QModelIndex());
  segmentsDataModel->insertRows(future,1,QModelIndex());
  QModelIndex index = segmentsDataModel->index(future,0,QModelIndex());
  segmentsDataModel->setData(index,line.isActive,Qt::EditRole);
  index = segmentsDataModel->index(future,1,QModelIndex());
  segmentsDataModel->setData(index,line.entrancePairIndex,Qt::EditRole);
  index = segmentsDataModel->index(future,2,QModelIndex());
  segmentsDataModel->setData(index,line.exitPairIndex,Qt::EditRole);
  index = segmentsDataModel->index(future,3,QModelIndex());
  segmentsDataModel->setData(index,line.lowEnergy,Qt::EditRole);
  index = segmentsDataModel->index(future,4,QModelIndex());
  segmentsDataModel->setData(index,line.highEnergy,Qt::EditRole);
  index = segmentsDataModel->index(future,5,QModelIndex());
  segmentsDataModel->setData(index,line.lowAngle,Qt::EditRole);
  index = segmentsDataModel->index(future,6,QModelIndex());
  segmentsDataModel->setData(index,line.highAngle,Qt::EditRole);
  index = segmentsDataModel->index(future,7,QModelIndex());
  segmentsDataModel->setData(index,line.dataType,Qt::EditRole);
  index = segmentsDataModel->index(future,8,QModelIndex());
  segmentsDataModel->setData(index,line.dataFile,Qt::EditRole);
  index = segmentsDataModel->index(future,9,QModelIndex());
  segmentsDataModel->setData(index,line.dataNorm,Qt::EditRole);
  index = segmentsDataModel->index(future,10,QModelIndex());
  segmentsDataModel->setData(index,line.varyNorm,Qt::EditRole);
  index = segmentsDataModel->index(future,11,QModelIndex());
  segmentsDataModel->setData(index,line.phaseJ,Qt::EditRole);
  index = segmentsDataModel->index(future,12,QModelIndex());
  segmentsDataModel->setData(index,line.phaseL,Qt::EditRole);
  segmentsDataView->resizeRowToContents(future);

  selectionModel->select(segmentsDataModel->index(future,0,QModelIndex()),
			 QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void SegmentsTab::moveSegTestLineUp() {
  moveSegTestLine(1);
}

void SegmentsTab::moveSegTestLineDown() {
  moveSegTestLine(0);
}

void SegmentsTab::moveSegTestLine(unsigned int upDown) {
  QItemSelectionModel *selectionModel = segmentsTestView->selectionModel();
  QModelIndexList selectionList = selectionModel->selectedRows();
  QModelIndex selectionIndex=selectionList.at(0);
  
  int previous = selectionIndex.row();
  int future;
  if(upDown==0) future = previous+1;
  else future = previous-1;

  SegmentsTestData line = segmentsTestModel->getLines().at(previous);
  segmentsTestModel->removeRows(previous,1,QModelIndex());
  segmentsTestModel->insertRows(future,1,QModelIndex());
  QModelIndex index = segmentsTestModel->index(future,0,QModelIndex());
  segmentsTestModel->setData(index,line.isActive,Qt::EditRole);
  index = segmentsTestModel->index(future,1,QModelIndex());
  segmentsTestModel->setData(index,line.entrancePairIndex,Qt::EditRole);
  index = segmentsTestModel->index(future,2,QModelIndex());
  segmentsTestModel->setData(index,line.exitPairIndex,Qt::EditRole);
  index = segmentsTestModel->index(future,3,QModelIndex());
  segmentsTestModel->setData(index,line.lowEnergy,Qt::EditRole);
  index = segmentsTestModel->index(future,4,QModelIndex());
  segmentsTestModel->setData(index,line.highEnergy,Qt::EditRole);
  index = segmentsTestModel->index(future,5,QModelIndex());
  segmentsTestModel->setData(index,line.energyStep,Qt::EditRole);
  index = segmentsTestModel->index(future,6,QModelIndex());
  segmentsTestModel->setData(index,line.lowAngle,Qt::EditRole);
  index = segmentsTestModel->index(future,7,QModelIndex());
  segmentsTestModel->setData(index,line.highAngle,Qt::EditRole);
  index = segmentsTestModel->index(future,8,QModelIndex());
  segmentsTestModel->setData(index,line.angleStep,Qt::EditRole);
  index = segmentsTestModel->index(future,9,QModelIndex());
  segmentsTestModel->setData(index,line.dataType,Qt::EditRole);
  index = segmentsTestModel->index(future,10,QModelIndex());
  segmentsTestModel->setData(index,line.phaseJ,Qt::EditRole);
  index = segmentsTestModel->index(future,11,QModelIndex());
  segmentsTestModel->setData(index,line.phaseL,Qt::EditRole);
  index = segmentsTestModel->index(future,12,QModelIndex());
  segmentsTestModel->setData(index,line.maxAngDistOrder,Qt::EditRole);
  segmentsTestView->resizeRowToContents(future);
  
  selectionModel->select(segmentsTestModel->index(future,0,QModelIndex()),
			 QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void SegmentsTab::updateSegDataButtons(const QItemSelection &selection) {
  QModelIndexList indexes=selection.indexes();
  
  if(indexes.isEmpty()) {
    //segDataEditButton->setEnabled(false);
    segDataDeleteButton->setEnabled(false);
    segDataUpButton->setEnabled(false);
    segDataDownButton->setEnabled(false);
  } else {
    // segDataEditButton->setEnabled(true);
    segDataDeleteButton->setEnabled(true);
    if(indexes.at(0).row()==0) segDataUpButton->setEnabled(false);
    else segDataUpButton->setEnabled(true);
    if(indexes.at(0).row()==segmentsDataModel->rowCount(QModelIndex())-1) segDataDownButton->setEnabled(false);
    else segDataDownButton->setEnabled(true);
  }
}

void SegmentsTab::updateSegTestButtons(const QItemSelection &selection) {
  QModelIndexList indexes=selection.indexes();
  
  if(indexes.isEmpty()) {
    //segTestEditButton->setEnabled(false);
    segTestDeleteButton->setEnabled(false);
    segTestUpButton->setEnabled(false);
    segTestDownButton->setEnabled(false);
  } else {
    //segTestEditButton->setEnabled(true);
    segTestDeleteButton->setEnabled(true);
    if(indexes.at(0).row()==0) segTestUpButton->setEnabled(false);
    else segTestUpButton->setEnabled(true);
    if(indexes.at(0).row()==segmentsTestModel->rowCount(QModelIndex())-1) segTestDownButton->setEnabled(false);
    else segTestDownButton->setEnabled(true);
  }
}

/*void SegmentsTab::openSegDataFile() {
  QString filename = QFileDialog::getOpenFileName(this);
  if(!filename.isEmpty()) {
    if(!this->readSegDataFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The data segments file could not be read."));
    else segDataFileText->setText(filename);
  }
}

void SegmentsTab::openSegDataFile(QString filename) {
  if(!filename.isEmpty()) {
    if(!this->readSegDataFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The data segments file could not be read."));
    else segDataFileText->setText(filename);
  }
}

void SegmentsTab::saveSegDataFile() {
  QString filename=segDataFileText->text();
  if(!filename.isEmpty()) {
    if(!this->writeSegDataFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The data segments file could not be written."));
  } else saveAsSegDataFile();
}

void SegmentsTab::saveAsSegDataFile() {
  QString filename = QFileDialog::getSaveFileName(this);
  if(!filename.isEmpty()) {
    if(!this->writeSegDataFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The data segments file could not be written."));
    else segDataFileText->setText(filename);
  }
}
*/

/*bool SegmentsTab::readSegDataFile(QString filename) {*/
bool SegmentsTab::readSegDataFile(QTextStream& inStream) {
  /*QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) return false;*/

  segmentsDataModel->removeRows(0,segmentsDataModel->getLines().size(),QModelIndex());

  int isActive;
  int entrancePairIndex;
  int exitPairIndex;
  double lowEnergy;
  double highEnergy;
  double lowAngle;
  double highAngle;
  int dataType;
  QString dataFile;
  double dataNorm;
  int varyNorm;
  double phaseJ;
  int phaseL;

  /*QTextStream in(&file);
    QString dummyString=in.readLine();*/
  QString line("");
  while(!inStream.atEnd()&&line.trimmed()!=QString("</segmentsData>")) {
    line=inStream.readLine();
    if(line.trimmed().isEmpty()) continue;
    if(!inStream.atEnd()&&line.trimmed()!=QString("</segmentsData>")) {
      QTextStream in(&line);
      in >> isActive >> entrancePairIndex >> exitPairIndex >> lowEnergy >> highEnergy >> lowAngle >> highAngle
	 >> dataType;
      if(dataType==2) in>>phaseJ>>phaseL;
      else {
	phaseJ=0.;
	phaseL=0;
      }
      in >> dataNorm >> varyNorm >> dataFile;
      if(in.status()!=QTextStream::Ok) return false;
      SegmentsDataData newLine = {isActive,entrancePairIndex,exitPairIndex,lowEnergy,highEnergy,lowAngle,
				  highAngle,dataType,dataFile,dataNorm,varyNorm,phaseJ,phaseL};
      addSegDataLine(newLine);
    }
  }
  if(line.trimmed()!=QString("</segmentsData>")) return false;
  /*file.close();*/
  return true;
}

/*bool SegmentsTab::writeSegDataFile(QString filename) {*/
bool SegmentsTab::writeSegDataFile(QTextStream& outStream) {
  /*QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) return false;*/

  QList<SegmentsDataData> lines = segmentsDataModel->getLines();

  /*QTextStream out(&file);
  out.setFieldAlignment(QTextStream::AlignLeft);

  out << qSetFieldWidth(15) << "active"
      << qSetFieldWidth(15) << "aa"
      << qSetFieldWidth(15) << "ir"
      << qSetFieldWidth(15) << "minE"
      << qSetFieldWidth(15) << "maxE"
      << qSetFieldWidth(15) << "minA"
      << qSetFieldWidth(15) << "maxA"
      << qSetFieldWidth(15) << "dataType"
      << qSetFieldWidth(15) << "dataNorm"
      << qSetFieldWidth(15) << "varyNorm"
      << qSetFieldWidth(0) << "dataFile"
      << endl;*/
  for(int i = 0; i<lines.size(); i++) {
    int dataType;
    if(lines.at(i).dataType==2) dataType=2;
    else if(lines.at(i).dataType==1) dataType=1;
    else dataType=0;
    outStream << qSetFieldWidth(15) << lines.at(i).isActive
	      << qSetFieldWidth(15) << lines.at(i).entrancePairIndex
	      << qSetFieldWidth(15) << lines.at(i).exitPairIndex
	      << qSetFieldWidth(15) << lines.at(i).lowEnergy
	      << qSetFieldWidth(15) << lines.at(i).highEnergy
	      << qSetFieldWidth(15) << lines.at(i).lowAngle
	      << qSetFieldWidth(15) << lines.at(i).highAngle
	      << qSetFieldWidth(15) << dataType;
    if(lines.at(i).dataType == 2) outStream  << qSetFieldWidth(15) << lines.at(i).phaseJ
					     << qSetFieldWidth(15) << lines.at(i).phaseL;
    outStream << qSetFieldWidth(15) << lines.at(i).dataNorm
	      << qSetFieldWidth(15) << lines.at(i).varyNorm
	      << qSetFieldWidth(0) << lines.at(i).dataFile << endl;
  }
  /*file.flush();
    file.close();*/

  return true;
}

/*void SegmentsTab::openSegTestFile() {
  QString filename = QFileDialog::getOpenFileName(this);
  if(!filename.isEmpty()) {
    if(!this->readSegTestFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The extrapolation segments file could not be read."));
    else segTestFileText->setText(filename);
  }
}

void SegmentsTab::openSegTestFile(QString filename) {
  if(!filename.isEmpty()) {
    if(!this->readSegTestFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The extrapolation segments file could not be read."));
    else segTestFileText->setText(filename);
  }
}

void SegmentsTab::saveSegTestFile() {
  QString filename=segTestFileText->text();
  if(!filename.isEmpty()) {
    if(!this->writeSegTestFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The extrapolation segments file could not be written."));
  } else saveAsSegTestFile();
}

void SegmentsTab::saveAsSegTestFile() {
  QString filename = QFileDialog::getSaveFileName(this);
  if(!filename.isEmpty()) {
    if(!this->writeSegTestFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The extrapolation segments file could not be written."));
    else segTestFileText->setText(filename);
  }
}
*/

/*bool SegmentsTab::readSegTestFile(QString filename) {*/
bool SegmentsTab::readSegTestFile(QTextStream& inStream) {
  /*QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) return false;*/

  segmentsTestModel->removeRows(0,segmentsTestModel->getLines().size(),QModelIndex());

  int isActive;
  int entrancePairIndex;
  int exitPairIndex;
  double lowEnergy;
  double highEnergy;
  double energyStep;
  double lowAngle;
  double highAngle;
  double angleStep;
  int dataType;
  double phaseJ;
  int phaseL;
  int maxAngDistOrder;

  /*QTextStream in(&file);
    QString dummyString=in.readLine();*/
  QString line("");
  while(!inStream.atEnd()&&line.trimmed()!=QString("</segmentsTest>")) {
    line = inStream.readLine();
    if(line.trimmed().isEmpty()) continue;
    if(!inStream.atEnd()&&line.trimmed()!=QString("</segmentsTest>")) {
      QTextStream in(&line);
      in >> isActive >> entrancePairIndex >> exitPairIndex >> lowEnergy >> highEnergy >> energyStep >> lowAngle >> highAngle >> angleStep
	 >> dataType;
      if(dataType==2) {
	in >> phaseJ >> phaseL;
      } else {
	phaseJ=0.;
	phaseL = 0;
      }
      if(dataType==3) {
	in >> maxAngDistOrder;
      } else {
	maxAngDistOrder=0;
      }
      if(in.status()!=QTextStream::Ok) return false;
      SegmentsTestData newLine = {isActive,entrancePairIndex,exitPairIndex,lowEnergy,highEnergy,energyStep,lowAngle,
				  highAngle,angleStep,dataType,phaseJ,phaseL,maxAngDistOrder};
      addSegTestLine(newLine);
    }
  }
  /*file.close();*/
  if(line.trimmed()!=QString("</segmentsTest>")) return false;
  return true;
}

/*bool SegmentsTab::writeSegTestFile(QString filename) {*/
bool SegmentsTab::writeSegTestFile(QTextStream& outStream) {
  /*QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) return false;*/

  QList<SegmentsTestData> lines = segmentsTestModel->getLines();

  /*QTextStream out(&file);
  out.setFieldAlignment(QTextStream::AlignLeft);

  out << qSetFieldWidth(15) << "active"
      << qSetFieldWidth(15) << "aa"
      << qSetFieldWidth(15) << "ir"
      << qSetFieldWidth(15) << "minE"
      << qSetFieldWidth(15) << "maxE"
      << qSetFieldWidth(15) << "eStep"
      << qSetFieldWidth(15) << "minA"
      << qSetFieldWidth(15) << "maxA"
      << qSetFieldWidth(15) << "aStep"
      << qSetFieldWidth(0)  << "dataType"
      << endl;*/
  for(int i = 0; i<lines.size(); i++) {
    int dataType;
    if(lines.at(i).dataType==3) dataType=3;
    else if(lines.at(i).dataType==2) dataType=2;
    else if(lines.at(i).dataType==1) dataType=1;
    else dataType=0;
    outStream << qSetFieldWidth(15) << lines.at(i).isActive
	      << qSetFieldWidth(15) << lines.at(i).entrancePairIndex
	      << qSetFieldWidth(15) << lines.at(i).exitPairIndex
	      << qSetFieldWidth(15) << lines.at(i).lowEnergy
	      << qSetFieldWidth(15) << lines.at(i).highEnergy
	      << qSetFieldWidth(15) << lines.at(i).energyStep
	      << qSetFieldWidth(15) << lines.at(i).lowAngle
	      << qSetFieldWidth(15) << lines.at(i).highAngle
	      << qSetFieldWidth(15) << lines.at(i).angleStep;
    if(dataType==2) {
      outStream << qSetFieldWidth(15)  << dataType
		<< qSetFieldWidth(15) << lines.at(i).phaseJ
		<< qSetFieldWidth(0) << lines.at(i).phaseL
		<< endl;
	
    } else if(dataType==3)  {
      outStream << qSetFieldWidth(15) << dataType 
		<< qSetFieldWidth(0) << lines.at(i).maxAngDistOrder
		<< endl;
    } else {
      outStream << qSetFieldWidth(0)  << dataType
		<< endl;
    }
  }
  /*file.close();*/

  return true;
}
