#include <Minuit2/MnUserParameters.h>
#include <iostream>
#include <iomanip>
#include <fstream>

void WriteParameterErrors(const ROOT::Minuit2::MnUserParameters &params,const std::vector<std::pair<double,double> > &errors,std::string outdir) {
  char filename[256];;
  sprintf(filename,"%sparam.errors",outdir.c_str());
  std::ofstream out;
  out.open(filename);
  if(out) {
    out.precision(7);
    for(int i=0;i<params.Params().size();i++) {
      out << std::setw(20) << params.GetName(i) 
	  << std::scientific << std::setw(20) <<  params.Value(i) 
	  << std::scientific << std::setw(20) <<  fabs(errors[i].first) 
	  << std::scientific << std::setw(20) <<  fabs(errors[i].second) 
	  << std::endl;
    }
    out.flush();
    out.close();
  } else std::cout << "Could not save param.errors file." << std::endl;
}
