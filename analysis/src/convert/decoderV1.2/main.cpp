#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <map>
#include <queue>
#include <array>

#include "TMutex.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH1.h"
#include "TFile.h"

//data format
#include "setup.h"
#include "XIA_Decode.h"

using namespace std;

/// function define  ///
int Data_Decoder(char* RunName, int RunNum);
int MergyVMEData(struct VME_DATA* dest, struct ADC_DATA *src, int ModNum);
int MergyVMEData(struct VME_DATA* dest, struct TDC_DATA *src, int ModNum);
Long64_t GetMinElement(Long64_t* data, int ndata);


//////////////
// for output rootfile
#ifdef XIA_USED
void XIA_Data_Decoder_thread(void *argv);
TH1F* hist_XIA_E[XIA_Modules][XIA_CH];
TTree* tr_XIA[XIA_Modules][XIA_CH];
MyXIAData *myXIAdata[XIA_Modules][XIA_CH];
int XIA_Modules_Type[XIA_Modules] = {100, 100, 100, 100};
#endif

#ifdef VME_USED
void VME_Data_Decoder_thread(void *argv);
void ADC_Data_Decoder_thread(void *argv, void *data);
void TDC_Data_Decoder_thread(void* argv, void *data);

TH1F* hist_ADC[ADC_Modules][ADC_CH];
TTree* tr_ADC[ADC_Modules];
ADC_DATA adc_data[ADC_Modules];

TH1F *hist_TDC[TDC_Modules][TDC_CH];
TTree* tr_TDC[TDC_Modules];
TDC_DATA tdc_data[TDC_Modules];

int VME_TS_CORR[ADC_Modules + TDC_Modules] = {850, 850}; //in unit ns, sync with XIA system
#endif

// for ts sync
void Data_TS_Sort_thread(void *argv); //
#define   Total_TS_Num (ADC_Modules+TDC_Modules+XIA_Modules)
#define   Total_Tree_Num (ADC_Modules+TDC_Modules+XIA_Modules*XIA_CH)
Long64_t TS_curr[Total_TS_Num];
TMutex *fMutext_Read;
//Long64_t ts_maximun;

time_t t1, t2;

//////////
int MergyVMEData(struct VME_DATA* dest, struct ADC_DATA *src, int ModNum)
{
    dest->ts[ModNum + TDC_Modules] = src->ts;
    memcpy(&dest->adc[ModNum][0], src->data, sizeof(Int_t)*ADC_CH);
    return 0;
}
int MergyVMEData(struct VME_DATA* dest, struct TDC_DATA *src, int ModNum)
{
    dest->ts[ModNum] = src->ts;
    memcpy(&dest->tdc[ModNum][0], src->data, sizeof(Int_t)*TDC_CH);
    return 0;
}
// get the minimun element in a Long64_t array
Long64_t GetMinElement(Long64_t* data, int ndata)
{
    if (data == NULL)
        return -2;
    Long64_t ret = FLAG_TS_FINISH + 1;
    for (int i = 0; i < ndata; i++)
    {
        if (data[i] > 0 && data[i] < ret)
            ret = data[i];
    }
    if (ret == FLAG_TS_FINISH + 1)
        return -1;
    return ret;
}
/////////////////
#ifdef XIA_USED
void XIA_Data_Decoder_thread(void *argv)
{
    int i, j;
    TString str_tmp, str_tmp1;
    //// initial some point and address for input data
    TTree* tr_data = (TTree*) argv;
    struct Entry entry_XIA;
    ((TBranch*)tr_data->GetBranch("XIA0"))->SetAddress(&entry_XIA);

    MyXIADecode* myXIA_decoder[XIA_Modules];
    for (i = 0; i < XIA_Modules; i++)
    {
#ifdef XIA_USE_EXT_TS
        myXIA_decoder[i] = new MyXIADecode(XIA_Modules_Type[i], ADC_TS_UNIT);
#else
        myXIA_decoder[i] = new MyXIADecode(XIA_Modules_Type[i]);
#endif
    }
    //start decode  /////////////
    UInt_t percent = 0;
    int Total_Length = 0;
    int Decode_Length = 0;
    int module_id;
    int ch;

    int nentry = tr_data->GetEntries();
    int readnum;
    for (i = 0; i < nentry; i++)
    {
        //show percentage completed:
        if (i >= 0.1 * percent * nentry - 1)
        {
            str_tmp = str_tmp.Format("%3.0f%% completed for XIA tree", 10.0 * percent);
            cout << str_tmp.Data() << endl;
            percent++;
        }
        /// for entry
        fMutext_Read->Lock();
        readnum = tr_data->GetEntry(i);
        fMutext_Read->UnLock();
        if (readnum < 1)
        {
            cout << "get XIA entry " << i << " error!" << endl;
            continue;
        }

        Total_Length = entry_XIA.nWord - 1;
        Decode_Length = 0;
        module_id = entry_XIA.data[0];
        if (module_id > XIA_Modules )
        {
            cout << "the Module id " << module_id << " is larger than pre_define num, plz check!!" << endl;
            continue;
        }
        do {
            /// decode data, one time for one event;
            Decode_Length += myXIA_decoder[module_id - 1]->Decode(&entry_XIA.data[1 + Decode_Length], Total_Length - Decode_Length);
            if (myXIA_decoder[module_id - 1]->GetEventStatus() == 1)
            {
                ch = myXIA_decoder[module_id - 1]->GetCh();
                hist_XIA_E[module_id - 1][ch]->Fill(myXIA_decoder[module_id - 1]->GetE());
                myXIA_decoder[module_id - 1]->GetData(myXIAdata[module_id - 1][ch]);
                myXIAdata[module_id - 1][ch]->SetModId(module_id);
                /// try mutex status
                tr_XIA[module_id - 1][ch]->Fill();

                //    fMutext_Read->Lock();
#ifdef XIA_USE_EXT_TS
                TS_curr[ADC_Modules + TDC_Modules + module_id - 1] = myXIAdata[module_id - 1][ch]->Ext_ts;
#else
                TS_curr[ADC_Modules + TDC_Modules + module_id - 1] = myXIAdata[module_id - 1][ch]->Event_ts;
#endif
                //  fMutext_Read->UnLock();
            }
        } while (Decode_Length < Total_Length); //end decode one entry
        //// record the latest Ts data
    }// end loop all entry
    //////
    for (i = ADC_Modules + TDC_Modules; i < Total_TS_Num; i++)
    {

        cout << "Ts for XIA[" << i - ADC_Modules - TDC_Modules << "]:" << TS_curr[i] << endl;
        //fMutext_Read->Lock();
        // if(TS_curr[i]>ts_maximun)
        //   ts_maximun=TS_curr[i];
        // TS_curr[i]=FLAG_TS_FINISH;
        //fMutext_Read->UnLock();
    }
    for (i = 0; i < XIA_Modules; i++)
        delete myXIA_decoder[i];
    cout << "XIA decode have finished" << endl;
}
#endif

/////////// VME data decode thread  //////////////
//
#ifdef VME_USED
void ADC_Data_Decoder_thread(void *argv, void* buffer)
{
    struct Entry *entry_data = (struct Entry*) argv;
    Int_t i, j, k;
    UInt_t module_id, module_ch;
    UInt_t module_sig, module_sub_sig;
    Long64_t ts_tmp;
    Int_t  data_tmp;

    ////////// start decode ADC value
    module_id = 0;
    for (k = 0; k < entry_data->nWord; k++)
    {
        //Get ADC signature:
        module_sig = ((entry_data->data[k] & 0xC0000000) >> 30);
        switch (module_sig)
        {
        ////// event header
        case 1:
            module_id = ((entry_data->data[k] & 0x00FF0000) >> 16);
            if (module_id <= (ADC_Modules) && module_id > 0)
            {
                memset(&adc_data[module_id - 1], 0, sizeof(ADC_DATA));
                continue;
            }
            else
            {
                // if wrong id, discard this event data
                module_id = 0;
                while (((entry_data->data[k] & 0xC0000000) >> 30) != 3)
                {
                    k++;
                    if (k >= entry_data->nWord)
                        break;
                }
            }
            break;
        ///// event data ///////////
        case 0:
            if (module_id == 0 )
                continue;
            module_sub_sig = ((entry_data->data[k] & 0x3FE00000) >> 21);
            /// data
            if (module_sub_sig == 0x20)
            {
                module_ch = ((entry_data->data[k] & 0x001F0000) >> 16);
                //add the ADC data.
                if (module_ch < ADC_CH)
                {
                    data_tmp = entry_data->data[k] & 0x03FFF;
                    hist_ADC[module_id - 1][module_ch]->Fill(data_tmp);
                    adc_data[module_id - 1].data[module_ch] = data_tmp;
                }
            }
            /// extended timestamp
            else if (module_sub_sig == 0x24)
                ts_tmp = entry_data->data[k] & 0xFFFF;
            break;
        /////// trailer
        case 3:
            if (module_id == 0)
                continue;
            ts_tmp = ((ts_tmp << 30) + (entry_data->data[k] & 0x3FFFFFFF)) * ADC_TS_UNIT + VME_TS_CORR[module_id - 1]; //get the 46bits ts
            if (ts_tmp >= TS_curr[module_id - 1] && ((ts_tmp - TS_curr[module_id - 1] ) < MAX_EVENT_TS_DIFF))
                //if(ts_tmp >= TS_curr[module_id-1])
            {
                adc_data[module_id - 1].ts = ts_tmp;
                //update adc queue
                tr_ADC[module_id - 1]->Fill();
                //update TS_curr
                // fMutext_Read->Lock();
                TS_curr[module_id - 1] = ts_tmp;
                //    fMutext_Read->UnLock();
            }
            else
            {
                cout << "ADC TS error" << endl;
                module_id = 0;
            }
            break;
        default:
            cout << "ADC entry error:" << endl;
            module_id = 0;
        }

    }//end decode one entry data

}

/////////////////
void MTDC_Data_Decoder_thread(void *argv, void* buffer)
{
    struct Entry *entry_data = (struct Entry*) argv;
    Int_t i, j, k;
    UInt_t module_id, module_ch;
    UInt_t module_sig, module_sub_sig;
    Long64_t ts_tmp;
    Int_t  data_tmp;

    ////////// start decode ADC value
    module_id = 0;
    for (k = 0; k < entry_data->nWord; k++)
    {
        //Get ADC signature:
        module_sig = ((entry_data->data[k] & 0xC0000000) >> 30);
        switch (module_sig)
        {
        ////// event header
        case 1:
            module_id = ((entry_data->data[k] & 0x00FF0000) >> 16);
            if (module_id <= (TDC_Modules) && module_id > 0)
            {
                memset(&tdc_data[module_id - 1], 0, sizeof(TDC_DATA));
                continue;
            }
            else
            {
                // if wrong id, discard this event data
                module_id = 0;
                while (((entry_data->data[k] & 0xC0000000) >> 30) != 3)
                {
                    k++;
                    if (k >= entry_data->nWord)
                        break;
                }
            }
            break;
        ///// event data ///////////
        case 0:
            if (module_id == 0 )
                continue;
            module_sub_sig = ((entry_data->data[k] & 0x3FE00000) >> 22);
            /// data
            if (module_sub_sig == 0x10)
            {
                module_ch = ((entry_data->data[k] & 0x003F0000) >> 16);
                //add the ADC data.
                if (module_ch < TDC_CH)
                {
                    data_tmp = entry_data->data[k] & 0x0FFFF;
                    hist_TDC[module_id - 1][module_ch]->Fill(data_tmp);
                    tdc_data[module_id - 1].data[module_ch] = data_tmp;
                }
            }
            /// extended timestamp
            else if (module_sub_sig == 0x12)
                ts_tmp = entry_data->data[k] & 0xFFFF;
            break;
        /////// trailer
        case 3:
            if (module_id == 0)
                continue;
            ts_tmp = ((ts_tmp << 30) + (entry_data->data[k] & 0x3FFFFFFF)) * ADC_TS_UNIT + VME_TS_CORR[ADC_Modules + module_id - 1]; //get the 46bits ts
            if (ts_tmp >= TS_curr[ADC_Modules + module_id - 1] && ((ts_tmp - TS_curr[ADC_Modules + module_id - 1] ) < MAX_EVENT_TS_DIFF))
                //if(ts_tmp >= TS_curr[module_id-1])
            {
                tdc_data[module_id - 1].ts = ts_tmp;
                //update adc queue
                tr_TDC[module_id - 1]->Fill();
                //update TS_curr
                // fMutext_Read->Lock();
                TS_curr[ADC_Modules + module_id - 1] = ts_tmp;
                //    fMutext_Read->UnLock();
            }
            else
            {
                cout << "MTDC TS error" << endl;
                module_id = 0;
            }
            break;
        default:
            cout << "MTDC entry error:" << endl;
            module_id = 0;
        }

    }//end decode one entry data

}

////////////////
void TDC_Data_Decoder_thread(void *argv, void * buffer)
{
    struct Entry *entry_data = (struct Entry*) argv;
    Int_t i, j, k;
    UInt_t module_id, module_ch;
    UInt_t module_sig;
    Long64_t ts_tmp, ts_tmp1;
    Int_t  data_tmp;

    //////////
    module_id = 0;
    module_ch = 0;
    ts_tmp = 0;
    ts_tmp1 = 0;
    for (k = 0; k < entry_data->nWord; k++)
    {
        //Get TDC signature:
        module_sig = (entry_data->data[k] >> 27);
        switch (module_sig)
        {
        //header
        case 0x8:
            module_id = (entry_data->data[k] & 0x1F);
            if (module_id <= (TDC_Modules) && module_id > 0)
            {
                memset(&tdc_data[module_id - 1], 0, sizeof(TDC_DATA));
                continue;
            }
            else
            {
                // if wrong id, discard this event data
                module_id = 0;
                while ((entry_data->data[k] >> 27) != 0x10)
                {
                    k++;
                    if (k >= entry_data->nWord)
                        break;
                }
            }
            break;
        ///// data
        case 0:
            if (module_id == 0)
                continue;
            if (module_id == V1290_Module_ID)
            {
                module_ch = ((entry_data->data[k] >> 21) & 0x1F);
                if (module_ch < TDC_CH)
                    //if(module_ch<TDC_CH && TDC_data_tmp[module_ch]==0)
                {
                    data_tmp = entry_data->data[k] & 0x1FFFFF;
                    hist_TDC[module_id - 1][module_ch]->Fill(data_tmp);
                    tdc_data[module_id - 1].data[module_ch] = data_tmp;
                }
            }
            else
            {
                module_ch = ((entry_data->data[k] >> 19) & 0x7F);
                if (module_ch < TDC_CH)
                    //if(module_ch<TDC_CH && TDC_data_tmp[module_ch]==0)
                {
                    data_tmp = (entry_data->data[k] & 0x7FFFF);
                    hist_TDC[module_id - 1][module_ch]->Fill(data_tmp);
                    tdc_data[module_id - 1].data[module_ch] = data_tmp;
                }
            }
            break;
        case 0x11:
            if (module_id == 0)
                continue;
            ts_tmp = entry_data->data[k] & 0x7FFFFFF;
            break;
        case 0x10:
            if (module_id == 0)
                continue;
            ts_tmp1 = (entry_data->data[k] & 0x1F); //get the loe 5bits ts
            ts_tmp = ((ts_tmp << 5) + ts_tmp1); //get the 32bit ts
            ts_tmp1 = (TS_curr[ADC_Modules + module_id - 1] / TDC_TS_UNIT) & 0xFFFFFFFF; //get the pre ts lower 32 bits
            if (ts_tmp < ts_tmp1)
            {
                Long64_t aa = 1;
                ts_tmp = (ts_tmp + (aa << 32) - ts_tmp1) * TDC_TS_UNIT;
            }
            else
                ts_tmp = (ts_tmp - ts_tmp1) * TDC_TS_UNIT;
            if (ts_tmp < MAX_EVENT_TS_DIFF)
            {
                ts_tmp += TS_curr[ADC_Modules + module_id - 1];
                //update adc queue
                tdc_data[module_id - 1].ts = ts_tmp;
                tr_TDC[module_id - 1]->Fill();
                //update TS_curr
                //  fMutext_Read->Lock();
                TS_curr[ADC_Modules + module_id - 1] = ts_tmp;
                //fMutext_Read->UnLock();
            }
            else
            {
                cout << "TDC ts error!" << endl;
                module_id = 0;
            }
            break;
        /// filler or some error data
        default:
            cout << "TDC entry error!" << endl;
            module_id = 0;
        }

    }//end decode one entry data

}


void VME_Data_Decoder_thread(void *argv)
{
    int i, j, k;
    TString str_tmp;
    //// initial some point and address for input data
    TTree* tr_VME = (TTree*) argv;
    struct Entry entry_ADC;
    struct Entry entry_MTDC;
    struct Entry entry_TDC;
#ifdef ADC_USED
    ((TBranch*)tr_VME->GetBranch("ADC0"))->SetAddress(&entry_ADC);
#endif
#ifdef MTDC_USED
    ((TBranch*)tr_VME->GetBranch("MTDC"))->SetAddress(&entry_MTDC);
#endif
#ifdef TDC_USED
    ((TBranch*)tr_VME->GetBranch("TDC0"))->SetAddress(&entry_TDC);
#endif

    ///////init tree and branch data
    //start decode  /////////////
    int nentry = tr_VME->GetEntries();
    int readnum;
    UInt_t percent = 0;
    memset(&entry_ADC, 0, sizeof(Entry));
    memset(&entry_MTDC, 0, sizeof(Entry));
    memset(&entry_TDC, 0, sizeof(Entry));
    for (i = 0; i < nentry; i++)
    {
        //show percentage completed:
        if (i >= 0.1 * percent * nentry - 1)
        {
            str_tmp = str_tmp.Format("%3.0f%% completed for VME tree", 10.0 * percent);
            cout << str_tmp.Data() << endl;
            percent++;
        }
        /// for entry


        fMutext_Read->Lock();
        readnum = tr_VME->GetEntry(i);
        fMutext_Read->UnLock();
        if (readnum < 1)
        {
            cout << " read VMe entry " << i << " error!" << endl;
            continue;
        }
        /// do not use thread
#ifdef ADC_USED
        ADC_Data_Decoder_thread(&entry_ADC, (void*) 0);
#endif

#ifdef MTDC_USED
        MTDC_Data_Decoder_thread(&entry_MTDC, (void*) 0);
#endif

#ifdef TDC_USED
        TDC_Data_Decoder_thread(&entry_TDC, (void *) 0);
#endif
    }// end loop all entry

    for (i = 0; i < ADC_Modules + TDC_Modules; i++)
    {
        cout << "VME TS[" << i << "]:" << TS_curr[i] << endl;
        //   fMutext_Read->Lock();
        // if(TS_curr[i]>ts_maximun)
        //   ts_maximun=TS_curr[i];
        // TS_curr[i]=FLAG_TS_FINISH;
        // fMutext_Read->UnLock();
    }
    cout << "VME decode have finished" << endl;
}
#endif


//////////// for ts sort
void Data_TS_Sort_thread(void *argv)
{
    int i, j, k;
    TString str_tmp;
    ///////for output format
    TTree* tr_sort = (TTree*) argv;
    //for XIA
#ifdef XIA_USED
    TClonesArray* XIA_final = new TClonesArray("MyXIAData", MAX_HITS_NUM_EVENT);
    int nevent;
    tr_sort->Branch("nXIA", &nevent, "nXIA/I");
    tr_sort->Branch("data_XIA", &XIA_final);
    // MyXIAData* XIA_data_tmp[XIA_Modules][XIA_CH];
#endif
    //for MADC
#ifdef VME_USED
    struct VME_DATA VME_final;
    str_tmp = str_tmp.Format("ts[%d]/L", ADC_Modules + TDC_Modules);
    for (i = 0; i < TDC_Modules; i++)
        str_tmp = str_tmp.Format("%s:TDC%d[%d]/I", str_tmp.Data(), i, TDC_CH);
    for (i = 0; i < ADC_Modules; i++)
        str_tmp = str_tmp.Format("%s:ADC%d[%d]/I", str_tmp.Data(), i, ADC_CH);
    tr_sort->Branch("VME_Data", &VME_final, str_tmp.Data());

    //struct ADC_DATA ADC_data_tmp[ADC_Modules];
    //struct TDC_DATA TDC_data_tmp[TDC_Modules];
#endif
    /////////for decode
    Long64_t TS_curr_of_Tree[Total_Tree_Num];
    Long64_t Entry_curr_max[Total_Tree_Num];   // record the total entry number of curr tree
    Long64_t Entry_curr_num[Total_Tree_Num];    // record the processing entry number of curr tree

    memset(TS_curr_of_Tree, 0, sizeof(Long64_t)*Total_Tree_Num);
    memset(Entry_curr_num, 0, sizeof(Long64_t)*Total_Tree_Num);
    /// search entry for sort
    Long64_t TS_curr_minimun = 0;
    Long64_t TS_curr_process = 0;
    Long64_t ts_tmp;
    int ts_count = 0;
    //Get entry from tree if ts > TS_curr_process && ts < TS_curr_minimun
#ifdef XIA_USED
    for (i = 0; i < XIA_Modules; i++)
        for (j = 0; j < XIA_CH; j++)
        {
            k = ADC_Modules + TDC_Modules + i * XIA_CH + j;
            Entry_curr_max[k] = tr_XIA[i][j]->GetEntries();
        }
#endif
    ///////////for VME
#ifdef VME_USED
    for (i = 0; i < ADC_Modules; i++)
        Entry_curr_max[i] = tr_ADC[i]->GetEntries();
    for (i = 0; i < TDC_Modules; i++)
    {
        k = i + ADC_Modules;
        Entry_curr_max[k] = tr_TDC[i]->GetEntries();
    }
#endif
    //// search coincidence data
    int mytest_count = 0;
    while (1)
    {
#ifdef XIA_USED
        for (i = 0; i < XIA_Modules; i++)
            for (j = 0; j < XIA_CH; j++)
            {
                k = i * XIA_CH + j + ADC_Modules + TDC_Modules;
                if (TS_curr_of_Tree[k] == 0 && Entry_curr_num[k] < Entry_curr_max[k])
                {
                    tr_XIA[i][j]->GetEntry(Entry_curr_num[k]);
                    Entry_curr_num[k]++;
#ifdef XIA_USE_EXT_TS
                    TS_curr_of_Tree[k] = myXIAdata[i][j]->Ext_ts;
#else
                    TS_curr_of_Tree[k] = myXIAdata[i][j]->Event_ts;
#endif
                }
            }
#endif
        ////////// VME
#ifdef VME_USED
        for (i = 0; i < ADC_Modules; i++)
        {
            if (TS_curr_of_Tree[i] == 0 && Entry_curr_num[i] < Entry_curr_max[i])
            {
                tr_ADC[i]->GetEntry(Entry_curr_num[i]);
                Entry_curr_num[i]++;
                TS_curr_of_Tree[i] = adc_data[i].ts;
            }
        }
        for (i = 0; i < TDC_Modules; i++)
        {
            k = i + ADC_Modules;
            if (TS_curr_of_Tree[k] == 0 && Entry_curr_num[k] < Entry_curr_max[k])
            {
                tr_TDC[i]->GetEntry(Entry_curr_num[k]);
                Entry_curr_num[k]++;
                TS_curr_of_Tree[k] = tdc_data[i].ts;
            }
        }
#endif
        //////
        TS_curr_process = GetMinElement(TS_curr_of_Tree, Total_Tree_Num);
        if (TS_curr_process > 0) // no event for process
        {
#ifdef XIA_USED
            nevent = 0;
            XIA_final->Clear("C");
            for (i = 0; i < XIA_Modules; i++)
                for (j = 0; j < XIA_CH; j++)
                {
                    k = i * XIA_CH + j + ADC_Modules + TDC_Modules;
                    if (TS_curr_of_Tree[k] != 0 && abs(TS_curr_of_Tree[k] - TS_curr_process) < TS_SYNC_Windows)
                    {
                        /// only accept the MAX_HITS_NUM_EVENT data
                        if (nevent < MAX_HITS_NUM_EVENT)
                        {
                            //MyXIAData* data_tmp=new ((*XIA_final)[nevent]) MyXIAData();
                            MyXIAData* data_tmp = (MyXIAData*)XIA_final->ConstructedAt(nevent);
                            if (data_tmp != NULL)
                            {
                                *data_tmp = *myXIAdata[i][j];
                                nevent++;
                            }
                            else
                            {
                                XIA_final->Remove(data_tmp);
                                cout << "get clone array error!" << endl;
                            }
                        }
                        //remove processed data and reset flag status
                        TS_curr_of_Tree[k] = 0;
                    }
                }
#endif
            ////////// VME
#ifdef VME_USED
            memset(&VME_final, 0, sizeof(VME_DATA));
            for (i = 0; i < ADC_Modules; i++)
            {
                if (TS_curr_of_Tree[i] != 0 && abs(TS_curr_of_Tree[i] - TS_curr_process) < TS_SYNC_Windows)
                {
                    MergyVMEData(&VME_final, &adc_data[i], i);
                    TS_curr_of_Tree[i] = 0;
                }
            }
            for (i = 0; i < TDC_Modules; i++)
            {
                k = i + ADC_Modules;
                if (TS_curr_of_Tree[k] != 0 && abs(TS_curr_of_Tree[k] - TS_curr_process) < TS_SYNC_Windows)
                {
                    MergyVMEData(&VME_final, &tdc_data[i], i);
                    TS_curr_of_Tree[k] = 0;
                }
            }
#endif

            tr_sort->Fill();
            if (TS_curr_process > ts_count * 1E9 * Time_for_Print_Info) //event 10s printf info
            {
                cout << (int)(TS_curr_process / 1E9) << " seconds data have been sort!" << endl;
                //cout<<"buff:"<< XIA_final->GetSize()<<endl;
                ts_count++;
            }
        }
        else
            break;
    } //end while sort TS_curr_process

#ifdef XIA_USED
    delete XIA_final;
#endif
    cout << "finish TS sort" << endl;
}



/////////////////
int Data_Decoder(char* RunName, int RunNum)
{
    int i, j, k;
    TString str_tmp;
    str_tmp = str_tmp.Format("../../../data/raw/%s%05d.root", RunName, RunNum);
    TFile* f_input = new TFile(str_tmp.Data());
    if (f_input->IsZombie())
    {
        cout << "open file " << str_tmp.Data() << " error!" << endl;
        delete f_input;
        return -1;
    }
    cout << "*** start process file " << str_tmp.Data() << " ********" << endl;
    ///////// initial output file format ///////////
    memset(TS_curr, 0, sizeof(Long64_t)*Total_TS_Num);
    fMutext_Read = new TMutex();
    //ts_maximun=0;

#ifdef XIA_USED
    TThread* XIA_Decode_thread_ptr;
    TTree* tr_XIA_old = (TTree*)f_input->Get("XIA");
    //TTree* tr_XIA_old=(TTree*)f_input->Get("data");
    if (tr_XIA_old == NULL)
    {
        cout << " Get the tree XIA error in file" << str_tmp.Data() << endl;
        cout << "plz check the setup.h" << endl;
        f_input->Close();
        return -1;
    }
#endif

#ifdef VME_USED
    TThread *VME_Decode_thread_ptr;
    //TTree* tr_VME_old=(TTree*)f_input->Get("Trigger");
    TTree* tr_VME_old = (TTree*)f_input->Get("VME");
    if (tr_VME_old == NULL)
    {
        cout << " Get the tree VMe error in file" << str_tmp.Data() << endl;
        cout << "plz check the setup.h" << endl;
        f_input->Close();
        return -1;
    }
#endif

    /// for tmp  output  //////////
    str_tmp = str_tmp.Format("../../../data/tmp/%s%05d_tmp.root", RunName, RunNum);
    TFile* f_output_tmp = new TFile(str_tmp.Data(), "RECREATE");
    if (f_output_tmp->IsZombie())
    {
        cout << "open file " << str_tmp.Data() << " error!" << endl;
        f_input->Close();
        return -1;
    }
    ////
    f_output_tmp->cd();

#ifdef XIA_USED
    for (i = 0; i < XIA_Modules; i++)
        for (j = 0; j < XIA_CH; j++)
        {
            str_tmp = str_tmp.Format("tr_XIA_%02d_%02d", i, j);
            tr_XIA[i][j] = new TTree(str_tmp.Data(), str_tmp.Data());
            myXIAdata[i][j] = new MyXIAData();
            tr_XIA[i][j]->Branch("data", "MyXIAData", &myXIAdata[i][j]);

            str_tmp = str_tmp.Format("hist_XIA_%02d_%02d", i, j);
            hist_XIA_E[i][j] = new TH1F(str_tmp.Data(), str_tmp.Data(), XIA_BINs, 0, XIA_MAX_BIN);
        }
    XIA_Decode_thread_ptr = new TThread("XIA_decode", (TThread::VoidRtnFunc_t) XIA_Data_Decoder_thread, (void*)tr_XIA_old);
    XIA_Decode_thread_ptr->Run();
#endif
    cout << "start***" << endl;

#ifdef VME_USED
    for (i = 0; i < ADC_Modules; i++)
    {
        str_tmp = str_tmp.Format("tr_ADC%02d", i);
        tr_ADC[i] = new TTree(str_tmp.Data(), str_tmp.Data());
        str_tmp = str_tmp.Format("ts/L:data[%d]/I", ADC_CH);
        tr_ADC[i]->Branch("ADC", &adc_data[i], str_tmp.Data());
        for (j = 0; j < ADC_CH; j++)
        {
            str_tmp = str_tmp.Format("hist_ADC_%02d_Ch%02d", i, j);
            hist_ADC[i][j] = new TH1F(str_tmp.Data(), str_tmp.Data(), ADC_BINs, 0, ADC_MAX_BIN);
        }
    }
    //for TDC
    for (i = 0; i < TDC_Modules; i++)
    {
        str_tmp = str_tmp.Format("tr_TDC%02d", i);
        tr_TDC[i] = new TTree(str_tmp.Data(), str_tmp.Data());
        str_tmp = str_tmp.Format("ts/L:data[%d]/I", TDC_CH);
        tr_TDC[i]->Branch("TDC", &tdc_data[i], str_tmp.Data());
        TS_curr[ADC_Modules + i] = VME_TS_CORR[ADC_Modules + i];
        for (j = 0; j < TDC_CH; j++)
        {
            str_tmp = str_tmp.Format("TDC_%02d_%02d", i, j);
            hist_TDC[i][j] = new TH1F(str_tmp.Data(), str_tmp.Data(), TDC_BINs, 0, TDC_MAX_BIN);
        }
    }
    //VME_Data_Decoder_thread((void*)tr_VME_old);
    VME_Decode_thread_ptr = new TThread("VME_decode", (TThread::VoidRtnFunc_t)VME_Data_Decoder_thread, (void*)tr_VME_old);
    VME_Decode_thread_ptr->Run();
#endif

    ///wait thread finish //
#ifdef XIA_USED
    if (XIA_Decode_thread_ptr->GetState() == TThread::kRunningState) //running status
        XIA_Decode_thread_ptr->Join();
#endif

#ifdef VME_USED
    if (VME_Decode_thread_ptr->GetState() == TThread::kRunningState) //running status
        VME_Decode_thread_ptr->Join();
#endif

    t2 = time(NULL);
    cout << "first finish: " << t2 - t1 << endl;
    //// for sort data
    //
    //for tmp  output  //////////
    str_tmp = str_tmp.Format("../../../data/final/%s%05d_final.root", RunName, RunNum);
    TFile* f_output = new TFile(str_tmp.Data(), "RECREATE");
    if (f_output->IsZombie())
    {
        cout << "open file " << str_tmp.Data() << " error!" << endl;
        f_input->Close();
        return -1;
    }
    ////
    f_output->cd();

    TTree* tr_sort = new TTree("tr", "data tree");
    tr_sort->SetAutoSave(TREEBUFFER);

    Data_TS_Sort_thread((void*)tr_sort);

    t2 = time(NULL);
    cout << "second finish: " << t2 - t1 << endl;
    /////////// finish decode and sort, save file
    f_output->cd();
    //tr_sort->Write("",TObject::kOverwrite);
    tr_sort->Write();
    TDirectory* f_dir = f_output->mkdir("hist_dir");
#ifdef XIA_USED
    XIA_Decode_thread_ptr->Delete();
    for (i = 0; i < XIA_Modules; i++)
    {
        for (j = 0; j < XIA_CH; j++)
        {
            f_dir->cd();
            hist_XIA_E[i][j]->Write();
            f_output_tmp->cd();
            delete hist_XIA_E[i][j];
        }
    }
#endif

#ifdef VME_USED
    VME_Decode_thread_ptr->Delete();
    for (i = 0; i < ADC_Modules; i++)
    {
        for (j = 0; j < ADC_CH; j++)
        {
            f_dir->cd();
            hist_ADC[i][j]->Write();
            f_output_tmp->cd();
            delete hist_ADC[i][j];
        }
    }
    for (i = 0; i < TDC_Modules; i++)
    {
        for (j = 0; j < TDC_CH; j++)
        {
            f_dir->cd();
            hist_TDC[i][j]->Write();
            f_output_tmp->cd();
            delete hist_TDC[i][j];
        }
    }
#endif
    f_output->Close();

#ifdef SAVE_TMP_DATA
    f_output_tmp->cd();
#ifdef XIA_USED
    for (i = 0; i < XIA_Modules; i++)
    {
        for (j = 0; j < XIA_CH; j++)
        {
            tr_XIA[i][j]->Write("", TObject::kOverwrite);
            //        tr_XIA[i][j]->Write();
            delete tr_XIA[i][j];
        }
    }
#endif

#ifdef VME_USED
    for (i = 0; i < ADC_Modules; i++)
    {
        tr_ADC[i]->Write("", TObject::kOverwrite);
        // tr_ADC[i]->Write();
        delete tr_ADC[i];
    }
    for (i = 0; i < TDC_Modules; i++)
    {
        tr_TDC[i]->Write("", TObject::kOverwrite);
        // tr_TDC[i]->Write();
        delete tr_TDC[i];
    }
#endif


#endif   //end save tmp data
    f_output_tmp->Close();

    f_input->Close();
    delete fMutext_Read;
    delete f_input;
    delete f_output_tmp;
    delete f_output;
    cout << "**** finish *******" << endl;
    return 0;
}
////////////////////////
int main(int argc, char* argv[])
{
    int i, j, k;
    int min = 304, max = 304;
    char RunName[256] = "run";
    t1 = time(NULL);
    t2 = time(NULL);
    /////////////init global parameters
    ///start read and decoder data
    cout << "filename:" << endl;
    cin >> RunName;
    cout << "run numbers, min, max:" << endl;
    cin >> min >> max;
    for (i = min; i <= max; i++)
    {
        Data_Decoder(RunName, i);
    }// end loop from mix to max num
    t2 = time(NULL);
    cout << "time diff:" << t2 - t1 << endl;

    //////////finish   //////////////
    cout << "finish all task!!!" << endl;
}


