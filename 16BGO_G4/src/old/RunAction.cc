//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file RunAction.cc
/// \brief Implementation of the B1::RunAction class

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include<iostream>
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "TFile.h"
#include "TTree.h"

#include "macro.hh"

namespace B1
{
  G4Mutex MUTEX = G4MUTEX_INITIALIZER;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction():
  G4UserRunAction(), fNCounts(0), fPriE(0),fPriE1(0),fPriName(0),fPriName1(0),fout(NULL), ftr(NULL),fTimer(NULL)
{
    fNCounts=0;
    for(int i=0;i<nScoringVolumes;i++) fEdep[i] = 0;
    for(int i=0;i<nScoringVolumes;i++) fEdep[i] = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
  fTimer = NULL;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* run)
{
  // G4cout<<"### RUN " <<run->GetRunID() <<"start." <<G4endl;
  // G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  // fTimer->Start();
  // G4cout<<G4endl
  //   <<"-----Start of Run-----"<<G4endl;
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  G4cout<<G4endl;
  if(IsMaster()) 
  {
    G4cout<<"-----Start of Master Run-----"<<G4endl;
    fTimer = new G4Timer;
    fTimer->Start();
  }
  else G4cout<<"-----Start of Work Run-----"<<G4endl;
  if(IsMaster())
  {
    fout = new TFile("out.root","RECREATE");
    ftr = new TTree("tr","tr");
    ftr->Branch("de",fEdep,"de[48]/D");
    ftr->Branch("Ts",fTs,"Ts[48]/D");
    ftr->Branch("priNamePrimary",&fPriName1,"priNamePrimary/I");
    ftr->Branch("priEPrimary",&fPriE1,"priEPrimary/D");
    ftr->Branch("priNameSecondary",&fPriName,"priNameSecondary/I");
    ftr->Branch("priESecondary",&fPriE,"priESecondary/D");
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nevents = run->GetNumberOfEvent();
  if(nevents==0) return;
  if(IsMaster()) 
  {
    ftr->Write();
    fout->Write();
    fout->Close();
    G4cout<<"-----End of Master Run-----"<<G4endl;
    G4cout<<" The run consists of "<<nevents<<" primaries"<<G4endl;
    G4cout<<" The ratio is "<<100.*fNCounts/nevents<<"%"<<G4endl;
    fTimer->Stop();
    G4cout<<" Time costed: "<<*fTimer<<G4endl;
    delete fTimer;
  }
  else {
    G4cout<<"-----End of Work Run-----"<<G4endl;
    G4cout<<" The run consists of "<<nevents<<" primaries"<<G4endl;
    G4cout<<" The ratio is "<<100.*fNCounts/nevents<<"%"<<G4endl;
    ((RunAction*)G4MTRunManager::GetMasterRunManager()->GetUserRunAction())->fNCounts+=fNCounts;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RunAction::AddCounts() {
    fNCounts++;
}
void RunAction::SetAndFill(G4double fE[nScoringVolumes], G4double fT[nScoringVolumes], G4double pE,G4double pE1,G4int pID,G4int pID1) {
    G4AutoLock lock(&MUTEX);
    for(int i=0;i<nScoringVolumes;i++) ((RunAction*)G4MTRunManager::GetMasterRunManager()->GetUserRunAction())->fEdep[i] = fE[i];
    for(int i=0;i<nScoringVolumes;i++) ((RunAction*)G4MTRunManager::GetMasterRunManager()->GetUserRunAction())->fTs[i] = fT[i];
    ((RunAction*)G4MTRunManager::GetMasterRunManager()->GetUserRunAction())->fPriE = pE;
    ((RunAction*)G4MTRunManager::GetMasterRunManager()->GetUserRunAction())->fPriE1 = pE1;
    ((RunAction*)G4MTRunManager::GetMasterRunManager()->GetUserRunAction())->fPriName = pID;
    ((RunAction*)G4MTRunManager::GetMasterRunManager()->GetUserRunAction())->fPriName1 = pID1;
    ((RunAction*)G4MTRunManager::GetMasterRunManager()->GetUserRunAction())->ftr->Fill();
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
