#ifndef RUNTAB_H
#define RUNTAB_H

#include <QWidget>

class ChooseFileButton;
class FilteredTextEdit;

QT_BEGIN_NAMESPACE

class QComboBox;
class QPushButton;
class QRadioButton;
class QLineEdit;
class QTextEdit;
class QGroupBox; 

QT_END_NAMESPACE

class RunTab : public QWidget {
  Q_OBJECT

 public:
  RunTab(QWidget* parent=0);
  friend class AZURESetup;
  friend class AZUREMainThread;

 private slots:
  void calculationTypeChanged(int index);
  void paramFileButtonChanged(bool checked);
  void integralsFileButtonChanged(bool checked);
  void fileTempButtonChanged(bool checked);
  void setChooseFile(QLineEdit* lineEdit);

 private:
  QComboBox* calcType;
  QPushButton* calcButton;
  QLineEdit* paramFileText;
  QLineEdit* integralsFileText;
  QRadioButton* newParamFileButton;
  QRadioButton* oldParamFileButton;
  QGroupBox* integralsFileGroup;
  QRadioButton* newIntegralsFileButton;
  QRadioButton* oldIntegralsFileButton;
  FilteredTextEdit* runtimeText;
  QLineEdit* chiVarianceText;
  QGroupBox* rateParamsGroup;
  QRadioButton* gridTempButton;
  QRadioButton* fileTempButton;
  QLineEdit* rateEntranceKey;
  QLineEdit* rateExitKey;
  QLineEdit* minTempText;
  QLineEdit* maxTempText;
  QLineEdit* tempStepText;
  QLineEdit* fileTempText;
  ChooseFileButton* rateParamsChoose;
  ChooseFileButton* paramFileChoose;
  ChooseFileButton* integralsFileChoose;
  
};



#endif
