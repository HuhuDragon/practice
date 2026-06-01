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
/// \file RunAction.hh
/// \brief Definition of the B1::RunAction class

#ifndef B1RunAction_h
#define B1RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"
#include "G4Timer.hh"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include "macro.hh"
class G4Run;

/// Run action class
///
/// In EndOfRunAction(), it calculates the dose in the selected volume
/// from the energy deposit accumulated via stepping and event actions.
/// The computed dose is then printed on the screen.

namespace B1
{

  class RunAction : public G4UserRunAction
  {
  public:
    RunAction();
    ~RunAction() override;

    void BeginOfRunAction(const G4Run *) override;
    void EndOfRunAction(const G4Run *) override;
    void SetEdep(G4double fE[nScoringVolumes]);
    void SetTs(G4double fT[nScoringVolumes]);
    void SetPrimary(G4int pID, G4int pID1, G4double pE, G4double pE1);
    void FillTree();
    void AddCounts();
    // void SetTimestamp(G4double timestamp);  // 设置时间戳
    void SetAndFill(G4double fE[nScoringVolumes], G4double fT[nScoringVolumes], G4double pE, G4double pE1, G4int pID, G4int pID1);
    // void AddbCounts();
    void SetTimeStamp(G4double eventTimeStamp);

  private:
    G4Timer *fTimer;

    // G4int frontcounts;
    // G4int backcounts;
    G4int fNCounts;
    G4double fEdep[nScoringVolumes];
    G4double InnerEdep[nInnerBGO];
    // G4double OuterEdep[nOuterBGO];
    G4double fTs[nScoringVolumes];
    G4int fPriName;
    G4double fPriE;
    G4int fPriName1;
    G4double fPriE1;
    TFile *fout;
    TTree *ftr;
    G4double fRunTimeStampe;
    G4double ftTs[nScoringVolumes];

  private:
    G4double fTimeStampe;
  };

}

#endif
