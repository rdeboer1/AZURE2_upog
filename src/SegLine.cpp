#include "SegLine.h"

SegLine ReadSegLine(std::istream &stream) {
  SegLine a;
  stream >> a.isActive >>  a.entrancekey >> a.exitkey >> a.min_e >> a.max_e >> a.min_a >> a.max_a >> a.diff >> a.dataNorm >> a.varyNorm >> a.datafile;
  if(a.diff==2) stream >> a.j >> a.l;
  return a;
}
