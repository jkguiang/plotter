{
  gSystem->Load("/home/users/jguiang/CORE/CMS3_CORE.so");

  gROOT->ProcessLine(".L ScanChain.C+");

  TChain *ch_data = new TChain("Events"); 
  //Manually changed Add file path and name of root file in ScanChain arguments
  ch_data->Add("/home/users/jguiang/projects/zpeak/zpeak_data/DataDoubleMuon.root");
  ScanChain(ch_data, "data.root");

  TChain *ch_mc = new TChain("Events"); 
  //Manually changed Add file path and name of root file in ScanChain arguments
  ch_mc->Add("/home/users/jguiang/projects/zpeak/zpeak_data/DYMC.root");
  ScanChain(ch_mc, "mc.root");
}
