#include "EventAction.hh"

#include "RunAction.hh"

#include <G4Event.hh>
#include <G4RunManager.hh>

void EventAction::BeginOfEventAction(const G4Event*) {
  anode_edep_.fill(0.0);
  total_edep_ = 0.0;
}

void EventAction::EndOfEventAction(const G4Event*) {
  auto* run_action = const_cast<RunAction*>(dynamic_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction()));
  if (run_action) {
    run_action->RecordEvent(anode_edep_[0], anode_edep_[1], anode_edep_[2], anode_edep_[3], total_edep_);
  }
}

void EventAction::AddToAnode(int idx, double edep) {
  if (idx >= 0 && idx < 4) anode_edep_[idx] += edep;
}

void EventAction::AddToTotal(double edep) { total_edep_ += edep; }
