#ifndef REACTIONRATE_H
#define REACTIONRATE_H

#include "EPoint.h"

extern double gsl_reactionrate_integration(double,CNuc*,const Config&,int,int);

struct RateData {
  double temperature;
  double rate;
};

///A function class to calculate the reaction rate

/*!
 * The ReactionRate function class is used to calculate the reaction rate based on a set of
 * R-Matrix parameters over a range of stellar temperatures.
 */

class ReactionRate {
 public:
  /*!
   * The ReactionRate object is created with reference to a CNuc object, a vector of Minuit parameters,
   * a Config structure, and a set of entrance and exit pair keys.
   */
  ReactionRate(CNuc *compound, const std::vector<double>&params, 
	       const Config &configure, int entranceKey, int exitKey) : 
    configure_(configure) {
    compound_=compound;
    compound_->FillCompoundFromParams(params);
    entrance_key_=entranceKey;
    exit_key_=exitKey;
  };
  /*!
   * Returns a pointer to the CNuc object.
   */
  CNuc *compound() const {return compound_;};
  /*!
   * Returns a reference to the Config structure.
   */
  const Config &configure() const {return configure_;};
  /*!
   * Returns the entrance pair key.
   */
  int entranceKey() const {return entrance_key_;};
  /*!
   * Returns the exit pair key.
   */
  int exitKey() const {return exit_key_;};
  void CalculateRates(double,double,double);
  /*!
   * Writes the rates to an output file.
   */
  void WriteRates();
 private:
  int entrance_key_;
  int exit_key_;
  CNuc *compound_;
  const Config &configure_;
  std::vector<RateData> rates_;
};



#endif
