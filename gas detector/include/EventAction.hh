#pragma once

#include <G4UserEventAction.hh>

#include <array>

class G4Event;

class EventAction : public G4UserEventAction {
 public:
  void BeginOfEventAction(const G4Event*) override;
  void EndOfEventAction(const G4Event*) override;

  void AddToAnode(int idx, double edep);
  void AddToTotal(double edep);

 private:
  std::array<double, 4> anode_edep_{};
  double total_edep_{0.0};
};
