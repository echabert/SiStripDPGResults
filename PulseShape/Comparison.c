#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLine.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include "tdrstyle.C"

using namespace std;


TH1F* generateCode(TF1* pulse, float low, float high, float step, ofstream& ofile)
{
  unsigned int size =0;
  double end = high+step/2.;
  double maximum = pulse->GetMaximum(); 
  double maximumX = pulse->GetMaximumX(); 
  for(float val=low;val<end;val+=step) ++size;
  ofile << "float evaluate(float x) {" << std::endl;
  ofile << "  // Automatically generated using TkPulseShape::generateCode(low="
            << low << ", high=" << high << ", step="<< step << ")" << std::endl;
  ofile << "  float valuesArray[" << size << "] = { " ;
  
  TH1F* h = new TH1F("h","",size,low,high);
  
  size=0;
  for(float val=low;val<end;val+=step) {
    if(size) ofile << ", " ;
    ofile << pulse->Eval(val+maximumX)/maximum;
    h->SetBinContent(size+1,pulse->Eval(val+maximumX)/maximum);
    if(!((++size)%5)) ofile << std::endl << "                           ";
  }
  ofile << " };" << std::endl;
  ofile << "  if(x<("<<low<<")) return 0;" << std::endl;
  ofile << "  if(x>("<<high<<")) return 0;" << std::endl;
  ofile << "  return valuesArray[unsigned int(((x-("<<low<<"))/("<<step<<"))+0.5)];" << std::endl;
  ofile << "}" << std::endl;

  return h;
}



int main(int argc, char* argv[]){

  float low = -50;
  float high = 150;
  float step = 0.1;

  setTDRStyle();
 

  //reference
  ifstream ifile("input.txt");
  vector<double> values;
  while(!ifile.eof()){
    double val;
    ifile>>val;
    values.push_back(val);
  }
  const auto maxIt = std::max_element(values.begin(), values.end());
  int pulset0Idx = std::distance(values.begin(), maxIt);
  TH1F* href = new TH1F("href","",(high-low)/step+1,low,high);
  cout<<pulset0Idx<<endl;
  cout<<low/step<<endl;
  for(int i=0;i<values.size();i++){
    int ibin = i-pulset0Idx+low/step;
    //cout<<"ibin = "<<ibin<<endl;
    href->SetBinContent(i-pulset0Idx-low/step,values[i]);
  }
  href->SetLineColor(kOrange);
  href->SetLineWidth(2);

  TFile* fin = TFile::Open("pulse_res.root");
  TCanvas* c = new TCanvas("c");
  TLegend* leg = new TLegend(0.5,0.4,0.8,0.8);
  string entries[] = {"gate [20 ns]","gate [8 ns]","sig. 320 #mum","sig. 500 #mum"};
  TH1F** histos = new TH1F*[4];
  //for(int i=1;i<4;i++){
  for(int i=2;i<4;i++){
    for(int j=1;j<5;j++){
	   stringstream ss,ssh, ssf;
	   ss<<"fpulse_"<<i<<"_"<<j;
       ssh<<"h_"<<i<<"_"<<j;
       ssf<<"pulse_"<<i<<"_"<<j<<".txt";
       TF1* pulse = (TF1*) fin->Get(ss.str().c_str());
       ofstream ofile(ssf.str().c_str());
	   TH1F* h =  generateCode(pulse, low, high, step, ofile );
	   h->SetLineWidth(2);
       if(i==2) histos[j-1] = h;
       h->SetName(ssh.str().c_str());
       c->cd();
	   if(j==1) h->SetLineColor(kBlack);
	   if(j==2) h->SetLineColor(kGreen);
	   if(j==3) h->SetLineColor(kRed);
	   if(j==4) h->SetLineColor(kBlue);
       //cout<<entries[j-1]<<endl;
       if(i==2) leg->AddEntry(h,entries[j-1].c_str(),"l");
       if(i==2&&j==1){
           h->GetXaxis()->SetTitle("time [ns]");
           h->GetYaxis()->SetTitle("amplitude [U.A.]");
           h->Draw();
       }
       //else h->Draw("same");
       else if(i==2 || i==3) h->Draw("same");
     }
  }
  leg->AddEntry(href,"reference","l");
  href->Draw("same");
  leg->Draw("same");
  c->Print("c.png");
  c->Print("c.root");
  //TO BE UNCOMMENTED
  /*
  histos[0]->Divide(href);
  histos[1]->Divide(href);
  histos[2]->Divide(href);
  histos[3]->Divide(href);
  */
  /*
  for(int i=1;i<histos[0]->GetNbinsX();i++){
    double histo0 = histos[0]->GetBinContent(i);
    double histo1 = histos[1]->GetBinContent(i);
    double histo2 = histos[2]->GetBinContent(i);
    double histo3 = histos[3]->GetBinContent(i);
    histos[0]->SetBinContent(i,(histo0-histo3)/max(histo0,histo3));
    histos[1]->SetBinContent(i,(histo1-histo3)/max(histo1,histo3));
    histos[2]->SetBinContent(i,(histo2-histo3)/max(histo2,histo3));
  }
  */

  TCanvas* c2 = new TCanvas("c2");
  cout<<histos[0]->GetNbinsX()<<endl;
  cout<<href->GetNbinsX()<<endl;
  double ymin = 0.3;
  double ymax = 1.7;
  double xmax = 100;
  histos[0]->GetYaxis()->SetRangeUser(ymin,ymax);
  histos[0]->GetXaxis()->SetRangeUser(-50,xmax);
  histos[0]->GetXaxis()->SetTitle("time [ns]");
  histos[0]->GetYaxis()->SetTitle("ratio");
  histos[0]->Draw();
  histos[1]->Draw("same");
  histos[2]->Draw("same");
  histos[3]->Draw("same");
  TLegend* leg2 = new TLegend(0.6,0.7,0.85,0.85);
  leg2->AddEntry(histos[0],"gate [20 ns]","l");
  leg2->AddEntry(histos[1],"gate [ 8 ns]","l");
  leg2->AddEntry(histos[2],"sig. 320 #mum","l");
  leg2->AddEntry(histos[3],"sig. 500 #mum","l");
  leg2->Draw("same");
  //TLine* line = new TLine(c2->GetUxmin(),-3,c2->GetUxmax(),3);
  //TLine* line = new TLine(c2->GetUxmin(),0,c2->GetUxmax(),0);
  TLine* line = new TLine(-50,1,xmax,1);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->SetLineStyle(2);
  line->Draw("same");
  TLine* line25 = new TLine(25,ymin,25,ymax);
  line25->SetLineColor(kRed);
  line25->SetLineWidth(2);
  line25->SetLineStyle(2);
  line25->Draw("same");
  TLine* linem25 = new TLine(-25,ymin,-25,ymax);
  linem25->SetLineColor(kRed);
  linem25->SetLineWidth(2);
  linem25->SetLineStyle(2);
  linem25->Draw("same");
  c2->Print("ratio.png");
  c2->Print("ratio.root");

}
