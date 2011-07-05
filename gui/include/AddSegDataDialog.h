#ifndef ADDSEGDATADIALOG_H
#define ADDSEGDATADIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>

class AddSegDataDialog : public QDialog {
  Q_OBJECT

 public:
  AddSegDataDialog(QWidget *parent=0);
  QSpinBox *entrancePairIndexSpin;
  QSpinBox *exitPairIndexSpin;
  QLineEdit *lowEnergyText;
  QLineEdit *highEnergyText;
  QLineEdit *lowAngleText;
  QLineEdit *highAngleText;
  QComboBox *dataTypeCombo;
  QLineEdit *dataFileText;
  QLineEdit *dataNormText;
  QCheckBox *varyNormCheck;
  QLineEdit *phaseJValueText;
  QLineEdit *phaseLValueText;

 public slots:
  void setChooseFile();
  void dataTypeChanged(int);
  
 private:
  QPushButton *okButton;
  QPushButton *cancelButton;
};

#endif
