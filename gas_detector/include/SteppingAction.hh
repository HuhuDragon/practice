#pragma once

#include <G4UserSteppingAction.hh>

class EventAction;

class SteppingAction : public G4UserSteppingAction {
 public:
  explicit SteppingAction(EventAction* event_action);

  void UserSteppingAction(const G4Step* step) override;

 private:
  EventAction* event_action_;
};
