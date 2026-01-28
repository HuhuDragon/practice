#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <map>
#include <queue>
#include <array>

#include "TROOT.h"
#include "TMutex.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH1.h"
#include "TF1.h"
#include "TFile.h"
#include "TBranch.h"
//#include "TSpectrum.h"

//data format
#include "setup.h"
#include "XIA_Decode.h"
#include "TRandom3.h"

using namespace std;

/////////////////
int Data_Ana(char* RunName, int RunNum)
{
 //   TRandom3 r(0);

    float Ecut = 100;
    double Slope_E[XIA_Modules][XIA_CH];
    double Offset_E[XIA_Modules][XIA_CH];
    int ret;
    int ModuleID, ChannelID;
    int E_row, CFD_row;
    long T_row;
 //   double E_Cal;

    double raw[54];
    double ts[54];
    double single[48];       //cal data, InnerBGO(0-15), OuterBGO(16-48);
    double sum;
    double singleInner[16];
    double sumInner;
    double tsCoin[4];
    int nhit[3];
    
    double singleInnerfront[8];
    double singleInnerback[8];
    double sumInnerfront;
    double sumInnerback;
    double singleOuter[32];
    double sumOuter;
    double plastic[4];
    double anti;
    int pileup;   //pile-up
    int runNumber;

    TString str_tmp;
    ifstream ifile;
    ifile.open("Cal.txt");
    // while(true)
    for(int i=0;i<XIA_Modules;i++)
        for(int j=0;j<XIA_CH;j++)
        {
            //ifile>>Slope_E[i][j];
            ifile>>Slope_E[i][j] >> Offset_E[i][j];
            // cout<<"Slope_E[" << i << "][" << j << "]" << Slope_E[i][j] << endl;
            // ifile>>Slope_E[i][j]>>Offset_E[i][j];
            if(ifile.eof()!=0) break;
    }

    str_tmp = str_tmp.Format("final/%s%05d_final.root", RunName, RunNum);
    TFile* f_input = new TFile(str_tmp.Data());
    if (f_input->IsZombie())
    {
        cout << "open file " << str_tmp.Data() << " error!" << endl;
        delete f_input;
        return -1;
    }
    cout << "*** start process file " << str_tmp.Data() << " ********" << endl;
    ///////// initial output file format ///////////
    TTree* tr_data = (TTree*)f_input->Get("tr");
    if (tr_data == NULL)
    {
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

    TTree *ntree = new TTree("ana", "ana");
    ntree->Branch("raw", raw, "raw[54]/D");
    ntree->Branch("ts", ts, "ts[54]/D");
    ntree->Branch("single", single, "single[48]/D");
    ntree->Branch("sum", &sum, "sum/D");
    ntree->Branch("singleInner", singleInner, "singleInner[16]/D");
    ntree->Branch("singleInnerfront", singleInnerfront, "singleInnerfront[8]/D");
    ntree->Branch("singleInnerback", singleInnerback, "singleInnerback[8]/D");
    ntree->Branch("sumInnerfront", &sumInnerfront, "sumInnerfront/D");
    ntree->Branch("sumInnerback", &sumInnerback, "sumInnerback/D");    
    
    ntree->Branch("sumInner", &sumInner, "sumInner/D");
    ntree->Branch("singleOuter", singleOuter, "singleOuter[32]/D");
    ntree->Branch("sumOuter", &sumOuter, "sumOuter/D");
    ntree->Branch("plastic", plastic, "plastic[4]/D");
    ntree->Branch("anti", &anti, "anti/D");
    ntree->Branch("pileup", &pileup, "pileup/I");
    ntree->Branch("runNumber", &runNumber, "runNumber/I");
    ntree->Branch("tsCoin", tsCoin, "tsCoin[4]/D");
    ntree->Branch("nhit", nhit, "nhit[3]/I");

//	TH1F* h1_Ge = new TH1F("h1_Ge","E_Ge",10000,0,3000);
//	TH1F* h1_Ge_vB = new TH1F("h1_Ge_vB","E_Ge veto of ",10000,0,3000);
//	TH1F* h1_Ge_vBP = new TH1F("h1_Ge_vBP","E_Ge veto of &PS",10000,0,3000);
	
//	TH1F* h1_time_diff = new TH1F("h1_time_diff","time diff Ge vs. ",1000,-10000,10000);
	
//	h1_Ge->SetLineColor(kBlack);
//	h1_Ge_vB->SetLineColor(kBlue);
//	h1_Ge_vBP->SetLineColor(kRed);

    int nentries = tr_data->GetEntries();
 
    for (int i = 0; i < nentries; i++)
    {
        ret = tr_data->GetEntry(i);
        if (ret < 1)
        {
            cout << "read entry " << i << " error!!" << endl;
            continue;
        }
//		if(i%10000==0) printf("%9d/%9d events have been processed! %3.0f%%\r",i,nentries,100.*i/nentries);
//		fflush(stdout);

 	for (int j = 0; j < 54; j++)
        {
            raw[j] = 0;
            ts[j] = 0;
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
        
    for (int j = 0; j < 4; j++)
        {
            tsCoin[j] = 0;
        }
    for (int j = 0; j < 3; j++)
        {
            nhit[j] = 0;
        }
        
  for (int j = 0; j < 8; j++)
        {
            singleInnerfront[j] = 0;
            singleInnerback[j] = 0;
        }

    for (int j = 0; j < 4; ++j)
        {
            plastic[j] = 0;
        }

        E_row = 0;
        T_row = 0;
        CFD_row = 0;
        sum = 0;
        sumInner = 0;
        sumOuter = 0;
        anti = 0;
        pileup = 0;
        runNumber = 0;

        // cout<<"XIA size:"<<nXIA<<endl;
//       Tot_E_Cal = 0;
	
        for (int j = 0; j < nXIA; j++)
        {
            ModuleID = ((MyXIAData*)(*XIA_data)[j])->channel / 100 - 1;
            ChannelID = ((MyXIAData*)(*XIA_data)[j])->channel % 100;
            E_row = ((MyXIAData*)(*XIA_data)[j])->Energy;
            T_row = ((MyXIAData*)(*XIA_data)[j])->Event_ts;
            CFD_row = ((MyXIAData*)(*XIA_data)[j])->CFD;
            raw[ChannelID+ModuleID*16] = E_row;
            ts[ChannelID+ModuleID*16] = T_row + CFD_row;

            if (E_row > Ecut && ModuleID == 0)
            {
                single[ChannelID + ModuleID * 16] = E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                sum = sum + E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                singleInner[ChannelID + ModuleID*16] = E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                sumInner = sumInner + E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];        
                if (ChannelID < 8)
                    {
                        singleInnerfront[ChannelID] = E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                        sumInnerfront = sumInnerfront + E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                    }
                if (ChannelID >= 8 && ChannelID < 16)
                    {
                        singleInnerback[ChannelID - 8] = E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                        sumInnerback = sumInnerback + E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                    }
            }

            if (E_row > Ecut && ModuleID == 1)
            {
                single[ChannelID + ModuleID * 16] = E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                sum = sum + E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                singleOuter[ChannelID + (ModuleID - 1) * 16] = E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                sumOuter = sumOuter + E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];            
            }

            if (E_row > Ecut && ModuleID == 2)
            {
                single[ChannelID + ModuleID * 16] = E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                sum = sum + E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                singleOuter[ChannelID + (ModuleID - 1) * 16] = E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                sumOuter = sumOuter + E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];    
            }

            if (E_row > Ecut && ModuleID == 3)
            {
                if (ChannelID == 0)
                {
                    single[29] = E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                    sum = sum + E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                    singleOuter[13] = E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                    sumOuter = sumOuter + E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                }

                if (ChannelID == 1)
                {
                    single[41] = E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                    sum = sum + E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                    singleOuter[25] = E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                    sumOuter = sumOuter + E_row * Slope_E[ModuleID][ChannelID] + Offset_E[ModuleID][ChannelID];
                }

                else if(ChannelID < 6)
                {
                    plastic[ChannelID -2] = E_row;
                    anti = anti + E_row;
                }
            }

            pileup = ((MyXIAData*)(*XIA_data)[j])->finish_code;
        }
        for (int j = 0,k = 0; j < 8; j+=2,k++)
        {
            int i = j + 12 > 15 ? j - 8 : j;// j + 12 > 15 ture=j - 8 else=j 
            if (abs(singleInner[j]-511)<78 &&abs(singleInner[i + 12]-511)<78&&anti<200&&sumOuter<200)
            {
                tsCoin[k] = ts[j];
            }
        }

            for (int i=0;i<48;i++)
            {
                if(single[i]>0&&i<16)
                {
                    nhit[0]+=1;
                }
                if(single[i]>0&&i>15)
                {
                    nhit[1]+=1;
                }          
            }
            for (int i=0;i<4;i++)
            {
            if(plastic[i]>0)
                {
                nhit[2]+=1;
                }
            }


        runNumber = RunNum;
//           cout<<sum<<endl;
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
int main(int argc, char* argv[])
{
    time_t t1, t2;
    int min = 304, max = 304;
    char RunName[256] = "run";
    t1 = time(NULL);
    /////////////init global parameters
    ///start read and decoder data
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
    }// end loop from mix to max num

    //////////finish   //////////////
    cout << "finish all task!!!" << endl;
}
