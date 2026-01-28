// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME classDataConvert
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "XIA_Decode.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_MyXIAData(void *p = nullptr);
   static void *newArray_MyXIAData(Long_t size, void *p);
   static void delete_MyXIAData(void *p);
   static void deleteArray_MyXIAData(void *p);
   static void destruct_MyXIAData(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MyXIAData*)
   {
      ::MyXIAData *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MyXIAData >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("MyXIAData", ::MyXIAData::Class_Version(), "XIA_Decode.h", 17,
                  typeid(::MyXIAData), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::MyXIAData::Dictionary, isa_proxy, 4,
                  sizeof(::MyXIAData) );
      instance.SetNew(&new_MyXIAData);
      instance.SetNewArray(&newArray_MyXIAData);
      instance.SetDelete(&delete_MyXIAData);
      instance.SetDeleteArray(&deleteArray_MyXIAData);
      instance.SetDestructor(&destruct_MyXIAData);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MyXIAData*)
   {
      return GenerateInitInstanceLocal((::MyXIAData*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::MyXIAData*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void delete_MyXIADecode(void *p);
   static void deleteArray_MyXIADecode(void *p);
   static void destruct_MyXIADecode(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MyXIADecode*)
   {
      ::MyXIADecode *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MyXIADecode >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("MyXIADecode", ::MyXIADecode::Class_Version(), "XIA_Decode.h", 45,
                  typeid(::MyXIADecode), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::MyXIADecode::Dictionary, isa_proxy, 4,
                  sizeof(::MyXIADecode) );
      instance.SetDelete(&delete_MyXIADecode);
      instance.SetDeleteArray(&deleteArray_MyXIADecode);
      instance.SetDestructor(&destruct_MyXIADecode);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MyXIADecode*)
   {
      return GenerateInitInstanceLocal((::MyXIADecode*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::MyXIADecode*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr MyXIAData::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *MyXIAData::Class_Name()
{
   return "MyXIAData";
}

//______________________________________________________________________________
const char *MyXIAData::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MyXIAData*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int MyXIAData::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MyXIAData*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MyXIAData::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MyXIAData*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MyXIAData::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MyXIAData*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr MyXIADecode::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *MyXIADecode::Class_Name()
{
   return "MyXIADecode";
}

//______________________________________________________________________________
const char *MyXIADecode::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MyXIADecode*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int MyXIADecode::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MyXIADecode*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MyXIADecode::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MyXIADecode*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MyXIADecode::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MyXIADecode*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void MyXIAData::Streamer(TBuffer &R__b)
{
   // Stream an object of class MyXIAData.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(MyXIAData::Class(),this);
   } else {
      R__b.WriteClassBuffer(MyXIAData::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_MyXIAData(void *p) {
      return  p ? new(p) ::MyXIAData : new ::MyXIAData;
   }
   static void *newArray_MyXIAData(Long_t nElements, void *p) {
      return p ? new(p) ::MyXIAData[nElements] : new ::MyXIAData[nElements];
   }
   // Wrapper around operator delete
   static void delete_MyXIAData(void *p) {
      delete ((::MyXIAData*)p);
   }
   static void deleteArray_MyXIAData(void *p) {
      delete [] ((::MyXIAData*)p);
   }
   static void destruct_MyXIAData(void *p) {
      typedef ::MyXIAData current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MyXIAData

//______________________________________________________________________________
void MyXIADecode::Streamer(TBuffer &R__b)
{
   // Stream an object of class MyXIADecode.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(MyXIADecode::Class(),this);
   } else {
      R__b.WriteClassBuffer(MyXIADecode::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_MyXIADecode(void *p) {
      delete ((::MyXIADecode*)p);
   }
   static void deleteArray_MyXIADecode(void *p) {
      delete [] ((::MyXIADecode*)p);
   }
   static void destruct_MyXIADecode(void *p) {
      typedef ::MyXIADecode current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MyXIADecode

namespace {
  void TriggerDictionaryInitialization_classDataConvert_Impl() {
    static const char* headers[] = {
"XIA_Decode.h",
nullptr
    };
    static const char* includePaths[] = {
"/opt/root/include/",
"/home/huhudragon/project/JUNA/source_cal/decoderV1.2/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "classDataConvert dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$XIA_Decode.h")))  MyXIAData;
class __attribute__((annotate("$clingAutoload$XIA_Decode.h")))  MyXIADecode;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "classDataConvert dictionary payload"

#ifndef MYTEST
  #define MYTEST 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "XIA_Decode.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"MyXIAData", payloadCode, "@",
"MyXIADecode", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("classDataConvert",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_classDataConvert_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_classDataConvert_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_classDataConvert() {
  TriggerDictionaryInitialization_classDataConvert_Impl();
}
