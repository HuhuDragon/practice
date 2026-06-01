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

namespace B1
{

  PrimaryGeneratorAction::PrimaryGeneratorAction()
  {
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);

    // 获取 γ 射线的定义
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition *gamma = particleTable->FindParticle("gamma");

    // 设置第一条 γ 射线
    fParticleGun->SetParticleDefinition(gamma);
  }

  PrimaryGeneratorAction::~PrimaryGeneratorAction()
  {
    delete fParticleGun;
  }

  void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent)
  {
    // 设置随机位置
    G4double x = G4RandGauss::shoot(0.0 * mm, 0.2 * mm);
    G4double y = G4RandGauss::shoot(0.0 * mm, 0.2 * mm);
    G4double z = G4RandGauss::shoot(1.5 * mm, 0.2 * mm);
    G4ThreeVector position(x, y, z);

    // 设置随机动量方向
    G4ThreeVector direction1 = G4RandomDirection();
    G4ThreeVector direction2 = G4RandomDirection();

    // 生成第一条 γ 射线的初态顶点
    fParticleGun->SetParticleEnergy(1.173 * MeV); // 第一条 γ 射线的能量
    fParticleGun->SetParticlePosition(position);
    fParticleGun->SetParticleMomentumDirection(direction1);
    fParticleGun->GeneratePrimaryVertex(anEvent);

    // 生成第二条 γ 射线的初态顶点
    fParticleGun->SetParticleEnergy(1.332 * MeV); // 第二条 γ 射线的能量
    fParticleGun->SetParticlePosition(position);
    fParticleGun->SetParticleMomentumDirection(direction2);
    fParticleGun->GeneratePrimaryVertex(anEvent);
  }

  // ....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}