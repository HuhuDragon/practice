#pragma once

#include "Config.hh"

#include <G4UserRunAction.hh>

#include <fstream>

class G4Run;

class RunAction : public G4UserRunAction {
 public:
  explicit RunAction(const SimConfig& cfg);
  ~RunAction() override;

  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run*) override;

  void RecordEvent(double e1, double e2, double e3, double e4, double etotal);

 private:
  SimConfig cfg_;
  std::ofstream out_;
};
