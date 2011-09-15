#include "AddECLineDialog.h"
#include <QtGui>

AddECLineDialog::AddECLineDialog(QWidget *parent) : QDialog(parent) {

  //  this->setMaximumSize(370,420);
  //  this->setMinimumSize(370,420);

  exitPairIndexLabel = new QLabel(tr("Capture Pair Index:"));
  minJLabel = new QLabel(tr("Minimum Initial J:"));
  maxJLabel = new QLabel(tr("Maximum Initial J:"));

  exitPairIndexSpin = new QSpinBox;
  exitPairIndexSpin->setMinimum(1);
  exitPairIndexSpin->setMaximum(100);
  exitPairIndexSpin->setSingleStep(1);
  QRegExp rx("^\\d{0,2}(\\.[05]{0,1})?$");
  QValidator *validator = new QRegExpValidator(rx, this);
  minJText = new QLineEdit;
  minJText->setValidator(validator);
  maxJText = new QLineEdit;
  maxJText->setValidator(validator);
  e1Check = new QCheckBox(tr("E1"));
  e1Check->setChecked(true);
  m1Check = new QCheckBox(tr("M1"));
  m1Check->setChecked(false);
  e2Check = new QCheckBox(tr("E2"));
  e2Check->setChecked(false);

  cancelButton = new QPushButton(tr("Cancel"));
  okButton = new QPushButton(tr("Accept"));
  okButton->setDefault(true);

  QGroupBox *valueBox = new QGroupBox;
  QGridLayout *valueLayout = new QGridLayout;
  valueLayout->addWidget(exitPairIndexLabel,1,0,Qt::AlignRight);
  valueLayout->addWidget(exitPairIndexSpin,1,1);
  valueLayout->addWidget(minJLabel,2,0,Qt::AlignRight);
  valueLayout->addWidget(minJText,2,1);
  valueLayout->addWidget(maxJLabel,3,0,Qt::AlignRight);
  valueLayout->addWidget(maxJText,3,1);

  QGroupBox *multBox= new QGroupBox(tr("Multipolarities"));
  QHBoxLayout *multLayout = new QHBoxLayout;
  multLayout->addWidget(e1Check);
  multLayout->addWidget(m1Check);
  multLayout->addWidget(e2Check);
  multBox->setLayout(multLayout);

  valueLayout->addWidget(multBox,4,0,1,2);
  valueBox->setLayout(valueLayout);

  QHBoxLayout *buttonBox = new QHBoxLayout;
  buttonBox->addWidget(cancelButton);
  buttonBox->addWidget(okButton);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(valueBox);
  mainLayout->addLayout(buttonBox);

  setLayout(mainLayout);

  connect(okButton, SIGNAL(clicked()),this,SLOT(accept()));
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

  setWindowTitle(tr("Add an External Capture Line"));
}

