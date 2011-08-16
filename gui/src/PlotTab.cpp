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
  xAxisAngleButton = new QRadioButton(tr("Angle"));
  xAxisIsLogCheck = new QCheckBox(tr("Use Logarithmic Scale"));
  QHBoxLayout* xAxisLayout = new QHBoxLayout;
  xAxisLayout->setContentsMargins(5,5,5,5);
  xAxisLayout->addWidget(xAxisEnergyButton);
  xAxisLayout->addWidget(xAxisAngleButton);
  xAxisLayout->addWidget(xAxisIsLogCheck);
  xAxisBox->setLayout(xAxisLayout);
  QGroupBox *yAxisBox = new QGroupBox(tr("Y-Axis Configuration"));
  yAxisXSButton = new QRadioButton(tr("Cross Section"));
  yAxisSFButton = new QRadioButton(tr("S-Factor"));
  yAxisIsLogCheck = new QCheckBox(tr("Use Logarithmic Scale"));
  QHBoxLayout* yAxisLayout = new QHBoxLayout;
  yAxisLayout->setContentsMargins(5,5,5,5);
  yAxisLayout->addWidget(yAxisXSButton);
  yAxisLayout->addWidget(yAxisSFButton);
  yAxisLayout->addWidget(yAxisIsLogCheck);
  yAxisBox->setLayout(yAxisLayout);

  topLayout->addWidget(xAxisBox,0,0);
  topLayout->addWidget(yAxisBox,0,1);
 
  azurePlot = new AZUREPlot(this);

  rightLayout->addLayout(topLayout);
  rightLayout->addWidget(azurePlot);
  
  QGridLayout *segmentPickerLayout = new QGridLayout;
  segmentPickerLayout->setContentsMargins(5,5,5,5);
  QGroupBox *whichSegTypeBox = new QGroupBox(tr("Segment Selection"));
  segDataButton = new QRadioButton(tr("Data Segments"));
  connect(segDataButton,SIGNAL(toggled(bool)),this,SLOT(segmentTypeChanged()));
  segTestButton = new QRadioButton(tr("Extrapolation Segments"));
  connect(segTestButton,SIGNAL(toggled(bool)),this,SLOT(segmentTypeChanged()));
  segmentPickerLayout->addWidget(segDataButton,0,0);
  segmentPickerLayout->addWidget(segTestButton,1,0);
  segmentSelectorList = new QListView;
  segmentSelectorList->setItemDelegate(new RichTextDelegate());
  segmentSelectorList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  segmentPickerLayout->addWidget(segmentSelectorList,2,0);
  whichSegTypeBox->setLayout(segmentPickerLayout);
  segDataButton->setChecked(true);

  refreshButton = new QPushButton(tr("Refresh"));
  exportButton = new QPushButton(tr("Export..."));
  printButton = new QPushButton(tr("Print..."));
  QHBoxLayout *buttonLayout = new QHBoxLayout; 
  buttonLayout->addWidget(refreshButton);
  buttonLayout->addWidget(exportButton);
  buttonLayout->addWidget(printButton);

  QVBoxLayout* leftLayout = new QVBoxLayout;
  leftLayout->addWidget(whichSegTypeBox);
  leftLayout->addLayout(buttonLayout);

  QGridLayout* mainLayout = new QGridLayout;
  mainLayout->addLayout(leftLayout,0,0);
  mainLayout->addLayout(rightLayout,0,1);
  mainLayout->setColumnStretch(1,1);

  setLayout(mainLayout);
}

void PlotTab::segmentTypeChanged() {
  if(segDataButton->isChecked()) 
    segmentSelectorList->setModel(segDataProxyModel);
  else if(segTestButton->isChecked())
    segmentSelectorList->setModel(segTestProxyModel);
}
