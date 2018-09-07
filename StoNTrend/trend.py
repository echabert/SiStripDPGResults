import ROOT
from ROOT import TFile, TCanvas, TLatex, TLegend, TH1F, TH2F, TF1, TGraph
ROOT.gROOT.ProcessLine(".x setTDRStyle.C")
ROOT.gStyle.SetOptTitle(0)
import CMS_lumi, tdrstyle


titles = ["TIB","TOB","TID","TEC(thin)","TEC(thick)"]
colors = {"TIB":8,"TOB":2,"TID":30,"TEC(thin)":32,"TEC(thick)":46}
#colors = {"TIB":8,"TOB":2,"TID":8,"TEC(thin)":8,"TEC(thick)":2}
mstyles = {"TIB":20,"TOB":29,"TID":21,"TEC(thin)":22,"TEC(thick)":34}
StoNval = {}
StoNval["2015B"] =  	[18.2, 	23.9, 	17.0, 	18.5, 	23.3]
StoNval["2015D"] =  	[17.8, 	22.9, 	17.1, 	18.2, 	23.0] 
StoNval["2016H"] =	[17.2, 	22.5, 	16.7, 	18.0, 	22.5]
StoNval["2017A"] =	[17.5, 	22.9, 	17.0, 	18.3, 	22.7]
StoNval["2017C"] = 	[17.0, 	22.4, 	16.7, 	17.9, 	22.4] 
#StoNval["2018"] = 	[15.1, 	19.1, 	15.8, 	15.5, 	19.4]
StoNval["2018A"] = 	[16.5, 	21.7, 	16.0, 	16.9, 	21.8]
#	$2018 (run 316590) 	
print StoNval


run1lumi = 29.4
lumis = {"2015B":0.1,"2015D":4.23,"2016H":45.68,"2017A":45.83,"2017C":64.98,"2018A":112.17}


ofile = TFile("StoNTrend.root","RECREATE")
c = TCanvas("c")
c1 = TCanvas("c1")
leg = TLegend(0.25,0.15,0.55,0.45)
leg2 = TLegend(0.25,0.15,0.55,0.45)
first = True
#for key,value in StoNval.iteritems():
indice = 0
histos = []
graphs = []
c300 = 0
g300 = TGraph(len(StoNval)*3)
g300.SetName("g300")
g300.SetMarkerStyle(1)
c500 = 0
g500 = TGraph(len(StoNval)*2)
g500.SetName("g500")
g500.SetMarkerStyle(1)
for key in titles: 
#for key, color in colors.iteritems():
  color = colors[key]
  print first, key
  histo = TH1F("h_"+key,"",len(StoNval),0,len(StoNval))
  histo.SetLineColor(colors[key])
  histo.SetMarkerColor(colors[key])
  histo.SetLineWidth(2)
  histo.GetYaxis().SetRangeUser(10,26)
  histo.GetYaxis().SetTitleOffset(0.82)
  
  graph = TGraph(len(StoNval))
  graph.SetName("g_"+key)
  graph.SetLineColor(colors[key])
  graph.SetMarkerSize(1.5)
  graph.SetMarkerColor(colors[key])
  #if key == "TOB" or key == "TEC(thick)":  graph.SetMarkerStyle(21)
  #else:	graph.SetMarkerStyle(21)
  graph.SetMarkerStyle(mstyles[key])
  graph.GetYaxis().SetRangeUser(10,26)
  graph.GetYaxis().SetTitleOffset(0.82)
  graph.SetLineWidth(2)
  graph.GetYaxis().SetRangeUser(10,26)
  graph.GetYaxis().SetRange(10,26)
  graph.GetYaxis().SetTitleOffset(0.82)
  
  b = 1
  for key_era in sorted(StoNval.keys()):
  #for key, val in StoNval.iteritems():
    val = StoNval[key_era]
    histo.SetBinContent(b,val[indice])
    #histo.GetXaxis().SetBinLabel(b,"#splitline{"+key_era+"}{#int L = "+str(lumis[key_era])+"fb^{-1}}")
    histo.GetXaxis().SetBinLabel(b,"#splitline{"+key_era+"}{"+str(run1lumi+lumis[key_era])+" fb^{-1}}")
    histo.GetYaxis().SetTitle("Signal-To-Noise ratio")
     
    graph.SetPoint(b-1,run1lumi+lumis[key_era],val[indice])
    #graph.GetXaxis().SetBinLabel(b,"#splitline{"+key_era+"}{"+str(run1lumi+lumis[key_era])+" fb^{-1}}")
    graph.GetYaxis().SetTitle("Signal-To-Noise ratio")
    graph.GetXaxis().SetTitle("Integrated luminosity [fb^{-1}]")
    
    if key == "TIB" or key == "TID" or key == "TEC(thin)": 
    	g300.SetPoint(c300,run1lumi+lumis[key_era],val[indice])
    	c300+=1
    else:
    	g500.SetPoint(c500,run1lumi+lumis[key_era],val[indice])
    	c500+=1
    b+=1

  histos.append(histo)
  graphs.append(graph)
  leg.AddEntry(histo,key)
  leg2.AddEntry(graph,key,"p")
  if first: 
  	print "here"
	c.cd()
	histo.Draw()
	c1.cd()
	graph.Draw("AP")
  	first = False
  else: 
  	print "there"
	c.cd()
	histo.Draw('same')
	c1.cd()
	graph.Draw('Psame')
  indice+=1
  
  g300.Draw("Psame")
  g300.Fit("pol1")
  g300.GetFunction("pol1").SetLineColor(8)
  g500.Draw("Psame")
  g500.Fit("pol1")
  g500.GetFunction("pol1").SetLineColor(2)
  histo.Write()
  graph.Write()
  c.cd()
  leg.Draw("same")
  c1.cd()
  leg2.Draw("same")

#change the CMS_lumi variables (see CMS_lumi.py)
CMS_lumi.lumi_7TeV = "4.8 fb^{-1}"
CMS_lumi.lumi_8TeV = "18.3 fb^{-1}"
CMS_lumi.writeExtraText = 1
CMS_lumi.extraText = "Preliminary"
CMS_lumi.lumi_sqrtS = "13 TeV" # used with iPeriod = 0, e.g. for simulation-only plots (default is an empty string)

iPos = 0
if( iPos==0 ): CMS_lumi.relPosX = 0.12

# 
# Simple example of macro: plot with CMS name and lumi text
#  (this script does not pretend to work in all configurations)
# iPeriod = 1*(0/1 7 TeV) + 2*(0/1 8 TeV)  + 4*(0/1 13 TeV) 
# For instance: 
#               iPeriod = 3 means: 7 TeV + 8 TeV
#               iPeriod = 7 means: 7 TeV + 8 TeV + 13 TeV 
#               iPeriod = 0 means: free form (uses lumi_sqrtS)
# Initiated by: Gautier Hamel de Monchenault (Saclay)
# Translated in Python by: Joshua Hardenbrook (Princeton)
# Updated by:   Dinko Ferencek (Rutgers)
#

iPeriod = 0

#draw the lumi text on the canvas
CMS_lumi.CMS_lumi(c, iPeriod, iPos)
CMS_lumi.CMS_lumi(c1, iPeriod, iPos)


c.Print('StoNTrend_histo.png')
c.Print('StoNTrend_histo.pdf')
c1.Print('StoNTrend_graph.png')
c1.Print('StoNTrend_graph.pdf')

c.Write()
c1.Write()
ofile.Close()
