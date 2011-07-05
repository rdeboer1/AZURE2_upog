#include <QtGui>
#include "AddTargetIntDialog.h"

AddTargetIntDialog::AddTargetIntDialog(QWidget *parent) : QDialog(parent) {
  segmentsListText = new QLineEdit;
  numPointsSpin = new QSpinBox;
  numPointsSpin -> setMinimum(1);
  numPointsSpin -> setMaximum(10000);
  numPointsSpin -> setSingleStep(10);
  numPointsSpin -> setValue(10);
  isConvolutionCheck = new QCheckBox(tr("Include Gaussian Convolution"));
  isConvolutionCheck->setChecked(false);
  connect(isConvolutionCheck,SIGNAL(toggled(bool)),this,SLOT(convolutionCheckChanged(bool)));
  sigmaText = new QLineEdit;
  sigmaText->setEnabled(false);
  isTargetIntegrationCheck = new QCheckBox(tr("Include Target Integration"));
  isTargetIntegrationCheck->setChecked(false);
  connect(isTargetIntegrationCheck,SIGNAL(toggled(bool)),this,SLOT(targetIntCheckChanged(bool)));
  densityText = new QLineEdit;
  densityText->setEnabled(false);
  stoppingPowerEqText = new QLineEdit;
  numParametersSpin = new QSpinBox;
  numParametersSpin -> setMinimum(0);
  numParametersSpin -> setMaximum(10);
  numParametersSpin -> setSingleStep(1);
  numParametersSpin -> setValue(0);
  connect(numParametersSpin,SIGNAL(valueChanged(int)),this,SLOT(parameterSpinChanged(int)));
  parametersTable = new QTableWidget(this);
  parametersTable->setColumnCount(2);
  parametersTable->setRowCount(0);
  parametersTable->verticalHeader()->hide();
  parametersTable->verticalHeader()->setHighlightSections(false);
  parametersTable->horizontalHeader()->setHighlightSections(false);
  parametersTable->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
  parametersTable->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
  parametersTable->setShowGrid(false);
  connect(parametersTable,SIGNAL(cellChanged(int,int)),this,SLOT(parameterChanged(int,int)));
  QStringList labelList;
  labelList.append(QString(tr("Parameter")));
  labelList.append(QString(tr("Value")));
  parametersTable->setHorizontalHeaderLabels(labelList);
  cancelButton = new QPushButton(tr("Cancel"));
  okButton = new QPushButton(tr("Accept"));
  okButton->setDefault(true);

  QHBoxLayout *segListLayout = new QHBoxLayout;
  segListLayout->addWidget(new QLabel(tr("Segments List:")));
  segListLayout->addWidget(segmentsListText);

  QHBoxLayout *numPointsLayout = new QHBoxLayout;
  numPointsLayout->addWidget(new QLabel(tr("Number of Sub-Points:")));
  numPointsLayout->addWidget(numPointsSpin);
  
  QHBoxLayout *topLayout = new QHBoxLayout;
  topLayout->addLayout(segListLayout);
  topLayout->addLayout(numPointsLayout);

  QGridLayout *checkBoxLayout = new QGridLayout;
  checkBoxLayout->addWidget(isConvolutionCheck,0,0);
  checkBoxLayout->addWidget(new QLabel(tr("Sigma [MeV]:")),0,1,Qt::AlignRight);
  checkBoxLayout->addWidget(sigmaText,0,2);

  checkBoxLayout->addWidget(isTargetIntegrationCheck,1,0);
  checkBoxLayout->addWidget(new QLabel(tr("Active Density [atoms/cm^2]:")),1,1,Qt::AlignRight);
  checkBoxLayout->addWidget(densityText,1,2);

  stoppingPowerBox = new QGroupBox(tr("Effective Stopping Cross Section [MeV cm^2/atoms]"));
  QVBoxLayout *stoppingPowerLayout = new QVBoxLayout;
  QHBoxLayout *stoppingPowerTopLayout = new QHBoxLayout;
  QHBoxLayout *equationLayout = new QHBoxLayout;
  equationLayout->addWidget(new QLabel(tr("y=")));
  equationLayout->addWidget(stoppingPowerEqText);
  QHBoxLayout *numParamLayout = new QHBoxLayout;
  numParamLayout->addWidget(new QLabel(tr("Number of Parameters:")));
  numParamLayout->addWidget(numParametersSpin);
  stoppingPowerTopLayout->addLayout(equationLayout);
  stoppingPowerTopLayout->addLayout(numParamLayout);
  stoppingPowerLayout->addLayout(stoppingPowerTopLayout);
  stoppingPowerLayout->addWidget(parametersTable);
  stoppingPowerBox->setLayout(stoppingPowerLayout);
  stoppingPowerBox->hide();

  QHBoxLayout *buttonBox = new QHBoxLayout;
  buttonBox->addWidget(cancelButton);
  buttonBox->addWidget(okButton);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addLayout(checkBoxLayout);
  mainLayout->addWidget(stoppingPowerBox);
  mainLayout->addLayout(buttonBox);

  setLayout(mainLayout);

  connect(okButton, SIGNAL(clicked()),this,SLOT(accept()));
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

  setWindowTitle(tr("Add an Target Effect Line"));
}

void AddTargetIntDialog::createParameterItem(int row, double value) {
  QTableWidgetItem *labelItem = new QTableWidgetItem(QString("a%1").arg(row));
  QTableWidgetItem *valueItem = new QTableWidgetItem(QString("%1").arg(value));
  labelItem->setTextAlignment(Qt::AlignCenter);
  labelItem->setFlags(Qt::ItemIsEditable);
  valueItem->setTextAlignment(Qt::AlignCenter);
  parametersTable->setItem(row,0,labelItem);
  parametersTable->setItem(row,1,valueItem);
}

void AddTargetIntDialog::convolutionCheckChanged(bool checked) {
  if(checked) sigmaText->setEnabled(true);
  else sigmaText->setEnabled(false);
}

void AddTargetIntDialog::targetIntCheckChanged(bool checked) {
  if(checked) {
    stoppingPowerBox->show();
    densityText->setEnabled(true);
  } else {
    stoppingPowerBox->hide();
    densityText->setEnabled(false);
    this->adjustSize();
  }
}

void AddTargetIntDialog::parameterSpinChanged(int newNumber) {
  parametersTable->clearContents();
  parametersTable->setRowCount(newNumber);
  for(int i=0;i<newNumber;i++) {
    if(i<tempParameters.size())
      createParameterItem(i,tempParameters.at(i));
    else 
      createParameterItem(i);
  }
}

void AddTargetIntDialog::parameterChanged(int row, int column) {
  if(column==1) {
    while(row>=tempParameters.size()) {
      double tempDouble=0.0;
      tempParameters.append(tempDouble);
    }
    tempParameters[row]=parametersTable->item(row,column)->text().toDouble();
  }
}
