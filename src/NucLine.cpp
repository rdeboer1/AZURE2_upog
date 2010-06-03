#include "NucLine.h"
  
NucLine ReadNucLine(std::ifstream &stream)
{
  NucLine a;
  stream >> a.LevelJ >> a.LevelPi >> a.LevelExE >> a.levelFix >> a.aa >> a.R 
	 >> a.s >> a.l >> a.Lid >> a.yn >> a.channelFix >> a.Gam >> a.J1 >> a.Pi1
     >> a.J2 >> a.Pi2 >> a.ExE >> a.M1 >> a.M2 >> a.Z1 >> a.Z2
     >> a.SepE_Entrance >> a.SepE >> a.J3 >> a.Pi3 >> a.ExE3 
     >> a.PType >> a.ChRad >> a.g1 >> a.g2;
  a.s=a.s/2.;
  a.l=a.l/2;

  return a;
}
