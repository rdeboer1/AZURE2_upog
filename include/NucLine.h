#ifndef NUCLINE_H
#define NUCLINE_H

#include<fstream>

struct NucLine {
  double LevelJ;
  int LevelPi;
  double LevelExE;
  int levelFix;
  int aa;
  int R;
  double s;
  int l;
  int Lid;
  int yn;
  int channelFix;
  double Gam;
  double J1;
  int Pi1;
  double J2;
  int Pi2;
  double ExE;
  double M1;
  double M2;
  int Z1;
  int Z2;
  double SepE_Entrance;
  double SepE;
  int J3;
  int Pi3;
  double ExE3;
  int PType;
  double ChRad;
  double g1;
  double g2;
};

NucLine ReadNucLine(std::ifstream &stream);

#endif
