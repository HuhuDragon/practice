#pragma once

#include "Config.hh"

#include <G4VUserActionInitialization.hh>

class ActionInitialization : public G4VUserActionInitialization {
 public:
  explicit ActionInitialization(const SimConfig& cfg);

  void Build() const override;

 private:
  SimConfig cfg_;
};
