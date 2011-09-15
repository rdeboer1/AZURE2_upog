#ifndef ADDECLINEDIALOG_H
#define ADDECLINEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>

class AddECLineDialog : public QDialog {
  Q_OBJECT

 public:
  AddECLineDialog(QWidget *parent=0);
  QSpinBox *exitPairIndexSpin;
  QLineEdit *minJText;
  QLineEdit *maxJText;
  QCheckBox *e1Check;
  QCheckBox *m1Check;
  QCheckBox *e2Check;
  
 private:
  QLabel *exitPairIndexLabel;
  QLabel *minJLabel;
  QLabel *maxJLabel;

  QPushButton *okButton;
  QPushButton *cancelButton;
};

#endif
