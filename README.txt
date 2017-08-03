Plots:
http://uaf-8.t2.ucsd.edu/~jguiang/zpeak/plotter.html#/

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
