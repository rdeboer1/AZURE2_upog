#include <Minuit2/MnUserParameters.h>
#include <iostream>
#include <fstream>

void ReadUserParameters(ROOT::Minuit2::MnUserParameters &params,std::string infile) {
  std::vector<std::string> names;
  std::vector<double> values;
  std::vector<double> errors;
  std::vector<bool> fixed;
  std::string tempname,tempfixed,tempfixed_nows;
  double tempvalue,temperror;

  std::ifstream in;
  in.open(infile.c_str());
  if(in) {
    while(!in.eof()) {
      in >> tempname >> tempvalue >> temperror; getline(in,tempfixed);
      if(!in.eof()) {
	names.push_back(tempname);
	values.push_back(tempvalue);
	errors.push_back(temperror);
	tempfixed_nows.clear();
	for(int i=0;i<tempfixed.length();i++) 
	  if(tempfixed[i]!=' '&&tempfixed[i]!='\t') 
	    tempfixed_nows.push_back(tempfixed[i]);
	if(tempfixed_nows=="fixed") fixed.push_back(true);
	else fixed.push_back(false);
      }
    }
    in.close();
  } else std::cout << "Could not read user parameter file." << std::endl;
  
  for(int i=0;i<params.Params().size();i++) {
    for(int ii=0;ii<names.size();ii++) {
      if(params.GetName(i)==names[ii]) {
	if(params.Value(i)==0.0&&values[ii]!=0.0&&
	   params.Parameter(i).IsFixed()) params.Release(i);
	if(params.Value(i)!=0.0&&values[ii]==0.0&&
	   !params.Parameter(i).IsFixed()) params.Fix(i);
	params.SetValue(i,values[ii]);
	params.SetError(i,errors[ii]);
	if(fixed[ii]&&!params.Parameter(i).IsFixed()) params.Fix(i);
      }
    }
  }
}
