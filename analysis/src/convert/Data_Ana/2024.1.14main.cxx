#include <stdio.h>
#include <stdlib.h>

#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <thread>

#include "TBranch.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TMutex.h"
#include "TROOT.h"
#include "TString.h"
#include "TTree.h"
// #include "TSpectrum.h"

// data format
#include "TRandom3.h"
#include "XIA_Decode.h"
#include "setup.h"

using namespace std;

/////////////////
int Data_Ana(char* RunName, int RunNum) {
  //   TRandom3 r(0);

  float Ecut = 100;
  double Slope_E[XIA_Modules][XIA_CH];
  //   double Offset_E[XIA_Modules][XIA_CH];
  int ret;
  int ModuleID, ChannelID;
  int E_row, CFD_row;
  long T_row;
  //   double E_Cal;

  double raw[64];
  long ts[64];
  long tsInner, tsOuter, tsAnti;
  long tsdelayOuter, tsdelayAnti;
  double single[48];  // cal data, InnerBGO(0-15), OuterBGO(16-48);
  double sum;
  double singleInner[16];
  double sumInner;
  double singleInnerFront[8];
  double sumInnerFront;
  double singleInnerBack[8];
  double sumInnerBack;
  double singleOuter[32];
  double sumOuter;
  double plastic[10];
  double anti;
  int pileup;  // pile-up
  int runNumber;
  int nhit,Innernhit;

  TString str_tmp;
  ifstream ifile;
  ifile.open("Cali20231124.txt");
  // while(true)
  for (int i = 0; i < XIA_Modules - 1; i++)
    for (int j = 0; j < XIA_CH; j++) {
      ifile >> Slope_E[i][j];
      // cout << "Slope_E[" << i << "][" << j << "]" << Slope_E[i][j] << endl;
      if (ifile.eof() != 0) break;
    }

  str_tmp = str_tmp.Format("final/%s%05d_final.root", RunName, RunNum);
  TFile* f_input = new TFile(str_tmp.Data());
  if (f_input->IsZombie()) {
    cout << "open file " << str_tmp.Data() << " error!" << endl;
    delete f_input;
    return -1;
  }
  cout << "*** start process file " << str_tmp.Data() << " ********" << endl;
  ///////// initial output file format ///////////
  TTree* tr_data = (TTree*)f_input->Get("tr");
  if (tr_data == NULL) {
    cout << " Get the tree error in file" << str_tmp.Data() << endl;
    f_input->Close();
    return -1;
  }

  int nXIA;
#ifdef XIA_USED
  TClonesArray* XIA_data = new TClonesArray("MyXIAData", MAX_HITS_NUM_EVENT);
  tr_data->SetBranchAddress("nXIA", &nXIA);
  tr_data->SetBranchAddress("data_XIA", &XIA_data);
  // MyXIAData* XIA_data_tmp[XIA_Modules][XIA_CH];
#endif

  str_tmp = str_tmp.Format("./cal/%s%05d_cal.root", RunName, RunNum);
  TFile* f_output = new TFile(str_tmp.Data(), "RECREATE", "ana");

  TTree* ntree = new TTree("ana", "ana");
  //    ntree->Branch("raw", raw, "raw[53]/D");
  // ntree->Branch("ts", ts, "ts[64]/L");
  ntree->Branch("tsInner", &tsInner, "tsInner/L");
  ntree->Branch("tsOuter", &tsOuter, "tsOuter/L");
  ntree->Branch("tsAnti", &tsAnti, "tsAnti/L");
   ntree->Branch("tsdelayOuter", &tsdelayOuter, "tsdelayOuter/L");
   ntree->Branch("tsdelayAnti", &tsdelayAnti, "tsdelayAnti/L");

  // ntree->Branch("single", single, "single[48]/D");
  ntree->Branch("sum", &sum, "sum/D");
  ntree->Branch("singleInner", singleInner, "singleInner[16]/D");
  ntree->Branch("sumInner", &sumInner, "sumInner/D");
  // ntree->Branch("singleInnerFront", singleInnerFront, "singleInnerFront[8]/D");
  ntree->Branch("sumInnerFront", &sumInnerFront, "sumInnerFront/D");
  // ntree->Branch("singleInnerBack", singleInnerBack, "singleInnerBack[8]/D");
  ntree->Branch("sumInnerBack", &sumInnerBack, "sumInnerBack/D");
  //    ntree->Branch("singleOuter", singleOuter, "singleOuter[32]/D");
  ntree->Branch("sumOuter", &sumOuter, "sumOuter/D");
  // ntree->Branch("plastic", plastic, "plastic[10]/D");
  ntree->Branch("anti", &anti, "anti/D");
  ntree->Branch("pileup", &pileup, "pileup/I");
  ntree->Branch("runNumber", &runNumber, "runNumber/I");
  ntree->Branch("nhit", &nhit, "nhit/I");
    ntree->Branch("Innernhit", &Innernhit, "Innernhit/I");
  

  //	TH1F* h1_Ge = new TH1F("h1_Ge","E_Ge",10000,0,3000);
  //	TH1F* h1_Ge_vB = new TH1F("h1_Ge_vB","E_Ge veto of ",10000,0,3000);
  //	TH1F* h1_Ge_vBP = new TH1F("h1_Ge_vBP","E_Ge veto of &PS",10000,0,3000);

  //	TH1F* h1_time_diff = new TH1F("h1_time_diff","time diff Ge vs.
  //",1000,-10000,10000);

  //	h1_Ge->SetLineColor(kBlack);
  //	h1_Ge_vB->SetLineColor(kBlue);
  //	h1_Ge_vBP->SetLineColor(kRed);

  int nentries = tr_data->GetEntries();

  long tsOutertmp, tsAntitmp;
  tsOutertmp = 0;
  tsAntitmp = 0;

  for (int i = 0; i < nentries; i++) {
    if (tsOuter > 0) tsOutertmp = tsOuter;
    if (tsAnti > 0) tsAntitmp = tsAnti;

    ret = tr_data->GetEntry(i);
    if (ret < 1) {
      cout << "read entry " << i << " error!!" << endl;
      continue;
    }
    //		if(i%10000==0) printf("%9d/%9d events have been processed!
    //%3.0f%%\r",i,nentries,100.*i/nentries); 		fflush(stdout);

    for (int j = 0; j < 64; j++) {
      raw[j] = 0;
      ts[j] = 0;
    }

    for (int j = 0; j < 48; j++) {
      single[j] = 0;
    }

    for (int j = 0; j < 32; j++) {
      singleOuter[j] = 0;
    }

    for (int j = 0; j < 16; j++) {
      singleInner[j] = 0;
    }

    for (int j = 0; j < 8; j++) {
      singleInnerFront[j] = 0;
      singleInnerBack[j] = 0;
    }

    for (int j = 0; j < 10; ++j) {
      plastic[j] = 0;
    }

    E_row = 0;
    T_row = 0;
    CFD_row = 0;
    tsInner = 0;
    tsOuter = 0;
    tsAnti = 0;
    tsdelayOuter = 0;
    tsdelayAnti = 0;
    sum = 0;
    sumInner = 0;
    sumOuter = 0;
    sumInnerBack = 0;
    sumInnerFront = 0;
    anti = 0;
    pileup = 0;
    runNumber = 0;
    nhit = 0;
    Innernhit=0;

    // cout<<"XIA size:"<<nXIA<<endl;
    //       Tot_E_Cal = 0;

    for (int j = 0; j < nXIA; j++) {
      ModuleID = ((MyXIAData*)(*XIA_data)[j])->channel / 100 - 1;
      ChannelID = ((MyXIAData*)(*XIA_data)[j])->channel % 100;
      E_row = ((MyXIAData*)(*XIA_data)[j])->Energy;
      T_row = ((MyXIAData*)(*XIA_data)[j])->Event_ts;
      CFD_row = ((MyXIAData*)(*XIA_data)[j])->CFD;
      raw[ChannelID + ModuleID * 16] = E_row;
      ts[ChannelID + ModuleID * 16] = T_row + CFD_row;

      if (E_row > Ecut && ModuleID == 0) {
        if (ChannelID < 10) {
          tsAnti = ts[ChannelID + ModuleID * 16];
          plastic[ChannelID] = E_row;
          anti = anti + E_row;
        }
        
        else if(ChannelID == 14)
         {
          nhit++;
          tsOuter = ts[ChannelID + ModuleID * 16];
          single[ChannelID + 1 * 16] = E_row * Slope_E[1][ChannelID];
          sum = sum + single[ChannelID + 1 * 16];
          singleOuter[ChannelID] =single[ChannelID + 1 * 16];
          sumOuter = sumOuter + single[ChannelID + 1 * 16];
        }
        
      }
      
      
      

      if (E_row > Ecut && ModuleID == 1) {
        ModuleID = ModuleID - 1;
        single[ChannelID + ModuleID * 16] =
            E_row * Slope_E[ModuleID][ChannelID];
        sum = sum + single[ChannelID + ModuleID * 16];
        singleInner[ChannelID + ModuleID * 16] =
            single[ChannelID + ModuleID * 16];
        sumInner = sumInner + single[ChannelID + ModuleID * 16];
        if (single[ChannelID + ModuleID * 16] > 0) {
          nhit++;
          Innernhit++;
          tsInner = ts[ChannelID + (ModuleID+1) * 16];
        }
        if (ChannelID < 8) {
          singleInnerFront[ChannelID] = single[ChannelID + ModuleID * 16];
          sumInnerFront = sumInnerFront + single[ChannelID + ModuleID * 16];
        }
        if (ChannelID >= 8 && ChannelID < 16) {
          singleInnerBack[ChannelID - 8] = single[ChannelID + ModuleID * 16];
          sumInnerBack = sumInnerBack + single[ChannelID + ModuleID * 16];
        }
      }

      if (E_row > Ecut && ModuleID == 2) {
        ModuleID = ModuleID - 1;
        nhit++;
        tsOuter = ts[ChannelID + (ModuleID+1) * 16];
        single[ChannelID + ModuleID * 16] =
            E_row * Slope_E[ModuleID][ChannelID];
        sum = sum + single[ChannelID + ModuleID * 16];
        singleOuter[ChannelID + (ModuleID - 1) * 16] =single[ChannelID + ModuleID * 16];
        sumOuter = sumOuter + single[ChannelID + ModuleID * 16];
      }

      if (E_row > Ecut && ModuleID == 3) {
        ModuleID = ModuleID - 1;
        nhit++;
        tsOuter = ts[ChannelID + (ModuleID+1) * 16];
        single[ChannelID + ModuleID * 16] =
            E_row * Slope_E[ModuleID][ChannelID];
        sum = sum + single[ChannelID + ModuleID * 16];
        singleOuter[ChannelID + (ModuleID - 1) * 16] =single[ChannelID + ModuleID * 16];
        sumOuter = sumOuter + single[ChannelID + ModuleID * 16];
      }

      pileup = ((MyXIAData*)(*XIA_data)[j])->finish_code;
    }

    if (i % 100000 == 0) {
      printf("%ld/%ld %.2f%\r", i, nentries, double(i) / nentries * 100);
      fflush(stdout);
    }

    if (tsInner > 0) {
      tsdelayOuter = tsInner - tsOutertmp;
      tsdelayAnti = tsInner - tsAntitmp;
    }

    runNumber = RunNum;
    //           cout<<sum<<endl;
   if (sum < 100) continue;
    ntree->Fill();
  }
  printf("\n");
  f_output->cd();
  f_output->Write();
  f_input->Close();
  f_output->Close();
  cout << "**** finish *******" << endl;

  return 0;
}
////////////////////////
int main(int argc, char* argv[]) {
  time_t t1, t2;
  int min = 304, max = 304;
  char RunName[256] = "run";
  t1 = time(NULL);
  /////////////init global parameters
  /// start read and decoder data
  cout << "filename:" << endl;
  cin >> RunName;
  cout << "run numbers, min, max:" << endl;
  cin >> min >> max;
  for (int i = min; i <= max; i++) {
    Data_Ana(RunName, i);
    t2 = time(NULL);
    cout << "time diff:" << t2 - t1 << endl;
    t1 = t2;
  }  // end loop from mix to max num

  //////////finish   //////////////
  cout << "finish all task!!!" << endl;
}
