#include "EditOptionsDialog.h"
#include <QtGui>

EditOptionsDialog::EditOptionsDialog(QWidget* parent) : QDialog(parent) {
  
  useGSLCoulCheck = new QCheckBox(tr("Use GSL Coulomb functions"));
  useBruneCheck = new QCheckBox(tr("Use Brune formalism"));
  ignoreExternalsCheck = new QCheckBox(tr("Ignore external width\nif internal width is zeroed"));
  useRMCCheck = new QCheckBox(tr("Use RMC capture formalism"));
  noTransformCheck = new QCheckBox(tr("Do not perform parameter\ntransformations"));

  QGroupBox* optionsBox = new QGroupBox(tr("AZURE2 Options"));
  QVBoxLayout* optionsLayout = new QVBoxLayout;
  optionsLayout->addWidget(useGSLCoulCheck);
  optionsLayout->addWidget(useBruneCheck);
  optionsLayout->addWidget(ignoreExternalsCheck);
  optionsLayout->addWidget(useRMCCheck);
  optionsLayout->addWidget(noTransformCheck);
  optionsBox->setLayout(optionsLayout);

  cancelButton = new QPushButton(tr("Cancel"));
  okButton = new QPushButton(tr("Accept"));
  okButton->setDefault(true);
  connect(okButton, SIGNAL(clicked()),this,SLOT(accept()));
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

  QHBoxLayout *buttonBox = new QHBoxLayout;
  buttonBox->addWidget(cancelButton);
  buttonBox->addWidget(okButton);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(optionsBox);
  mainLayout->addLayout(buttonBox);
  setWindowTitle(tr("Edit Options"));
  setLayout(mainLayout);
}
