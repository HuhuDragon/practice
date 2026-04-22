#include "SteppingAction.hh"

#include "EventAction.hh"

#include <G4Step.hh>

SteppingAction::SteppingAction(EventAction* event_action) : event_action_(event_action) {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
  const double edep = step->GetTotalEnergyDeposit();
  if (edep <= 0.0) return;

  const auto* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  const auto& name = volume->GetName();

  if (name == "AnodeE1") event_action_->AddToAnode(0, edep);
  if (name == "AnodeE2") event_action_->AddToAnode(1, edep);
  if (name == "AnodeE3") event_action_->AddToAnode(2, edep);
  if (name == "AnodeE4") event_action_->AddToAnode(3, edep);
  if (name == "CathodeEtotal" || name == "GasChamber") event_action_->AddToTotal(edep);
}
