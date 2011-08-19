#ifndef AZUREPLOT_H
#define AZUREPLOT_H

#include <qwt_plot.h>
#include <qwt_symbol.h>

class QwtPlotCurve;
class QwtPlotIntervalCurve;
class QwtPlotZoomer;

struct PlotPoint {
  double energy;
  double angle;
  double fitCrossSection;
  double fitSFactor;
  double dataCrossSection;
  double dataErrorCrossSection;
  double dataSFactor;
  double dataErrorSFactor;
};

class PlotEntry {
 public: 
  PlotEntry(int type, int entranceKey, int exitKey, int index, QString filename);
  ~PlotEntry();

  int type() const {return type_;};

  bool readData();
  void attach(QwtPlot*,int,int,QwtSymbol::Style);
  void detach();

 private:
  int type_;
  int entranceKey_;
  int exitKey_;
  int index_;
  QString filename_;
  QwtPlotCurve* dataCurve_;
  QwtPlotIntervalCurve* dataErrorCurve_;
  QwtPlotCurve* fitCurve_;
  QVector<PlotPoint> points_;
};

class AZUREPlot : public QwtPlot {

  Q_OBJECT

 public:
  AZUREPlot(QWidget* parent = 0);
  void setXAxisLog(bool set);
  void setYAxisLog(bool set);
  void setXAxisType(unsigned int type);
  void setYAxisType(unsigned int type);

 public slots:
  void draw(QList<PlotEntry*> newEntries);
  void update();
  void exportPlot();
  void print();

 private:
  unsigned int xAxisType;
  unsigned int yAxisType;
  QList<PlotEntry*> entries;
  QwtPlotZoomer* zoomer;
};

#endif
