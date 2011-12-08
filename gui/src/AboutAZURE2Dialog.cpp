#include "AboutAZURE2Dialog.h"
#include <QtGui>

AboutAZURE2Dialog::AboutAZURE2Dialog(QWidget *parent) : QDialog(parent) {
  setWindowTitle(tr("About AZURE2"));

  QLabel *label = new QLabel("<center><img src=\":/azure-icon.png\" width=\"128\" height=\"128\" /><br/>"
			     "<b>AZURE2</b><br/><br/>"
			     "E. Uberseder, R.J. deBoer, R.E. Azuma<br/>"
			     "Joint Institute For Nuclear Astrophysics (JINA)</center>",
			     this);
  QPushButton*  okButton = new QPushButton(tr("Ok"),this);
  okButton->setMaximumSize(80,30);

  QVBoxLayout* layout = new QVBoxLayout(this);

  layout->addWidget(label);
  layout->addWidget(okButton);
  layout->setAlignment(okButton,Qt::AlignHCenter);
  
  connect(okButton,SIGNAL(clicked()),this,SLOT(close()));
  
  setLayout(layout);
}
