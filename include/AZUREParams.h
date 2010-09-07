#ifndef AZUREPARAMS_H
#define AZUREPARAMS_H

#include <Minuit2/MnUserParameters.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "Constants.h"

///A container class to hold Minuit parameters in AZURE

/*!
 * The AZUREParams class holds the Minuit parameters determined in the fit.
 * The class also has member functions corresponding to reading and writing of 
 * the parameters and their errors.
 */

class AZUREParams {
 public:
  ROOT::Minuit2::MnUserParameters &GetMinuitParams();
  void ReadUserParameters(std::string);
  void WriteUserParameters(std::string,bool);
  void WriteParameterErrors(const std::vector<std::pair<double,double> >&,std::string);
 private:
  ROOT::Minuit2::MnUserParameters params_;
};

#endif
