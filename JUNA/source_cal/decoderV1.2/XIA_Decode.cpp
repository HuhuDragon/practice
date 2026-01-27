#ifndef XIA_DECODE_CPP
#define XIA_DECODE_CPP

#include <iostream>
#include <map>
#include "XIA_Decode.h"

using namespace std;
/// class data
ClassImp(MyXIAData);
MyXIAData::MyXIAData()
{
    Event_ts=0;  //in ns unit;
    Ext_ts=0;
    finish_code=0;
    crate_id=0;
    channel=0;
    CFD=0;       //in ns unit
    CFD_Bit=0;
    Energy=0;
    trace_length=0;
    trace=new unsigned short[MAX_TRACE_LENGTH];
    nQDC=0;
    QDC=new unsigned int[8];
    nESum=0;
    ESum=new unsigned int[4];
}

MyXIAData::~MyXIAData()
{
	delete ESum;
	ESum=0;
	delete QDC;
	QDC=0;
	delete trace;
	trace=0;
}


void MyXIAData::SetModId(int id)
{
    channel+=id*100;
}

MyXIAData& MyXIAData::operator=(const MyXIAData& src)
{
    if(this != &src)
    {
        this->finish_code  =   src.finish_code;
        this->crate_id     =   src.crate_id;
        this->channel      =   src.channel;
        this->Event_ts     =   src.Event_ts;  //in ns unit;
        this->CFD          =   src.CFD;       //in ns unit
        this->CFD_Bit      =   src.CFD_Bit;
        this->Energy       =   src.Energy;
        this->trace_length =   src.trace_length;
        this->Ext_ts       =   src.Ext_ts;
        this->nESum        =   src.nESum;
        if(nESum !=0)
            memcpy(this->ESum,src.ESum,sizeof(UInt_t)*src.nESum);
        this->nQDC         =    src.nQDC;
        if(nQDC !=0 )
            memcpy(this->QDC,src.QDC,sizeof(UInt_t)*src.nQDC);
        this->trace_length = src.trace_length;
        if(trace_length != 0)
            memcpy(this->trace,src.trace,sizeof(UShort_t)*src.trace_length); //trace_length
    }
    return *this;
}



//class decode

ClassImp(MyXIADecode);

MyXIADecode::MyXIADecode( int type,double unit)
{
    buff_length=0;
    memset(buff_tmp,0,(MAX_TRACE_LENGTH+MAX_HEADER_LENGTH)*sizeof(unsigned int));
    trace_length=0;
    memset(trace,0,sizeof(unsigned short)*MAX_TRACE_LENGTH);
    Modtype=type;
    event_status=0;
    Ext_ts_unit=unit;
}

MyXIADecode::~MyXIADecode()
{
}

void MyXIADecode::ClearData()
{
    buff_length=0;
    trace_length=0;
    nQDC=0;
    nESum=0;
    event_status=0;
}



int MyXIADecode::GetTrace(unsigned short *data)
{
    if(data != NULL && trace_length < MAX_TRACE_LENGTH)
    {
        memcpy(data,trace,trace_length*sizeof(unsigned short));
        return trace_length;
    }
    else
        return 0;
}

int MyXIADecode::GetQDC(unsigned int *data)
{
    if(data != NULL)
    {
        memcpy(data,QDC,sizeof(unsigned int)*nQDC);
        return nQDC;
    }
    else
        return 0;
}

int MyXIADecode::Decode(unsigned int *data,int data_length)
{
    event_status=0;
    if(data == NULL)
        return -1;
    unsigned int header_data1;
    if(buff_length != 0)
        header_data1=buff_tmp[0];
    else
        header_data1=data[0];
    int event_length=((header_data1<<1)>>18);
    if(event_length >(MAX_TRACE_LENGTH+MAX_HEADER_LENGTH))
    {
        cout<<"some problem in data, the event length is too long"<<endl;
        return -1;
    }

    if(event_length <= (data_length+buff_length))
    {
        event_length-=buff_length;
        memcpy(&buff_tmp[buff_length],data,(event_length)*sizeof(unsigned int));
        // process event;
        Decode_Event();
        header_data1=((header_data1<<15)>>27);
        int trace_length=(event_length-(header_data1))*2;
        if(trace_length>0 && trace_length<=MAX_TRACE_LENGTH)
            memcpy(trace,(UShort_t*)&buff_tmp[header_data1],trace_length*sizeof(UShort_t));
        event_status=1;
        buff_length=0;
        return event_length;
    }
    else
    {
        /// partial event, storage into buff_tmp
        memcpy(&buff_tmp[buff_length],data,data_length*sizeof(unsigned int));
        buff_length=data_length;
        return data_length;
    }
}

int MyXIADecode::Decode1(unsigned int*,int)
{
    return 0;
}

void MyXIADecode::Decode_Event()
{
    finish_code=((buff_tmp[0])>>31);
    UInt_t event_length=((buff_tmp[0]<<1)>>18);
    UInt_t head_length=((buff_tmp[0]<<15)>>27);
//    head_length=((buff_tmp[0]<<15)>>27);
    trace_length=2*(event_length-head_length);
    crate_id = ((buff_tmp[0]>>12)&0xF);
    slot_id  =  ((buff_tmp[0]>>4)&0xF);
    channel  =  (buff_tmp[0]&0xF);

  //  if(slot_id != 2 || channel != 4)
    //    printf("error, slot Id: %d, ch:%d\n",slot_id,channel);

    switch(Modtype)
    {
        case 500:
            Event_ts=(buff_tmp[2]&0xFFFF);
            Event_ts =  ((Event_ts<<32)+buff_tmp[1])*10; 
            CFD=((buff_tmp[2]>>16)&0x1FFF);
            if(CFD != 0)
            {
                CFD=2.0*(((buff_tmp[2])>>29)&0x7)-2.0+2.0*((buff_tmp[2]>>16)&0x1FFF)/8192;
                CFD_Bit=0;
            }
            else
            {
                CFD_Bit=1;
            }
            break; 
        case 250: 
            Event_ts=(buff_tmp[2]&0xFFFF);
            Event_ts =  ((Event_ts<<32)+buff_tmp[1])*8; 
            CFD_Bit=(buff_tmp[2]>>31)&0x1;
            if(CFD_Bit == 1)
                CFD=0;
            else
                CFD=0.0-4.0*(((buff_tmp[2])>>30)&0x1)+4.0*((buff_tmp[2]>>16)&0x3FFF)/16384;
            break;
        case 100:
            Event_ts=(buff_tmp[2]&0xFFFF);
            Event_ts =  ((Event_ts<<32)+buff_tmp[1])*10; 
            CFD_Bit=(buff_tmp[2]>>31)&0x1;
            if(CFD_Bit == 1)
                CFD=0;
            else
            {
                CFD=10.0*((buff_tmp[2]>>16)&0x7FFF)/32768;
            }
            break;
    }
    Energy=(buff_tmp[3]&0xFFFF);

    ////////
    head_length-=4;
    int i=0;
    int j=4;
    nESum=0;
    nQDC=0;
    if(((head_length) & 0x4) == 4) // energy sum data
    {
        nESum=4;
        for(i=0;i<4;i++)
            ESum[i]=buff_tmp[j++];
    }
    if((head_length &0x8) == 8) // QDC data
    {
        nQDC=8;
        for(i=0;i<8;i++)
        {
            QDC[i]=buff_tmp[j++];
        }
    }
    if((head_length &0x2) == 2) // Ext_ts data
    {
        Ext_ts=(buff_tmp[j+1]&0xFFFF);
        Ext_ts=(buff_tmp[j]+(Ext_ts<<32))*Ext_ts_unit;
        j+=2;
    }
}

int MyXIADecode::GetData(MyXIAData* p)
{
      p->finish_code  =   finish_code;
      p->crate_id     =   crate_id;
      p->channel      =   channel;
      p->Event_ts     =   Event_ts;  //in ns unit;
      p->CFD          =   CFD;       //in ns unit
      p->CFD_Bit      =   CFD_Bit;
      p->Energy       =   Energy;
      p->trace_length =   trace_length;
      p->Ext_ts       =   Ext_ts;
      p->nESum        =   nESum;
      if(nESum !=0)
          memcpy(p->ESum,ESum,sizeof(UInt_t)*nESum);
      ///
      p->nQDC         =    nQDC;
      if(nQDC !=0 )
          memcpy(p->QDC,QDC,sizeof(UInt_t)*nQDC);
      ///////
      p->trace_length = trace_length;
      if(trace_length != 0 && trace_length < MAX_TRACE_LENGTH)
          memcpy(p->trace,trace,sizeof(UShort_t)*trace_length); //trace_length
      return 0;
}

//int MyXIADecode::GetData(XIA_DATA* p)
//{
//      p->finish_code  =   finish_code;
//      p->crate_id     =   crate_id;
//      p->mod_id       =   slot_id;
//      p->channel      =   channel;
//      p->Event_ts     =   Event_ts;  //in ns unit;
//      p->CFD          =   CFD;       //in ns unit
//      p->CFD_Bit      =   CFD_Bit;
//      p->Energy       =   Energy;
//      p->trace_length =   trace_length;
//      p->Ext_ts       =   Ext_ts;
//      p->nESum        =   nESum;
//      if(nESum !=0)
//          memcpy(p->ESum,ESum,sizeof(UInt_t)*nESum);
//      p->nQDC         =    nQDC;
//      if(nQDC !=0 )
//          memcpy(p->QDC,QDC,sizeof(UInt_t)*nQDC);
//      p->trace_length = trace_length;
//      if(trace_length != 0)
//          memcpy(p->trace,trace,sizeof(UShort_t)*trace_length); //trace_length
//      return 0;
//}
#endif
