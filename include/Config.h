#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>

/// A configuration structure for AZURE

/*!
 * The configuration structure is created from the runtime file passed to the 
 * AZURE executable, as well as the options specified in the command shell prompt.
 */

struct Config {
  ///The runtime configuration file name.
  std::string configfile;
  ///A boolean specifying if the calculation should use the A-Matrix.
  bool isAMatrix;
  ///A boolean specifying if error analysis via Minos should be performed.
  bool performError;
  ///If performError is true, sets the value of Up (the acceptable variance from the minimum chi-squared.
  double chiVariance;
  ///The nuclear input file name.
  std::string nucfile;
  ///The external capture input file name.
  std::string ecfile;
  ///The data segments input file name.
  std::string segfile;
  ///The extrapolation segments input file name.
  std::string extrapfile;
  ///The target effects input file name.
  std::string targeteffectsfile;
  ///The path of the output directory
  std::string outputdir;
  ///The name of the parameters file from which to read.
  std::string paramfile;
  ///The name of the external capture amplitudes file from which to read.
  std::string integralsfile;
  ///The path of the check files directory.
  std::string checkdir;
  ///The specifier for the compound nucleus check output (none, screen, or file)
  std::string checknucleus;
  ///The specifier for the reaction pathways check output (none, screen, or file)
  std::string checkpathways;
  ///The specifier for the data check output (none, screen, or file)
  std::string checkdata;
  ///The specifier for the energy dependent quantities check output (none, screen, or file)
  std::string checkpene;
  ///The specifier for the Legendre polynomials check output (none, screen, or file)
  std::string checklegpoly;
  ///The specifier for the boundary condition check output (none, screen, or file)
  std::string checkboundcon;
  ///The specifier for the angular coefficient check output (none, screen, or file)
  std::string checkangdists;
  ///The specifier for the Coulomb amplitudes check output (none, screen, or file)
  std::string checkcoulamp;
  ///A boolean specifying if a fit should be performed.
  bool performFit;
  ///A boolean specifying if AZURE will be executed in data driven mode.
  bool withData;
  ///A boolean specifying if initial parameters should be read from an external file.
  bool oldParameters;
  ///A boolean specifying if external capture will be included.
  bool isEC;
  ///A boolean specifying if the external capture amplitudes will be read from and external file.
  bool oldECFile;
  ///A boolean specifying if the reaction rate should be calculated.
  bool calcRate;
  ///A boolean specifying if the input parameters are to be transformed (set to false by --no-transform)
  bool transformParams;
  ///A boolean specifying if the Brune parametrization is to be used.
  bool isBrune;
  ///If a reaction rate is calculated, value indicates the entrance pair key.
  int rateEntrancePair;
  ///If a reaction rate is calculated, value indicates the exit pair key.  
  int rateExitPair;
  ///If a reaction rate is calculated, value indicates the minimum temperature in GK.
  double rateMinTemp;
  ///If a reaction rate is calculated, value indicates the maximum temperature in GK.
  double rateMaxTemp;
  ///If a reaction rate is calculated, value indicates the temperature step in GK.
  double rateTempStep;
  ///A constant indicating the maximum order of the Legendre polynomials to calculate.
  static const int maxLOrder=10;
};

extern int ReadConfigFile(Config&);
#ifndef NO_STAT
extern int CheckForInputFiles(Config&);
#endif

#endif
