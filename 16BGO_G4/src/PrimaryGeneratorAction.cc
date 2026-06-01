#include "PrimaryGeneratorAction.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4RandomDirection.hh"
#include "G4IonTable.hh"
#include "G4ChargedGeantino.hh"
#include "G4DecayTable.hh"
#include "G4PhotonEvaporation.hh"

namespace B1
{

  PrimaryGeneratorAction::PrimaryGeneratorAction()
  {
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);

    // G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    // G4String particleName;
    // G4ParticleDefinition *particle = particleTable->FindParticle(particleName = "gamma");
    // fParticleGun->SetParticleDefinition(particle);

    // // 获取 γ 射线的定义
    // G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    // G4ParticleDefinition *gamma = particleTable->FindParticle("gamma");

    // // 设置第一条 γ 射线
    // fParticleGun->SetParticleDefinition(gamma);
  }

  PrimaryGeneratorAction::~PrimaryGeneratorAction()
  {
    delete fParticleGun;
  }

  void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
  {

    G4ParticleDefinition *particle = G4IonTable::GetIonTable()->GetIon(8, 15, 0.); // O-15
    // G4ParticleDefinition *particle = G4IonTable::GetIonTable()->GetIon(15, 30, 0.); // P-30
    // G4ParticleDefinition *particle = G4IonTable::GetIonTable()->GetIon(16, 31, 0.); // S-31

    G4double random = G4UniformRand();

    // G4ParticleDefinition *particle;
    // if (random < 8. / (14. + 8.))
    // {
    //   // 生成S-31粒子 (分支比8%)
    //   particle = G4IonTable::GetIonTable()->GetIon(16, 31, 0.); // S-31-T1/2=2.5534s
    //   // particle = G4IonTable::GetIonTable()->GetIon(8, 16, 0.); // S-31-T1/2=2.5534s
    // }
    // else
    // {
    //   // 生成P-30粒子 (分支比14%)
    //   particle = G4IonTable::GetIonTable()->GetIon(15, 30, 0.); // P-30-T1/2=149.88s
    //   // particle = G4IonTable::GetIonTable()->GetIon(8, 16, 0.); // S-31-T1/2=2.5534s
    // }

    fParticleGun->SetParticleDefinition(particle);
    // 设置随机位置
    G4double x = G4RandGauss::shoot(0.0 * mm, 0.1 * mm);
    G4double y = G4RandGauss::shoot(0.0 * mm, 0.1 * mm);
    G4double z = G4RandGauss::shoot(0.0 * mm, 0.0 * mm);
    G4ThreeVector position(x, y, z);

    // 设置随机动量方向
    G4ThreeVector direction1 = G4RandomDirection();
    G4ThreeVector direction2(0.0, 0.0, -1.0);

    fParticleGun->SetParticleEnergy(0. * keV);
    fParticleGun->SetParticlePosition(position);
    fParticleGun->SetParticleMomentumDirection(direction2);
    fParticleGun->SetParticleCharge(0. * eplus);
    fParticleGun->GeneratePrimaryVertex(anEvent);
  }

}