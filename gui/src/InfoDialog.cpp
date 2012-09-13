#include "InfoDialog.h"
#include <QtGui>

InfoDialog::InfoDialog(const QString& string, QWidget* parent) : QDialog(parent) {
  setWindowTitle(tr("Documentation"));
  setMinimumSize(600,400);

  QTextEdit* textEdit = new QTextEdit;
  textEdit->setReadOnly(true);
  textEdit->setAcceptRichText(false);
  textEdit->setHtml(string);

  QPushButton*  okButton = new QPushButton(tr("OK"),this);
  okButton->setMaximumSize(80,30);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(textEdit);
  layout->addWidget(okButton);
  layout->setAlignment(okButton,Qt::AlignHCenter);
 
  connect(okButton,SIGNAL(clicked()),this,SLOT(close()));

  setLayout(layout);
}
