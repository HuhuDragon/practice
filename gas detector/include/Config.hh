#pragma once

#include <string>
#include <vector>

struct ParticleSpec {
  std::string name{"Fe60"};
  int z{26};
  int a{60};
  int charge_state{20};
  double kinetic_energy_mev{100.0};
  double ratio{1.0};
};

struct SimConfig {
  // 允许单粒子（particle）或多粒子混合束（particles）配置。
  ParticleSpec particle{};
  std::vector<ParticleSpec> particles{};

  std::string gas_material{"G4_PROPANE"};
  double gas_pressure_mbar{38.7};

  int particles_per_round{1000};
  int rounds{1};

  std::string output_root{"events.root"};
};

SimConfig LoadConfig(const std::string& yaml_path);
