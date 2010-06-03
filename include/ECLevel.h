#ifndef ECLEVEL_H
#define ECLEVEL_H

#include "ECLine.h"

///An AZURE external capture component.

/*!
 * When the external capture file is read, they configuration parameters are stored in a ECLevel object
 * under the corresponding entrance pair.  The size of the vector of ECLevel objects for a given entrance
 * pair will be determined by the number of entries coresponding to that entrance pair in the external
 * capture file.  The final state JGroup and ALevel objects are either created or determined to exist in
 * from the nuclear input file, and are subsequently stored in the ECLevel object.
 */

class ECLevel {
 public:
  ECLevel(ECLine);
  int GetMinL() const;
  int GetMaxL() const;
  int GetMaxMult() const;
  int GetJGroupNum() const;
  int GetLevelNum() const;
  int GetPairNum() const;
  double GetMinJ() const;
  double GetMaxJ() const;
  void SetLevelNum(int);
  void SetJGroupNum(int);
  void SetPairNum(int);
 private:
  int min_ec_lvalue_;
  int max_ec_lvalue_;
  int max_ec_mult_;
  int ec_final_jnum_;
  int ec_final_lnum_;
  int ec_final_pairnum_;
  double min_ec_jvalue_;
  double max_ec_jvalue_;
};

#endif
