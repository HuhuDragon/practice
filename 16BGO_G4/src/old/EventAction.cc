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
/// \file EventAction.cc
/// \brief Implementation of the B1::EventAction class

#include "EventAction.hh"
#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4ParticleGun.hh"


#include "macro.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction)
:G4UserEventAction(), fRunAction(runAction)
{
  for (G4int i = 0; i < nScoringVolumes; ++i)
    {
      fEdep[i] = 0.;
      fTs[i]=0;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event*event)
{
  for (int i = 0; i < nScoringVolumes; ++i)
  {
    fEdep[i] = 0.;
  }
  G4int eventNb=event->GetEventID();
  G4int nevents1 = G4RunManager::GetRunManager()->GetNumberOfEventsToBeProcessed();
  if(eventNb%10000==0)
  {
    printf("---->Begin of %d %.2f%\r",eventNb,100*(G4double)eventNb/nevents1);
    fflush(stdout);
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event*)
{
  G4bool fillflag = false;
  for(int i=0;i<nScoringVolumes;i++)
  {
    if(fEdep[i]>1.*keV)
    {
      fEdep[i] = G4RandGauss::shoot(fEdep[i]/keV,0.8484*pow(fEdep[i]/keV,0.5395));
      // fEdep[i] = G4RandGauss::shoot(fEdep[i]/keV,1.0923*pow(fEdep[i]/keV,4.46e-1));

      fillflag = true;
    }
  }
  fRunAction->AddCounts();
  // fRunAction->SetEdep(fEdep);
  // fRunAction->SetTs(fTs);
  G4int fPriName =0;
  G4int fPriName1 =0;
  G4double fPriE = 0.;
  G4double fPriE1 = 0.;
  const G4ParticleGun* particleGun = static_cast<const PrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction())->GetParticleGun();
  const G4ParticleGun* particleGun1 = static_cast<const PrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction())->GetParticleGun1();
  TString priname = particleGun->GetParticleDefinition()->GetParticleName();
  if(priname=="mu-"||priname=="mu+") fPriName = 1;
  else if(priname=="gamma") fPriName =2;
  else if(priname=="e-"||priname=="e+") fPriName = 3;
  else if(priname=="proton") fPriName =4;
  else if(priname=="neutron") fPriName =5;
  TString priname1 = particleGun1->GetParticleDefinition()->GetParticleName();
  if(priname1=="mu-"||priname1=="mu+") fPriName1 = 1;
  else if(priname1=="gamma") fPriName1 =2;
  else if(priname1=="e-"||priname1=="e+") fPriName1 = 3;
  else if(priname1=="proton") fPriName1 =4;
  else if(priname1=="neutron") fPriName1 =5;
  fPriE = particleGun->GetParticleEnergy()/keV;
  fPriE1 = particleGun1->GetParticleEnergy()/keV;
  // fRunAction->SetPrimary(fPriName,fPriName1,fPriE,fPriE1);
  
  if(fillflag)
  // fRunAction->FillTree();
  fRunAction->SetAndFill(fEdep,fTs,fPriE,fPriE1,fPriName,fPriName1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void EventAction::AddEdep(G4double edepStep,G4int n)
{
    fEdep[n] += edepStep;
}
void EventAction::SetTs(G4double tsStep,G4int n) {fTs[n] = tsStep;}
G4double EventAction::GetTs(G4int n) {return fTs[n];}
}
