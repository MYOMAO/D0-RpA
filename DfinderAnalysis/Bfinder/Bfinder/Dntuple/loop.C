using namespace std;

#include "loop.h"
#include "format.h"
#include "Dntuple.h"

Bool_t istest = false;
bool fillZeroCandEvt = true;
int loop(TString infile="", TString outfile="", Bool_t REAL=false, Bool_t isPbPb=true, Int_t startEntries=0, Int_t endEntries=-1, Bool_t skim=false, Bool_t gskim=true, Bool_t checkMatching=true, Bool_t iseos=false, Bool_t SkimHLTtree=false)
{
  if(istest)
    {
      infile="/store/group/phys_heavyions/HeavyFlavourRun2/DfinderRun2/MC_official/Pythia8_prompt_D0pt0p0_Pthat0_pp502_TuneCUETP8M1/crab_DfinderMC_pp_20160502_dPt0tkPt0p5_D0Dstar/160502_191346/0000/finder_pp_53.root";
      outfile="test.root";
      REAL=false;
      isPbPb=false;
      skim=false;
      checkMatching=true;
      iseos=true;
    }
  cout<<endl;
  if(REAL) cout<<"--- Processing - REAL DATA";
  else cout<<"--- Processing - MC";
  if(isPbPb) cout<<" - PbPb";
  else cout<<" - pp";
  cout<<endl;

  TString ifname;
  if(iseos) ifname = Form("root://eoscms.cern.ch//eos/cms%s",infile.Data());
  else ifname = infile;
  if (!TFile::Open(ifname))   { cout << " fail to open file" << endl; return 0;}
  TFile* f = TFile::Open(ifname);
  TTree* root = (TTree*)f->Get("Dfinder/root");  
  TTree* hltroot = (TTree*)f->Get("hltanalysis/HltTree");
  TTree* skimroot = (TTree*)f->Get("skimanalysis/HltTree");
  TTree* hiroot = (TTree*)f->Get("hiEvtAnalyzer/HiTree");

  DntupleBranches     *Dntuple = new DntupleBranches;
  EvtInfoBranches     *EvtInfo = new EvtInfoBranches;
  VtxInfoBranches     *VtxInfo = new VtxInfoBranches;
  TrackInfoBranches   *TrackInfo = new TrackInfoBranches;
  DInfoBranches       *DInfo = new DInfoBranches;
  GenInfoBranches     *GenInfo = new GenInfoBranches;

  if(SkimHLTtree) SetHlttreestatus(hltroot, isPbPb);
  setHltTreeBranch(hltroot);
  setHiTreeBranch(hiroot);

  EvtInfo->setbranchadd(root);
  VtxInfo->setbranchadd(root);
  TrackInfo->setbranchadd(root);
  DInfo->setbranchadd(root);
  GenInfo->setbranchadd(root);

  Long64_t nentries = root->GetEntries();
  if(endEntries>nentries || endEntries == -1) endEntries = nentries;
  TFile* outf = TFile::Open(Form("%s", outfile.Data()),"recreate");

  int isDchannel[14];
  isDchannel[0] = 1; //D0(k+pi-)
  isDchannel[1] = 1; //D0(k-pi+)
  isDchannel[2] = 1; //D*(D0(k-pi+)pi+)
  isDchannel[3] = 1; //D*(D0(k+pi-)pi-)
  isDchannel[4] = 1; //D*(D0(k-pi-pi+pi+)pi+)
  isDchannel[5] = 1; //D*(D0(k+pi+pi-pi-)pi-)
  isDchannel[6] = 1; 
  isDchannel[7] = 1; 
  isDchannel[8] = 1; 
  isDchannel[9] = 1; 
  isDchannel[10] = 1; 
  isDchannel[11] = 1;
  isDchannel[12] = 1; //B+(D0(k-pi+)pi+)
  isDchannel[13] = 1; //B-(D0(k-pi+)pi-)

  cout<<"--- Building trees"<<endl;
  bool detailMode = true;
  bool D0kpimode = true;
  TTree* ntD1 = new TTree("ntDkpi","");           Dntuple->buildDBranch(ntD1,D0kpimode,detailMode);
  D0kpimode = false;
  TTree* ntD2 = new TTree("ntDkpipi","");         Dntuple->buildDBranch(ntD2,D0kpimode,detailMode);
  TTree* ntD3 = new TTree("ntDkpipipi","");       Dntuple->buildDBranch(ntD3,D0kpimode,detailMode);
  TTree* ntD4 = new TTree("ntDPhikkpi","");       Dntuple->buildDBranch(ntD4,D0kpimode,detailMode);
  TTree* ntD5 = new TTree("ntDD0kpipi","");       Dntuple->buildDBranch(ntD5,D0kpimode,detailMode);
  TTree* ntD6 = new TTree("ntDD0kpipipipi","");   Dntuple->buildDBranch(ntD6,D0kpimode,detailMode);
  TTree* ntD7 = new TTree("ntBptoD0pi","");       Dntuple->buildDBranch(ntD7,D0kpimode,detailMode);
  TTree* ntGen = new TTree("ntGen","");           Dntuple->buildGenBranch(ntGen);
  TTree* ntHlt = hltroot->CloneTree(0);
  ntHlt->SetName("ntHlt");
  TTree* ntSkim = skimroot->CloneTree(0);
  ntSkim->SetName("ntSkim");
  TTree* ntHi = hiroot->CloneTree(0);
  ntHi->SetName("ntHi");
  cout<<"--- Building trees finished"<<endl;

  cout<<"--- Check the number of events for three trees"<<endl;
  cout<<root->GetEntries()<<" "<<hltroot->GetEntries()<<" "<<hiroot->GetEntries();
  cout<<" "<<skimroot->GetEntries()<<endl;
  cout<<endl;
  cout<<"--- Processing events"<<endl;
  for(int i=startEntries;i<endEntries;i++)
    {
      root->GetEntry(i);
      hltroot->GetEntry(i);
      skimroot->GetEntry(i);
      hiroot->GetEntry(i);
      if(i%1000==0) cout<<setw(7)<<i<<" / "<<endEntries<<endl;
      if(checkMatching)
        {
          if(((int)Df_HLT_Event!=EvtInfo->EvtNo||(int)Df_HLT_Run!=EvtInfo->RunNo||(int)Df_HLT_LumiBlock!=EvtInfo->LumiNo) || 
             ((int)Df_HiTree_Evt!=EvtInfo->EvtNo||(int)Df_HiTree_Run!=EvtInfo->RunNo||(int)Df_HiTree_Lumi!=EvtInfo->LumiNo))
            {
              cout<<"Error: not matched "<<i<<" | (Hlt,Dfr,Hi) | ";
              cout<<"EvtNo("<<Df_HLT_Event<<","<<EvtInfo->EvtNo<<","<<Df_HiTree_Evt<<") ";
              cout<<"RunNo("<<Df_HLT_Run<<","<<EvtInfo->RunNo<<","<<Df_HiTree_Run<<") ";
              cout<<"LumiNo("<<Df_HLT_LumiBlock<<","<<EvtInfo->LumiNo<<","<<Df_HiTree_Lumi<<")"<<endl;
              continue;
            }
        }
      int Dtypesize[7]={0,0,0,0,0,0,0};
      if(!REAL) fillZeroCandEvt = true;// on MC one should not ignore any reco event 
      Dntuple->makeDNtuple(isDchannel, Dtypesize, REAL, fillZeroCandEvt, skim, EvtInfo, VtxInfo, TrackInfo, DInfo, GenInfo, ntD1, ntD2, ntD3, ntD4, ntD5, ntD6, ntD7);

      bool zeroCand = true;
      for(int t=0;t<7;t++)
        {
          if(Dtypesize[t] != 0) zeroCand = false;        
        }
      if(fillZeroCandEvt || !zeroCand)
        {
          ntHlt->Fill();
          ntSkim->Fill();
          ntHi->Fill();
          if(!REAL) Dntuple->fillDGenTree(ntGen, GenInfo, gskim);
        }

    }
  outf->Write();
  cout<<"--- Writing finished"<<endl;
  outf->Close();

  cout<<"--- In/Output files"<<endl;
  cout<<ifname<<endl;
  cout<<outfile<<endl;
  cout<<endl;

  return 0;
}

int main(int argc, char *argv[])
{
  if(argc==3)
    {
      loop(argv[1], argv[2]);
    }
  else
    {
      std::cout << "Usage: mergeForest <input_collection> <output_file>" << std::endl;
      return 1;
    }
  
  return 0;
}

