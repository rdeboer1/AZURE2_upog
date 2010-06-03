#ifndef AZUREFBUFFER_H
#define AZUREFBUFFER_H

#include <fstream>
#include <string>
#include <assert.h>

///A container class for a pointer to a file buffer

/*!
 * The AZUREFBuffer class contains a pointer to an acutal file buffer, as well as the entrance
 * and exit pair keys to which the file buffer corresponds.  
 */

class AZUREFBuffer {
 public:
  /*!
   * The AZUREFBuffer object is created with an entrance and exit pair key, as well as an output directory.
   * The filename is determined, and a file buffer is created with that filename.
   */
  AZUREFBuffer(int entranceKey,int exitKey,std::string outputdir) {
    char filename[256];
    entrancekey_=entranceKey;
    exitkey_=exitKey;
    sprintf(filename,"%sAZUREOut_aa=%d_R=%d.out",outputdir.c_str(),entranceKey,exitKey);
    fbuffer_=new std::filebuf;
    fbuffer_->open(filename,std::ios::out);
    assert(fbuffer_->is_open());
  };
  /*!
   * The file buffer is closed and destroyed with the instance of AZUREFBuffer.
   */
  ~AZUREFBuffer() {
    fbuffer_->close();
    delete fbuffer_;
  };
  /*!
   * Returns the entrance pair key of the object.
   */
  int GetEntranceKey() const {return entrancekey_;};
  /*!
   * Returns the exit pair key of the object.
   */
  int GetExitKey() const {return exitkey_;}
  /*!
   * Returns a pointer to the corresponding file buffer.
   */
  std::filebuf *GetFBuffer() {return fbuffer_;};
 private:
  int entrancekey_;
  int exitkey_;
  std::filebuf *fbuffer_;
};

#endif
