#ifndef PLOTTAB_H
#define PLOTTAB_H

#include <QWidget>
#include <QSortFilterProxyModel>

class AZUREPlot;
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
  PlotTab(SegmentsDataModel* dataModel, SegmentsTestModel* testModel, QWidget* parent = 0);

 public slots:
  void segmentTypeChanged();

 private:
  AZUREPlot* azurePlot;
  QRadioButton* segDataButton;
  QRadioButton* segTestButton;
  QListView* segmentSelectorList;
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
