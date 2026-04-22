#include "RunAction.hh"

#include <G4AnalysisManager.hh>
#include <G4Run.hh>
#include <G4SystemOfUnits.hh>

RunAction::RunAction(const SimConfig& cfg) : cfg_(cfg) {}

RunAction::~RunAction() {
  delete G4AnalysisManager::Instance();
}

void RunAction::BeginOfRunAction(const G4Run*) {
  auto* analysis = G4AnalysisManager::Instance();
  analysis->SetDefaultFileType("root");
  analysis->SetVerboseLevel(1);

  analysis->OpenFile(cfg_.output_root);

  analysis->CreateNtuple("gas", "Gas detector energy deposition");
  analysis->CreateNtupleIColumn("event");
  analysis->CreateNtupleDColumn("e1_MeV");
  analysis->CreateNtupleDColumn("e2_MeV");
  analysis->CreateNtupleDColumn("e3_MeV");
  analysis->CreateNtupleDColumn("e4_MeV");
  analysis->CreateNtupleDColumn("etotal_MeV");
  analysis->FinishNtuple();

  event_id_ = 0;
}

void RunAction::EndOfRunAction(const G4Run*) {
  auto* analysis = G4AnalysisManager::Instance();
  analysis->Write();
  analysis->CloseFile();
}

void RunAction::RecordEvent(double e1, double e2, double e3, double e4, double etotal) {
  auto* analysis = G4AnalysisManager::Instance();
  analysis->FillNtupleIColumn(0, event_id_++);
  analysis->FillNtupleDColumn(1, e1 / MeV);
  analysis->FillNtupleDColumn(2, e2 / MeV);
  analysis->FillNtupleDColumn(3, e3 / MeV);
  analysis->FillNtupleDColumn(4, e4 / MeV);
  analysis->FillNtupleDColumn(5, etotal / MeV);
  analysis->AddNtupleRow();
}
