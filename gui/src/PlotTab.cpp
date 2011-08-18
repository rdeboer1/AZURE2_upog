#include "PlotTab.h"
#include "AZUREPlot.h"
#include "SegmentsDataModel.h"
#include "SegmentsTestModel.h"
#include "RichTextDelegate.h"
#include <QtGui>

QVariant SegTestProxyModel::data(const QModelIndex& index, int role) const {
  if (index.isValid() && role == Qt::DisplayRole) {
    QModelIndex sourceIndex = mapToSource(index);
    return QString("#%1: %2").arg(sourceIndex.row()+1).arg(static_cast<SegmentsTestModel*>(sourceModel())->getReactionLabel(sourceIndex));
  }
  return QVariant();
}

QVariant SegDataProxyModel::data(const QModelIndex& index, int role) const {
  if (index.isValid() && role == Qt::DisplayRole) {
    QModelIndex sourceIndex = mapToSource(index);
    return QString("#%1: %2").arg(sourceIndex.row()+1).arg(static_cast<SegmentsDataModel*>(sourceModel())->getReactionLabel(sourceIndex));
  } 
  return QVariant();
}

PlotTab::PlotTab(SegmentsDataModel* dataModel, SegmentsTestModel* testModel, QWidget* parent) : QWidget(parent)  {

  azurePlot = new AZUREPlot(this);

  segDataProxyModel = new SegDataProxyModel(this);
  segDataProxyModel->setSourceModel(dataModel);
  segDataProxyModel->setDynamicSortFilter(true);
  segDataProxyModel->setFilterKeyColumn(0);
  segDataProxyModel->setFilterRole(Qt::CheckStateRole);
  segDataProxyModel->setFilterRegExp(QString("%1").arg(Qt::Checked));
  segTestProxyModel = new SegTestProxyModel(this);
  segTestProxyModel->setSourceModel(testModel);
  segTestProxyModel->setDynamicSortFilter(true);
  segTestProxyModel->setFilterKeyColumn(0);
  segTestProxyModel->setFilterRole(Qt::CheckStateRole);
  segTestProxyModel->setFilterRegExp(QString("%1").arg(Qt::Checked));

  QVBoxLayout* rightLayout = new QVBoxLayout;

  QGridLayout *topLayout = new QGridLayout;

  QGroupBox *xAxisBox = new QGroupBox(tr("X-Axis Configuration"));
  xAxisEnergyButton = new QRadioButton(tr("Energy"));
  connect(xAxisEnergyButton,SIGNAL(toggled(bool)),this,SLOT(xAxisTypeChanged()));
  xAxisEnergyButton->setChecked(true);
  xAxisAngleButton = new QRadioButton(tr("Angle"));
  connect(xAxisAngleButton,SIGNAL(toggled(bool)),this,SLOT(xAxisTypeChanged()));
#ifdef MACX_SPACING
  xAxisIsLogCheck = new QCheckBox(tr("Use Log Scale"));
#else
  xAxisIsLogCheck = new QCheckBox(tr("Use Logarithmic Scale"));
#endif
  connect(xAxisIsLogCheck,SIGNAL(toggled(bool)),this,SLOT(xAxisLogScaleChanged(bool)));
  QHBoxLayout* xAxisLayout = new QHBoxLayout;
  xAxisLayout->setContentsMargins(5,5,5,5);
  xAxisLayout->addWidget(xAxisEnergyButton);
  xAxisLayout->addWidget(xAxisAngleButton);
  xAxisLayout->addWidget(xAxisIsLogCheck);
  xAxisBox->setLayout(xAxisLayout);
  QGroupBox *yAxisBox = new QGroupBox(tr("Y-Axis Configuration"));
  yAxisXSButton = new QRadioButton(tr("Cross Section"));
  connect(yAxisXSButton,SIGNAL(toggled(bool)),this,SLOT(yAxisTypeChanged()));  
  yAxisXSButton->setChecked(true);
  yAxisSFButton = new QRadioButton(tr("S-Factor"));
  connect(yAxisSFButton,SIGNAL(toggled(bool)),this,SLOT(yAxisTypeChanged()));  
#ifdef MACX_SPACING
  yAxisIsLogCheck = new QCheckBox(tr("Use Log Scale"));
#else
  yAxisIsLogCheck = new QCheckBox(tr("Use Logarithmic Scale"));
#endif
  connect(yAxisIsLogCheck,SIGNAL(toggled(bool)),this,SLOT(yAxisLogScaleChanged(bool)));
  yAxisIsLogCheck->setChecked(true);
  QHBoxLayout* yAxisLayout = new QHBoxLayout;
  yAxisLayout->setContentsMargins(5,5,5,5);
  yAxisLayout->addWidget(yAxisXSButton);
  yAxisLayout->addWidget(yAxisSFButton);
  yAxisLayout->addWidget(yAxisIsLogCheck);
  yAxisBox->setLayout(yAxisLayout);

  topLayout->addWidget(xAxisBox,0,0);
  topLayout->addWidget(yAxisBox,0,1);

  rightLayout->addLayout(topLayout);
  rightLayout->addWidget(azurePlot);
  
  dataSegmentSelectorList = new QListView;
  testSegmentSelectorList = new QListView;
  dataSegmentSelectorList->setAttribute(Qt::WA_MacShowFocusRect, 0);
  testSegmentSelectorList->setAttribute(Qt::WA_MacShowFocusRect, 0);
  dataSegmentSelectorList->setModel(segDataProxyModel);
  testSegmentSelectorList->setModel(segTestProxyModel);
  dataSegmentSelectorList->setItemDelegate(new RichTextDelegate());
  testSegmentSelectorList->setItemDelegate(new RichTextDelegate());
  dataSegmentSelectorList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  testSegmentSelectorList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  dataSegmentSelectorList->setResizeMode(QListView::Adjust);
  testSegmentSelectorList->setResizeMode(QListView::Adjust);
  connect(dataSegmentSelectorList->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(dataSegmentSelectionChanged(QItemSelection)));
  connect(testSegmentSelectorList->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(testSegmentSelectionChanged(QItemSelection)));

  QGroupBox *dataSegmentSelectorBox = new QGroupBox(tr("Data Segment Selection"));
  QGridLayout *dataSegmentSelectorLayout = new QGridLayout;
  dataSegmentSelectorLayout->setContentsMargins(5,5,5,5);
  QGroupBox *testSegmentSelectorBox = new QGroupBox(tr("Extrapolation Segment Selection"));
  QGridLayout *testSegmentSelectorLayout = new QGridLayout;
  testSegmentSelectorLayout->setContentsMargins(5,5,5,5);

  dataSegmentSelectorLayout->addWidget(dataSegmentSelectorList,0,0);
  testSegmentSelectorLayout->addWidget(testSegmentSelectorList,0,0);
  dataSegmentSelectorBox->setLayout(dataSegmentSelectorLayout);
  testSegmentSelectorBox->setLayout(testSegmentSelectorLayout);

  refreshButton = new QPushButton(tr("Refresh"));
  exportButton = new QPushButton(tr("Export..."));
  printButton = new QPushButton(tr("Print..."));
  QHBoxLayout *buttonLayout = new QHBoxLayout; 
  buttonLayout->addWidget(refreshButton);
  buttonLayout->addWidget(exportButton);
  buttonLayout->addWidget(printButton);

  QVBoxLayout* leftLayout = new QVBoxLayout;
  leftLayout->addWidget(dataSegmentSelectorBox);
  leftLayout->addWidget(testSegmentSelectorBox);
  leftLayout->addLayout(buttonLayout);

  QGridLayout* mainLayout = new QGridLayout;
  mainLayout->addLayout(leftLayout,0,0);
  mainLayout->addLayout(rightLayout,0,1);
  mainLayout->setColumnStretch(1,1);

  setLayout(mainLayout);
}

void PlotTab::dataSegmentSelectionChanged(QItemSelection selection) {
}

void PlotTab::testSegmentSelectionChanged(QItemSelection selection) {
}

void PlotTab::xAxisTypeChanged() {
  if(xAxisEnergyButton->isChecked()) 
    azurePlot->setXAxisType(0);
  else if(xAxisAngleButton->isChecked())
    azurePlot->setXAxisType(1);
}

void PlotTab::yAxisTypeChanged() {
  if(yAxisXSButton->isChecked()) 
    azurePlot->setYAxisType(0);
  else if(yAxisSFButton->isChecked())
    azurePlot->setYAxisType(1);
}

void PlotTab::xAxisLogScaleChanged(bool checked) {
  azurePlot->setXAxisLog(checked);
}

void PlotTab::yAxisLogScaleChanged(bool checked) {
  azurePlot->setYAxisLog(checked);
}
