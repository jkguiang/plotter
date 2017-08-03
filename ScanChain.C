// -*- C++ -*-
// Usage:
// > root -b -q doAll.C

// C++
#include <iostream>
#include <vector>

// ROOT
#include "TBenchmark.h"
#include "TChain.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTreeCache.h"

// CMS3
#include "CMS3.h"
#include "/home/users/jguiang/CORE/MuonSelections.h"
#include "/home/users/jguiang/CORE/TriggerSelections.h"

using namespace std;
using namespace tas;

int ScanChain(TChain* chain, char sample_name[], bool fast = true, int nEvents = -1, string skimFilePrefix = "test") {

    // Benchmark
    TBenchmark *bmark = new TBenchmark();
    bmark->Start("benchmark");

    // Directory Setup
    TDirectory *rootdir = gDirectory->GetDirectory("Rint:");

    // Invariant Mass
    TH1F *mass = new TH1F("mass", "Invariant Mass", 100,0,200);
    mass->SetDirectory(rootdir);

    // Tight Lepton Transverse Momuntum
    TH1F *lt_pt = new TH1F("lt_pt", "Tight Lepton Transverse Momentum", 100,0,200);
    lt_pt->SetDirectory(rootdir);

    // Loose Lepton Transverse Momuntum
    TH1F *ll_pt = new TH1F("ll_pt", "Loose Lepton Transverse Momentum", 100,0,200);
    ll_pt->SetDirectory(rootdir);

    // Missing Transverse Energy
    TH1F *met = new TH1F("met", "Missing Transverse Energy", 100,0,200);
    met->SetDirectory(rootdir);

    // Jets for pt > 40
    TH1F *jets = new TH1F("jets", "Jets for p_{T} > 40", 10,0,10);
    jets->SetDirectory(rootdir);

    // Hadronic Transverse Momentum
    TH1F *ht = new TH1F("ht", "H_{T} for p_{T} > 40", 150,0,300);
    ht->SetDirectory(rootdir);

    // Tight Lepton Phi
    TH1F *lt_phi = new TH1F("lt_phi", "Tight Lepton Phi", 160,-4,4);
    lt_phi->SetDirectory(rootdir);

    // Loose Lepton Phi
    TH1F *ll_phi = new TH1F("ll_phi", "Loose Lepton Phi", 160,-4,4);
    ll_phi->SetDirectory(rootdir);

    // Tight Lepton Eta
    TH1F *lt_eta = new TH1F("lt_eta", "Tight Lepton Eta", 160,-4,4);
    lt_eta->SetDirectory(rootdir);

    // Loose Lepton Eta
    TH1F *ll_eta = new TH1F("ll_eta", "Loose Lepton Eta", 160,-4,4);
    ll_eta->SetDirectory(rootdir);

    // Small Masses
    TH1F *small_mass = new TH1F("small_mass", "Upsilon and JPsi", 100,0,20);
    small_mass->SetDirectory(rootdir);

    // Loop over events to Analyze
    unsigned int nEventsTotal = 0;
    unsigned int nEventsChain = chain->GetEntries();
    if (nEvents >= 0) nEventsChain = nEvents;
    TObjArray *listOfFiles = chain->GetListOfFiles();
    TIter fileIter(listOfFiles);
    TFile *currentFile = 0;

    // File Loop
    while ( (currentFile = (TFile*)fileIter.Next()) ) {

        // Get File Content
        TFile file(currentFile->GetTitle());
        TTree *tree = (TTree*)file.Get("Events");
        if (fast) TTreeCache::SetLearnEntries(10);
        if (fast) tree->SetCacheSize(128*1024*1024);
        cms3.Init(tree);

        // Loop over Events in current file
        if (nEventsTotal >= nEventsChain) continue;
        unsigned int nEventsTree = tree->GetEntriesFast();
        for (unsigned int event = 0; event < nEventsTree; ++event) {

            // Get Event Content
            if (nEventsTotal >= nEventsChain) continue;
            if (fast) tree->LoadTree(event);
            cms3.GetEntry(event);
            ++nEventsTotal;

            // Progress
            CMS3::progress( nEventsTotal, nEventsChain );

            // Analysis Code
            /*
                Histograms:
                    jets -> Number of jets
                    ht -> Hadronic transverse momentum
                    mass -> Invariant mass for opp sign, same flav, dileptons
                    met -> Missing transverse energy
                    lt_pt -> Tight lepton transverse momentum
                    ll_pt -> Loose lepton transverse momentum
                    lt_phi
                    ll_phi      Phi
                    jet_phi
                    lt_eta
                    ll_eta      Eta
                    jet_eta
            */

            // Trigger
            if (cms3.evt_isRealData() == 1 && !passHLTTriggerPattern("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v10")){
                continue;
            }

            // Find best hypothesis
            if (cms3.hyp_lt_id().size() == 0){
                continue;
            }

            const double Z_mass = 91.1852; // From pdg
            int bhi = 0; // Best Hypothesis Index
            bool bh_found = false;
            double gbl_dm = 1000; // Global difference in mass, only changes if cur_dm < dm
            double cur_dm;

            for (unsigned int i = 0; i < cms3.hyp_lt_id().size(); i++){

                // Constraints                
                if (cms3.hyp_lt_id().at(i) != -cms3.hyp_ll_id().at(i) || abs(cms3.hyp_lt_id().at(i)) != 13){
                    continue;
                }
                if (cms3.hyp_lt_p4().at(i).pt() < 20 || cms3.hyp_ll_p4().at(i).pt() < 10){
                    continue;
                }
                if (!muonID(hyp_ll_index().at(i), STOP_medium_v3) || !muonID(hyp_lt_index().at(i), STOP_medium_v3)){
                    continue;
                }

                cur_dm = abs(cms3.hyp_p4().at(i).M() - Z_mass);
                if  (cur_dm < gbl_dm){
                    gbl_dm = cur_dm;
                    bh_found = true;
                    bhi = i;
                }
            }

            if (bh_found == false){
                continue;
            }

            // Loop over jets
            int jetSum = 0;
            double htSum = 0;
            for (unsigned int j = 0; j < cms3.pfjets_p4().size(); j++){
                if (cms3.pfjets_p4()[j].pt() > 40){
                    auto jet_eta = cms3.pfjets_p4().at(j).eta();
                    auto jet_phi = cms3.pfjets_p4().at(j).phi();
                    // Discard data for tight lepton eta > 2.4
                    if (abs(jet_eta) > 2.4){
                        continue;
                    }
                    auto lt_eta = cms3.hyp_lt_p4().at(bhi).eta();
                    auto ll_eta = cms3.hyp_ll_p4().at(bhi).eta();
                    auto lt_phi = cms3.hyp_lt_p4().at(bhi).phi();
                    auto ll_phi = cms3.hyp_ll_p4().at(bhi).phi();

                    // We want inside phi between two jets, (dphi > pi) -> outside angle
                    double dphi = abs(lt_phi - jet_phi);
                    if (dphi > M_PI){
                        dphi = 2*M_PI - dphi;
                    }
                    double dR = sqrt(pow((dphi), 2) + pow((lt_eta - jet_eta), 2));
                    if (dR < 0.3){
                        continue;
                    }

                    dphi = abs(ll_phi - jet_phi);
                    if (dphi > M_PI){
                        dphi = 2*M_PI - dphi;
                    }
                    dR = sqrt(pow((dphi), 2) + pow((ll_eta - jet_eta), 2));
                    if (dR < 0.3){
                        continue;
                    }

                    // Only add to number of jets if fails above tests
                    jetSum += 1;
                    htSum += abs(cms3.pfjets_p4()[j].pt());
                }
            }


            // Fill Histograms
            jets->Fill(jetSum);
            if (htSum > 0){
                ht->Fill(htSum);
            }
            met->Fill(cms3.evt_pfmet());
            mass->Fill(cms3.hyp_p4().at(bhi).M());
            small_mass->Fill(cms3.hyp_p4().at(bhi).M());
            lt_pt->Fill(cms3.hyp_lt_p4().at(bhi).pt());
            ll_pt->Fill(cms3.hyp_ll_p4().at(bhi).pt());
            lt_phi->Fill(cms3.hyp_lt_p4().at(bhi).phi());
            ll_phi->Fill(cms3.hyp_ll_p4().at(bhi).phi());
            lt_eta->Fill(cms3.hyp_lt_p4().at(bhi).eta());
            ll_eta->Fill(cms3.hyp_ll_p4().at(bhi).eta());

        }
  
        // Clean Up
        delete tree;
        file.Close();
    }
    if (nEventsChain != nEventsTotal) {
        cout << Form( "ERROR: number of events from files (%d) is not equal to total number of events (%d)", nEventsChain, nEventsTotal ) << endl;
    }
  
    // Write Histograms
    TFile* f = new TFile(sample_name, "RECREATE");
    jets->Write();
    ht->Write();
    met->Write();
    mass->Write();
    small_mass->Write();
    lt_pt->Write();
    ll_pt->Write();
    lt_phi->Write();
    ll_phi->Write();
    lt_eta->Write();
    ll_eta->Write();
    
    f->Close();

    // Clear histograms  
    jets->Delete();
    ht->Delete();
    met->Delete();
    mass->Delete();
    small_mass->Delete();
    lt_pt->Delete();
    ll_pt->Delete();
    lt_phi->Delete();
    ll_phi->Delete();
    lt_eta->Delete();
    ll_eta->Delete();

    // return
    bmark->Stop("benchmark");
    cout << endl;
    cout << nEventsTotal << " Events Processed" << endl;
    cout << "------------------------------" << endl;
    cout << "CPU  Time: " << Form( "%.01f", bmark->GetCpuTime("benchmark")  ) << endl;
    cout << "Real Time: " << Form( "%.01f", bmark->GetRealTime("benchmark") ) << endl;
    cout << endl;
    delete bmark;
    return 0;
}
