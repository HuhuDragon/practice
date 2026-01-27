#ifndef _XIA_DECODE_H
#define _XIA_DECODE_H

#include <iostream>
#include <TApplication.h>
#include <TROOT.h>
#include <TTree.h>
#include <TThread.h>
#include <TObject.h>

//for pixie-16 module, 100, 250, and 500 MSPS type
// one mod need one instance, and decode the data;

#define MAX_TRACE_LENGTH 32768
#define MAX_HEADER_LENGTH 18

class MyXIAData:public TObject
{
    public:
        Long64_t  Event_ts;  //in ns unit;
        Long64_t  Ext_ts;
        UShort_t   finish_code;
        UShort_t   crate_id;
        UShort_t   channel;
        UShort_t   Energy;
        double     CFD;       //in ns unit
        UShort_t   CFD_Bit;
        UInt_t     nESum;
        UInt_t     *ESum;   //[nESum]
        UInt_t     nQDC;
        UInt_t     *QDC;  //[nQDC]
        UInt_t     trace_length;
        UShort_t   *trace; //[trace_length]

        MyXIAData();
        virtual ~MyXIAData();

       void SetModId(int id);
       MyXIAData& operator=(const MyXIAData& src);

       ClassDef(MyXIAData,1)
};


class MyXIADecode: public TObject
{
    private:
        UShort_t   finish_code;
        UShort_t   crate_id;
        UShort_t   slot_id;
        UShort_t   channel;
        Long64_t  Event_ts;  //in ns unit;
        double     CFD;       //in ns unit
        UShort_t   CFD_Bit;
        UShort_t   Energy;
        UInt_t     nESum;
        UInt_t     ESum[4];   //nESum
        UInt_t     nQDC;
        UInt_t     QDC[8];   //nQDC
        UInt_t     trace_length;
        UShort_t   trace[MAX_TRACE_LENGTH]; //trace_length
        Long64_t  Ext_ts;

        double Ext_ts_unit;
        int event_status;                                      //!
        int Modtype;                                          //!
        int buff_length;                                     //!
        UInt_t buff_tmp[MAX_TRACE_LENGTH+MAX_HEADER_LENGTH]; //!

        //UInt_t head_length;

    public:
        MyXIADecode(int Modtype,double Ext_ts_unit=1.0);
        MyXIADecode(int Modtype,TTree*);
        virtual ~MyXIADecode();

        void ClearData();

        UShort_t GetCh(){return channel;};
        UShort_t GetE(){return Energy;};
        int GetEventStatus(){return event_status;};
        unsigned long GetTs(){return Event_ts;};
        UInt_t GetExtTs(){return Ext_ts;};
        UShort_t GetCFD(){return CFD;};
        int GetTraceLength(){return trace_length;};
        int GetTrace(UShort_t *data);
        int GetQDC(UInt_t *data);
        int GetData(MyXIAData* p);

        int Decode(UInt_t *,int);
        void Decode_Event();
        int Decode1(UInt_t*,int);

        ClassDef(MyXIADecode,1)
};


#endif

