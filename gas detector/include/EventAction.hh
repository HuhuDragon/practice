#pragma once

#include <G4UserEventAction.hh>

#include <array>

class G4Event;

class EventAction : public G4UserEventAction {
 public:
  explicit EventAction(int total_events);

  void BeginOfEventAction(const G4Event*) override;
  void EndOfEventAction(const G4Event*) override;

  void AddToAnode(int idx, double edep);
  void AddToTotal(double edep);

 private:
  void PrintProgress();

  int total_events_{0};
  int finished_events_{0};
  int last_percent_{-1};

  std::array<double, 4> anode_edep_{};
  double total_edep_{0.0};
};
