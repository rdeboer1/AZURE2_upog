#include "PairsTab.h"
#include "LevelsTab.h"
#include "SegmentsTab.h"
#include "TargetIntTab.h"
#include "RunTab.h"
#include "PlotTab.h"

std::vector<QString> setInfoStrings(QString infoString1 = "",
				    QString infoString2 = "",
				    QString infoString3 = "",
				    QString infoString4 = "",
				    QString infoString5 = "") {
  std::vector<QString> stringVector;
  if(infoString1!="") stringVector.push_back(infoString1);
  if(infoString2!="") stringVector.push_back(infoString2);
  if(infoString3!="") stringVector.push_back(infoString3);  
  if(infoString4!="") stringVector.push_back(infoString4);  
  if(infoString5!="") stringVector.push_back(infoString5);  
  return stringVector;
};

const std::vector<QString> PairsTab::infoText = 
  setInfoStrings("<p>This tab is used to define the particle pairs of the reaction. These are the two particles which fuse together to form or result from the decay of the compound nucleus. The theory is limited to two particle interactions. &gamma;-ray and &beta;-decays are currently only supported as decay pairs and must be distinguished by the different drop down selection when the pair is created. Since these decay pairs are specialized, some of the information fields are automatically filled in and may not be edited. Gamma ray decays are limited to bound states.</p>"
"<p>Use the &#43; or &#45; buttons on the lower left corner to add or delete a decay pair respectively.</p>"
"<p>When a particle pair is created it is assigned a number which is displayed on the left hand side of the particle pair row. These numbers are those referenced when creating segments on the Segments tab.</p>"
"<p>The tabs are meant to be filled out starting with this tab and moving to the left.  The information provided in this tab is the basis for several automatic calculations which are performed in the subsequent tabs, especially the Levels and Channels tab. If changes are later made to values in this tab after subsequent information has been filled out in other tabs, those changes will be applied automatically, possibly resulting in significant changes in the other tabs.</p>");

const std::vector<QString> LevelsTab::infoText = 
  setInfoStrings("<p>This tab is used to create the different excitation levels considered in the calculation. Levels can be created by clicking the &#43; button on the lower left corner. A level can be deleted by highlighting the level and then clicking the &#45; button. Levels may be above or below the particle separation energies. If a level is below a particle separation energy, the program automatically recognizes this and changes the label from a partial width to an ANC for the appropriate channels.</p>"
"<p>The energetically and momentum allowed reaction channels are determined automatically by the code for each level based on the information provided in the Particle Pair tab and the spin-party and energy of the entered levels. If a change is made to values in the Particle Pairs tab, these changes are automatically applied to the levels calculated in this tab.</p>"
"<p>Levels can be included (excluded) from the calculation by checking (unchecking) the box on the far left for each level. A level can have its energy fixed by checking the box marked under the Fix? column. Partial widths or ANCs can be fixed by checking the Fix? box under the Selected Channels window.</p>"
"<p>Gamma ray decays are limited to bound states.</p>");

const std::vector<QString> SegmentsTab::infoText = 
  setInfoStrings("Segments tab info.");

const std::vector<QString> TargetIntTab::infoText = 
  setInfoStrings("Experimental Effect tab info.");

const std::vector<QString> RunTab::infoText = 
  setInfoStrings("Run tab info.");

const std::vector<QString> PlotTab::infoText = 
  setInfoStrings("Plot tab info.");
