#ifndef SETUP_H
#define SETUP_H
#include <stdio.h>
#include <stdlib.h>
#include <Rtypes.h>

#define XIA_USED
#define XIA_ESum_ENABLE
#define XIA_QDC_ENABLE
#define XIA_WAVE_ENABLE
#define XIA_USE_EXT_TS
#define XIA_Modules   12
#define XIA_CH 16
#define XIA_BINs 16384
#define XIA_MAX_BIN 65532
// #define VME_USED
#define ADC_USED
#define ADC_Modules 1
#define ADC_CH  32
#define ADC_TS_UNIT 25    //ns
#define ADC_BINs   8192
#define ADC_MAX_BIN 8192

#define TDC_USED
#define MTDC_USED
#define TDC_Modules 1
#define V1290_Module_ID 0
#define TDC_CH 34
#define TDC_TS_UNIT 25    //ns
#define TDC_BINs   4000
#define TDC_MAX_BIN  16000   //unit 100ps??

//#define SCALER_USED
#define Scaler_Modules 1
#define Scaler_CH  6


/// for ts event
#define SAVE_TMP_DATA
#define FLAG_TS_FINISH        0x7FFFFFFFFFFFFFF0   //the max of Long64_t
#define MAX_HITS_NUM_EVENT   12
#define MAX_EVENT_TS_DIFF    1E9    // one event per 100ms??
#define TS_SYNC_Windows      150   //  ns
#define SEARCH_Windows_Extended 3*TS_SYNC_Windows //  ns
#define MAX_data_Buffer (ADC_Modules*8>64? (ADC_Modules*8*1024):131072 )   /// one ADC:8k Dword buffer,XIA:256k*16bit buffer(131072 Dword)
#define TREEBUFFER    50000000                  //50MB 
#define Time_for_Print_Info  100           //10s

//Trigger entry structures in raw root file
enum DecodeType
{
	ADC = 1,
	TDC,
	XIA
};

struct Entry
{
	Int_t nWord;
	UInt_t data[MAX_data_Buffer];
};

struct ADC_DATA
{
	Long64_t ts;
	Int_t data[ADC_CH];
};

struct TDC_DATA
{
	Long64_t ts;
	Int_t data[TDC_CH];
};

struct VME_DATA
{
	Long64_t ts[ADC_Modules + TDC_Modules];
	Int_t tdc[TDC_Modules][TDC_CH];
	Int_t adc[ADC_Modules][ADC_CH];
};
#endif
