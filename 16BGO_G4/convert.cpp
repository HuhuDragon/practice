#include "iostream"
#include <iomanip>
#include <stdlib.h>
#include "TFile.h"
#include "TTree.h"
#include "fstream"
#include "sstream"
#include "TBranch.h"
#include "TString.h"
#include "TMutex.h"
#include "TRandom3.h"
#include "TH1.h"
#include "TH2.h"

using namespace std;

void convert()
{
    Double_t Ecut = 100;
    Double_t raw[48];
    Double_t E_raw[48];
    Double_t singleInner[16];
    Double_t singleOuter[32];
    Double_t ts;
    Double_t times;
    Double_t sumInner;
    Double_t sumOuter;
    Double_t sum;
    Int_t ret;
    int nhit;
    TH1F *Inner;
    TH1F *Outer;

    TString str_tmp;
    str_tmp = str_tmp.Format("out.root");
    TFile *f_input = new TFile(str_tmp.Data());
    if (f_input->IsZombie())
    {
        cout << "open file " << str_tmp.Data() << " error!" << endl;
        delete f_input;
    }
    cout << "*** start process file " << str_tmp.Data() << " ********" << endl;
    TTree *tr_data = (TTree *)f_input->Get("tr");
    if (tr_data == NULL)
    {
        cout << " Get the tree error in file" << str_tmp.Data() << endl;
        f_input->Close();
    }
    tr_data->SetBranchAddress("de", raw);
    tr_data->SetBranchAddress("timestamp", &times);

    str_tmp = str_tmp.Format("eff.root"); // 输出文件
    TFile *f_output = new TFile(str_tmp.Data(), "RECREATE", "ana");

    TTree *ntree = new TTree("ana", "ana");
    ntree->Branch("E_raw", E_raw, "E_raw[48]/D");
    ntree->Branch("singleInner", singleInner, "singleInner[16]/D");
    // ntree->Branch("singleOuter", singleOuter, "singleOuter[32]/D");
    ntree->Branch("times", &times, "ts/D");
    ntree->Branch("sumInner", &sumInner, "sumInner/D");
    ntree->Branch("sumOuter", &sumOuter, "sumOuter/D");
    ntree->Branch("sum", &sum, "sum/D");
    ntree->Branch("nhit", &nhit, "nhit/I");

    int nentries = tr_data->GetEntries();
    for (int i = 0; i < nentries; i++)
    {
        // cout<<"read entry "<<i<<endl;
        if (i % 1000 == 0)
        {
            printf("%9d/%9d %3.0f%%\r", i, nentries, 100.0 * (double)i / (double)nentries);
            fflush(stdout);
        }
        ret = tr_data->GetEntry(i);
        if (ret < 1)
        {
            cout << "read entry " << i << " error!!" << endl;
            continue;
        }
        sumInner = 0;
        sumOuter = 0;
        sum = 0;
        nhit = 0;
        for (int j = 0; j < 16; j++)
        {
            singleInner[j] = 0;
        }
        for (int j = 0; j < 48; j++)
        {
            E_raw[j] = raw[j];
            // Ts[j] = ts[j];
        }
        for (int j = 0; j < 16; j++)
        {
            if (raw[j] > Ecut)
            {
                singleInner[j] = raw[j];
            }
        }
        for (int j = 0; j < 48; j++)
        {
            if (E_raw[j] > Ecut && j < 16)
            {
                sumInner += E_raw[j];
            }

            else if (E_raw[j] > Ecut && j > 15)
            {
                sumOuter += E_raw[j];
            }

            if (E_raw[j] > Ecut)
            {
                sum += E_raw[j];
                ts = times;
                nhit++;
            }
        }

        ntree->Fill();
    }
    f_output->Write();
    f_input->Close();
    f_output->Close();
    cout << "**** finish *******" << endl;
}
