#include "ECLevel.h"
#include "ECLine.h"

/*!
 * ECLevel objects are created from entries in the external capture file.
 */

ECLevel::ECLevel(struct ECLine ecLine) {
  min_ec_lvalue_=ecLine.limin;
  max_ec_lvalue_=ecLine.limax;
  max_ec_mult_=ecLine.maxmult;
  min_ec_jvalue_=ecLine.jimin;
  max_ec_jvalue_=ecLine.jimax;
  ec_final_jnum_=0;
  ec_final_lnum_=0;
  ec_final_pairnum_=0;
}

/*!
 * Returns the minumum value for the initial orbital angular momentum.
 */

int ECLevel::GetMinL() const {
  return min_ec_lvalue_;
}

/*!
 * Returns the maximum value for the intial orbital angular momentum.
 */

int ECLevel::GetMaxL() const {
  return max_ec_lvalue_;
}

/*!
 * Returns the maximum multipolarity for the capture gamma.
 */

int ECLevel::GetMaxMult() const {
  return max_ec_mult_;
}

/*!
 * Returns the position of the final state \f$ J^\pi \f$ group in the JGroup vector.
 */

int ECLevel::GetJGroupNum() const {
  return ec_final_jnum_;
}

/*!
 * Returns the position of the final state level in the ALevel vector.
 */

int ECLevel::GetLevelNum() const {
  return ec_final_lnum_;
}

/*!
 * Returns the position of the gamma,particle pair in the PPair vector.
 */

int ECLevel::GetPairNum() const {
  return ec_final_pairnum_;
}

/*!
 * Returns the minimum value of the initial total spin.
 */

double ECLevel::GetMinJ() const {
  return min_ec_jvalue_;
}

/*!
 * Returns the maximum value of the initial total spin.
 */

double ECLevel::GetMaxJ() const {
  return max_ec_jvalue_;
}

/*! 
 * Sets the position of the final state level in the ALevel vector.
 */

void ECLevel::SetLevelNum(int levelNum) {
  ec_final_lnum_=levelNum;
}

/*! 
 * Sets the position of the final state \f$ J^\pi \f$ group in the JGroup vector.
 */

void ECLevel::SetJGroupNum(int jGroupNum) {
  ec_final_jnum_=jGroupNum;
}


/*!
 * Sets the position of the gamma,particle pair in the PPair vector.
 */

void ECLevel::SetPairNum(int pairNum) {
  ec_final_pairnum_=pairNum;
}
