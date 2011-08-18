#ifndef PLOTTAB_H
#define PLOTTAB_H

#include <QWidget>
#include <QSortFilterProxyModel>

class Config;
class AZUREPlot;
class PlotEntry;
class SegmentsDataModel;
class SegmentsTestModel;

QT_BEGIN_NAMESPACE

class QRadioButton;
class QListView;
class QCheckBox;
class QPushButton;

QT_END_NAMESPACE

class SegTestProxyModel : public QSortFilterProxyModel {
 public:
  SegTestProxyModel(QWidget* parent = 0) : QSortFilterProxyModel(parent) {};
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
};

class SegDataProxyModel : public QSortFilterProxyModel {
 public:
  SegDataProxyModel(QWidget* parent = 0) : QSortFilterProxyModel(parent) {};
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
};

class PlotTab : public QWidget {

  Q_OBJECT

 public: 
  PlotTab(Config& config, SegmentsDataModel* dataModel, SegmentsTestModel* testModel, QWidget* parent = 0);
  QList<PlotEntry*> getDataSegments();
  QList<PlotEntry*> getTestSegments();
  
 public slots:
  void draw();
  void xAxisTypeChanged();
  void yAxisTypeChanged();
  void xAxisLogScaleChanged(bool);
  void yAxisLogScaleChanged(bool);

 private:
  Config& configure;
  AZUREPlot* azurePlot;
  QListView* dataSegmentSelectorList;
  QListView* testSegmentSelectorList;
  QRadioButton* yAxisXSButton;
  QRadioButton* yAxisSFButton;
  QRadioButton* xAxisEnergyButton;
  QRadioButton* xAxisAngleButton;
  QCheckBox* xAxisIsLogCheck;
  QCheckBox* yAxisIsLogCheck;
  SegTestProxyModel* segTestProxyModel;
  SegDataProxyModel* segDataProxyModel;
  QPushButton* refreshButton;
  QPushButton* exportButton;
  QPushButton* printButton;
};

#endif
