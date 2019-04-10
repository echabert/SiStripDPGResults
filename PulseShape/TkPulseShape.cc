#include <TF1.h>
#include <TMath.h>
#include "Math/Integrator.h"
#include "TkPulseShape.h"

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


TF1* TkPulseShape::f_PhysSig;
TF1* TkPulseShape::f_APVShape;
TF1* TkPulseShape::f_prod;

double pulse_raw(double x, double y, double z, double t){

  double result1, result2, result3;

  result1 = z * y * exp(-t / y);
  result1/= pow(y,2) - (x + z) * y + z * x;

  result2 = z * x * exp(-t / x);
  result2/= pow(x,2) - (x - z) * y - z * x;

  result3 = pow(z,2) * exp(-t / z);
  result3/= pow(z,2) + (x - z) * y - z * x;

  return result1 + result2 + result3;
}

double pulse_x0(double y, double z, double t){
  return z / (y - z) * (exp(-t / y) - exp(-t / z));
}

double pulse_yz(double x, double z, double t){
  double result1, result2;

  result1 = exp(-t / x) - exp(-t / z);
  result1*= z * x / (z - x);

  result2 = t * exp(-t / z);

  return (result1 + result2) / (z - x);
}

double pulse_x0_yz(double z, double t){
  return t / z * exp(-t / z);
}

double pulse(double x, double y, double z, double t){

  if(x > y){
    double pivot = x;
    x = y;
    y = pivot;
  }

  if((x == 0) && (y == z)) return pulse_x0_yz(z, t);
  else if(x == 0) return pulse_x0(y, z, t);
  else if(y == z) return pulse_yz(x, z, t);

  else return pulse_raw(x, y, z, t);
}



double TkPulseShape::Signal(double* x, double* par){
   double y0 = par[0];
   double x1 = par[1];
   double y1 = par[2];
   double x0 = par[3];
   //cout<<x[0]<<endl;
   //std::cout<<"PhysSig - parameters: "<<par[0]<<" "<<par[1]<<" "<<par[2]<<" "<<par[3]<<std::endl;
   if(x[0]<0) return 0;
   if(x[0]>x0) return 0;
   if(x[0]<x1){
      return (y1-y0)/x1*x[0]+y0;
   }
   else{
       return (-y1)/(x0-x1)*x[0]+(y1+y1/(x0-x1)*x1);
   }
   return 0;
}


double TkPulseShape::SignalTimesPeak(double* x, double* par){
    //std::cout<<"x[0] = "<<x[0]<<std::endl;
    //std::cout<<"f_PhysSig->Eval(x[0]) = "<<f_PhysSig->Eval(x[0])<<" f_APVShape->Eval(par[0]-x[0]) = "<<f_APVShape->Eval(par[0]-x[0])<<std::endl;
    return f_PhysSig->Eval(x[0])*f_APVShape->Eval(par[0]-x[0]); 
}

double TkPulseShape::fpeak(double *x, double *par){
  double xx  = par[0];
  double y   = par[1];
  double z   = par[2];
  double a_0 = par[3];
  double s   = par[4];
  double t_0 = par[5];
  double t   = x[0] - t_0;

  //if( debug) std::cout<<par[0]<<" "<<par[1]<<" "<<par[2]<<" "<<par[3]<<" "<<par[4]<<" "<<par[5]<<std::endl;
  if(x[0] < t_0) return a_0;
  return a_0 + s * pulse(xx, y, z, t);
}

double TkPulseShape::fdeconv(double *x, double *par){
  double xm = par[6]*(x[0]-25);
  double xp = par[6]*(x[0]+25);
  double xz = par[6]*x[0];
  return 1.2131 * fpeak(&xp,par) - 1.4715 * fpeak(&xz,par) + 0.4463 * fpeak(&xm,par);
}


Double_t TkPulseShape::fpeak_convoluted(Double_t *x, Double_t *par)
{
 f_prod->SetParameter(0, x[0]);
 //ROOT::Math::IntegratorOneDim ig(ROOT::Math::IntegrationOneDim::kADAPTIVE,0.01,0.01); 
 ROOT::Math::IntegratorOneDim ig(ROOT::Math::IntegrationOneDim::kADAPTIVE);
 ig.SetFunction(*f_prod); 
 return ig.Integral(par[0],par[1]);
}

Double_t TkPulseShape::fdeconv_convoluted(Double_t *x, Double_t *par)
{
  Double_t xm = (x[0]-25);
  Double_t xp = (x[0]+25);
  Double_t xz = x[0];
  return 1.2131*TkPulseShape::fpeak_convoluted(&xp,par)-1.4715*TkPulseShape::fpeak_convoluted(&xz,par)+0.4463*TkPulseShape::fpeak_convoluted(&xm,par);
}


TF1* TkPulseShape::getDeconvFitter() const { return deconv_fitter; }

TF1* TkPulseShape::getPeakFitter() const { return peak_fitter; }



void TkPulseShape::setPhysSigParameters(double par1, double par2, double par3, double par4){
  f_PhysSig->SetParameters(par1,par2,par3,par4);
  PhysSigxmax_ = par4;
  peak_fitter->SetParameter(1,PhysSigxmax_);
  deconv_fitter->SetParameter(1,PhysSigxmax_);
}

void TkPulseShape::setAPVPeakParameters(double par1, double par2, double par3, double par4, double par5, double par6){
    f_APVShape->SetParameters(par1,par2,par3,par4,par5,par6);
}

TkPulseShape::TkPulseShape(int fitMode, double xmin, double xmax, double PhysSigxmin, double PhysSigxmax) 
{
  fitMode_ = fitMode; // 1 = deconvolution, 2 = peak
  xmin_ = xmin;
  xmax_ = xmax;
  PhysSigxmin_ = PhysSigxmin;
  PhysSigxmax_ = PhysSigxmax;
  npx_ = 2000;
  //TF1 pointers
  f_PhysSig = new TF1("f_PhysSig",Signal,xmin_,xmax_,4);
  //filled with default values
  f_PhysSig->SetParameter(0,1.4);
  f_PhysSig->SetParameter(1,4.2);
  f_PhysSig->SetParameter(2,1.4);
  f_PhysSig->SetParameter(3,PhysSigxmax_);
  f_PhysSig->SetNpx(npx_);
  //filled with default values
  f_APVShape = new TF1("f_APVShape",fpeak,xmin_,xmax_,6);
  f_APVShape->SetParameters(13.5,31,43.4,2.25,430,24);
  f_APVShape->SetNpx(npx_);
  //filled
  f_prod = new TF1("f_prod",SignalTimesPeak,xmin_,xmax_,1); 
  f_prod->SetNpx(npx_);
  //filled
  deconv_fitter = new TF1("deconv_fitter",fdeconv_convoluted,xmin_,xmax_,2);
  deconv_fitter->SetParameters(PhysSigxmin_,PhysSigxmax_);
  deconv_fitter->SetNpx(npx_);
  peak_fitter = new TF1("peak_fitter",fpeak_convoluted,xmin_,xmax_,2);
  peak_fitter->SetParameters(PhysSigxmin_,PhysSigxmax_);
  peak_fitter->SetNpx(npx_);

}

TkPulseShape::~TkPulseShape() 
{
  delete f_PhysSig;
  delete f_APVShape;
  delete f_prod;
  delete deconv_fitter;
  delete peak_fitter;
}

float TkPulseShape::getCorrectionFactor(float offset) const {
    TF1* pulse = getTheoreticalPulse();
    if(fitMode_ == 1) { return 1./pulse->Eval(offset); }
    else { return 1./pulse->Eval(100+offset); }
}

TF1* TkPulseShape::getTheoreticalPulse() const { 
    return (fitMode_ == 1) ?  deconv_fitter: peak_fitter;
}

void TkPulseShape::getArray(float low, float high, float step)
{
  TF1* pulse = getTheoreticalPulse();
  double end = high+step/2.;
  double maximum = pulse->GetMaximum(); 
  for(float val=low;val<end;val+=step) {
    std::cout << "(" << val << ", " << pulse->Eval(val)/maximum << ")" << std::endl;
  }
}

void TkPulseShape::generateCode(float low, float high, float step)
{
  unsigned int size =0;
  TF1* pulse = getTheoreticalPulse();
  double end = high+step/2.;
  double maximum = pulse->GetMaximum(); 
  double maximumX = pulse->GetMaximumX(); 
  for(float val=low;val<end;val+=step) ++size;
  std::cout << "float evaluate(float x) {" << std::endl;
  std::cout << "  // Automatically generated using TkPulseShape::generateCode(low="
            << low << ", high=" << high << ", step="<< step << ")" << std::endl;
  std::cout << "  float valuesArray[" << size << "] = { " ;
  size=0;
  for(float val=low;val<end;val+=step) {
    if(size) std::cout << ", " ;
    std::cout << pulse->Eval(val+maximumX)/maximum;
    if(!((++size)%5)) std::cout << std::endl << "                           ";
  }
  std::cout << " };" << std::endl;
  std::cout << "  if(x<("<<low<<")) return 0;" << std::endl;
  std::cout << "  if(x>("<<high<<")) return 0;" << std::endl;
  std::cout << "  return valuesArray[unsigned int(((x-("<<low<<"))/("<<step<<"))+0.5)];" << std::endl;
  std::cout << "}" << std::endl;
}
