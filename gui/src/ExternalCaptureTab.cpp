#include <QtGui>
#include "ExternalCaptureTab.h"
#include "AddECLineDialog.h"
#include "RichTextDelegate.h"

ExternalCaptureTab::ExternalCaptureTab(QWidget *parent) : QWidget(parent) {

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
  
  addButton=new QPushButton(tr("+"));
  addButton->setMaximumSize(28,28);
  deleteButton = new QPushButton(tr("-"));
  deleteButton->setMaximumSize(28,28);
  deleteButton->setEnabled(false);
  connect(addButton,SIGNAL(clicked()),this,SLOT(addLine()));
  connect(deleteButton,SIGNAL(clicked()),this,SLOT(deleteLine()));

  QGridLayout *buttonBox = new QGridLayout;
  buttonBox->addWidget(addButton,0,0);
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
    deleteButton->setEnabled(false);
  } else {
    deleteButton->setEnabled(true);
  }
}

bool ExternalCaptureTab::readExternalCaptureFile(QTextStream& inStream) {

  externalCaptureModel->removeRows(0,externalCaptureModel->getLines().size(),QModelIndex());

  int isActive;
  int exitPairIndex;
  double minJ;
  double maxJ;
  int multMask;

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
  return true;
}

bool ExternalCaptureTab::writeExternalCaptureFile(QTextStream& outStream) {

  QList<ExternalCaptureData> lines = externalCaptureModel->getLines();

  for(int i = 0; i<lines.size(); i++) {
    outStream << qSetFieldWidth(8) << lines.at(i).isActive
	      << qSetFieldWidth(8) << lines.at(i).exitPairIndex
	      << qSetFieldWidth(8) << int(lines.at(i).minJ*2)
	      << qSetFieldWidth(8) << int(lines.at(i).maxJ*2)
	      << qSetFieldWidth(8) << lines.at(i).multMask
	      << qSetFieldWidth(0) << "! USER IS RESPONSIBLE FOR CHOOSING ALL DC L-VALUES."
	      << endl;
  }
  return true;
}
