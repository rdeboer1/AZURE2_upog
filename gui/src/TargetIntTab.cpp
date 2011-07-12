#include "TargetIntTab.h"
#include <QtGui>


TargetIntTab::TargetIntTab(QWidget *parent) : QWidget(parent) {
  /*fileText = new QLineEdit;
  fileText->setReadOnly(true);
  QPushButton *loadButton = new QPushButton(tr("Open..."));
  QPushButton *saveAsButton = new QPushButton(tr("Save As..."));
  QPushButton *saveButton = new QPushButton(tr("Save"));
  connect(loadButton,SIGNAL(clicked()),this,SLOT(openFile()));
  connect(saveAsButton,SIGNAL(clicked()),this,SLOT(saveAsFile()));
  connect(saveButton,SIGNAL(clicked()),this,SLOT(saveFile()));*/


  targetIntModel = new TargetIntModel(this);
  targetIntView = new QTableView;
  targetIntView->setModel(targetIntModel);
  targetIntView->verticalHeader()->setHighlightSections(false);
  targetIntView->horizontalHeader()->setHighlightSections(false);
  targetIntView->setColumnHidden(4,true);
  targetIntView->setColumnHidden(6,true);
  targetIntView->setColumnHidden(7,true);
  targetIntView->setColumnHidden(8,true);
  targetIntView->setColumnHidden(9,true);
  targetIntView->setColumnWidth(0,27);
  targetIntView->horizontalHeader()->setResizeMode(0,QHeaderView::Fixed);
  targetIntView->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
  targetIntView->horizontalHeader()->setResizeMode(2,QHeaderView::Stretch);
  targetIntView->horizontalHeader()->setResizeMode(3,QHeaderView::Stretch);
  targetIntView->horizontalHeader()->setResizeMode(5,QHeaderView::Stretch);
  targetIntView->setSelectionBehavior(QAbstractItemView::SelectRows);
  targetIntView->setSelectionMode(QAbstractItemView::SingleSelection);
  targetIntView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  targetIntView->setShowGrid(false);
  connect(targetIntView->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(updateButtons(QItemSelection)));
  connect(targetIntView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(editLine()));
  
  //addButton = new QPushButton(tr("Add Line"));
  addButton = new QPushButton(tr("+"));
  addButton->setMaximumSize(28,28);
  connect(addButton,SIGNAL(clicked()),this,SLOT(addLine()));
  //editButton = new QPushButton(tr("Edit Line"));
  //editButton->setEnabled(false);
  //connect(editButton,SIGNAL(clicked()),this,SLOT(editLine()));
  //deleteButton = new QPushButton(tr("Delete Line"));
  deleteButton = new QPushButton(tr("-"));
  deleteButton->setMaximumSize(28,28);
  deleteButton->setEnabled(false);
  connect(deleteButton,SIGNAL(clicked()),this,SLOT(deleteLine()));
  
  /*QGroupBox *fileBox = new QGroupBox(tr("Target Effects File"));
  QGridLayout *fileLayout = new QGridLayout;
  fileLayout->setContentsMargins(6,6,6,6);
  fileLayout->addWidget(fileText,0,0);
  fileLayout->addWidget(loadButton,0,1);
  fileLayout->addWidget(saveAsButton,0,2);
  fileLayout->addWidget(saveButton,0,3);
  fileLayout->setColumnStretch(0,4);
  fileLayout->setColumnStretch(1,1);
  fileLayout->setColumnStretch(2,1);
  fileLayout->setColumnStretch(3,1);
  fileBox->setLayout(fileLayout);*/

  QGridLayout *buttonBox = new QGridLayout;
  buttonBox->addWidget(addButton,0,0);
  //buttonBox->addWidget(editButton);
  buttonBox->addWidget(deleteButton,0,1);
  buttonBox->addItem(new QSpacerItem(28,28),0,2);
  buttonBox->setColumnStretch(0,0);
  buttonBox->setColumnStretch(1,0);
  buttonBox->setColumnStretch(2,1);
#ifdef MACX_SPACING
  buttonBox->setHorizontalSpacing(12);
#else 
  buttonBox->setHorizontalSpacing(0);
#endif

  QGridLayout *mainLayout = new QGridLayout;
  /*mainLayout->addWidget(fileBox,0,0);
  mainLayout->addWidget(targetIntView,1,0);
  mainLayout->addLayout(buttonBox,2,0);
  mainLayout->setRowStretch(0,0);
  mainLayout->setRowStretch(1,1);
  mainLayout->setRowStretch(2,0);*/
  mainLayout->addWidget(targetIntView,0,0);
  mainLayout->addLayout(buttonBox,1,0);
  mainLayout->setRowStretch(0,1);
  mainLayout->setRowStretch(1,0);
  
  setLayout(mainLayout);
}

void TargetIntTab::addLine() {
  AddTargetIntDialog aDialog;
  if(aDialog.exec()) {
    TargetIntData newLine;
    newLine.isActive=1;
    newLine.segmentsList=aDialog.segmentsListText->text();
    newLine.numPoints=aDialog.numPointsSpin->value();
    if(aDialog.isConvolutionCheck->isChecked()) 
      newLine.isConvolution=true;
    else newLine.isConvolution=false;
    newLine.sigma=aDialog.sigmaText->text().toDouble();
    if(aDialog.isTargetIntegrationCheck->isChecked())
      newLine.isTargetIntegration=true;
    else newLine.isTargetIntegration=false;
    newLine.density=aDialog.densityText->text().toDouble();
    newLine.stoppingPowerEq=aDialog.stoppingPowerEqText->text();
    newLine.numParameters=aDialog.numParametersSpin->value();
    for(int i=0;i<newLine.numParameters;i++) 
      newLine.parameters.append(aDialog.tempParameters.at(i));
    addLine(newLine);
  }
}

void TargetIntTab::addLine(TargetIntData line) {
  QList<TargetIntData> lines = targetIntModel->getLines();
  targetIntModel->insertRows(lines.size(),1,QModelIndex());
  QModelIndex index = targetIntModel->index(lines.size(),0,QModelIndex());
  targetIntModel->setData(index,line.isActive,Qt::EditRole);
  index = targetIntModel->index(lines.size(),1,QModelIndex());
  targetIntModel->setData(index,line.segmentsList,Qt::EditRole);
  index = targetIntModel->index(lines.size(),2,QModelIndex());
  targetIntModel->setData(index,line.numPoints,Qt::EditRole);
  index = targetIntModel->index(lines.size(),3,QModelIndex());
  targetIntModel->setData(index,line.isConvolution,Qt::EditRole);
  index = targetIntModel->index(lines.size(),4,QModelIndex());
  targetIntModel->setData(index,line.sigma,Qt::EditRole);
  index = targetIntModel->index(lines.size(),5,QModelIndex());
  targetIntModel->setData(index,line.isTargetIntegration,Qt::EditRole);
  index = targetIntModel->index(lines.size(),6,QModelIndex());
  targetIntModel->setData(index,line.density,Qt::EditRole);
  index = targetIntModel->index(lines.size(),7,QModelIndex());
  targetIntModel->setData(index,line.stoppingPowerEq,Qt::EditRole);
  index = targetIntModel->index(lines.size(),8,QModelIndex());
  targetIntModel->setData(index,line.numParameters,Qt::EditRole);
  index = targetIntModel->index(lines.size(),9,QModelIndex());
  targetIntModel->setData(index,QVariant::fromValue<QList<double> >(line.parameters),Qt::EditRole);
  targetIntView->resizeRowsToContents();
}

void TargetIntTab::editLine() {
  QItemSelectionModel *selectionModel = targetIntView->selectionModel();
  QModelIndexList indexes = selectionModel->selectedRows();
  QModelIndex index = indexes[0];

  QModelIndex i=targetIntModel->index(index.row(),1,QModelIndex());
  QVariant var=targetIntModel->data(i,Qt::EditRole);
  QString segmentsList=var.toString();
  i=targetIntModel->index(index.row(),2,QModelIndex());
  var=targetIntModel->data(i,Qt::EditRole);
  int numPoints=var.toInt();
  i=targetIntModel->index(index.row(),3,QModelIndex());
  var=targetIntModel->data(i,Qt::EditRole);
  bool isConvolution = var.toBool();
  i=targetIntModel->index(index.row(),4,QModelIndex());
  var=targetIntModel->data(i,Qt::EditRole);
  QString sigma = var.toString();
  i=targetIntModel->index(index.row(),5,QModelIndex());
  var=targetIntModel->data(i,Qt::EditRole);
  bool isTargetIntegration = var.toBool();
  i=targetIntModel->index(index.row(),6,QModelIndex());
  var=targetIntModel->data(i,Qt::EditRole);
  QString density = var.toString();
  i=targetIntModel->index(index.row(),7,QModelIndex());
  var=targetIntModel->data(i,Qt::EditRole);
  QString stoppingPowerEq = var.toString();
  i=targetIntModel->index(index.row(),8,QModelIndex());
  var=targetIntModel->data(i,Qt::EditRole);
  int numParameters = var.toInt();
  i=targetIntModel->index(index.row(),9,QModelIndex());
  var=targetIntModel->data(i,Qt::EditRole);
  QList<double> parameters = qVariantValue<QList<double> >(var);

  AddTargetIntDialog aDialog;
  aDialog.setWindowTitle(tr("Edit a Target Effect Line"));
  aDialog.segmentsListText->setText(segmentsList);
  aDialog.numPointsSpin->setValue(numPoints);
  if(isConvolution)
    aDialog.isConvolutionCheck->setChecked(true);
  else aDialog.isConvolutionCheck->setChecked(false);
  aDialog.sigmaText->setText(sigma);
  if(isTargetIntegration) aDialog.isTargetIntegrationCheck->setChecked(true);
  else aDialog.isTargetIntegrationCheck->setChecked(false);
  aDialog.densityText->setText(density);
  aDialog.stoppingPowerEqText->setText(stoppingPowerEq);
  aDialog.tempParameters=parameters;
  aDialog.numParametersSpin->setValue(numParameters);

  if(aDialog.exec()) {
    QString newSegmentsList = aDialog.segmentsListText->text();
    if(segmentsList!=newSegmentsList) {
      i=targetIntModel->index(index.row(),1,QModelIndex());
      targetIntModel->setData(i,newSegmentsList,Qt::EditRole);
    }
    int newNumPoints = aDialog.numPointsSpin->value();
    if(numPoints!=newNumPoints) {
      i=targetIntModel->index(index.row(),2,QModelIndex());
      targetIntModel->setData(i,newNumPoints,Qt::EditRole);
    }
    bool newIsConvolution=false;
    if(aDialog.isConvolutionCheck->isChecked()) newIsConvolution=true;
    if(isConvolution!=newIsConvolution) {
      i=targetIntModel->index(index.row(),3,QModelIndex());
      targetIntModel->setData(i,newIsConvolution,Qt::EditRole);
    }
    QString newSigma = aDialog.sigmaText->text();
    if(sigma!=newSigma) {
      i=targetIntModel->index(index.row(),4,QModelIndex());
      targetIntModel->setData(i,newSigma,Qt::EditRole);
    }
    bool newIsTargetIntegration=false;
    if(aDialog.isTargetIntegrationCheck->isChecked()) newIsTargetIntegration=true;
    if(isTargetIntegration!=newIsTargetIntegration) {
      i=targetIntModel->index(index.row(),5,QModelIndex());
      targetIntModel->setData(i,newIsTargetIntegration,Qt::EditRole);
    }
    QString newDensity = aDialog.densityText->text();
    if(density!=newDensity) {
      i=targetIntModel->index(index.row(),6,QModelIndex());
      targetIntModel->setData(i,newDensity,Qt::EditRole);
    }
    QString newStoppingPowerEq=aDialog.stoppingPowerEqText->text();
    if(stoppingPowerEq!=newStoppingPowerEq) {
      i=targetIntModel->index(index.row(),7,QModelIndex());
      targetIntModel->setData(i,newStoppingPowerEq,Qt::EditRole);
    }
    int newNumParameters=aDialog.numParametersSpin->value();
    if(numParameters!=newNumParameters) {
      i=targetIntModel->index(index.row(),8,QModelIndex());
      targetIntModel->setData(i,newNumParameters,Qt::EditRole);
    }
    QList<double> newParameters = aDialog.tempParameters;
    if(parameters!=newParameters||numParameters!=newNumParameters) {
      parameters.clear();
      for(int j=0;j<newNumParameters;j++) 
	parameters.append(newParameters.at(j));
      i=targetIntModel->index(index.row(),9,QModelIndex());
      targetIntModel->setData(i,QVariant::fromValue<QList<double> >(parameters),Qt::EditRole);
    }
  }
}

void TargetIntTab::deleteLine() {
  QItemSelectionModel *selectionModel = targetIntView->selectionModel();
  QModelIndexList indexes = selectionModel->selectedRows();
  QModelIndex index=indexes.at(0);
 
  targetIntModel->removeRows(index.row(),1,QModelIndex());
}

void TargetIntTab::updateButtons(const QItemSelection &selection) {
  QModelIndexList indexes=selection.indexes();
  
  if(indexes.isEmpty()) {
    //editButton->setEnabled(false);
    deleteButton->setEnabled(false);
  } else {
    //editButton->setEnabled(true);
    deleteButton->setEnabled(true);
  }
}

/*void TargetIntTab::saveFile() {
  QString filename=fileText->text();
  if(!filename.isEmpty()) {
    if(!this->writeFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The target effects file could not be written."));
  } else saveAsFile();
}

void TargetIntTab::saveAsFile() {
  QString filename = QFileDialog::getSaveFileName(this);
  if(!filename.isEmpty()) {
    if(!this->writeFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The target effects file could not be written."));
    else fileText->setText(filename);
  }
  }*/

/*bool TargetIntTab::writeFile(QString filename) {*/
bool TargetIntTab::writeFile(QTextStream& outStream) {
  /*QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) return false;*/
  
  QList<TargetIntData> lines = targetIntModel->getLines();

  /*QTextStream out(&file);
    out.setFieldAlignment(QTextStream::AlignLeft);*/

  for(int i=0;i<lines.size();i++) {
    outStream <<  qSetFieldWidth(15) << lines.at(i).isActive
	      <<  qSetFieldWidth(15) << '\"'+lines[i].segmentsList.remove(' ')+'\"'
	      <<  qSetFieldWidth(15) << lines.at(i).numPoints;
    if(lines.at(i).isConvolution) outStream << qSetFieldWidth(15) << '1';
    else outStream << qSetFieldWidth(15) << '0';
    outStream << qSetFieldWidth(15) << lines.at(i).sigma;
    if(lines.at(i).isTargetIntegration) outStream << qSetFieldWidth(15) << '1';
    else outStream << qSetFieldWidth(15) << '0';
    outStream << qSetFieldWidth(15) << lines.at(i).density
	      << qSetFieldWidth(0) << " \""+lines[i].stoppingPowerEq.remove(' ')+"\" "
	      << qSetFieldWidth(0) << lines.at(i).numParameters << qSetFieldWidth(0) << ' ';
    for(int j=0;j<lines.at(i).numParameters;j++) 
      outStream << lines.at(i).parameters.at(j) <<  qSetFieldWidth(0) << ' ';
    outStream<<endl;
	
  }
  /*file.flush();
    file.close();*/
  return true;
}

/*void TargetIntTab::openFile() {
  QString filename = QFileDialog::getOpenFileName(this);
  if(!filename.isEmpty()) {
    if(!this->readFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The target effects file could not be read."));
    else fileText->setText(filename);
  }
}

void TargetIntTab::openFile(QString filename) {
  if(!filename.isEmpty()) {
    if(!this->readFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The target effects file could not be read."));
    else fileText->setText(filename);
  }
  }*/

/*bool TargetIntTab::readFile(QString filename) {*/
bool TargetIntTab::readFile(QTextStream& inStream) {

  /*QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) return false;*/
  
  targetIntModel->removeRows(0,targetIntModel->getLines().size(),QModelIndex());
  
  int isActive;
  QString segmentsList;
  int numPoints;
  int isConvolution;
  double sigma;
  int isTargetIntegration;
  double density;
  QString stoppingPowerEq;
  int numParameters;
  QList<double> parameters;


  /*QTextStream in(&file);*/
  QString line("");
  while(!inStream.atEnd()&&line.trimmed()!=QString("</targetInt>")) {
    line=inStream.readLine();
    if(line.trimmed().isEmpty()) continue;
    if(!inStream.atEnd()&&line.trimmed()!=QString("</targetInt>")) {
      parameters.clear();
      QTextStream in(&line);
      in >> isActive >> segmentsList >> numPoints >> isConvolution >> sigma >> isTargetIntegration 
	 >> density >> stoppingPowerEq >> numParameters;
      if(in.status()!=QTextStream::Ok) return false;
      int i=0;
      while(i<numParameters&&!inStream.atEnd()) {	
	double tempParameter;
	in >> tempParameter;
	parameters.append(tempParameter);
	i++;
      }
      if(inStream.atEnd()) return false;
      bool tempIsConvolution=false;
      if(isConvolution==1) tempIsConvolution=true;
      bool tempIsTargetIntegration=false;
      if(isTargetIntegration==1) tempIsTargetIntegration=true;
      TargetIntData newLine = {isActive,segmentsList.remove('\"'),numPoints,tempIsConvolution,sigma,tempIsTargetIntegration,
			       density,stoppingPowerEq.remove('\"'),numParameters,parameters};
      addLine(newLine);
    }
  }
  /*file.close();*/
  if(line.trimmed()!=QString("</targetInt>")) return false;
  return true;
}
