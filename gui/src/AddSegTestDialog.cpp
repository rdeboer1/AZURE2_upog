#include "AddSegTestDialog.h"
#include <QtGui>

AddSegTestDialog::AddSegTestDialog(QWidget *parent) : QDialog(parent) {

  //  this->setMaximumSize(370,420);
  //  this->setMinimumSize(370,420);

  entrancePairIndexSpin = new QSpinBox;
  entrancePairIndexSpin->setMinimum(1);
  entrancePairIndexSpin->setMaximum(100);
  entrancePairIndexSpin->setSingleStep(1);
  exitPairIndexSpin = new QSpinBox;
  exitPairIndexSpin->setMinimum(1);
  exitPairIndexSpin->setMaximum(100);
  exitPairIndexSpin->setSingleStep(1);
  lowEnergyText = new QLineEdit;
  highEnergyText = new QLineEdit;
  energyStepText = new QLineEdit;
  lowAngleText = new QLineEdit;
  highAngleText = new QLineEdit;
  angleStepText = new QLineEdit;
  dataTypeCombo = new QComboBox;
  dataTypeCombo->addItem(tr("Angle Integrated"));
  dataTypeCombo->addItem(tr("Differential"));
  dataTypeCombo->addItem(tr("Phase Shift"));
  dataTypeCombo->addItem(tr("Angular Distribution"));
  connect(dataTypeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(dataTypeChanged(int)));
  phaseJValueText = new QLineEdit;
  phaseJValueText->setVisible(false);
  phaseJValueText->setMaximumWidth(50);
  phaseLValueText = new QLineEdit;
  phaseLValueText->setVisible(false);
  phaseLValueText->setMaximumWidth(50);

  cancelButton = new QPushButton(tr("Cancel"));
  okButton = new QPushButton(tr("Accept"));
  okButton->setDefault(true);

  QGroupBox *valueBox = new QGroupBox;
  QGridLayout *valueLayout = new QGridLayout;
  QGridLayout *pairLayout = new QGridLayout;
  pairLayout->addWidget(new QLabel(tr("Entrance Pair Index:")),0,0,Qt::AlignRight);
  pairLayout->addWidget(entrancePairIndexSpin,0,1);
  pairLayout->addWidget(new QLabel(tr("Exit Pair Index:")),0,2,Qt::AlignRight);
  pairLayout->addWidget(exitPairIndexSpin,0,3);
  valueLayout->addLayout(pairLayout,0,0,1,2);
  QGroupBox *energyBox = new QGroupBox(tr("Energy"));
  QGridLayout *energyLayout = new QGridLayout;
  energyLayout->addWidget(new QLabel(tr("Low Energy:")),0,0,Qt::AlignRight);
  energyLayout->addWidget(lowEnergyText,0,1);
  energyLayout->addWidget(new QLabel(tr("High Energy:")),1,0,Qt::AlignRight);
  energyLayout->addWidget(highEnergyText,1,1);
  energyLayout->addWidget(new QLabel(tr("Energy Step:")),2,0,Qt::AlignRight);
  energyLayout->addWidget(energyStepText,2,1);
  energyBox->setLayout(energyLayout);
  valueLayout->addWidget(energyBox,1,0);
  QGroupBox *angleBox = new QGroupBox(tr("Angle"));
  QGridLayout *angleLayout = new QGridLayout;
  angleLayout->addWidget(new QLabel(tr("Low Angle:")),0,0,Qt::AlignRight);
  angleLayout->addWidget(lowAngleText,0,1);
  angleLayout->addWidget(new QLabel(tr("High Angle:")),1,0,Qt::AlignRight);
  angleLayout->addWidget(highAngleText,1,1);
  angleLayout->addWidget(new QLabel(tr("Angle Step:")),2,0,Qt::AlignRight);
  angleLayout->addWidget(angleStepText,2,1);
  angleBox->setLayout(angleLayout);
  valueLayout->addWidget(angleBox,1,1);

  QGridLayout* lowerLayout = new QGridLayout;
  lowerLayout->addWidget(new QLabel(tr("Data Type:")),0,0,Qt::AlignRight);
  lowerLayout->addWidget(dataTypeCombo,0,1);
  lowerLayout->addItem(new QSpacerItem(1,25),0,2);
  lowerLayout->setColumnStretch(2,1);

  QHBoxLayout* phaseLayout = new QHBoxLayout;
  phaseJValueLabel = new QLabel(tr("J:"));
  phaseJValueLabel->setVisible(false);
  phaseLayout->addWidget(phaseJValueLabel);
  phaseLayout->addWidget(phaseJValueText);
  phaseLValueLabel = new QLabel(tr("l:"));
  phaseLValueLabel->setVisible(false);
  phaseLayout->addWidget(phaseLValueLabel);
  phaseLayout->addWidget(phaseLValueText);
  angDistLabel = new QLabel(tr("Maximum Order"));
  angDistSpin = new QSpinBox;
  angDistSpin->setMinimum(0);
  angDistSpin->setMaximum(10);
  angDistSpin->setSingleStep(1);
  angDistLabel->setVisible(false);
  angDistSpin->setVisible(false);
  phaseLayout->addWidget(angDistLabel);
  phaseLayout->addWidget(angDistSpin);
  lowerLayout->addLayout(phaseLayout,0,3);

  valueLayout->addLayout(lowerLayout,2,0,1,2);
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

  setWindowTitle(tr("Add an Extrapolation Segment Line"));
}

void AddSegTestDialog::dataTypeChanged(int index) {
  if(index==3) {
    angDistLabel->setVisible(true);
    angDistSpin->setVisible(true);
    lowAngleText->setEnabled(false);
    highAngleText->setEnabled(false);
    angleStepText->setEnabled(false);
    lowAngleText->setText("0");
    highAngleText->setText("0");
    angleStepText->setText("0");
  } else {
    angDistLabel->setVisible(false);
    angDistSpin->setVisible(false);
    lowAngleText->setEnabled(true);
    highAngleText->setEnabled(true);
    angleStepText->setEnabled(true);
  }
  if(index==2) {
    phaseJValueLabel->setVisible(true);
    phaseLValueLabel->setVisible(true);
    phaseJValueText->setVisible(true);
    phaseLValueText->setVisible(true);
  } else {
    phaseJValueLabel->setVisible(false);
    phaseLValueLabel->setVisible(false);
    phaseJValueText->setVisible(false);
    phaseLValueText->setVisible(false);
  }
}
