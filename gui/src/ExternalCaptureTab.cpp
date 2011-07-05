#include <QtGui>
#include "ExternalCaptureTab.h"
#include "AddECLineDialog.h"
#include "RichTextDelegate.h"

ExternalCaptureTab::ExternalCaptureTab(QWidget *parent) : QWidget(parent) {
  /*fileText=new QLineEdit;
  fileText->setReadOnly(true);
  QPushButton *loadButton=new QPushButton(tr("Open..."));
  QPushButton *saveAsButton=new QPushButton(tr("Save As..."));
  QPushButton *saveButton=new QPushButton(tr("Save"));
  connect(loadButton,SIGNAL(clicked()),this,SLOT(openFile()));
  connect(saveButton,SIGNAL(clicked()),this,SLOT(saveFile()));
  connect(saveAsButton,SIGNAL(clicked()),this,SLOT(saveAsFile()));*/

  externalCaptureModel = new ExternalCaptureModel(this);
  externalCaptureView = new QTableView;
  externalCaptureView->setModel(externalCaptureModel);
  externalCaptureView->verticalHeader()->hide();
  externalCaptureView->horizontalHeader()->setStretchLastSection(true);
  externalCaptureView->horizontalHeader()->setHighlightSections(false);
  externalCaptureView->setSelectionBehavior(QAbstractItemView::SelectRows);
  externalCaptureView->setSelectionMode(QAbstractItemView::SingleSelection);
  externalCaptureView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  externalCaptureView->setShowGrid(false);
  connect(externalCaptureView->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(updateButtons(QItemSelection)));
  connect(externalCaptureView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(editLine()));


  externalCaptureView->setColumnWidth(0,27);
  externalCaptureView->setColumnWidth(1,160);
  externalCaptureView->horizontalHeader()->setResizeMode(0,QHeaderView::Fixed);
  externalCaptureView->horizontalHeader()->setResizeMode(1,QHeaderView::Fixed);
  RichTextDelegate *rt = new RichTextDelegate();
  externalCaptureView->setItemDelegateForColumn(1,rt);
    
  for(int i = 2; i<ExternalCaptureData::SIZE;i++) externalCaptureView->horizontalHeader()->setResizeMode(i,QHeaderView::Stretch);
  
  //addButton=new QPushButton(tr("Add Line"));
  //deleteButton = new QPushButton(tr("Delete Line"));
  addButton=new QPushButton(tr("+"));
  addButton->setMaximumSize(28,28);
  deleteButton = new QPushButton(tr("-"));
  deleteButton->setMaximumSize(28,28);
  deleteButton->setEnabled(false);
  //editButton = new QPushButton(tr("Edit Line"));
  //editButton->setEnabled(false);
  connect(addButton,SIGNAL(clicked()),this,SLOT(addLine()));
  //connect(editButton,SIGNAL(clicked()),this,SLOT(editLine()));
  connect(deleteButton,SIGNAL(clicked()),this,SLOT(deleteLine()));

  /* QGroupBox *fileBox = new QGroupBox(tr("External Capture File"));
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
  buttonBox->setHorizontalSpacing(0);

  QGridLayout *mainLayout = new QGridLayout;
  /*mainLayout->addWidget(fileBox,0,0);
  mainLayout->addWidget(externalCaptureView,1,0);
  mainLayout->addLayout(buttonBox,2,0);
  mainLayout->setRowStretch(0,0);
  mainLayout->setRowStretch(1,1);
  mainLayout->setRowStretch(2,0);*/
  mainLayout->addWidget(externalCaptureView,0,0);
  mainLayout->addLayout(buttonBox,1,0);
  mainLayout->setRowStretch(0,1);
  mainLayout->setRowStretch(1,0);

  setLayout(mainLayout);
}

void ExternalCaptureTab::addLine() {
  AddECLineDialog aDialog;
  if(aDialog.exec()) {
    ExternalCaptureData newLine;
    newLine.isActive=1;
    newLine.exitPairIndex=aDialog.exitPairIndexSpin->value();
    newLine.minJ=(aDialog.minJText->text()).toDouble();
    newLine.maxJ=(aDialog.maxJText->text()).toDouble();
    unsigned int newMask=0;
    if(aDialog.e1Check->isChecked()) newMask |= (1<<0);
    if(aDialog.m1Check->isChecked()) newMask |= (1<<1);
    if(aDialog.e2Check->isChecked()) newMask |= (1<<2);
    newLine.multMask=newMask;
    addLine(newLine);
  }  
}

void ExternalCaptureTab::addLine(ExternalCaptureData line) {
  QList<ExternalCaptureData> lines = externalCaptureModel->getLines();
  if(externalCaptureModel->isECLine(line)==-1) {
    externalCaptureModel->insertRows(lines.size(),1,QModelIndex());
    QModelIndex index = externalCaptureModel->index(lines.size(),0,QModelIndex());
    externalCaptureModel->setData(index,line.isActive,Qt::EditRole);
    index = externalCaptureModel->index(lines.size(),1,QModelIndex());
    externalCaptureModel->setData(index,line.exitPairIndex,Qt::EditRole);
    index = externalCaptureModel->index(lines.size(),2,QModelIndex());
    externalCaptureModel->setData(index,line.minJ,Qt::EditRole);
    index = externalCaptureModel->index(lines.size(),3,QModelIndex());
    externalCaptureModel->setData(index,line.maxJ,Qt::EditRole);
    index = externalCaptureModel->index(lines.size(),4,QModelIndex());
    externalCaptureModel->setData(index,line.multMask,Qt::EditRole);
 
    externalCaptureView->resizeRowToContents(lines.size());
  } else {
    QMessageBox::information(this,tr("Duplicate Line"),tr("This line already exists."));
  }

}

void ExternalCaptureTab::editLine() {
  QItemSelectionModel *selectionModel = externalCaptureView->selectionModel();
  QModelIndexList indexes = selectionModel->selectedRows();
  QModelIndex index=indexes.at(0);
  
  QModelIndex i=externalCaptureModel->index(index.row(),1,QModelIndex());
  QVariant var=externalCaptureModel->data(i,Qt::EditRole);
  int exitPairIndex=var.toInt();
  i=externalCaptureModel->index(index.row(),2,QModelIndex());
  var=externalCaptureModel->data(i,Qt::EditRole);
  QString minJ=var.toString();
  i=externalCaptureModel->index(index.row(),3,QModelIndex());
  var=externalCaptureModel->data(i,Qt::EditRole);
  QString maxJ=var.toString();
  i=externalCaptureModel->index(index.row(),4,QModelIndex());
  var=externalCaptureModel->data(i,Qt::EditRole);
  unsigned int multMask=var.toInt();

  AddECLineDialog aDialog;
  aDialog.setWindowTitle(tr("Edit an External Capture Line"));
  aDialog.exitPairIndexSpin->setValue(exitPairIndex);
  aDialog.minJText->setText(minJ);
  aDialog.maxJText->setText(maxJ);
  if(multMask&(1<<0)) aDialog.e1Check->setChecked(true);
  else aDialog.e1Check->setChecked(false);
  if(multMask&(1<<1)) aDialog.m1Check->setChecked(true);
  else aDialog.m1Check->setChecked(false);
  if(multMask&(1<<2)) aDialog.e2Check->setChecked(true);
  else aDialog.e2Check->setChecked(false);
  if(aDialog.exec()) {
    int newExitPairIndex=aDialog.exitPairIndexSpin->value();
    if(newExitPairIndex!=exitPairIndex) {
      i=externalCaptureModel->index(index.row(),1,QModelIndex());
      externalCaptureModel->setData(i,newExitPairIndex,Qt::EditRole);
    }
    QString newMinJ=aDialog.minJText->text();
    if(newMinJ!=minJ) {
      i=externalCaptureModel->index(index.row(),2,QModelIndex());
      externalCaptureModel->setData(i,newMinJ,Qt::EditRole);
    }
    QString newMaxJ=aDialog.maxJText->text();
    if(newMaxJ!=maxJ) {
      i=externalCaptureModel->index(index.row(),3,QModelIndex());
      externalCaptureModel->setData(i,newMaxJ,Qt::EditRole);
    }
    unsigned char newMultMask=0;
    if(aDialog.e1Check->isChecked()) newMultMask |= (1<<0);
    if(aDialog.m1Check->isChecked()) newMultMask |= (1<<1);
    if(aDialog.e2Check->isChecked()) newMultMask |= (1<<2);
    if(newMultMask!=multMask) {
      i=externalCaptureModel->index(index.row(),4,QModelIndex());
      externalCaptureModel->setData(i,newMultMask,Qt::EditRole);
    }
  }
}

void ExternalCaptureTab::deleteLine() {
  QItemSelectionModel *selectionModel = externalCaptureView->selectionModel();
  QModelIndexList indexes = selectionModel->selectedRows();
  QModelIndex index=indexes.at(0);
  
  externalCaptureModel->removeRows(index.row(),1,QModelIndex());
}

void ExternalCaptureTab::updateButtons(const QItemSelection &selection) {
  QModelIndexList indexes=selection.indexes();
  if(indexes.isEmpty()) {
    //editButton->setEnabled(false);
    deleteButton->setEnabled(false);
  } else {
    //editButton->setEnabled(true);
    deleteButton->setEnabled(true);
  }
}

/*void ExternalCaptureTab::openFile() {
  QString filename = QFileDialog::getOpenFileName(this);
  if(!filename.isEmpty()) {
    if(!this->readExternalCaptureFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The external capture file could not be read."));
    else fileText->setText(filename);
  }
}

void ExternalCaptureTab::openFile(QString filename) {
  if(!filename.isEmpty()) {
    if(!this->readExternalCaptureFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The external capture file could not be read."));
    else fileText->setText(filename);
  }
}

void ExternalCaptureTab::saveFile() {
  QString filename=fileText->text();
  if(!filename.isEmpty()) {
    if(!this->writeExternalCaptureFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The external capture file could not be written."));
  } else saveAsFile();
}

void ExternalCaptureTab::saveAsFile() {
  QString filename = QFileDialog::getSaveFileName(this);
  if(!filename.isEmpty()) {
    if(!this->writeExternalCaptureFile(filename)) 
      QMessageBox::information(this,
			       tr("Can't Access File"),
			       tr("The external capture file could not be written."));
    else fileText->setText(filename);
  }
  }*/

/*bool ExternalCaptureTab::readExternalCaptureFile(QString filename) {*/
bool ExternalCaptureTab::readExternalCaptureFile(QTextStream& inStream) {
  /*QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) return false;*/

  externalCaptureModel->removeRows(0,externalCaptureModel->getLines().size(),QModelIndex());

  int isActive;
  int exitPairIndex;
  double minJ;
  double maxJ;
  int multMask;

  /*QTextStream in(&file);
    QString dummyString=in.readLine();*/
  QString line("");
  while(line.trimmed()!=QString("</externalCapture>")&&!inStream.atEnd()) {
    line = inStream.readLine();
    if(line.trimmed().isEmpty()) continue;
    if(line.trimmed()!=QString("</externalCapture>")&&!inStream.atEnd()) {
      QTextStream in(&line);
      in >> isActive >> exitPairIndex >> minJ >> maxJ >> multMask;
      if(in.status()!=QTextStream::Ok) return false;
      ExternalCaptureData newLine = {isActive,exitPairIndex,minJ/2.0,maxJ/2.0,multMask};
      addLine(newLine);
    }
  }
  if(line.trimmed()!=QString("</externalCapture>")) return false;
  /*file.close();*/
  return true;
}

/*bool ExternalCaptureTab::writeExternalCaptureFile(QString filename) {*/
bool ExternalCaptureTab::writeExternalCaptureFile(QTextStream& outStream) {
  /*QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) return false;*/

  QList<ExternalCaptureData> lines = externalCaptureModel->getLines();

  /*QTextStream out(&file);
    out.setFieldAlignment(QTextStream::AlignLeft);

  out << qSetFieldWidth(8) << "is_dc"
      << qSetFieldWidth(8) << "aa"
      << qSetFieldWidth(8) << "ir"
      << qSetFieldWidth(8) << "Jmin"
      << qSetFieldWidth(8) << "Jmax"
      << qSetFieldWidth(8) << "limin"
      << qSetFieldWidth(8) << "limax"
      << qSetFieldWidth(8) << "ldcmax"
      << qSetFieldWidth(0) << "! enter physical values, except for Jmin,Jmax=2*physical value"
      << endl;*/
  for(int i = 0; i<lines.size(); i++) {
    outStream << qSetFieldWidth(8) << lines.at(i).isActive
	      << qSetFieldWidth(8) << lines.at(i).exitPairIndex
	      << qSetFieldWidth(8) << int(lines.at(i).minJ*2)
	      << qSetFieldWidth(8) << int(lines.at(i).maxJ*2)
	      << qSetFieldWidth(8) << lines.at(i).multMask
	      << qSetFieldWidth(0) << "! USER IS RESPONSIBLE FOR CHOOSING ALL DC L-VALUES."
	      << endl;
  }
  /*file.close();*/
  return true;
}
