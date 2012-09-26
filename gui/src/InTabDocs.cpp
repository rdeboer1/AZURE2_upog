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
  setInfoStrings("<ul><li><p>This tab is used to define the particle pairs of the reaction. These are the two particles which fuse together to form or result from the decay of the compound nucleus. The theory is limited to two particle interactions. &gamma;-ray and &beta;-decays are currently only supported as decay pairs and must be distinguished by the different drop down selection when the pair is created. Since these decay pairs are specialized, some of the information fields are automatically filled in and may not be edited. &gamma;-ray decays are limited to bound states.</p></ul>"
"<ul><li><p>Use the &#43; or &#45; buttons on the lower left corner to add or delete a decay pair respectively.</p></ul>"
"<ul><li><p>When a particle pair is created it is assigned a number which is displayed on the left hand side of the particle pair row. These numbers are those referenced when creating segments on the <i>Segments</i> tab.</p></ul>"
"<ul><li><p>The tabs are meant to be filled out starting with this tab and moving to the left.  The information provided in this tab is the basis for several automatic calculations which are performed in the subsequent tabs, especially the <i>Levels and Channels</i> tab. If changes are later made to values in this tab after subsequent information has been filled out in other tabs, those changes will be applied automatically, possibly resulting in significant changes in the other tabs.</p></ul>");

const std::vector<QString> LevelsTab::infoText = 
  setInfoStrings("<ul><li><p>This tab is used to create the different excitation levels considered in the calculation. Levels can be created by clicking the &#43; button on the lower left corner. A level can be deleted by highlighting the level and then clicking the &#45; button. Levels may be above or below the particle separation energies. If a level is below a particle separation energy, the program automatically recognizes this and changes the label from a partial width to an ANC for the appropriate channels.</p></ul>"
"<ul><li><p>The energetically and momentum allowed reaction channels are determined automatically by the code for each level based on the information provided in the <i>Particle Pairs</i> tab and the spin-party and energy of the entered levels. If a change is made to values in the <i>Particle Pairs</i> tab, these changes are automatically applied to the levels calculated in this tab.</p></ul>"
"<ul><li><p>Levels can be included (excluded) from the calculation by checking (unchecking) the box on the far left for each level. A level can have its energy fixed by checking the box marked under the <b>Fix?</b> column. Partial widths or ANCs can be fixed by checking the <b>Fix?</b> box under the Selected Channels window.</p></ul>"
"<ul><li><p>&gamma;-ray decays are limited to bound states.</p></ul>"
"<ul><li><p>Relative interferences for channels can be specified by changing the sign of the corresponding partial width or ANC.</p></ul>"
"<ul><li><p>As one of the general features of <i>R</i>-matrix theory, the number of levels must be truncated both in number and in spin parity to some finite amount. One result of this is an ambiguity in the number of angular momentum terms which are summed in the hard sphere phase terms which are included in scattering and in the external capture calculations. To define these angular momenta, the code only uses those which are present in the levels included in the calculation. The user may therefore have to include dummy levels in the calculation. These dummy levels simply need to be created at an arbitrary energy with the spin-parities that are not present in the real levels under consideration. The energy can be fixed and the partial widths may be set to zero. These dummy levels can often just be the background pole terms, one of which should be present for each allowed spin parity combination, but in this case the partial widths should be allowed to vary.</p></ul>");

const std::vector<QString> SegmentsTab::infoText = 
  setInfoStrings("<ul><li><p>This tab is used to define the different data sets which will be considered in the minimization analysis and to define regions for pure calculations.</p></ul>"
"<ul><li><p>In the upper half of the tab, the data which will be considered when a <b>Calculate Segments With Data</b> option is executed under the <i>Calculate</i> tab are designated by assigning data segments. A data segments are also used as a convenient way to sort the input data so that it may be more easily viewed graphically on the <i>Plot</i> tab. A data set is identified with a reaction by assigning an entrance and exit pair using the corresponding numbers assigned to each particle pair in the <i>Particle Pairs</i> tab. The four allowed data types are Angle Integrated cross section, Differential cross section, Phase Shifts, and Angle Integrated Total cross sections. High and Low values of energy and angle are used to selected data which is inclusive between the ranges which are defined. For example, to plot an excitation curve for differential cross section data, a range of energy can be entered and a single angle can be specified by giving the same angle in the Low Angle and High Angle boxes. On execution, the program then looks in the specified data file and pulls only those data points which meet the specified ranges.</p></ul>"
"<ul><li><p>In the lower half the tab segments can be made which are used when the <b>Calculate Segments Without Data</b> option is executed under the <i>Calculate</i> tab. These segments define regions in energy or angle where a pure calculation is made. The energy and angle inputs now represent the upper and lower energy which will be calculated. Additional energy and angle step sizes must also be defined in order to specify the spacing of the calculation. These segments may also be used to extract angular distribution coefficents.</p></ul>"
"<ul><li><p>Like the particle pairs under the <i>Particle Pairs</i> tab, each segment is also assigned a numerical value. These values can be referenced in the <i>Experimental Effects</i> tab.</p></ul>"
"<ul><li><p>Data files should be created as text files with four columns of space or tab delimitation. The four columns are ordered from left to right as energy, angle, cross section, cross section uncertainty. The frame of reference for data files is the laboratory system in normal kinematics (i.e. light particle projectile, heavy particle target).</p></ul>"
"<ul><li><p>Experimental uncertainties are often categorized into systematic and statistical. The statistical uncertified are often different for each data point and these are the uncertainties assumed to be provided in the data file. Systematic uncertainties often apply to a data set as a whole. The percent systematic uncertainty for a data segment can be specified when creating the segment and the <b>Vary Norm?</b> box should be checked. For convenience a normalization can be applied to the cross section. The percent uncertainty is then taken relative to this normalization.</p></ul>");

const std::vector<QString> TargetIntTab::infoText = 
  setInfoStrings("<ul><li><p>This tab is used to apply experimental effect corrections to input data. Often experimentally reported cross sections still retain some effects due to beam energy loss in targets or geometric effects of the setup.</p></ul>"
"<ul><li><p>Modeling of experimental effects including Gaussian energy convolution, energy loss using stopping cross section curves, and geometrical attenuation coefficients are all available. These parameters characterizing these corrections can be specified in an experimental effect and that experimental effect can then be associated with a segment created in the <i>Segments</i> tab using the segment numbers. When creating an experimental effect, segment numbers can be entered into the box labeled <b>Segments List</b>. These numbers can be comma delimited or a range can be specified by putting the lower segment number followed by a dash and then the upper segment number.</p></ul>"
"<ul><li><p>Examples: ``5, 8, 13'' and ``4-12'' and ``3,6,7-14'' are all valid. </p></ul>");

const std::vector<QString> RunTab::infoText = 
  setInfoStrings("<ul><li><p>This tab controls the execution of the code. The code offers several modes of operation including <b>Calculate Segments From Data</b>, <b>Fit Segments From Data</b>, <b>Calculate Segments Without Data</b>, <b>Perform MINOS Error Analysis</b>, and <b>Calculate Reaction Rate</b> which can be selected from the drop down menu labeled <b>Calculation Type</b>.</p></ul>"
"<ul><li><p>The code operates in the following way. When a new calculation is first performed, the starting parameter values must be taken from those entered using the proceeding tabs. Therefore, for a new calculation the Create New Parameters option must always be selected. If external capture calculations are necessary, the Create new Integrals File option must also be selected.  For initial calculations the <b>Calculate Segments From Data</b> option should be selected. This will only make a calculation based on the input parameters that are initially given. This option is useful for preliminary testing and to adjust initial parameters by hand in preparation for a fit.</p></ul>"
"<ul><li><p>The integrations necessary for the external capture calculations can be quite time consuming. For this reason their results are stored in a file called <code>ECint.dat</code>. This file needs to only be created once and then subsequent calculations can be made using the results by selecting the Use option and then selecting the <code>ECint.dat</code> file from the user's specified output directory. However, if the input data, the channel radius, or the experimental effects are modified this file needs to be recalculated.</p></ul>"
"<ul><li><p>After a fit has been performed using the <b>Fit Segments From Data</b> mode, the final <i>R</i>-matrix parameters are stored in the files <code>param.sav</code> and <code>normalizations.out</code>.  By selecting the <code>param.sav</code> file from the working output directory the fit can then be reproduced. If the <code>param.sav</code> file is selected the <code>normalizations.out</code> file is automatically selected from the same directory. In this way the results from the fit can be used to make extrapolations using the <b>Calculate Segments Without Data</b> option or calculate the resulting reaction rate using <b>Calculate Reaction Rate</b>.</p></ul>"
"<ul><li><p>The observable parameters resulting from the fit are output in the file <code>parameters.out</code>. If the user wishes to use these parameters as new starting values for a fit they must be copied over by hand.</p></ul>"
"<ul><li><p>Results of the MINOS uncertainty analysis are output in the files <code>param.errors</code> and <code>covariance_matrix.out</code>.</p></ul>"
"<ul><li><p>Results of the reaction rate calculation are output in <code>reactionrates.out</code>.</p></ul>");

const std::vector<QString> PlotTab::infoText = 
  setInfoStrings("<ul><li><p>This tab is used to plot the cross sections (or corresponding S-factors). A segment can be plotted by clicking on a segment from the list and then clicking the <b>Draw</b> button in the lower left corner. The segment is unselected by clicking on the segment a second time. Multiple segments may be plotted at once by clicking on each segment a single time and then clicking the <b>Draw</b> button. A segment is selected when the background color changes.</p></ul>");