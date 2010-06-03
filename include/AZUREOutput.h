#ifndef AZUREOUTPUT_H
#define AZUREOUTPUT_H

#include "AZUREFBuffer.h"
#include <vector>

///A class to assist in writing AZURE output files

/*!
 * The EData::WriteOutputFiles function simply loops over all ESegment and EPoint objects when
 * writing the output of an AZURE calculation. To ensure that all output for a given entrance and exit
 * pair combination is written to a single file, the AZUREOutput class is used.  The AZUREOutput object
 * is a container for a vector of AZUREFBuffer objects.
 */

class AZUREOutput {
 public:
  AZUREOutput(std::string);
  ~AZUREOutput();
  std::filebuf *operator()(int,int);
  int NumAZUREFBuffers() const;
  int IsAZUREFBuffer(int,int);
  std::string GetOutputDir() const;
  void AddAZUREFBuffer(AZUREFBuffer*);
  AZUREFBuffer *GetAZUREFBuffer(int);
 private:
  std::string outputdir_;
  std::vector<AZUREFBuffer*> azurefbuffers_;
};

#endif
