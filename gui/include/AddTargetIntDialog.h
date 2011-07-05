#ifndef ADDTARGETINTDIALOG_H
#define ADDTARGETINTDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE

class QLineEdit;
class QSpinBox;
class QCheckBox;
class QTableWidget;
class QGroupBox;
class QSize;

QT_END_NAMESPACE

class AddTargetIntDialog : public QDialog {
  Q_OBJECT

 public:
  AddTargetIntDialog(QWidget *parent=0);
  QCheckBox *isConvolutionCheck;
  QCheckBox *isTargetIntegrationCheck;
  QLineEdit *sigmaText;
  QLineEdit *segmentsListText;
  QSpinBox *numPointsSpin;
  QSpinBox *numParametersSpin;
  QLineEdit *densityText;
  QLineEdit *stoppingPowerEqText;
  QTableWidget *parametersTable;
  QList<double> tempParameters;
  void createParameterItem(int row, double value = 0.0);

 public slots:
  void convolutionCheckChanged(bool checked);
  void targetIntCheckChanged(bool checked);
  void parameterSpinChanged(int newNumber);
  void parameterChanged(int row, int column);

 private:
  QPushButton *okButton;
  QPushButton *cancelButton;
  QGroupBox *stoppingPowerBox;
};

#endif
