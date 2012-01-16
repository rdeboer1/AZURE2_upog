#include <QtGui>
#include <iostream>
#include "AZURESetup.h"

inline void initResource() { Q_INIT_RESOURCE(AZURESetup); }

int start_gui(int argc, char *argv[]) {
  QApplication app(argc, argv);
  
  initResource();
  QCoreApplication::setOrganizationName("jina");
  QCoreApplication::setApplicationName("azure2");

  AZURESetup azureSetup;
  azureSetup.show();
  
  QString filename="";
  for(int i=1;i<argc;i++) 
    if(strncmp(argv[i],"--",2)!=0) {
      filename=argv[i];
      break;
    }
  if(!filename.trimmed().isEmpty()) azureSetup.open(filename);
  
  return app.exec();
}
