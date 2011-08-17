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
  void setXAxisType(unsigned int type) {xAxisType=type;};
  void setYAxisType(unsigned int type) {yAxisType=type;};
  unsigned int getXAxisType() const {return xAxisType;};
  unsigned int getYAxisType() const {return yAxisType;};
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
