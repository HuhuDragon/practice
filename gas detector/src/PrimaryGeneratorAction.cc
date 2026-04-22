#include "PrimaryGeneratorAction.hh"

#include <G4Event.hh>
#include <G4IonTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleGun.hh>
#include <G4SystemOfUnits.hh>

#include <Randomize.hh>

#include <algorithm>

PrimaryGeneratorAction::PrimaryGeneratorAction(const SimConfig& cfg) : cfg_(cfg), gun_(new G4ParticleGun(1)) {
  BuildBeamComposition();
  gun_->SetParticleMomentumDirection({0, 0, 1});
  gun_->SetParticlePosition({0, 0, -185.0 * mm});
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete gun_; }

void PrimaryGeneratorAction::BuildBeamComposition() {
  beam_particles_.clear();
  cumulative_ratios_.clear();

  if (!cfg_.particles.empty()) {
    beam_particles_ = cfg_.particles;
  } else {
    beam_particles_.push_back(cfg_.particle);
  }

  double cumulative = 0.0;
  for (const auto& p : beam_particles_) {
    const double safe_ratio = std::max(0.0, p.ratio);
    cumulative += safe_ratio;
    cumulative_ratios_.push_back(cumulative);
  }

  // 若全部比例为 0，则退化为均匀比例。
  if (cumulative <= 0.0) {
    cumulative_ratios_.clear();
    cumulative = 0.0;
    for (size_t i = 0; i < beam_particles_.size(); ++i) {
      cumulative += 1.0;
      cumulative_ratios_.push_back(cumulative);
    }
  }
}

const ParticleSpec& PrimaryGeneratorAction::SampleParticle() const {
  const double total = cumulative_ratios_.back();
  const double x = G4UniformRand() * total;
  const auto it = std::lower_bound(cumulative_ratios_.begin(), cumulative_ratios_.end(), x);
  const size_t idx = static_cast<size_t>(std::distance(cumulative_ratios_.begin(), it));
  return beam_particles_[std::min(idx, beam_particles_.size() - 1)];
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  const auto& p = SampleParticle();
  auto* ion = G4IonTable::GetIonTable()->GetIon(p.z, p.a, 0.0);

  gun_->SetParticleDefinition(ion);
  gun_->SetParticleCharge(p.charge_state * eplus);
  gun_->SetParticleEnergy(p.kinetic_energy_mev * MeV);
  gun_->GeneratePrimaryVertex(event);
}
