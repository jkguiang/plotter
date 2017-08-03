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
    bool bars_legend = true;

    public:
        // Overload Constructor
        plotter(TFile*, TFile*);

        // Plotting Functions
        void main_plot(TString, TString, int, int); // Plots histograms with preset options
        void bars_plot(TString, TString, int, int); // Suited for plots with large bins relative to dataset
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
void plotter::main_plot(TString ref, TString title, int xmin, int xmax){

    TCanvas *C = new TCanvas(ref, ref, c_width, c_height);
    data = (TH1F*)f_data->Get(ref)->Clone("data");
    mc = (TH1F*)f_mc->Get(ref)->Clone("mc");

    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);

    mc->GetXaxis()->SetTitle(get_xLabel(ref));
    mc->GetXaxis()->SetRangeUser(xmin, xmax);
    mc->SetTitle(title);
    mc->SetFillColor(38);

    data->SetMarkerStyle(kFullCircle);
    data->SetLineColor(kBlack);

    // Normalizing
    mc->Scale(sf);

    mc->Draw("HIST");
    data->Draw("E1 SAME");
    
    C->SaveAs(save_path + ref + save_ext);

    return; 
}

void plotter::bars_plot(TString ref, TString title, int xmin, int xmax){

    TCanvas *C = new TCanvas(ref, ref, c_width, c_height);
    data = (TH1F*)f_data->Get(ref)->Clone("data");
    mc = (TH1F*)f_mc->Get(ref)->Clone("mc");

    C->SetLogy(1);
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0);

    mc->SetFillColor(38);
    mc->SetLineColor(kBlue);
    mc->GetXaxis()->SetTitle(get_xLabel(ref));
    mc->GetXaxis()->SetRangeUser(xmin, xmax);
    mc->SetTitle(title);

    // Normalizing
    mc->Scale(sf);

    data->SetFillStyle(3002);
    data->SetFillColor(8);
    data->SetLineColor(kGreen);
    mc->Draw("HIST");
    data->Draw("HIST SAME");

    if (bars_legend == true){
        auto legend = new TLegend(0.9,0.9,0.6,0.6);
        legend->SetHeader("Legend","C"); // option "C" allows to center the header
        legend->AddEntry(mc,"Monte Carlo","f");
        legend->AddEntry(data, "Data","f");
        legend->SetTextSize(0.05);
        legend->Draw();
    }
    
    C->SaveAs(save_path + ref + save_ext);
    bars_legend = true; // Reset legend boolean to true (default)
    
    return;

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
    TString names[11] = {"mass", "small_mass", "met", "ht", "ll_pt", "lt_pt", "ll_phi", "lt_phi", "ll_eta", "let_eta", "jets"};
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
    bars_legend = new_status;
    return;
}

// Main function
void ooplt(){

    TFile *f_data = new TFile("/home/users/jguiang/projects/zpeak/looper/data.root");
    TFile *f_mc = new TFile("/home/users/jguiang/projects/zpeak/looper/mc.root");

    plotter *pltr = new plotter(f_data, f_mc);

    pltr->main_plot("mass", "Invariant Mass", 0, 200);

    pltr->plot_data("small_mass", "Upsilon and J/#Psi", 0, 20, "HIST");

    pltr->main_plot("met", "Missing Transverse Energy", 0, 200);

    pltr->bars_plot("jets", "Jets for p_{T} > 40", 0, 10);

    pltr->main_plot("ht", "Hadronic Transverse Momentum", 40, 300);

    pltr->main_plot("lt_pt", "Tight Lepton Transverse Momentum", 20, 200);

    pltr->main_plot("ll_pt", "Loose Lepton Transverse Momentum", 10, 200);

    pltr->set_barsLegend(false);
    pltr->bars_plot("lt_phi", "Tight Lepton #phi", -4, 4);

    pltr->set_barsLegend(false);
    pltr->bars_plot("ll_phi", "Loose Lepton #phi", -4, 4);

    pltr->set_barsLegend(false);
    pltr->bars_plot("lt_eta", "Tight Lepton #eta", -4, 4);

    pltr->set_barsLegend(false);
    pltr->bars_plot("ll_eta", "Loose Lepton #eta", -4, 4);

    return;
}
