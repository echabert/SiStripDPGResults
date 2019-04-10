#ifndef __TKPULSESHAPE__ 
#define __TKPULSESHAPE__

#include <TF1.h>
#include <TMath.h>
#include "Math/Integrator.h"
#include <iostream>

/* New Pulse Shape Fit by G. Auzinger June 2017
following methods are used to describe a pulse shape for a 3 stage CR-CR-RC pre-amplifier (CR) 
+ shaper (CR + RC) with time constants x, y, z respectively
some special cases (x=0. y=z, x=0 && y=z) are considered (divergence of equation) and
the shaper CR y is approximated via a pol(6) of x which gives a resonable respresentation and
reduces the number of free parameters -- this shape is derived from first principle and can
be used to fit peak mode signals and deconvolution mode signals
following parameters are added in addition to the time constants x, z:
a_0 ... baseline offset amplitude
s   ... scale parameter
t_0 ... turn_on time of the pulse
par[5] ... scale parameter for the time slices in deco mode
*/


// ROOT script to generate fast and accurate pulse shape functions
// use (in ROOT interpreter) :
//.L parametrization.C+
// parametrizePulse deconv(1)
// parametrizePulse peak(2)
// deconv.generateCode(-30,35,0.1); > deconv.C
// peak.generateCode(-60,400,0.5); > peak.C

//bool debug = false;


//simple functions - used by TKPulsShape class

double pulse_raw(double x, double y, double z, double t);
double pulse_x0(double y, double z, double t);
double pulse_yz(double x, double z, double t);
double pulse_x0_yz(double z, double t);
double pulse(double x, double y, double z, double t);


class TkPulseShape {

  public:
    // constructor: takes the mode as input
    // mode 1 = deconvolution mode
    // mode 2 = peak mode
    // xmin and xmax are the range of the APV (convoluted) signal
    // PhySigxmin and Physigxmax correspond to the physical signal
    TkPulseShape(int fitMode = 1, double xmin = -50, double xmax = 100, double PhysSigxmin = 0, double PhysSigxmax = 6);
    ~TkPulseShape();
	
    TF1* getDeconvFitter() const;
	TF1* getPeakFitter() const;	
    // returns a pointer to the theoretical pulse function
    TF1* getTheoreticalPulse() const;
    // gives the correction factor for a given offset
    float getCorrectionFactor(float offset) const;
    // returns an array evaluated in a given range with a given step
    void getArray(float low, float high, float step);
    // generates code to evaluate the function
    void generateCode(float low, float high, float step);
  

    void setPhysSigParameters(double par1, double par2, double par3, double par4);
    void setAPVPeakParameters(double par, double par2, double par3, double par4, double par5, double par6);

	static double Signal(double* x, double* par);
	static double SignalTimesPeak(double* x, double* par);
	static double fpeak(double *x, double *par);
	static double fdeconv(double *x, double *par);
	static double fpeak_convoluted(double *x, double *par);
	static double fdeconv_convoluted(double *x, double *par);
  

  
   private:
     // members
     int fitMode_;
     int npx_;
     //range of the APV signal
     double xmin_;
     double xmax_;
     //range of the physical signal
     double PhysSigxmin_;
     double PhysSigxmax_;

     static TF1* f_PhysSig;
     static TF1* f_APVShape;
     static TF1* f_prod;
     TF1* deconv_fitter;
     TF1* peak_fitter;

};




#endif
