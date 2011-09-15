#ifndef ADDSEGDATADIALOG_H
#define ADDSEGDATADIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>

QT_BEGIN_NAMESPACE

class QLabel;

QT_END_NAMESPACE

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
  QLabel* phaseLValueLabel;
  QLabel* phaseJValueLabel;

 public slots:
  void setChooseFile();
  void dataTypeChanged(int);
  
 private:
  QPushButton *okButton;
  QPushButton *cancelButton;
};

#endif
