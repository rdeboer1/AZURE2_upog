#ifndef AZUREPLOT_H
#define AZUREPLOT_H

#include <qwt_plot.h>

class AZUREPlot : public QwtPlot {

  Q_OBJECT

 public:
  AZUREPlot(QWidget* parent = 0);
  bool getXAxisLog() const { return xAxisLog;};
  bool getYAxisLog() const { return yAxisLog;};
  void setXAxisLog(bool set) {xAxisLog=set;};
  void setYAxisLog(bool set) {yAxisLog=set;};
  void fillFromDataSegment();

 public slots:
  void update();

 private:
  unsigned int xAxisType;
  unsigned int yAxisType;
  bool xAxisLog;
  bool yAxisLog;
};

#endif
