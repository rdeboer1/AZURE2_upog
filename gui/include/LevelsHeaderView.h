#ifndef LEVELSHEADERVIEW_H
#define LEVELSHEADERVIEW_H

#include <QHeaderView>

class LevelsHeaderView : public QHeaderView {
 public:
  LevelsHeaderView(Qt::Orientation orientation, QWidget *parent) : 
    QHeaderView(orientation,parent) {
      setClickable(true);
    };
 protected:
   virtual void mouseReleaseEvent ( QMouseEvent * e ) {
     int pos = orientation() == Qt::Horizontal ? e->x() : e->y();
     int section = logicalIndexAt(pos);
     if(section>1) QHeaderView::mouseReleaseEvent(e);
    };
};

#endif
