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
int Data_Ana(char *RunName, int RunNum)
{
  //   TRandom3 r(0);

  float Ecut = 10;
  double Slope_E[XIA_Modules][XIA_CH];
  double alpha[3];
  //   double Offset_E[XIA_Modules][XIA_CH];
  int ret;
  int ModuleID, ChannelID;
  int E_row, CFD_row;
  long T_row;
  //   double E_Cal;

  double raw[48];
  long ts[48];
  long tscoin[4];
  long tsInner, tsOuter;
  double single[48]; // cal data, InnerBGO(0-15), OuterBGO(16-48);
  double sum;
  double singleInner[16];
  double sumInner;
  double singleInnerFront[8];
  double sumInnerFront;
  double singleInnerBack[8];
  double sumInnerBack;
  double singleOuter[32];
  double sumOuter;
  int pileup; // pile-up
  int runNumber;
  int nhit, Innernhit;

  TString str_tmp;
  ifstream ifile;

  ifile.open("cali_factor.txt");
  ifile >> alpha[0] >> alpha[1] >> alpha[2];
  ifile.close();

  ifile.open("alig_factor.txt");
  // while(true)
  for (int i = 0; i < XIA_Modules - 1; i++)
    for (int j = 0; j < XIA_CH; j++)
    {
      ifile >> Slope_E[i][j];
      // Slope_E[i][j]=Slope_E[i][j]*0.985;
      // cout << "Slope_E[" << i << "][" << j << "]" << Slope_E[i][j] << endl;
      if (ifile.eof() != 0)
        break;
    }

  str_tmp = str_tmp.Format("final/%s%05d_final.root", RunName, RunNum);
  TFile *f_input = new TFile(str_tmp.Data());
  if (f_input->IsZombie())
  {
    cout << "open file " << str_tmp.Data() << " error!" << endl;
    delete f_input;
    return -1;
  }
  cout << "*** start process file " << str_tmp.Data() << " ********" << endl;
  ///////// initial output file format ///////////
  TTree *tr_data = (TTree *)f_input->Get("tr");
  if (tr_data == NULL)
  {
    cout << " Get the tree error in file" << str_tmp.Data() << endl;
    f_input->Close();
    return -1;
  }

  int nXIA;
#ifdef XIA_USED
  TClonesArray *XIA_data = new TClonesArray("MyXIAData", MAX_HITS_NUM_EVENT);
  tr_data->SetBranchAddress("nXIA", &nXIA);
  tr_data->SetBranchAddress("data_XIA", &XIA_data);
  // MyXIAData* XIA_data_tmp[XIA_Modules][XIA_CH];
#endif

  str_tmp = str_tmp.Format("cal/%s%05d_cal.root", RunName, RunNum);
  TFile *f_output = new TFile(str_tmp.Data(), "RECREATE", "ana");

  TTree *ntree = new TTree("ana", "ana");
  ntree->Branch("raw", raw, "raw[48]/D");
  ntree->Branch("ts", ts, "ts[48]/L");
  ntree->Branch("tscoin", tscoin, "tscoin[4]/L");
  ntree->Branch("tsInner", &tsInner, "tsInner/L");
  ntree->Branch("tsOuter", &tsOuter, "tsOuter/L");
  ntree->Branch("single", single, "single[48]/D");
  ntree->Branch("sum", &sum, "sum/D");
  ntree->Branch("singleInner", singleInner, "singleInner[16]/D");
  ntree->Branch("sumInner", &sumInner, "sumInner/D");
  ntree->Branch("singleInnerFront", singleInnerFront, "singleInnerFront[8]/D");
  ntree->Branch("sumInnerFront", &sumInnerFront, "sumInnerFront/D");
  ntree->Branch("singleInnerBack", singleInnerBack, "singleInnerBack[8]/D");
  ntree->Branch("sumInnerBack", &sumInnerBack, "sumInnerBack/D");
  ntree->Branch("singleOuter", singleOuter, "singleOuter[32]/D");
  ntree->Branch("sumOuter", &sumOuter, "sumOuter/D");
  ntree->Branch("pileup", &pileup, "pileup/I");
  ntree->Branch("runNumber", &runNumber, "runNumber/I");
  ntree->Branch("nhit", &nhit, "nhit/I");
  ntree->Branch("Innernhit", &Innernhit, "Innernhit/I");

  int nentries = tr_data->GetEntries();

  long tsOutertmp;
  tsOutertmp = 0;

  for (int i = 0; i < nentries; i++)
  {
    if (tsOuter > 0)
      tsOutertmp = tsOuter;

    ret = tr_data->GetEntry(i);
    if (ret < 1)
    {
      cout << "read entry " << i << " error!!" << endl;
      continue;
    }

    for (int j = 0; j < 48; j++)
    {
      raw[j] = 0;
      ts[j] = 0;
    }
    for (int j = 0; j < 4; j++)
    {
      tscoin[j] = 0;
    }

    for (int j = 0; j < 48; j++)
    {
      single[j] = 0;
    }

    for (int j = 0; j < 32; j++)
    {
      singleOuter[j] = 0;
    }

    for (int j = 0; j < 16; j++)
    {
      singleInner[j] = 0;
    }

    for (int j = 0; j < 8; j++)
    {
      singleInnerFront[j] = 0;
      singleInnerBack[j] = 0;
    }

    E_row = 0;
    T_row = 0;
    CFD_row = 0;
    tsInner = 0;
    tsOuter = 0;
    sum = 0;
    sumInner = 0;
    sumOuter = 0;
    sumInnerBack = 0;
    sumInnerFront = 0;
    pileup = 0;
    runNumber = 0;
    nhit = 0;
    Innernhit = 0;

    for (int j = 0; j < nXIA; j++)
    {
      ModuleID = ((MyXIAData *)(*XIA_data)[j])->channel / 100 - 1;
      ChannelID = ((MyXIAData *)(*XIA_data)[j])->channel % 100;
      E_row = ((MyXIAData *)(*XIA_data)[j])->Energy;
      T_row = ((MyXIAData *)(*XIA_data)[j])->Event_ts;
      CFD_row = ((MyXIAData *)(*XIA_data)[j])->CFD;
      ts[ChannelID + ModuleID * 16] = T_row + CFD_row;

      if (E_row > Ecut && ModuleID == 0)
      {
        raw[ChannelID + ModuleID * 16] = E_row;
        single[ChannelID + ModuleID * 16] =
            E_row * Slope_E[ModuleID][ChannelID];
        single[ChannelID + ModuleID * 16] =
            single[ChannelID + ModuleID * 16] * single[ChannelID + ModuleID * 16] * alpha[0] + single[ChannelID + ModuleID * 16] * alpha[1] + alpha[2];
        sum = sum + single[ChannelID + ModuleID * 16];
        singleInner[ChannelID + ModuleID * 16] =
            single[ChannelID + ModuleID * 16];
        sumInner = sumInner + single[ChannelID + ModuleID * 16];
        if (single[ChannelID + ModuleID * 16] > 0)
        {
          nhit++;
          Innernhit++;
          tsInner = ts[ChannelID + (ModuleID) * 16];
        }
        if (ChannelID < 8)
        {
          singleInnerFront[ChannelID] = single[ChannelID + ModuleID * 16];
          sumInnerFront = sumInnerFront + single[ChannelID + ModuleID * 16];
        }
        if (ChannelID >= 8 && ChannelID < 16)
        {
          singleInnerBack[ChannelID - 8] = single[ChannelID + ModuleID * 16];
          sumInnerBack = sumInnerBack + single[ChannelID + ModuleID * 16];
        }
      }

      if (E_row > Ecut && ModuleID == 1)
      {
        nhit++;
        tsOuter = ts[ChannelID + (ModuleID) * 16];
        raw[ChannelID + ModuleID * 16] = E_row;
        single[ChannelID + ModuleID * 16] =
            E_row * Slope_E[ModuleID][ChannelID];
        single[ChannelID + ModuleID * 16] =
            single[ChannelID + ModuleID * 16] * single[ChannelID + ModuleID * 16] * alpha[0] + single[ChannelID + ModuleID * 16] * alpha[1] + alpha[2];
        sum = sum + single[ChannelID + ModuleID * 16];
        singleOuter[ChannelID + (ModuleID - 1) * 16] = single[ChannelID + ModuleID * 16];
        sumOuter = sumOuter + single[ChannelID + ModuleID * 16];
      }

      if (E_row > Ecut && ModuleID == 2)
      {
        nhit++;
        tsOuter = ts[ChannelID + (ModuleID) * 16];
        raw[ChannelID + ModuleID * 16] = E_row;
        single[ChannelID + ModuleID * 16] =
            E_row * Slope_E[ModuleID][ChannelID];
        single[ChannelID + ModuleID * 16] =
            single[ChannelID + ModuleID * 16] * single[ChannelID + ModuleID * 16] * alpha[0] + single[ChannelID + ModuleID * 16] * alpha[1] + alpha[2];
        sum = sum + single[ChannelID + ModuleID * 16];
        singleOuter[ChannelID + (ModuleID - 1) * 16] = single[ChannelID + ModuleID * 16];
        sumOuter = sumOuter + single[ChannelID + ModuleID * 16];
      }

      pileup = ((MyXIAData *)(*XIA_data)[j])->finish_code;
    }

    for (int j = 0, k = 0; j < 8; j += 2, k++)
    {
      int i = j + 12 > 15 ? j - 8 : j; // j + 12 > 15 ture=j - 8 else=j
      if (abs(singleInner[j] - 511) < 78 && abs(singleInner[i + 12] - 511) < 78 && sumOuter < 200)
      {
        tscoin[k] = ts[j + 16];
      }
    }

    if (i % 100000 == 0)
    {
      printf("%ld/%ld %.2f%\r", i, nentries, double(i) / nentries * 100);
      fflush(stdout);
    }

    runNumber = RunNum;
    // cout<<sum<<endl;
    if (sum < 50)
      continue;
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
int main(int argc, char *argv[])
{
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
  for (int i = min; i <= max; i++)
  {
    Data_Ana(RunName, i);
    t2 = time(NULL);
    cout << "time diff:" << t2 - t1 << endl;
    t1 = t2;
  } // end loop from mix to max num

  //////////finish   //////////////
  cout << "finish all task!!!" << endl;
}
