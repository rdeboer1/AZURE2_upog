#include "PlotTab.h"
#include "AZUREPlot.h"
#include <QVBoxLayout>

PlotTab::PlotTab(QWidget* parent) : QWidget(parent)  {
  azurePlot = new AZUREPlot(this);

  QVBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->addWidget(azurePlot);

  setLayout(mainLayout);
}

