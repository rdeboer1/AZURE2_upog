#ifndef PLOTTAB_H
#define PLOTTAB_H

#include <QWidget>

class AZUREPlot;

class PlotTab : public QWidget {

  Q_OBJECT

 public: 
  PlotTab(QWidget* parent = 0);

 private:
  AZUREPlot* azurePlot;
};

#endif
