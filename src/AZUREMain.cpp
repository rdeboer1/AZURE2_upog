#include "AZURECalc.h"
#include "AZUREMain.h"
#include "AZUREParams.h"
#include "Config.h"
#include "ReactionRate.h"

int AZUREMain::operator()(){
  //Fill compound nucleus from nucfile
  std::cout << "Filling Compound Nucleus..." << std::endl;
  if(compound()->Fill(configure())==-1) {
    std::cout << "Could not fill compound nucleus from file." 
	      << std::endl;
    return -1;
  }
  if(configure().checknucleus=="screen"|| configure().checknucleus=="file") compound()->PrintNuc(configure());

  if(!configure().calcRate) {
    //Fill the data object from the segments and data file
    //  Compound object is passed to the function for pair key verification and
    //  center of mass conversions, s-factor conversions, etc.
    std::cout << "Filling Data Structures..." << std::endl;
    if(configure().withData) {
      if(data()->Fill(configure(),compound())==-1) {
	std::cout << "Could not fill data object from file." << std::endl;
	return -1;
      }
    } else {
      if(data()->MakePoints(configure(),compound())==-1) {
	std::cout << "Could not fill data object from file." << std::endl;
	return -1;
      }
    } 
    if(configure().checkdata=="screen"||configure().checkdata=="file") data()->PrintData(configure());
  } else {
    if(!compound()->IsPairKey(configure().rateParams.entrancePair)||!compound()->IsPairKey(configure().rateParams.exitPair)) {
      std::cout << "Reaction rate pairs do not exist in compound nucleus." 
		<< configure().rateParams.entrancePair << configure().rateParams.exitPair << std::endl;
      return -1;
    } else {
      compound()->GetPair(compound()->GetPairNumFromKey(configure().rateParams.entrancePair))->SetEntrance();
    }
  }

  //Initialize compound nucleus object
  compound()->Initialize(configure());
 
  //Create new parameters for minuit, fill them from compound nucleus object and data file.
  AZUREParams params;
  compound()->FillMnParams(params.GetMinuitParams());
  data()->FillMnParams(params.GetMinuitParams());
  if(!configure().oldParameters) {
    std::cout << "Creating New param.par File..." << std::endl;
    params.WriteUserParameters(configure().outputdir,false);
  } else {
    std::cout << "Reading User Parameter File..." << std::endl;
    params.ReadUserParameters(configure().paramfile);
  }

  if(!configure().calcRate) {
    //Initialize data object
    data()->Initialize(compound(),configure());
  
    //Declare a new instance of FCNBase
    AZURECalc theFunc(data(),compound(),configure());
    theFunc.SetErrorDef(1.0);
    
    if(configure().performFit) {
      //Call Minuit for function minimization, write minimized parameters to params
      if(configure().isAMatrix) std::cout << "Performing A-Matrix Fit..." << std::endl; 
      else std::cout << "Performing R-Matrix Fit..." << std::endl;
      data()->SetFit(true);
      ROOT::Minuit2::MnMigrad migrad(theFunc,params.GetMinuitParams());
      ROOT::Minuit2::FunctionMinimum min=migrad();
      if(configure().performError) {
	std::cout << std::endl 
		  << "Performing parameter error analysis with Up=" <<  configure().chiVariance << "." << std::endl;
	data()->SetErrorAnalysis(true);
	theFunc.SetErrorDef(configure().chiVariance);
	ROOT::Minuit2::MnMinos minos(theFunc,min);
	std::vector<std::pair<double,double> > errors;
	for(int i = 0; i<params.GetMinuitParams().Params().size(); i++) { 
	  std::cout << "\tParameter " << i+1 << "..." << std::endl;
	  if(!params.GetMinuitParams().Parameter(i).IsFixed()) {
	    std::pair< double, double > error=minos(i);
	    errors.push_back(error);
	  } else errors.push_back(std::pair< double, double > (0.,0.));
	}
	params.WriteParameterErrors(errors,configure().outputdir);
      }
      params.GetMinuitParams()=min.UserParameters();
      params.WriteUserParameters(configure().outputdir,true);
    } else {
      if(configure().isAMatrix) std::cout << "Performing A-Matrix Calculation..." << std::endl; 
      else std::cout << "Performing R-Matrix Calculation..." << std::endl; 
    }
    data()->SetFit(false);
    data()->SetErrorAnalysis(false);
    double chiSquared=theFunc(params.GetMinuitParams().Params());
    if(configure().withData) {
      std::cout << std::endl << std::endl;
      for(ESegmentIterator segment=data()->GetSegments().begin();
	  segment<data()->GetSegments().end();segment++) 
	std::cout << "Segment #"
		  << segment->GetSegmentKey() 
		  << " Chi-Squared/N: "
		  << segment->GetSegmentChiSquared()/segment->NumPoints()
		  << std::endl;
      std::cout << "Total Chi-Squared: " 
		<< chiSquared << std::endl << std::endl;
    }

    //Write Output Files
    std::cout << "Writing output files..." << std::endl;
    data()->WriteOutputFiles(configure());
  } else {
    //Calculate Reaction Rate
    // This uses the adaptive integration routines of GSL.  As the energy stepsize is 
    // unknown until integration, AZURE is called not in segment control mode but as 
    // an energy dependent function.  As every data point must be reinitialized (new energy 
    // dependent terms calculated) the routine is slow.  This should be a tradeoff 
    // for good accuracy.
    std::cout << "Performing reaction rate calculation..." << std::endl;
    ReactionRate reactionRate(compound(),params.GetMinuitParams().Params(),configure(),
			      configure().rateParams.entrancePair,configure().rateParams.exitPair);
    reactionRate.CalculateRates(configure().rateParams.minTemp,configure().rateParams.maxTemp,configure().rateParams.tempStep);
    reactionRate.WriteRates();
  }

  std::cout << "Performing final parameter transformation..." << std::endl;
  compound()->TransformOut(configure());
  compound()->PrintTransformParams(configure().outputdir);

  return 0;
}
