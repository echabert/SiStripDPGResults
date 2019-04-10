{

    gStyle->SetOptStat(0);
    TFile* f18 = TFile::Open("c_2018.root");
    TCanvas* c18 = (TCanvas*) f18->Get("c");
    TH1F* h18 = (TH1F*) c18->FindObject("h_2_3");
    
    TFile* f16 = TFile::Open("c_2016.root");
    TCanvas* c16 = (TCanvas*) f16->Get("c");
    TH1F* h16 = (TH1F*) c16->FindObject("h_2_3");
    h16->SetLineColor(kBlue);

    TLegend* leg = new TLegend(0.50,0.5,0.8,0.8);
    leg->AddEntry(h16,"2016","l");
    leg->AddEntry(h18,"2018","l");
    TCanvas* c = new TCanvas();
    h18->GetXaxis()->SetTitle("t [ns]");
    //h18->GetYaxis()->SetTitle("U.A.");
    h18->Draw();
    h16->Draw("same");
    leg->Draw("same");
    c->Print("c_16-18.png");
}


