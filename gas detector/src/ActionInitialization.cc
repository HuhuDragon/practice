#include "ActionInitialization.hh"

#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"

ActionInitialization::ActionInitialization(const SimConfig& cfg) : cfg_(cfg) {}

void ActionInitialization::Build() const {
  SetUserAction(new PrimaryGeneratorAction(cfg_));

  auto* run_action = new RunAction(cfg_);
  SetUserAction(run_action);

  auto* event_action = new EventAction();
  SetUserAction(event_action);

  SetUserAction(new SteppingAction(event_action));
}
