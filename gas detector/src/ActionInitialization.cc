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

  const int total_events = cfg_.particles_per_round * cfg_.rounds;
  auto* event_action = new EventAction(total_events);
  SetUserAction(event_action);

  SetUserAction(new SteppingAction(event_action));
}
