#include "ECMGroup.h"
#include <assert.h>

/*!
 * This constructor is used to create hard sphere external reaction pathways. The type of radiation
 * is specified, as well as the position of the final state in the ECLevel vector.  The final channel number is also specified.
 */
ECMGroup::ECMGroup(char radType, int multipolarity, int lInitial, double jInitial, int finalChannelNum, int ecNum) :
    radtype_(radType), mult_(multipolarity), li_(lInitial), ji_(jInitial), chf_(finalChannelNum), ecnum_(ecNum), ischancap_(false),
    chdecay_(0), chkgroup_(0), chmgroup_(0), statspinfactor_(0.0){
}

/*!
 * This constructor is used to create resonant external reaction pathways. The constructor is passed identical
 * information as in the previous case, with the addition of references to the resonant reaction pathways through
 * which the external pathways will pass.
 */

ECMGroup::ECMGroup(char radType, int multipolarity, int lInitial, double jInitial, int finalChannelNum, int ecNum, int decayNum, int kGroupNum, int mGroupNum) :
    radtype_(radType), mult_(multipolarity), li_(lInitial), ji_(jInitial), chf_(finalChannelNum), ecnum_(ecNum), ischancap_(true),
    chdecay_(decayNum), chkgroup_(kGroupNum), chmgroup_(mGroupNum), statspinfactor_(0.0){
}

/*!
 * Returns true if the pathways is a resonant external pathway, otherwise returns false.
 */

bool ECMGroup::IsChannelCapture() const {
  return ischancap_;
}

/*!
 * Returns the radiation type for the capture gamma.
 */

char ECMGroup::GetRadType() const {
  return radtype_;
}

/*!
 * Returns the multipolarity of the capture gamma.
 */

int ECMGroup::GetMult() const {
  return mult_;
}

/*!
 * Returns the initial orbital momentum value for the reaction pathway.
 */

int ECMGroup::GetL() const {
  return li_;
}

/*!
 * Returns the final channel number for the pathway.
 */

int ECMGroup::GetFinalChannel() const {
  return chf_;
}

/*!
 * Returns the position of the final state in the ECLevel vector.
 */

int ECMGroup::GetECNum() const {
  return ecnum_;
}

/*!
 * Returns the position of the resonant exit pair in the Decay vector.  Used only for resonant external pathways.
 */

int ECMGroup::GetChanCapDecay() const {
  return chdecay_;
}

/*!
 * Returns the resonant KGroup number.  Used only for resonant external pathways.
 */

int ECMGroup::GetChanCapKGroup() const {
  return chkgroup_;
}

/*!
 * Returns the resonant MGroup number.  Used only for resonant external pathways.
 */

int ECMGroup::GetChanCapMGroup() const {
  return chmgroup_;
}

/*!
 * Returns the initial spin value for the pathway.
 */

double ECMGroup::GetJ() const {
  return ji_;
}

/*!
 * Returns the statistical spin factor, \f$ g_J \f$, for the pathway.
 */

double ECMGroup::GetStatSpinFactor() const {
  return statspinfactor_;
}

/*!
 * Sets the statistical spin factor, \f$ g_J \f$, for the pathway.
 */

void ECMGroup::SetStatSpinFactor(double a) {
  statspinfactor_=a;
}

