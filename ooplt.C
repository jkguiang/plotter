//Run this script to parse data and save histograms in "histograms.root"

//Root includes
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

//C++ includes
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

struct plot {

    TH1F* hist;
    double integral;

};

class plotter {
    
    // Member Variables
    TFile* f_data;
    TFile* f_mc;
    TH1F* data;
    TH1F* mc;
    vector<TFile*> pathPile;
    vector<TString> stackNames;

    int c_width; // Canvas width
    int c_height; // Canvas height

    TString save_ext;
    TString save_path;

    // Plot Specific Variables
    bool legend_bool;
    bool logY_bool;
    int ratio_rebin;

    public:
        // Overload Constructor
        plotter(TFile*, TFile*);

        // Plotting Functions
        void basic_plot(TString, TString, int, int, TString); // Plots histograms with preset options
        void ratio_plot(TString, TString, int, int, TString); // Advanced plot, with ratio plot of data vs mc
        void plot_data(TString, TString, int, int, TString); // Just plots data, plot must be customized manually
        void plot_mc(TString, TString, int, int, TString); // Just plots monte carlo, plot must be customized manually

        void add_toPaths(TString, TString);
        void plot_stack(TString, TString, int, int, TString);

        // Getters
        TString get_xLabel(TString);

        // Setters
        void set_canvasSize(int, int);
        void set_saveExtension(TString);
        void set_savePath(TString);
        void set_ratioRebin(int);
        void set_logY(bool);

        // Plot Specific Setters
        void set_barsLegend(bool); // true = on, false = off

};

plotter::plotter(TFile* new_fdata, TFile* new_fmc) {

    f_data = new_fdata;
    f_mc = new_fmc;
    pathPile = {};

    c_width = 1000;
    c_height = 1200;

    save_ext = ".pdf";
    save_path = "/home/users/jguiang/public_html/AutoPlotter/static/pdfs/";

    legend_bool = false;
    logY_bool = false;
    ratio_rebin = 1;
}

// Plotting Functions
void plotter::basic_plot(TString ref, TString title, int xmin, int xmax, TString plt_typ) {

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

    // Default plot: MC -> blue solid fill, Data -> black markers w/ errorbars
    if (plt_typ == "DEFAULT") {

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
    if (plt_typ == "BARS") {

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
    if (legend_bool == true) {
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

void plotter::ratio_plot(TString ref, TString title, int xmin, int xmax, TString plt_typ) {
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

    // Ensure that first plot accomodates for maximum of second plot
    mc->SetMaximum(max(mc->GetMaximum(), data->GetMaximum()));

    // Draw Commands
    // Default plot: MC -> blue solid fill, Data -> black markers w/ errorbars
    if (plt_typ == "DEFAULT") {

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
    if (plt_typ == "BARS") {

        pad1->SetLogy(1);

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

    // lower plot will be in pad
    c->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.1, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetLeftMargin(0.125);
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
    ratio->Rebin(ratio_rebin);
    ratio->Draw("PE");       // Draw the ratio plot

    // Y axis mc plot settings
    mc->GetYaxis()->SetTitleSize(20);
    mc->GetYaxis()->SetTitleFont(43);
    mc->GetYaxis()->SetTitleOffset(3.1); // prev 1.55

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
    ratio->GetXaxis()->SetRangeUser(xmin, xmax);

    c->SaveAs(save_path + ref + save_ext);
}

void plotter::plot_data(TString ref, TString title, int xmin, int xmax, TString root_param) {

    TCanvas *C = new TCanvas(ref, ref, c_width, c_height);
    data = (TH1F*)f_data->Get(ref)->Clone("data");
    mc = (TH1F*)f_mc->Get(ref)->Clone("mc");

    gStyle->SetOptStat(0);

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

void plotter::plot_mc(TString ref, TString title, int xmin, int xmax, TString root_param) {

    TCanvas *C = new TCanvas(ref, ref, c_width, c_height);
    data = (TH1F*)f_data->Get(ref)->Clone("data");
    mc = (TH1F*)f_mc->Get(ref)->Clone("mc");

    gStyle->SetOptStat(0);

    // Color formatting done here because TPainter variables could be hard to pass in, also arguments list is already long
    // and adding more functions would just make the plotter more convoluted
    mc->SetMarkerStyle(kFullCircle);
    mc->SetLineColor(kBlue);
    mc->SetFillColor(38);
    mc->GetXaxis()->SetTitle(get_xLabel(ref));
    mc->SetTitle(title);

    mc->Draw(root_param);
    
    C->SaveAs(save_path + ref + save_ext);
    
    return;
}

void plotter::add_toPaths(TString new_mcPath, TString new_name) {

    TFile *new_fmc = new TFile(new_mcPath);
    pathPile.push_back(new_fmc);
    stackNames.push_back(new_name);

    return;
}

void plotter::plot_stack(TString ref, TString save_name, int xmin, int xmax, TString opt) {
    
    // Initialize Canvas
    TCanvas *C = new TCanvas(ref, ref, c_width, c_height);

    gStyle->SetOptStat(0);

    // Initialize Stack
    THStack *stack = new THStack("stack", "");

    // Upper plot will be in pad1
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0.125); // Upper and lower plot are separated
    pad1->SetLeftMargin(0.140);
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad

    // Legend
    auto legend = new TLegend(0.9,0.9,0.6,0.7);
    legend->SetHeader("","C"); // option "C" allows to center the header

    // Data file: f_data
    data = (TH1F*)f_data->Get(ref)->Clone("data");
    data->SetMarkerStyle(kFullCircle);
    data->SetLineColor(kBlack);
    data->SetLineWidth(2);
    int data_bins = data->GetNbinsX();  // For use in ratio plot
    int orig_xmax = data->GetXaxis()->GetXmax();  // For use in ratio plot
    int orig_xmin = data->GetXaxis()->GetXmin();  // For use in ratio plot
    legend->AddEntry(data, "Data", "PE");

    // Y axis stack plot settings
    data->GetYaxis()->SetTitle("Entries");
    // X axis stack plot settings
    data->GetXaxis()->SetTitle(get_xLabel(ref));
    data->GetXaxis()->SetRangeUser(xmin, xmax);

    // Create new histo to collect all mc for ratio plot
    TH1F * mc_merged = new TH1F("merged", "merged_hists", data_bins, orig_xmin, orig_xmax);
    int colors[] = {7, 3, 9, 28, 46};
    // parse over  mc files, plot histograms, then push to stack
    vector<plot> plots;
    for (unsigned int i = 0; i < pathPile.size(); i++) {
        plot new_plot;
        plots.push_back(new_plot);

        TString new_name = "mc" + (TString)((char)i);
        TH1F* new_mc = (TH1F*)pathPile[i]->Get(ref)->Clone(new_name);
        new_mc->SetFillColor(colors[i]);
        legend->AddEntry(new_mc, stackNames[i], "f");
        mc_merged->Add(new_mc);

        // Update plots array
        plots.at(i).hist = new_mc;
        plots.at(i).integral = new_mc->Integral();
    }

    // Plot Organization
    if (opt == "DEFAULT") {
        for (unsigned int i = 0; i < plots.size(); i++) {
            stack->Add(plots.at(i).hist);
        }
    }
    else {
        vector<double> integrals;
        vector<TH1F*> sort_hists;

        for (unsigned int i = 0; i < plots.size(); i++) {
            integrals.push_back(plots.at(i).integral);
        }

        sort(integrals.begin(), integrals.end(), greater<double>());
        
        for (unsigned int i = 0; i < integrals.size(); i++) {
            for (unsigned int j = 0; j < plots.size(); j++) {
            
                if (integrals[i] == plots.at(j).integral) {
                    sort_hists.push_back(plots.at(j).hist);
                }
            
            }
        }
        
        if (opt == "G") {
            for (unsigned int i = 0; i < sort_hists.size(); i++) {
                stack->Add(sort_hists.at(i));
            }
        }

        else if (opt == "L") {
            reverse(sort_hists.begin(), sort_hists.end());
            for (unsigned int i = 0; i < sort_hists.size(); i++) {
                stack->Add(sort_hists.at(i));
            }
        }
    } // End else statement

    data->Draw("PE");
    stack->Draw("SAME HIST");
    data->Draw("PE SAME");

    // Ensure that first plot accomodates for maximum of second plot with space for legend, and minimum to ensure all data is drawn
    data->SetMaximum(max(stack->GetMaximum(), data->GetMaximum()));
    data->SetMinimum(0.5);

    // Draw Legend
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->Draw();

    // Draw Textboxes

    // Set log scale if requested
    if (logY_bool == true) {
        pad1->SetLogy(1);
        logY_bool = false;
    }
    

    // lower plot will be in pad
    C->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.1, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetLeftMargin(0.140);
    pad2->Draw();
    pad2->cd();       // pad2 becomes the current pad

    // Define the ratio plot
    TH1F *ratio = (TH1F*)data->Clone("ratio");
    ratio->SetLineColor(kBlack);
    ratio->SetMinimum(0);  // Define Y ..
    ratio->SetMaximum(2); // .. range
    ratio->Sumw2();
    ratio->SetStats(0);      // No statistics on lower plot

    ratio->Divide(mc_merged);
    ratio->SetMarkerStyle(21);
    ratio->Rebin(ratio_rebin);
    ratio_rebin = 1;
    ratio->Draw("PE");       // Draw the ratio plot

    // Y axis stack plot settings
    data->GetYaxis()->SetTitleSize(20);
    data->GetYaxis()->SetTitleFont(43);
    data->GetYaxis()->SetTitleOffset(3.1); // prev 1.55

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
    ratio->GetXaxis()->SetRangeUser(xmin, xmax);
    ratio->GetXaxis()->SetTitle("");

    // Draw horizontal line at 1
    TLine *line = new TLine(xmin, 1, xmax, 1);
    line->Draw("SAME");
    gStyle->SetLineStyle(7);

    C->SaveAs(save_path + save_name + save_ext);

    // Delete hists to avoid "memory leak" errors
    delete mc_merged;

    return;
}



// Getters
TString plotter::get_xLabel(TString ref) {

    TString names[11] = {"mass", "small_mass", "met", "ht", "ll_pt", "lt_pt", "ll_phi", "lt_phi", "ll_eta", "lt_eta", "jets"};
    TString xlabels[11] = {"GeV", "GeV", "GeV", "GeV", "GeV", "GeV", "Radians", "Radians", "#eta", "#eta", "Number of jets"};

    for (unsigned int i = 0; i < 11; i++) {
        if (ref==names[i]) {
            return xlabels[i];
        }
    }

    return "";
}

// Setters
void plotter::set_canvasSize(int new_width, int new_height) {
    c_width = new_width;
    c_height = new_height;
    return;
}

void plotter::set_saveExtension(TString new_ext) {
    save_ext = new_ext;
    return;
}

void plotter::set_savePath(TString new_path) {
    save_path = new_path;
    return;
}

void plotter::set_barsLegend(bool new_status) {
    legend_bool = new_status;
    return;
}

void plotter::set_ratioRebin(int new_ratio_rebin) {
    ratio_rebin = new_ratio_rebin;
    return;
}

void plotter::set_logY(bool new_status) {
    logY_bool = new_status;
    return;
}

void stacks() {

    TFile *f_data = new TFile("/home/users/jguiang/projects/zpeak/plotter/data.root");

    plotter *pltr = new plotter(f_data, NULL);
    
    // Add MC to stack
    pltr->add_toPaths("/home/users/jguiang/projects/zpeak/plotter/mc_TT.root", "T#bar{T}");
    pltr->add_toPaths("/home/users/jguiang/projects/zpeak/plotter/mc_Z_M50.root", "DY M > 50");
    pltr->add_toPaths("/home/users/jguiang/projects/zpeak/plotter/mc_Z_M10to50.root", "DY 10 < M < 50");

    // Plot stacks
    pltr->plot_stack("mass", "inv_mass", 0, 200, "L");

    pltr->plot_stack("ht", "ht", 40, 200, "L");

    pltr->set_logY(true);
    pltr->plot_stack("jets", "jets", 0, 10, "L");
    
    pltr->plot_stack("met", "met", 0, 200, "L");
    
    pltr->plot_stack("lt_pt", "lt_pt", 20, 200, "L");

    pltr->plot_stack("ll_pt", "ll_pt", 10, 200, "L");

    pltr->set_logY(true);
    pltr->plot_stack("lt_phi", "lt_phi", -4, 4, "L");

    pltr->set_logY(true);
    pltr->plot_stack("ll_phi", "ll_phi", -4, 4, "L");

    pltr->set_logY(true);
    pltr->plot_stack("lt_eta", "lt_eta", -4, 4, "L");

    pltr->set_logY(true);
    pltr->plot_stack("ll_eta", "ll_eta", -4, 4, "L");

    return;
}


// Main function
void ooplt() {
    
    stacks();

    return;
}
