#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>

#define USE_AMATRIX              (1<<0)
#define PERFORM_ERROR_ANALYSIS   (1<<1)
#define PERFORM_FIT              (1<<2)
#define CALCULATE_WITH_DATA      (1<<3)
#define USE_PREVIOUS_PARAMETERS  (1<<4)
#define USE_EXTERNAL_CAPTURE     (1<<5)
#define USE_PREVIOUS_INTEGRALS   (1<<6)
#define CALCULATE_REACTION_RATE  (1<<7)
#define TRANSFORM_PARAMETERS     (1<<8)
#define USE_BRUNE_FORMALISM      (1<<9)
#define IGNORE_ZERO_WIDTHS       (1<<10)
#define USE_RMC_FORMALISM        (1<<11)

/// A structure holding the reaction rate calculation configuration

/*!
 * The RateParams structure holds the configuration information for a reaction rate calculation.
 */

struct RateParams {
  ///False for looped temperatures, true for temperatures from file.
  bool useFile;
  ///String containing filename with temperatures to use
  std::string temperatureFile;
  ///The entrance pair number for the rate calculation
  int entrancePair;
  ///The exit pair number for the rate calculation
  int exitPair;
  ///The minimum temperature for the rate calculation
  double minTemp;
  ///The maximum temperature for the rate calculation
  double maxTemp;
  ///The temperature step for the rate calculation
  double tempStep;
};

/// A configuration structure for AZURE

/*!
 * The configuration structure is created from the runtime file passed to the 
 * AZURE executable, as well as the options specified in the command shell prompt.
 */

class Config {
 public:
  ///The runtime configuration file name.
  std::string configfile;
  ///A bitmask for the encoding of configuration flags
  unsigned int mask;
  ///If performError is true, sets the value of Up (the acceptable variance from the minimum chi-squared.
  double chiVariance;
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
  ///Parameters for calculating reaction rate.
  RateParams  rateParams;
  ///A constant indicating the maximum order of the Legendre polynomials to calculate.
  static const int maxLOrder=10;
};

extern int ReadConfigFile(Config&);
#ifndef NO_STAT
extern int CheckForInputFiles(Config&);
#endif

#endif
