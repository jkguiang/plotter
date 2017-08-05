Example Plotter Interface:
http://uaf-8.t2.ucsd.edu/~jguiang/AutoPlotter/plotter.html

Contents:
ooplt.C - Object-orieted plotter
ScanChain.C - Looping program that generates basic root histograms and saves them to a .root file for plotting by ooplt
webinterface - Contains files relevant to .html plot display

ooplt:
main_plot(TString ref, TString title, int xmin, int xmax, TString plt_typ)
ref: name of histogram in TFile
title: title of histogram
xmin, xmax: range of x-axis
plt_type:
    "DEFAULT" - Monte Carlo plotted in solid blue, data with black markers and errorbars
    "BARS" - Monte Carlo plotted in solid blue, data in transparent green

run.sh:
./run.sh {1}
    setup - set up directories and files, run doAll.C, run ooplt.C, update web interface
    plot - run ooplt.C, update web interface
    scan - run doAll.C
