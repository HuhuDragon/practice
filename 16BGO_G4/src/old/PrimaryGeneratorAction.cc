// ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorAction.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4RandomDirection.hh"
#include "G4DecayTable.hh"
#include "G4PhotonEvaporation.hh"

namespace B1
{

  PrimaryGeneratorAction::PrimaryGeneratorAction()
  {
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName;
    G4ParticleDefinition *particle = particleTable->FindParticle(particleName = "gamma");

    fParticleGun->SetParticleDefinition(particle);
  }

  PrimaryGeneratorAction::~PrimaryGeneratorAction()
  {
    delete fParticleGun;
  }

  void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
  {
    // Generate one of the two characteristic gamma energies of 60Co (1173 keV or 1332 keV)
    // G4double energy = (G4UniformRand() < 0.5) ? 1173.0 * keV : 1332.0 * keV;
    G4double energy = 1460.0 *keV;
    fParticleGun->SetParticleEnergy(energy);

    // Set random position around (0, 0, 1.5) mm with Gaussian spread
    G4double x = G4RandGauss::shoot(0.0 * mm, 0.2 * mm);
    G4double y = G4RandGauss::shoot(0.0 * mm, 0.2 * mm);
    G4double z = G4RandGauss::shoot(1.5 * mm, 0.2 * mm);
    fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));
    
    // Set a random momentum direction
    fParticleGun->SetParticleMomentumDirection(G4RandomDirection());
    
    // Generate the primary vertex
    fParticleGun->GeneratePrimaryVertex(anEvent);
  }

  // ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
