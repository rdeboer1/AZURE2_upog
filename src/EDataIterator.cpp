#include "EDataIterator.h"
#include "ESegment.h"

EDataIterator::EDataIterator(std::vector<ESegment>* segments) :
  segments_(segments) {
  segmentIterator_=segments_->begin();
  pointIterator_=(*segmentIterator_).GetPoints().begin();
}

EDataIterator::EDataIterator(const EDataIterator& it) :
  segments_(it.segments_), segmentIterator_(it.segmentIterator_), pointIterator_(it.pointIterator_) {
}

EDataIterator& EDataIterator::operator++() {
  if(pointIterator_ < ((*segmentIterator_).GetPoints().end())-1) {
    pointIterator_++;
  } else if(segmentIterator_ < segments_->end()-1) {
    segmentIterator_++;
    pointIterator_=(*segmentIterator_).GetPoints().begin();
  } else if ((segmentIterator_ == (segments_->end()-1)) && 
	     (pointIterator_ == ((*segmentIterator_).GetPoints().end()-1)))
    pointIterator_++;
  return *this;
}

EDataIterator EDataIterator::operator++(int) {
  EDataIterator temp(*this);
  operator++();
  return temp;
}


bool EDataIterator::operator==(const EDataIterator& rhs) {
  return (rhs.segmentIterator_==segmentIterator_ && rhs.pointIterator_==pointIterator_) ? (true) : (false);
}

bool EDataIterator::operator!=(const EDataIterator& rhs) {
  return (rhs.segmentIterator_==segmentIterator_ && rhs.pointIterator_==pointIterator_) ? (false) : (true);
}

EDataIterator& EDataIterator::SetEnd() {
  segmentIterator_=segments_->end()-1;
  pointIterator_=(*segmentIterator_).GetPoints().end();
  return *this;
}

ESegmentIterator& EDataIterator::segment() {
  return segmentIterator_;
}

EPointIterator& EDataIterator::point() {
  return pointIterator_;
}
