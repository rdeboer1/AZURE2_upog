#include <QtGui>
#include <iostream>
#include "AZURESetup.h"

extern int main_from_gui(int argc, char *argv[]);

int main(int argc, char *argv[])
{
  bool useGUI = true;
  for(int i=1;i<argc;i++) 
    if(strcmp(argv[i],"--no-gui")==0) useGUI = false;
  if(useGUI) {
    QApplication app(argc, argv);

    AZURESetup azureSetup;
    azureSetup.show();

    if(argc>1) {
      QString filename=argv[1];
      azureSetup.open(filename);
    }

    return app.exec();
  } else return main_from_gui(argc,argv);
}
