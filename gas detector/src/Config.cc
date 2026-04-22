#include "Config.hh"

#include <yaml-cpp/yaml.h>

namespace {
ParticleSpec ParseParticleNode(const YAML::Node& node, const ParticleSpec& defaults) {
  ParticleSpec p = defaults;
  if (node["name"]) p.name = node["name"].as<std::string>();
  if (node["z"]) p.z = node["z"].as<int>();
  if (node["a"]) p.a = node["a"].as<int>();
  if (node["charge_state"]) p.charge_state = node["charge_state"].as<int>();
  if (node["kinetic_energy_mev"]) p.kinetic_energy_mev = node["kinetic_energy_mev"].as<double>();
  if (node["ratio"]) p.ratio = node["ratio"].as<double>();
  return p;
}
}  // namespace

SimConfig LoadConfig(const std::string& yaml_path) {
  SimConfig cfg;
  YAML::Node root = YAML::LoadFile(yaml_path);

  if (const auto particle = root["particle"]) {
    cfg.particle = ParseParticleNode(particle, cfg.particle);
  }

  if (const auto particles = root["particles"]; particles && particles.IsSequence()) {
    cfg.particles.clear();
    for (const auto& node : particles) {
      cfg.particles.push_back(ParseParticleNode(node, cfg.particle));
    }
  }

  if (const auto detector = root["detector"]) {
    if (detector["gas_material"]) cfg.gas_material = detector["gas_material"].as<std::string>();
    if (detector["gas_pressure_mbar"]) cfg.gas_pressure_mbar = detector["gas_pressure_mbar"].as<double>();
  }

  if (const auto run = root["run"]) {
    if (run["particles_per_round"]) cfg.particles_per_round = run["particles_per_round"].as<int>();
    if (run["rounds"]) cfg.rounds = run["rounds"].as<int>();
    if (run["output_root"]) cfg.output_root = run["output_root"].as<std::string>();
    if (run["output_csv"]) cfg.output_root = run["output_csv"].as<std::string>();  // 向后兼容旧字段
  }

  return cfg;
}
