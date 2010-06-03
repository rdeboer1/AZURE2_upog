#include <Minuit2/MnUserParameters.h>
#include <iostream>
#include <iomanip>
#include <fstream>

void WriteUserParameters(const ROOT::Minuit2::MnUserParameters &params,std::string outdir, bool fitParameters) {
  char filename[256];;
  if(fitParameters) sprintf(filename,"%sparam.sav",outdir.c_str());
  else sprintf(filename,"%sparam.par",outdir.c_str());
  std::ofstream out;
  out.open(filename);
  if(out) {
    out.precision(7);
    for(int i=0;i<params.Params().size();i++) {
      out << std::setw(20) << params.GetName(i) 
	  << std::scientific << std::setw(20) <<  params.Value(i) 
	  << std::scientific << std::setw(20) <<  params.Error(i) << std::endl;
    }
    out.flush();
    out.close();
  } else std::cout << "Could not save param.par file." << std::endl;
}
