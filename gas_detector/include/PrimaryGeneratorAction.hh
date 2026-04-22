#pragma once

#include "Config.hh"

#include <G4VUserPrimaryGeneratorAction.hh>

#include <vector>

class G4Event;
class G4ParticleGun;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
 public:
  explicit PrimaryGeneratorAction(const SimConfig& cfg);
  ~PrimaryGeneratorAction() override;

  void GeneratePrimaries(G4Event* event) override;

 private:
  void BuildBeamComposition();
  const ParticleSpec& SampleParticle() const;

  SimConfig cfg_;
  G4ParticleGun* gun_;
  std::vector<ParticleSpec> beam_particles_;
  std::vector<double> cumulative_ratios_;
};
