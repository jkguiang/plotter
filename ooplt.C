//Run this script to parse data and save histograms in "histograms.root"

//Root includes
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

//C++ includes
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class plotter{
    
    // Member Variables
    TFile* f_data;
    TFile* f_mc;
    TH1F* data;
    TH1F* mc;

    double sf; // Scaling factor
    int c_width; // Canvas width
    int c_height; // Canvas height

    TString save_ext;
    TString save_path;

    // Plot Specific Variables
    bool legend_bool = false;

    public:
        // Overload Constructor
        plotter(TFile*, TFile*);

        // Plotting Functions
        void main_plot(TString, TString, int, int, TString); // Plots histograms with preset options
        void adv_plot(TString, TString, int, int); // Advanced plot, with ratio plot of data vs mc
        void plot_data(TString, TString, int, int, TString); // Just plots data, plot must be customized manually
        void plot_mc(TString, TString, int, int, TString); // Just plots monte carlo, plot must be customized manually

        // Getters
        TString get_xLabel(TString);
        double get_scaleFactor(int, int);

        // Setters
        void set_canvasSize(int, int);
        void set_scaleFactor(double);
        void set_saveExtension(TString);
        void set_savePath(TString);

        // Plot Specific Setters
        void set_barsLegend(bool); // true = on, false = off

};

plotter::plotter(TFile* new_fdata, TFile* new_fmc){

    f_data = new_fdata;
    f_mc = new_fmc;

    sf = get_scaleFactor(38, 53);
    c_width = 1200;
    c_height = 900;

    save_ext = ".png";
    save_path = "/home/users/jguiang/public_html/zpeak/static/";
}

// Plotting Functions
void plotter::main_plot(TString ref, TString title, int xmin, int xmax, TString plt_typ){

    TCanvas *C = new TCanvas(ref, ref, c_width, c_height);
    data = (TH1F*)f_data->Get(ref)->Clone("data");
    mc = (TH1F*)f_mc->Get(ref)->Clone("mc");

    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);

    mc->GetXaxis()->SetTitle(get_xLabel(ref));
    mc->GetXaxis()->SetRangeUser(xmin, xmax);
    mc->SetTitle(title);

    // Ensure that first plot accomodates for maximum of second plot
    mc->SetMaximum(max(mc->GetMaximum(), data->GetMaximum()));

    // Normalizing
    mc->Scale(sf);

    // Default plot: MC -> blue solid fill, Data -> black markers w/ errorbars
    if (plt_typ == "DEFAULT"){

        mc->SetFillColor(38);
        mc->SetLineColor(kBlue);
        mc->SetLineWidth(2);
        data->SetMarkerStyle(kFullCircle);
        data->SetLineColor(kBlack);
        data->SetLineWidth(2);

        mc->Draw("HIST");
        data->Draw("E1 SAME");
    }    

    // Plot more suited towards plots w/ large bin sizes relative to dataset (i.e. jets plot)
    if (plt_typ == "BARS"){

        C->SetLogy(1);

        mc->SetFillColor(38);
        mc->SetLineColor(kBlue);
        mc->SetLineWidth(2);
        data->SetFillStyle(3002);
        data->SetFillColor(8);
        data->SetLineColor(kGreen);
        data->SetLineWidth(2);

        mc->Draw("HIST");
        data->Draw("HIST SAME");
            
    }

    // Legend
    if (legend_bool == true){
        auto legend = new TLegend(0.9,0.9,0.6,0.6);
        legend->SetHeader("Legend","C"); // option "C" allows to center the header
        legend->AddEntry(mc,"Monte Carlo","f");
        legend->AddEntry(data, "Data","f");
        legend->SetTextSize(0.05);
        legend->Draw();
    }
    
    legend_bool = false; // Reset legend boolean to false (default)

    C->SaveAs(save_path + ref + save_ext);

    return; 
}

void plotter::adv_plot(TString ref, TString title, int xmin, int xmax){
    // Define the Canvas
    TCanvas *c = new TCanvas(ref, ref, c_width, c_height);
    data = (TH1F*)f_data->Get(ref)->Clone("data");
    mc = (TH1F*)f_mc->Get(ref)->Clone("mc");

    gStyle->SetErrorX(0); // Remove x error bars

    // Upper plot will be in pad1
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0.125); // Upper and lower plot are separated
    pad1->SetLeftMargin(0.125);
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad
    mc->SetStats(0);          // No statistics on upper plot

    // Extra formatting
    mc->GetXaxis()->SetTitle(get_xLabel(ref));
    mc->GetYaxis()->SetTitle("Entries");
    mc->GetXaxis()->SetRangeUser(xmin, xmax);

    // Normalizing
    mc->Scale(sf);

    // Draw Commands
    mc->Draw("hist");               // Draw mc
    data->Draw("e1 same");    // Draw data on top of mc

    // lower plot will be in pad
    c->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetLeftMargin(0.125);
    pad2->SetGridx(); // vertical grid
    pad2->Draw();
    pad2->cd();       // pad2 becomes the current pad

    // Define the ratio plot
    TH1F *ratio = (TH1F*)data->Clone("ratio");
    ratio->SetLineColor(kBlack);
    ratio->SetMinimum(0);  // Define Y ..
    ratio->SetMaximum(2); // .. range
    ratio->Sumw2();
    ratio->SetStats(0);      // No statistics on lower plot
    ratio->Divide(mc);
    ratio->SetMarkerStyle(21);
    ratio->Draw("ep");       // Draw the ratio plot

    // mc settings
    mc->SetLineColor(kBlue);
    mc->SetFillColor(38);
    mc->SetLineWidth(2);

    // Y axis mc plot settings
    mc->GetYaxis()->SetTitleSize(20);
    mc->GetYaxis()->SetTitleFont(43);
    mc->GetYaxis()->SetTitleOffset(3.1); // prev 1.55

    // data settings
    data->SetMarkerStyle(kFullCircle);
    data->SetLineColor(kBlack);

    // Ratio plot (ratio) settings
    ratio->SetTitle(""); // Remove the ratio title

    // Y axis ratio plot settings
    ratio->GetYaxis()->SetTitle("Data/Monte Carlo");
    ratio->GetYaxis()->SetNdivisions(505);
    ratio->GetYaxis()->SetTitleSize(20);
    ratio->GetYaxis()->SetTitleFont(43);
    ratio->GetYaxis()->SetTitleOffset(1.55);
    ratio->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    ratio->GetYaxis()->SetLabelSize(15);

    // X axis ratio plot settings
    ratio->GetXaxis()->SetTitleSize(20);
    ratio->GetXaxis()->SetTitleFont(43);
    ratio->GetXaxis()->SetTitleOffset(4.);
    ratio->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    ratio->GetXaxis()->SetLabelSize(15);

    c->SaveAs(save_path + ref + save_ext);
}

void plotter::plot_data(TString ref, TString title, int xmin, int xmax, TString root_param){

    TCanvas *C = new TCanvas(ref, ref, c_width, c_height);
    data = (TH1F*)f_data->Get(ref)->Clone("data");
    mc = (TH1F*)f_mc->Get(ref)->Clone("mc");

    // Color formatting done here because TPainter variables could be hard to pass in
    data->SetMarkerStyle(kFullCircle);
    data->SetLineColor(kGreen);
    data->SetFillColor(8);
    data->GetXaxis()->SetTitle(get_xLabel(ref));
    data->SetTitle(title);

    data->Draw(root_param);
    
    C->SaveAs(save_path + ref + save_ext);
 
    return;
}

void plotter::plot_mc(TString ref, TString title, int xmin, int xmax, TString root_param){

    TCanvas *C = new TCanvas(ref, ref, c_width, c_height);
    data = (TH1F*)f_data->Get(ref)->Clone("data");
    mc = (TH1F*)f_mc->Get(ref)->Clone("mc");

    // Color formatting done here because TPainter variables could be hard to pass in
    mc->SetMarkerStyle(kFullCircle);
    mc->SetLineColor(kBlue);
    mc->SetFillColor(38);
    mc->GetXaxis()->SetTitle(get_xLabel(ref));
    mc->SetTitle(title);

    mc->Draw(root_param);
    
    C->SaveAs(save_path + ref + save_ext);
    
    return;
}

// Getters
TString plotter::get_xLabel(TString ref){
    TString names[11] = {"mass", "small_mass", "met", "ht", "ll_pt", "lt_pt", "ll_phi", "lt_phi", "ll_eta", "lt_eta", "jets"};
    TString xlabels[11] = {"GeV", "GeV", "GeV", "GeV", "GeV", "GeV", "Radians", "Radians", "#eta", "#eta", "Number of jets"};

    for (unsigned int i = 0; i < 11; i++){
        if (ref==names[i]){
            return xlabels[i];
        }
    }

    return "";
}

double plotter::get_scaleFactor(int peak_lower, int peak_upper){
    
    TH1F *sf_data = (TH1F*)f_data->Get("mass"); 
    TH1F *sf_mc = (TH1F*)f_mc->Get("mass"); 

    double new_sf = ((double)(sf_data->Integral(peak_lower,peak_upper)))/((double)(sf_mc->Integral(peak_lower,peak_upper)));

    cout << new_sf << endl;

    return new_sf;

}

// Setters
void plotter::set_canvasSize(int new_width, int new_height){
    c_width = new_width;
    c_height = new_height;
    return;
}

void plotter::set_scaleFactor(double new_sf){
    sf = new_sf;
    return;
}

void plotter::set_saveExtension(TString new_ext){
    save_ext = new_ext;
    return;
}

void plotter::set_savePath(TString new_path){
    save_path = new_path;
    return;
}

void plotter::set_barsLegend(bool new_status){
    legend_bool = new_status;
    return;
}

void adv_plots(){

    TFile *f_data = new TFile("/home/users/jguiang/projects/zpeak/plotter/data.root");
    TFile *f_mc = new TFile("/home/users/jguiang/projects/zpeak/plotter/mc.root");

    plotter *pltr = new plotter(f_data, f_mc);

    pltr->set_canvasSize(1000, 1200);
    pltr->adv_plot("mass", "Invariant Mass", 0, 200);

    pltr->plot_data("small_mass", "Upsilon and J/#Psi", 0, 20, "HIST");

    pltr->adv_plot("met", "Missing Transverse Energy", 0, 200);

    pltr->main_plot("jets", "Jets for p_{T} > 40", 0, 10, "BARS");

    pltr->adv_plot("ht", "Hadronic Transverse Momentum", 40, 300);

    pltr->adv_plot("lt_pt", "Tight Lepton Transverse Momentum", 20, 200);

    pltr->adv_plot("ll_pt", "Loose Lepton Transverse Momentum", 10, 200);

    pltr->set_barsLegend(false);
    pltr->main_plot("lt_phi", "Tight Lepton #phi", -4, 4, "BARS");

    pltr->set_barsLegend(false);
    pltr->main_plot("ll_phi", "Loose Lepton #phi", -4, 4, "BARS");

    pltr->set_barsLegend(false);
    pltr->main_plot("lt_eta", "Tight Lepton #eta", -4, 4, "BARS");

    pltr->set_barsLegend(false);
    pltr->main_plot("ll_eta", "Loose Lepton #eta", -4, 4, "BARS");


    return;
}

void main_plots(){

    TFile *f_data = new TFile("/home/users/jguiang/projects/zpeak/plotter/data.root");
    TFile *f_mc = new TFile("/home/users/jguiang/projects/zpeak/plotter/mc.root");

    plotter *pltr = new plotter(f_data, f_mc);

//    pltr->set_canvasSize(1200, 900);
//    pltr->main_plot("mass", "Invariant Mass", 0, 200, "DEFAULT");

//    pltr->plot_data("small_mass", "Upsilon and J/#Psi", 0, 20, "HIST");

//    pltr->main_plot("met", "Missing Transverse Energy", 0, 200, "DEFAULT")

    pltr->main_plot("jets", "Jets for p_{T} > 40", 0, 10, "BARS");

//    pltr->main_plot("ht", "Hadronic Transverse Momentum", 40, 300, "DEFAULT");

//    pltr->main_plot("lt_pt", "Tight Lepton Transverse Momentum", 20, 200, "DEFAULT");

//    pltr->main_plot("ll_pt", "Loose Lepton Transverse Momentum", 10, 200, "DEFAULT");

    pltr->set_barsLegend(false);
    pltr->main_plot("lt_phi", "Tight Lepton #phi", -4, 4, "BARS");

    pltr->set_barsLegend(false);
    pltr->main_plot("ll_phi", "Loose Lepton #phi", -4, 4, "BARS");

    pltr->set_barsLegend(false);
    pltr->main_plot("lt_eta", "Tight Lepton #eta", -4, 4, "BARS");

    pltr->set_barsLegend(false);
    pltr->main_plot("ll_eta", "Loose Lepton #eta", -4, 4, "BARS");

    return;
}

// Main function
void ooplt(){
    
    adv_plots();
//    main_plots();

    return;
}
