Example Plotter Interface:
http://uaf-8.t2.ucsd.edu/~jguiang/AutoPlotter

Contents:
ooplt.C - Object-orieted plotter
ScanChain.C - Looping program that generates basic root histograms and saves them to a .root file for plotting by ooplt
webinterface - Contains files relevant to .html plot display

run.sh:
./run.sh {1}
    1.  setup - set up directories and files, run doAll.C, run ooplt.C, update web interface
        plot - run ooplt.C, update web interface
        scan - run doAll.C

AutoPlotter Features:
-Beautiful UI
-Quick search with text highlights
-Unique url for searches to allow for easy sharing
-Slider for changing thumbnail size

Features to implement:
-Tooltip displaying metadata for each graph on hover
