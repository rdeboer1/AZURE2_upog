#include "ExtrapLine.h"

ExtrapLine ReadExtrapLine(std::istream &stream) {
  ExtrapLine a;
  stream >> a.isActive >> a.entrancekey >> a.exitkey >> a.min_e >> 
    a.max_e >> a.e_step >> a.min_a >> a.max_a >> a.a_step >> a.diff;
  if(a.diff==2) stream >> a.j >> a.l;
  return a;
}
