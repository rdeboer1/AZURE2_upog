#ifndef ADDSEGTESTDIALOG_H
#define ADDSEGTESTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>

class AddSegTestDialog : public QDialog {
  Q_OBJECT

 public:
  AddSegTestDialog(QWidget *parent=0);
  QSpinBox *entrancePairIndexSpin;
  QSpinBox *exitPairIndexSpin;
  QLineEdit *lowEnergyText;
  QLineEdit *highEnergyText;
  QLineEdit *energyStepText;
  QLineEdit *lowAngleText;
  QLineEdit *highAngleText;
  QLineEdit *angleStepText;
  QComboBox *dataTypeCombo;
  QLineEdit *phaseJValueText;
  QLineEdit *phaseLValueText;
 
 public slots:
  void dataTypeChanged(int);
 
 private:
  QPushButton *okButton;
  QPushButton *cancelButton;
};

#endif
