#include <iostream>
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

struct Event
{
    Double_t raw[48];
    Double_t timestamp;
};

bool compareEvents(const Event &a, const Event &b)
{
    return a.timestamp < b.timestamp;
}

void new_convert()
{
    Double_t raw[48];
    Double_t timestamp;
    Double_t Ecut = 100;
    Double_t E_raw[48];
    Double_t singleInner[16];
    Double_t singleOuter[32];
    Double_t sumInner;
    Double_t sumOuter;
    Double_t sum;
    int nhit;

    TString input_file = "out.root";
    TFile *f_input = new TFile(input_file.Data());
    if (f_input->IsZombie())
    {
        cout << "open file " << input_file.Data() << " error!" << endl;
        delete f_input;
        return;
    }
    cout << "*** start process file " << input_file.Data() << " ********" << endl;

    TTree *tr_data = (TTree *)f_input->Get("tr");
    if (tr_data == NULL)
    {
        cout << " Get the tree error in file " << input_file.Data() << endl;
        f_input->Close();
        return;
    }

    vector<Event> events;
    Double_t times;
    tr_data->SetBranchAddress("de", raw);
    tr_data->SetBranchAddress("timestamp", &times);

    int nentries = tr_data->GetEntries();
    for (int i = 0; i < nentries; i++)
    {
        tr_data->GetEntry(i);
        Event event;
        for (int j = 0; j < 48; j++)
        {
            event.raw[j] = raw[j];
        }
        event.timestamp = times;
        events.push_back(event);

        // 更新进度条
        cout << "\rReading entries: " << fixed << setprecision(2)
             << (static_cast<double>(i + 1) / nentries) * 100 << "% complete" << flush;
    }
    cout << endl;

    // 按 timestamp 排序
    sort(events.begin(), events.end(), compareEvents);

    // 合并事件
    vector<Event> merged_events;
    Event current_event;
    for (size_t i = 0; i < events.size(); i++)
    {
        double flag = 0;
        double deltat = events[i].timestamp - current_event.timestamp;
        if (deltat < 100.)
            flag = 1;
        if (i == 0 || (events[i].timestamp - current_event.timestamp) >= 150)
        {
            if (i != 0)
            {
                merged_events.push_back(current_event);
            }
            current_event = events[i];
        }
        else
        {
            for (int j = 0; j < 48; j++)
            {
                if (flag == 1)
                {
                    events[i].raw[j] = events[i].raw[j] * exp(-deltat / 100.);
                }
                current_event.raw[j] += events[i].raw[j];
            }
        }

        // 更新进度条
        cout << "\rMerging events: " << fixed << setprecision(2)
             << (static_cast<double>(i + 1) / events.size()) * 100 << "% complete" << flush;
    }
    merged_events.push_back(current_event);
    cout << endl;

    // 写入新文件
    TString output_file = "eff.root";
    TFile *f_output = new TFile(output_file.Data(), "RECREATE", "ana");
    if (f_output->IsZombie())
    {
        cout << "open file " << output_file.Data() << " error!" << endl;
        f_input->Close();
        return;
    }

    TTree *ntree = new TTree("ana", "ana");
    ntree->Branch("E_raw", E_raw, "E_raw[48]/D");
    ntree->Branch("singleInner", singleInner, "singleInner[16]/D");
    ntree->Branch("times", &times, "ts/D");
    ntree->Branch("sumInner", &sumInner, "sumInner/D");
    ntree->Branch("sumOuter", &sumOuter, "sumOuter/D");
    ntree->Branch("sum", &sum, "sum/D");
    ntree->Branch("nhit", &nhit, "nhit/I");

    for (const auto &event : merged_events)
    {
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
            E_raw[j] = event.raw[j];
        }
        for (int j = 0; j < 16; j++)
        {
            if (event.raw[j] > Ecut)
            {
                singleInner[j] = event.raw[j];
                sumInner += event.raw[j];
                sum += event.raw[j];
                nhit++;
            }
        }
        for (int j = 16; j < 48; j++)
        {
            if (event.raw[j] > Ecut)
            {
                sumOuter += event.raw[j];
                sum += event.raw[j];
                nhit++;
            }
        }
        times = event.timestamp;
        ntree->Fill();

        // 更新进度条
        static size_t processed = 0;
        cout << "\rWriting to file: " << fixed << setprecision(2)
             << (static_cast<double>(++processed) / merged_events.size()) * 100 << "% complete" << flush;
    }
    cout << endl;

    f_output->Write();
    f_input->Close();
    f_output->Close();
    delete f_input;
    delete f_output;
    cout << "**** finish *******" << endl;
}