#include "AZUREPlot.h"
#include <QtGui>
#include "qwt_plot_curve.h"
#include "qwt_plot_intervalcurve.h"
#include "qwt_interval_symbol.h"
#include "qwt_scale_engine.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_renderer.h"
#include <iostream>

QwtText AZUREZoomer::trackerTextF( const QPointF &pos ) const
{
    QString text;
    switch (rubberBand()) {
        case HLineRubberBand:
            text.sprintf( "%.4g", pos.y() );
            break;
        case VLineRubberBand:
            text.sprintf( "%.4g", pos.x() );
            break;
        default:
            text.sprintf( "%.4g, %.4g", pos.x(), pos.y() );
    }
    return QwtText( text );
}


PlotEntry::PlotEntry(int type, int entranceKey, int exitKey, int index, QString filename) :
  type_(type), entranceKey_(entranceKey), exitKey_(exitKey), index_(index), filename_(filename),
  dataCurve_(NULL), dataErrorCurve_(NULL), fitCurve_(NULL) {
}

PlotEntry::~PlotEntry() {
    if(dataCurve_) delete dataCurve_;
    if(dataErrorCurve_) delete dataErrorCurve_;	
    if(fitCurve_) delete fitCurve_;
}

bool PlotEntry::readData() {
  QFile file(filename_);
  if(!file.open(QIODevice::ReadOnly)) {
    return false;
  }
  QTextStream inStream(&file);
  QString line("");
  bool previousLineBreak=false;
  bool foundBlock=false;
  int blockNumber=0;
  while(!inStream.atEnd()&&!foundBlock) {
    line=inStream.readLine();
    if(line.trimmed().isEmpty()) {
      if(!previousLineBreak) {
	previousLineBreak=true;
	continue;
      }
      if(blockNumber==index_) {
	foundBlock=true;
	break;
      } else {
	blockNumber++;
	previousLineBreak=false;
	points_.clear();
	continue;
      }
    }
    if(!inStream.atEnd()&&!foundBlock) {
      QTextStream in(&line);
      PlotPoint newPoint = {0.,0.,0.,0.,0.,0.,0.,0.};
      if(type_==0) {
	in >> newPoint.energy >> newPoint.angle >> newPoint.fitCrossSection >> newPoint.fitSFactor 
	   >> newPoint.dataCrossSection >> newPoint.dataErrorCrossSection >> newPoint.dataSFactor 
	   >> newPoint.dataErrorSFactor;
      } else {
	in >> newPoint.energy >> newPoint.angle >> newPoint.fitCrossSection >> newPoint.fitSFactor; 
      }
      points_.push_back(newPoint);
    }
  }
  inStream.flush();
  file.close();
  if(!foundBlock) {
    return false;
  }
  return true;
}

void PlotEntry::attach(QwtPlot* plot, int xAxisType, int yAxisType, QwtSymbol::Style style) {
  QVector<QPointF> fit(points_.size());
  if(type_==0) {
    QVector<QPointF> data(points_.size());
    QVector<QwtIntervalSample> error(points_.size());

    if(xAxisType==0&&yAxisType==0) {
      for(int i=0;i<points_.size();i++) {
	data[i]=QPointF(points_[i].energy,points_[i].dataCrossSection);
	fit[i]=QPointF(points_[i].energy,points_[i].fitCrossSection);
	error[i]=QwtIntervalSample(points_[i].energy,
				   QwtInterval(points_[i].dataCrossSection-points_[i].dataErrorCrossSection,
					       points_[i].dataCrossSection+points_[i].dataErrorCrossSection));
      }
    } else if(xAxisType==0&&yAxisType==1) {
      for(int i=0;i<points_.size();i++) {
	data[i]=QPointF(points_[i].energy,points_[i].dataSFactor);
	fit[i]=QPointF(points_[i].energy,points_[i].fitSFactor);
	error[i]=QwtIntervalSample(points_[i].energy,
				   QwtInterval(points_[i].dataSFactor-points_[i].dataErrorSFactor,
					       points_[i].dataSFactor+points_[i].dataErrorSFactor));
      }
    } else if(xAxisType==1&&yAxisType==0) {
      for(int i=0;i<points_.size();i++) {
	data[i]=QPointF(points_[i].angle,points_[i].dataCrossSection);
	fit[i]=QPointF(points_[i].angle,points_[i].fitCrossSection);
	error[i]=QwtIntervalSample(points_[i].angle,
		QwtInterval(points_[i].dataCrossSection-points_[i].dataErrorCrossSection,
		points_[i].dataCrossSection+points_[i].dataErrorCrossSection));
      }
    } else if(xAxisType==1&&yAxisType==1) {
      for(int i=0;i<points_.size();i++) {
	data[i]=QPointF(points_[i].angle,points_[i].dataSFactor);
	fit[i]=QPointF(points_[i].angle,points_[i].fitSFactor);
	error[i]=QwtIntervalSample(points_[i].angle,
		QwtInterval(points_[i].dataSFactor-points_[i].dataErrorSFactor,
		points_[i].dataSFactor+points_[i].dataErrorSFactor));
      }
    }

    dataCurve_ = new QwtPlotCurve;
    dataCurve_->setRenderHint( QwtPlotItem::RenderAntialiased );
    dataCurve_->setStyle( QwtPlotCurve::NoCurve );
    QwtSymbol *symbol = new QwtSymbol(style);
    symbol->setSize( 6 );
    symbol->setPen( QPen( Qt::black ) );
    symbol->setColor( QColor( Qt::black ) );

    dataErrorCurve_ = new QwtPlotIntervalCurve;
    dataErrorCurve_->setRenderHint( QwtPlotItem::RenderAntialiased );
    dataErrorCurve_->setStyle( QwtPlotIntervalCurve::NoCurve );
    QwtIntervalSymbol *errorBar =
      new QwtIntervalSymbol( QwtIntervalSymbol::Bar );
    errorBar->setWidth( 8 ); 
    errorBar->setPen( QPen( Qt::black ) );

    dataCurve_->setSymbol( symbol );
    dataCurve_->setSamples(data);

    dataErrorCurve_->setSymbol(errorBar);
    dataErrorCurve_->setSamples(error);  
  } else {
    if(xAxisType==0&&yAxisType==0) {
      for(int i=0;i<points_.size();i++) 
	fit[i]=QPointF(points_[i].energy,points_[i].fitCrossSection);
    } else if(xAxisType==0&&yAxisType==1) {
      for(int i=0;i<points_.size();i++) 
	fit[i]=QPointF(points_[i].energy,points_[i].fitSFactor);
    } else if(xAxisType==1&&yAxisType==0) {
      for(int i=0;i<points_.size();i++) 
	fit[i]=QPointF(points_[i].angle,points_[i].fitCrossSection);
    } else if(xAxisType==1&&yAxisType==1) {
      for(int i=0;i<points_.size();i++) 
	fit[i]=QPointF(points_[i].angle,points_[i].fitSFactor);
    }
  }
  
  fitCurve_ = new QwtPlotCurve;
  fitCurve_->setRenderHint( QwtPlotItem::RenderAntialiased );
  fitCurve_->setStyle( QwtPlotCurve::Lines );
  fitCurve_->setPen( QPen( Qt::red , 2 ) );

  fitCurve_->setSymbol(new QwtSymbol(QwtSymbol::NoSymbol));
  fitCurve_->setSamples(fit);

  if(dataCurve_) dataCurve_->attach(plot);
  if(dataErrorCurve_) dataErrorCurve_->attach(plot);
  if(fitCurve_) fitCurve_->attach(plot);
}

void PlotEntry::detach() {
  if(dataCurve_) dataCurve_->detach();
  if(dataErrorCurve_) dataErrorCurve_->detach();
  if(fitCurve_) fitCurve_->detach();
}

AZUREPlot::AZUREPlot(QWidget* parent) : QwtPlot(parent) {
  setCanvasBackground(QColor(Qt::white));
  setAutoReplot(true);

  zoomer = new AZUREZoomer( canvas() );
  zoomer->setRubberBandPen( QColor( Qt::black ) );
  zoomer->setTrackerPen( QColor( Qt::black ) );
  zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
			   Qt::RightButton, Qt::ControlModifier );
  zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
			   Qt::RightButton );
  
  QwtPlotPanner *panner = new QwtPlotPanner( canvas() );
  panner->setMouseButton( Qt::MidButton );

}


void AZUREPlot::setXAxisLog(bool set) {
  setAxisAutoScale(QwtPlot::xBottom,true);
  setAxisAutoScale(QwtPlot::yLeft,true);
  if(set) setAxisScaleEngine(QwtPlot::xBottom,new QwtLog10ScaleEngine);
  else setAxisScaleEngine(QwtPlot::xBottom,new QwtLinearScaleEngine);
  zoomer->setZoomBase(false);
};

void AZUREPlot::setYAxisLog(bool set) {
  setAxisAutoScale(QwtPlot::xBottom,true);
  setAxisAutoScale(QwtPlot::yLeft,true);
  if(set) setAxisScaleEngine(QwtPlot::yLeft,new QwtLog10ScaleEngine);
  else setAxisScaleEngine(QwtPlot::yLeft,new QwtLinearScaleEngine);
  zoomer->setZoomBase(false);
};

void AZUREPlot::setXAxisType(unsigned int type) {
  QwtText text = (type==0) ? QwtText(QString("Center of Mass Energy [MeV]")) : QwtText(QString("Center of Mass Angle [degrees]"));
  setAxisTitle(QwtPlot::xBottom,text);

  xAxisType=type;
  update();
}

void AZUREPlot::setYAxisType(unsigned int type) {
  QwtText text = (type==0) ? QwtText(QString("Cross Section [b]")) : QwtText(QString("S-Factor [MeV b]"));
  setAxisTitle(QwtPlot::yLeft,text);
  yAxisType=type;
  update();
}

void AZUREPlot::draw(QList<PlotEntry*> newEntries) {
  for(int i = 0; i<entries.size(); i++) {
    entries[i]->detach();
    delete entries[i]; 
  }
  if(entries.size()>0) entries.clear();
  
  int numDataEntries=0;
  for(int i = 0; i<newEntries.size(); i++) {
    if(newEntries[i]->readData()) {
      QwtSymbol::Style style = (newEntries[i]->type()==0) ? (QwtSymbol::Style) 
	numDataEntries++ : QwtSymbol::NoSymbol;
      newEntries[i]->attach(this,xAxisType,yAxisType,style);
      entries.push_back(newEntries[i]);
    } else delete newEntries[i];
  }   
  setAxisAutoScale(QwtPlot::xBottom,true);
  setAxisAutoScale(QwtPlot::yLeft,true);
  replot();
  zoomer->setZoomBase(false);
}

void AZUREPlot::update() {
  setAxisAutoScale(QwtPlot::xBottom,true);
  setAxisAutoScale(QwtPlot::yLeft,true);
  int numDataEntries=0;
  for(int i = 0; i<entries.size(); i++) {
    entries[i]->detach();
    QwtSymbol::Style style = (entries[i]->type()==0) ? (QwtSymbol::Style) numDataEntries++ : QwtSymbol::NoSymbol;
    entries[i]->attach(this,xAxisType,yAxisType,style);
  }
  replot();
  zoomer->setZoomBase(false);
}

void AZUREPlot::exportPlot()
{
#ifndef QT_NO_PRINTER
  QString fileName = "AZUREPlot.pdf";
#else
  QString fileName = "AZUREPlot.png";
#endif

#ifndef QT_NO_FILEDIALOG
    const QList<QByteArray> imageFormats =
      QImageWriter::supportedImageFormats();
    
    QStringList filter;
    filter += "PDF Documents (*.pdf)";
#ifndef QWT_NO_SVG
    filter += "SVG Documents (*.svg)";
#endif
    filter += "Postscript Documents (*.ps)";
    
    if (imageFormats.size()>0) {
      QString imageFilter("Images (");
      for (int i=0;i<imageFormats.size();i++) {
	if (i>0) imageFilter += " ";
	  imageFilter += "*.";
	  imageFilter += imageFormats[i];
        }
      imageFilter += ")";
  
      filter += imageFilter;
    }
    fileName = QFileDialog::getSaveFileName(
					    this, "Export File Name", fileName,
					    filter.join(";;"), NULL, QFileDialog::DontConfirmOverwrite);
#endif
    if(!fileName.isEmpty()) {
      QwtPlotRenderer renderer;
      renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, true);
      renderer.renderDocument(this, fileName, QSizeF(300, 200), 85);
    }
}

void AZUREPlot::print()
{
    QPrinter printer(QPrinter::HighResolution);
    QString docName("AZUREPlot");
    printer.setDocName (docName);

    printer.setCreator("AZURE2");
    printer.setOrientation(QPrinter::Landscape);

    QPrintDialog dialog(&printer);
    if (dialog.exec()) {
        QwtPlotRenderer renderer;
        renderer.renderTo(this, printer);
    }
}
