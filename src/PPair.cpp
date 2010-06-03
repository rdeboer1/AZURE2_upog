#include "PPair.h"

/*!
 * A particle pair object is created from and entry in the nuclear input file.
 */

PPair::PPair(NucLine nucLine) 
{
  pair_z_[0]=nucLine.Z1;
  pair_z_[1]=nucLine.Z2;
  pair_m_[0]=nucLine.M1;
  pair_m_[1]=nucLine.M2;
  pair_pi_[0]=nucLine.Pi1;
  pair_pi_[1]=nucLine.Pi2;
  pair_g_[0]=nucLine.g1;
  pair_g_[1]=nucLine.g2;
  pair_j_[0]=nucLine.J1;
  pair_j_[1]=nucLine.J2;
  pair_ex_e_=nucLine.ExE;
  pair_sep_e_=nucLine.SepE;
  pair_ch_rad_=nucLine.ChRad;
  pair_ptype_=nucLine.PType;
  pair_key_=nucLine.R;
  red_mass_=nucLine.M1*nucLine.M2/(nucLine.M1+nucLine.M2);
  i1i2factor_=1.0/(2.*nucLine.J1+1.0)/(2.*nucLine.J2+1.0);
  entrance_=false;
  ec_entrance_=false;
}

/*!
 * Returns true if the particle pair is an internal entrance pair, otherwise returns false.
 */

bool PPair::IsEntrance() const {
  return entrance_;
}

/*!
 * Returns true if the particle pair is an external entrance pair, otherwise returns false.
 */

bool PPair::IsECEntrance() const {
  return ec_entrance_;
}

/*!
 * Returns the atomic number of the specified particle (1 or 2).
 */

int PPair::GetZ(int particle) const {
  return  pair_z_[particle-1];
}

/*!
 * Returns the parity of the specified particle (1 or 2).
 */

int PPair::GetPi(int particle) const {
  return  pair_pi_[particle-1];
}

/*!
 * Returns the integer particle pair type.  Pair types currently used in AZURE are 0: particle,particle and 10: particle,gamma.
 */

int PPair::GetPType() const {
  return  pair_ptype_;
}

/*!
 * Returns the number of decay particle pairs for a given pair.  Size of Decay vector will only be nonzero if PPair object is 
 * an entrance pair.
 */

int PPair::NumDecays() const {
  return decays_.size();
}

/*!
 * Tests a given decay particle pair to determine if it is in the Decay vector.  If the decay particle pair exists
 * in the vector, the position in the vector is returned.  Otherwise, the function returns 0.
 */

int PPair::IsDecay(Decay decay) {
  bool b=false;
  int c=0;
  while(!b&&c<this->NumDecays())
    {
      if(decay.GetPairNum()==this->GetDecay(c+1)->GetPairNum()) b=true;
	 c++;
    }
  if(b) return c;
  else return 0;
}

/*!
 * Tests a given particle pair number to determine if there exists a corresponding particle pair decay in the Decay vector.
 * If the object exists, the position in the vector is returned.  Otherwise, the function returns 0.
 */

int PPair::IsDecay(int pairNum) {
  bool b=false;
  int c=0;
  while(!b&&c<this->NumDecays())
    {
      if(pairNum==this->GetDecay(c+1)->GetPairNum()) b=true;
	 c++;
    }
  if(b) return c;
  else return 0;
}

/*!
 * Returns the pair key for the particle pair.
 */

int PPair::GetPairKey() const {
  return pair_key_;
}

/*!
 * Returns the number of external capture components for the given entrance pair.
 */

int PPair::NumECLevels() const {
  return ec_levels_.size();
}

/*!
 * Returns the mass number of the specified particle (1 or 2).
 */

double PPair::GetM(int particle) const {
  return pair_m_[particle-1];
}

/*!
 * Returns the g-factor of the specified particle (1 or 2).
 */

double PPair::GetG(int particle) const {
  return pair_g_[particle-1];
}

/*!
 * Returns the total spin of the specified particle (1 or 2).
 */

double PPair::GetJ(int particle) const {
  return pair_j_[particle-1];
}

/*!
 * Returns the excitation energy of the particle pair.
 */

double PPair::GetExE() const {
  return pair_ex_e_;
}

/*!
 * Returns the seperation energy of the particle pair.
 */

double PPair::GetSepE() const {
  return pair_sep_e_;
}

/*!
 * Returns the channel radius of the particle pair.
 */

double PPair::GetChRad()  const {
  return pair_ch_rad_;
}

/*!
 * Returns the reduced mass of the particle pair.
 */

double PPair::GetRedMass() const {
  return red_mass_;
}

/*!
 * Returns the factor \f$ \frac{1}{(2I_1+1)(2I_2+1)} \f$ of the particle pair.
 */

double PPair::GetI1I2Factor() const {
  return i1i2factor_;
}

/*!
 * Adds a decay particle pair to the Decay vector.
 */

void PPair::AddDecay(Decay decay) {
  decays_.push_back(decay);
}

/*!
 * Sets the particle pair to be an internal entrance pair.
 */

void PPair::SetEntrance() {
  entrance_=true;
}

/*!
 * Sets the particle pair to be an external entrance pair.
 */

void PPair::SetECEntrance() {
  ec_entrance_=true;
}

/*!
 * Adds an external capture component to the ECLevel vector.
 */

void PPair::AddECLevel(ECLevel ecLevel) {
  ec_levels_.push_back(ecLevel);
}

/*!
 * Returns a pointer to the decay particle pair specified by a position in the Decay vector.
 */

Decay *PPair::GetDecay(int decayNum) {
  Decay *b=&decays_[decayNum-1];
  return b;
}

/*!
 * Returns a pointer to the external capture component specified by a position in the ECLevel vector.
 */

ECLevel *PPair::GetECLevel(int ecLevel) {
  ECLevel *b=&ec_levels_[ecLevel-1];
  return b;
}
