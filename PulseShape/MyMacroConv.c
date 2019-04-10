#include <string>
#include <vector>
#include <sstream>

#include "TStyle.h"
#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include <TApplication.h>

//#include "SiStripPulseShape.h"
//#include "parametrization.h"
#include "TkPulseShape.h"
#include "tdrstyle.C"

using namespace std;

//option 1: mean
//option 2: max
//option 3: median
float Max(TH1F* h, int option = 0){
  float mean = h->GetMean();
  float max = h->GetBinCenter(h->GetMaximumBin());
  //search the median with quantiles
  int nprobSum = 1;
  double q[] = {0};
  double probSum[] = {0.5};
  h->GetQuantiles(nprobSum,q,probSum);
  float med = q[0];
  if(option == 0) return mean;
  if(option == 1) return max;
  if(option == 2) return med;
  return mean;
}


//void MyMacro(){
int main(int argc, char** argv){
    int option  = 1;
    int physSig = 1;
    if(argc == 3){
        option = atoi(argv[1]);
        physSig = atoi(argv[2]);
        cout<<"option:  "<<option<<endl;
        cout<<"physSig: "<<physSig<<endl;
    }
    else{
      cerr<<"usage: ./exe option_param  option_phys_sig "<<endl; 
      cerr<<"option for parametrization: 1 (mean) - 2 (max) - 3 (med) "<<endl;
      cerr<<"option for physical signal: 1 (gate: 20 ns) - 2 (gate: 8 ns) - 3 (realistic sig. 320 microns) - 4 (realistic sig. 500 microns)"<<endl;
      return -1;
    }


    TApplication *myapp=new TApplication("myapp",0,0);
    
    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);
    //gStyle->SetBoxStat(0);

    setTDRStyle();
    
    //read results
//

//Only the fits from TP are used ... 
//
//string ifilename_Tp = "parameters_TP_324477.root"; //deconv
//string ifilename_Tp = "../parameters_TP_324489.root"; //peak
string ifilename_Tp = "../2016/parameters_TM_267211.root";
TFile* ifile_Tp = TFile::Open(ifilename_Tp.c_str());
//string ifilename_Tm = "parameters_TM_324476.root"; //deconv
string ifilename_Tm = "../parameters_TM_324490.root"; //peak
//string ifilename_Tm = "../2016/parameters_TM_267211.root";
TFile* ifile_Tm = TFile::Open(ifilename_Tm.c_str());

int nparams = 6;
double* params_Tp = new double[nparams];
double* params_Tm = new double[nparams];
TH1F** h_Tp = new TH1F*[nparams];
TH1F** h_Tm = new TH1F*[nparams];
for(int i=0;i<nparams;i++){
  stringstream ss;
  ss<<"param";
  ss<<i;
  h_Tp[i] = (TH1F*) ifile_Tp->Get(ss.str().c_str());
  h_Tm[i] = (TH1F*) ifile_Tm->Get(ss.str().c_str());
  cout<<"param["<<i<<"]="<<Max(h_Tp[i],option)<<endl;
  params_Tp[i] = Max(h_Tp[i],option);
  params_Tm[i] = Max(h_Tm[i],option);
  cout<<params_Tp[i]<<" "<<params_Tm[i]<<endl;
}



TkPulseShape pulse(1,-50,200);
TF1* fpulse = pulse.getTheoreticalPulse();
pulse.setAPVPeakParameters( params_Tp[0], params_Tp[1],params_Tp[2],params_Tp[3],params_Tp[4],params_Tp[5]);
if(physSig==1) pulse.setPhysSigParameters(1./20,19.99,1./20,20);
if(physSig==2) pulse.setPhysSigParameters(1./8,7.99,1./8,8);
if(physSig==3) pulse.setPhysSigParameters(1.5,4.4,0.25,8);
if(physSig==4) pulse.setPhysSigParameters(1.2,4.2,0.2,17);
fpulse->Draw();
stringstream ss;
ss<<"fpulse_"<<option<<"_"<<physSig;
fpulse->SetName(ss.str().c_str());

TFile* ofile = new TFile("pulse_res.root","UPDATE");
fpulse->Write();
ofile->Close();

//myapp->Run();
return 0;




}


