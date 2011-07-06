#include <QtGui>
#include <iostream>
#include "AZURESetup.h"

int start_gui(int argc, char *argv[]) {
  QApplication app(argc, argv);
  
  AZURESetup azureSetup;
  azureSetup.show();
  
  if(argc>1) {
    QString filename=argv[1];
    azureSetup.open(filename);
  }
  return app.exec();
}
