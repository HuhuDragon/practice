#include "ActionInitialization.hh"
#include "Config.hh"
#include "DetectorConstruction.hh"

#include <G4RunManagerFactory.hh>
#include <G4UImanager.hh>
#include <QGSP_BIC.hh>

#include <iostream>
#include <memory>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: ./gas_detector <config.yaml>\n";
    return 1;
  }

  const auto cfg = LoadConfig(argv[1]);

  auto run_manager = std::unique_ptr<G4RunManager>(G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default));
  run_manager->SetUserInitialization(new DetectorConstruction(cfg));
  run_manager->SetUserInitialization(new QGSP_BIC);
  run_manager->SetUserInitialization(new ActionInitialization(cfg));
  run_manager->Initialize();

  const int total_events = cfg.particles_per_round * cfg.rounds;
  G4UImanager::GetUIpointer()->ApplyCommand("/run/beamOn " + std::to_string(total_events));

  return 0;
}
