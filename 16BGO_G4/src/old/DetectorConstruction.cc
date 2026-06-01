//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"
#include "macro.hh"
#include "math.h"

namespace B1
{

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction()
  {
    for (int i = 0; i < nScoringVolumes; i++)
      fScoringVolume[i] = nullptr;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  DetectorConstruction::~DetectorConstruction()
  {
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  G4VPhysicalVolume *DetectorConstruction::Construct()
  {
    G4double PE_Bdensity, temperature, pressure, PE_Bfractionmass;
    G4String name, symbol;
    G4double  n, a;
    G4int PE_Bncomponents;

    G4NistManager *nist = G4NistManager::Instance();
    G4Material *mAir = nist->FindOrBuildMaterial("G4_AIR");
    // G4Material *mBGO = nist->FindOrBuildMaterial("G4_BGO");
    G4Material *mVac = nist->FindOrBuildMaterial("G4_Galactic");
    G4Material *mC = nist->FindOrBuildMaterial("G4_C");
    G4Material *mAl = nist->FindOrBuildMaterial("G4_Al");
    G4Material *mFe = nist->FindOrBuildMaterial("G4_Fe");
    G4Material *mCu = nist->FindOrBuildMaterial("G4_Cu");
    // G4Material *mAg = nist->FindOrBuildMaterial("G4_Ag");
    G4Material *mSteel = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    G4Material *mpoly = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
    G4Material *mB = nist->FindOrBuildMaterial("G4_B");
    G4Material *mTa = nist->FindOrBuildMaterial("G4_Ta");
    G4Material *mPb = nist->FindOrBuildMaterial("G4_Pb");
    G4Material *mPla = nist->FindOrBuildMaterial("G4_KAPTON");
    PE_Bdensity = 1.1 * g / cm3;//旧模拟用的1.1;密度变大前半部分BGO探测效率变小；//最终数据处理使用1.1的密度
    G4Material *mPE_B = new G4Material("PE_B", PE_Bdensity, PE_Bncomponents = 2);
    mPE_B->AddMaterial(mpoly, PE_Bfractionmass = 80 * perCent);
    mPE_B->AddMaterial(mB, PE_Bfractionmass = 30 * perCent);
    // mPE_B->AddMaterial(mC, PE_Bfractionmass = 20 * perCent);

    G4double BGOdensity;
    G4int BGOncomponents, BGOnatoms;
    // G4double BGOfractionmass;

    G4NistManager* nistManager = G4NistManager::Instance();

    // 获取基本元素
    G4Element* Bi = nistManager->FindOrBuildElement("Bi");
    G4Element* Ge = nistManager->FindOrBuildElement("Ge");
    G4Element* O = nistManager->FindOrBuildElement("O");

    BGOdensity = 7.13 * g/cm3; // 库中密度7.13

    // 创建自定义BGO材料
    G4Material* mBGO = new G4Material("mBGO", BGOdensity, BGOncomponents=3);

    // 添加成分和它们的质量分数或原子数
    mBGO->AddElement(Bi, BGOnatoms=4);
    mBGO->AddElement(Ge, BGOnatoms=3);
    mBGO->AddElement(O, BGOnatoms=12);

    // 或者，使用质量分数
    // customBGO->AddElement(Bi, BGOfractionmass=0.5);
    // customBGO->AddElement(Ge, BGOfractionmass=0.3);
    // customBGO->AddElement(O, BGOfractionmass=0.2);   

    // 假设ESR膜主要由银构成，并具有特定的密度
    G4Element* Ag = nist->FindOrBuildElement("Ag");
    G4Element* C = nist->FindOrBuildElement("C");
    G4Element* H = nist->FindOrBuildElement("H");
  

    G4double ESRdensity =0.01 * g/cm3;  // 
    G4Material* mESR = new G4Material("CustomESR", ESRdensity, 4);
    // 添加元素和它们的比例，这些比例应根据实际成分调整
    mESR->AddElement(Ag, 5 * perCent); // 示例比例，表示银占总质量的20%
    mESR->AddElement(C, 50 * perCent);  // 同上，碳
    mESR->AddElement(H, 25 * perCent);  // 同上，氢
    mESR->AddElement(O, 20 * perCent);  // 同上，氧





    auto worldVisAtt = new G4VisAttributes(G4Colour(1., 1., 1., 1));
    auto AirVisAtt = new G4VisAttributes(G4Colour(1., 1., 1., 0.5));
    auto CShellVisAtt = new G4VisAttributes(G4Colour(193 / 255., 53 / 255., 29 / 255., 0.1));
    auto VacuumVisAtt = new G4VisAttributes(G4Colour(192 / 255., 130 / 255., 135 / 255., 0.6));
    auto outerBGOVisAtt = new G4VisAttributes(G4Colour(246 / 255., 225 / 255., 198 / 255., 1));
    auto innerBGOVisAtt = new G4VisAttributes(G4Colour(144 / 255., 201 / 255., 231 / 255., 1));
    auto Al_backVisAtt = new G4VisAttributes(G4Colour(250 / 255., 134 / 255., 0., 1));
    auto Al_tubeVisAtt = new G4VisAttributes(G4Colour(1., 1., 1., 0.5));
    auto CTVisAtt = new G4VisAttributes(G4Colour(0.45, 0.25, 0.0));
    auto tarVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
    auto TaVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0,0.5));
    auto PE_BVisAtt = new G4VisAttributes(G4Colour(33 / 255., 158 / 255., 188 / 255., 0.5));
    auto VacVisAtt = new G4VisAttributes(G4Colour(0.2, 0.2, 0.2, 0.5));
    auto PbVisAtt = new G4VisAttributes(G4Colour(0., 1., 0, 0.5));
    // Option to switch on/off checking of volumes overlaps
    //
    G4bool checkOverlaps = true;

    //
    // World
    //
    G4double env_sizeXY = 200. * cm;
    G4double env_sizeZ = 200. * cm;

    G4double world_sizeXY = 1.2 * env_sizeXY;
    G4double world_sizeZ = 1.2 * env_sizeZ;
    G4Box *solidWorld =
        new G4Box("World",                                                    // its name
                  0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ); // its size

    G4LogicalVolume *logicWorld =
        new G4LogicalVolume(solidWorld, // its solid
                            mAir,       // its material
                            "World");   // its name

    G4VPhysicalVolume *physWorld =
        new G4PVPlacement(0,               // no rotation
                          G4ThreeVector(), // at (0,0,0)
                          logicWorld,      // its logical volume
                          "World",         // its name
                          0,               // its mother  volume
                          false,           // no boolean operation
                          0,               // copy number
                          checkOverlaps);  // overlaps checking

    G4Box *solidEnv =
        new G4Box("Envelope",                                           // its name
                  0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5 * env_sizeZ); // its size

    G4LogicalVolume *logicEnv =
        new G4LogicalVolume(solidEnv,    // its solid在世界的中心位置
                            mAir,        // its material
                            "Envelope"); // its name

    // G4PVPlacement* phyenv =
    new G4PVPlacement(0,               // no rotation
                      G4ThreeVector(), // at (0,0,0)
                      logicEnv,        // its logical volume
                      "Envelope",      // its name
                      logicWorld,      // its mother  volume
                      false,           // no boolean operation
                      0,               // copy number
                      checkOverlaps);  // overlaps checking
    logicWorld->SetVisAttributes(worldVisAtt);
    // 准备字符串数组
    G4String innerBGOname[16] = {};
    for (G4int i = 0; i < 16; i++)
      innerBGOname[i] = "innerBGO" + std::to_string(i + 1);
    G4String outerBGOname[32] = {};
    for (G4int i = 0; i < 32; i++)
      outerBGOname[i] = "outerBGO" + std::to_string(i + 1);
    G4String innerBGOinname[16] = {};
    for (G4int i = 0; i < 16; i++)
      innerBGOinname[i] = "innerBGOin" + std::to_string(i + 1);
    G4String outerBGOinname[32] = {};
    for (G4int i = 0; i < 32; i++)
      outerBGOinname[i] = "outerBGOin" + std::to_string(i + 1);
    G4String innerAl_backname[16] = {};
    for (G4int i = 0; i < 16; i++)
      innerAl_backname[i] = "innerAl_back" + std::to_string(i + 1);
    G4String outerAl_backname[32] = {};
    for (G4int i = 0; i < 32; i++)
      outerAl_backname[i] = "outerAl_back" + std::to_string(i + 1);
    G4String innerAirname[16] = {};
    for (G4int i = 0; i < 16; i++)
      innerAirname[i] = "innerAir" + std::to_string(i + 1);
    G4String outerAirname[32] = {};
    for (G4int i = 0; i < 32; i++)
      outerAirname[i] = "outerAir" + std::to_string(i + 1);

    // PE_BShell
    G4double sizePE_B_X = 387.5 * mm;
    G4double sizePE_B_Y = 387.5 * mm;
    G4double sizePE_B_Z = 305 * mm;
    G4Box *solidPE_B = new G4Box("PE_B", 0.5 * sizePE_B_X, 0.5 * sizePE_B_Y, 0.5 * sizePE_B_Z);
    // 定义钻孔的参数
    G4double holeSize = 64.5 * mm; // 孔的尺寸
    G4int numHoles = 25;           // 孔的数量
    G4double holeSpacing = 4.85* mm; // 孔的间距图纸4.85
    G4double holed = (holeSize+holeSpacing)*2.;//64.5+4.85
    // 创建钻孔的实体
    G4Box *holeSolid = new G4Box("HoleSolid", holeSize / 2, holeSize / 2, sizePE_B_Z);
    G4SubtractionSolid *PE_Bsub;
    // 循环在正方体上均匀钻孔-从含硼聚乙烯架子上挖去孔
    for (int i = 0; i < numHoles; i++)
    {
      G4int f = i / 5;
      G4double hole_x = i % 5 * (holeSize + holeSpacing) - holed * mm;//0%5=0，hole_x=- holed
      G4double hole_y = f * (holeSize + holeSpacing) - holed * mm;//1/5=0，hole_y=-holed
      G4ThreeVector holePosition(hole_x, hole_y, 0.);
      if (i == 0)
        PE_Bsub = new G4SubtractionSolid("solidholesub", solidPE_B, holeSolid, 0, holePosition); // 更新正方体的实体为新的带孔实体
      else
        PE_Bsub = new G4SubtractionSolid("solidholesub", PE_Bsub, holeSolid, 0, holePosition);
    }
    G4LogicalVolume *logicPE_B = new G4LogicalVolume(PE_Bsub, mPE_B, "PE_B");
    G4ThreeVector posPE_B = G4ThreeVector(0.0, 0.0, 0.0);
    // G4PVPlacement* phyPE_B =
    new G4PVPlacement(0, posPE_B, logicPE_B, "PE_B", logicEnv, false, 0, checkOverlaps);
    logicPE_B->SetVisAttributes(PE_BVisAtt);

    // BGOin实际是碳外壳
    G4double sizeBGOinX = 63.5 * mm;//变薄模拟谱康坪变低，全能峰效率变高
    G4double sizeBGOinY = 63.5 * mm;
    G4double sizeBGOinZ = (sizePE_B_Z / 2) * mm;//sizePE_B_Z=305*mm/2==152.5MM
    G4double BGOCouterd =0.0 *mm;//BGO碳外壳距离中心点距离
    G4Box *solidBGOin = new G4Box("BGOin", 0.5 * sizeBGOinX, 0.5 * sizeBGOinY, 0.5 * sizeBGOinZ);
    // G4Box *solidcenterBGOin = new G4Box("BGOin", 0.5 * sizeBGOinX, 0.5 * sizeBGOinY, 1.5 * sizeBGOinZ);
    // G4LogicalVolume *centerBGOin = new G4LogicalVolume(solidcenterBGOin, mAir, "centerBGOin");
    // new G4PVPlacement(0, G4ThreeVector(0,0,0), centerBGOin, "BGOin", logicBGOinset, false, 0, checkOverlaps);
    // G4VPhysicalVolume *innerphyBGOin[16];
    G4LogicalVolume *innerlogicBGOin[16];
    // G4VPhysicalVolume *outerphyBGOin[32];
    G4LogicalVolume *outerlogicBGOin[32];
    for (G4int i = 0; i < 8; i++)
    {
      innerlogicBGOin[i] = new G4LogicalVolume(solidBGOin, mC, "innerBGOin");
      innerlogicBGOin[i + 8] = new G4LogicalVolume(solidBGOin, mC, "innerBGOin");
      innerlogicBGOin[i]->SetVisAttributes(AirVisAtt);
      innerlogicBGOin[i + 8]->SetVisAttributes(AirVisAtt);
      if (i==4)
      {
        G4double x = (holeSize + holeSpacing) * std::sin(-(int)i / 2 * 90 * deg);
        G4double y = (holeSize + holeSpacing) * std::cos((int)i / 2 * 90 * deg);
        G4double z = sizeBGOinZ / 2 +BGOCouterd * mm;
        G4ThreeVector posinnerBGOin = G4ThreeVector(x, y, z);
        G4ThreeVector posinnerBGOin1 = G4ThreeVector(x, y, -z-0.0*mm);
        // innerphyBGOin[i]=
        new G4PVPlacement(0, posinnerBGOin, innerlogicBGOin[i], innerBGOinname[i], logicEnv, false, 0, checkOverlaps);
        // innerphyBGOin[i+8]=
        new G4PVPlacement(0, posinnerBGOin1, innerlogicBGOin[i + 8], innerBGOinname[i + 8], logicEnv, false, 0, checkOverlaps);        
      }
      else if (i % 2 == 0)//0、2、4、6//i=0,x=0,y=d;i=2,x=-d,y=0;
      {
        G4double x = (holeSize + holeSpacing) * std::sin(-(int)i / 2 * 90 * deg);
        G4double y = (holeSize + holeSpacing) * std::cos((int)i / 2 * 90 * deg);
        G4double z = sizeBGOinZ / 2 +BGOCouterd * mm;
        G4ThreeVector posinnerBGOin = G4ThreeVector(x, y, z);
        G4ThreeVector posinnerBGOin1 = G4ThreeVector(x, y, -z);
        // innerphyBGOin[i]=
        new G4PVPlacement(0, posinnerBGOin, innerlogicBGOin[i], innerBGOinname[i], logicEnv, false, 0, checkOverlaps);
        // innerphyBGOin[i+8]=
        new G4PVPlacement(0, posinnerBGOin1, innerlogicBGOin[i + 8], innerBGOinname[i + 8], logicEnv, false, 0, checkOverlaps);
      }
      else if (i < 4)//i==0,x=-d,y=0;i=1,x=d,y=d;
      {
        G4double x = -(holeSize + holeSpacing);
        G4double y = (holeSize + holeSpacing) * std::sin(i * 90 * deg);
        G4double z = sizeBGOinZ / 2 + BGOCouterd * mm;//BGO外壳摆放距离中心位置
        G4ThreeVector posinnerBGOin = G4ThreeVector(x, y, z);
        G4ThreeVector posinnerBGOin1 = G4ThreeVector(x, y, -z);
        /// innerphyBGOin[i]=
        new G4PVPlacement(0, posinnerBGOin, innerlogicBGOin[i], innerBGOinname[i], logicEnv, false, 0, checkOverlaps);
        // innerphyBGOin[i+8]=
        new G4PVPlacement(0, posinnerBGOin1, innerlogicBGOin[i + 8], innerBGOinname[i + 8], logicEnv, false, 0, checkOverlaps);
      }
      else if (i < 8)
      {
        G4double x = (holeSize + holeSpacing);
        G4double y = -(holeSize + holeSpacing) * std::sin(i * 90 * deg);
        G4double z = sizeBGOinZ / 2 + BGOCouterd * mm;
        G4ThreeVector posinnerBGOin = G4ThreeVector(x, y, z);
        G4ThreeVector posinnerBGOin1 = G4ThreeVector(x, y, -z);
        // innerphyBGOin[i]=
        new G4PVPlacement(0, posinnerBGOin, innerlogicBGOin[i], innerBGOinname[i], logicEnv, false, 0, checkOverlaps);
        // innerphyBGOin[i+8]=
        new G4PVPlacement(0, posinnerBGOin1, innerlogicBGOin[i + 8], innerBGOinname[i + 8], logicEnv, false, 0, checkOverlaps);
      }
    }
    for (G4int i = 0; i < 16; i++)
    {
      outerlogicBGOin[i] = new G4LogicalVolume(solidBGOin, mC, "outerBGOin");
      outerlogicBGOin[i + 16] = new G4LogicalVolume(solidBGOin, mC, "outerBGOin");
    }
    for (G4int i = 0; i < 16; i++)
    {
      if (i % 4 == 0)
      {
        G4double x = 2 * (holeSize + holeSpacing) * std::sin(-(int)i / 4 * 90 * deg);
        G4double y = 2 * (holeSize + holeSpacing) * std::cos((int)i / 4 * 90 * deg);
        G4double z = sizeBGOinZ / 2 + BGOCouterd * mm;
        G4ThreeVector posouterBGOin = G4ThreeVector(x, y, z);
        G4ThreeVector posouterBGOin1 = G4ThreeVector(x, y, -z);
        //  outerphyBGOin[i]=
        new G4PVPlacement(0, posouterBGOin, outerlogicBGOin[i], outerBGOinname[i], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+16]=
        new G4PVPlacement(0, posouterBGOin1, outerlogicBGOin[i + 16], outerBGOinname[i + 16], logicEnv, false, 0, checkOverlaps);
      }
      else if (i % 2 == 0)
      {
        if (i < 8)
        {
          G4double x = -2 * (holeSize + holeSpacing);
          G4double y = 2 * (holeSize + holeSpacing) * std::sin(i * 45 * deg);
          G4double z = sizeBGOinZ / 2 + BGOCouterd * mm;
          G4ThreeVector posouterBGOin = G4ThreeVector(x, y, z);
          G4ThreeVector posouterBGOin1 = G4ThreeVector(x, y, -z);
          // outerphyBGOin[i]=
          new G4PVPlacement(0, posouterBGOin, outerlogicBGOin[i], outerBGOinname[i], logicEnv, false, 0, checkOverlaps);
          // outerphyBGOin[i+16]=
          new G4PVPlacement(0, posouterBGOin1, outerlogicBGOin[i + 16], outerBGOinname[i + 16], logicEnv, false, 0, checkOverlaps);
        }
        else
        {
          G4double x = 2 * (holeSize + holeSpacing);
          G4double y = -2 * (holeSize + holeSpacing) * std::sin(i * 45 * deg);
          G4double z = sizeBGOinZ / 2 + BGOCouterd * mm;
          G4ThreeVector posouterBGOin = G4ThreeVector(x, y, z);
          G4ThreeVector posouterBGOin1 = G4ThreeVector(x, y, -z);
          // outerphyBGOin[i]=
          new G4PVPlacement(0, posouterBGOin, outerlogicBGOin[i], outerBGOinname[i], logicEnv, false, 0, checkOverlaps);
          //  outerphyBGOin[i+16]=
          new G4PVPlacement(0, posouterBGOin1, outerlogicBGOin[i + 16], outerBGOinname[i + 16], logicEnv, false, 0, checkOverlaps);
        }
      }
      else if (i == 1)
      {
        G4double x = sqrt(5) * (holeSize + holeSpacing) * cos(atan(2)) * mm;
        G4double y = sqrt(5) * (holeSize + holeSpacing) * sin(atan(2)) * mm;
        G4double z = sizeBGOinZ / 2 + BGOCouterd * mm;
        // outerphyBGOin[i]=
        new G4PVPlacement(0, G4ThreeVector(-x, y, z), outerlogicBGOin[i], outerBGOinname[i], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+16]=
        new G4PVPlacement(0, G4ThreeVector(-x, y, -z), outerlogicBGOin[i + 16], outerBGOinname[i + 16], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+2]=
        new G4PVPlacement(0, G4ThreeVector(-y, x, z), outerlogicBGOin[i + 2], outerBGOinname[i + 2], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+2+16]=
        new G4PVPlacement(0, G4ThreeVector(-y, x, -z), outerlogicBGOin[i + 2 + 16], outerBGOinname[i + 2 + 16], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+4]=
        new G4PVPlacement(0, G4ThreeVector(-y, -x, z), outerlogicBGOin[i + 4], outerBGOinname[i + 4], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+4+16]=
        new G4PVPlacement(0, G4ThreeVector(-y, -x, -z), outerlogicBGOin[i + 4 + 16], outerBGOinname[i + 4 + 16], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+6]=
        new G4PVPlacement(0, G4ThreeVector(-x, -y, z), outerlogicBGOin[i + 6], outerBGOinname[i + 6], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+6+16]=
        new G4PVPlacement(0, G4ThreeVector(-x, -y, -z), outerlogicBGOin[i + 6 + 16], outerBGOinname[i + 6 + 16], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+8]=
        new G4PVPlacement(0, G4ThreeVector(x, -y, z), outerlogicBGOin[i + 8], outerBGOinname[i + 8], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+8+16]=
        new G4PVPlacement(0, G4ThreeVector(x, -y, -z), outerlogicBGOin[i + 8 + 16], outerBGOinname[i + 8 + 16], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+10]=
        new G4PVPlacement(0, G4ThreeVector(y, -x, z), outerlogicBGOin[i + 10], outerBGOinname[i + 10], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+10+16]=
        new G4PVPlacement(0, G4ThreeVector(y, -x, -z), outerlogicBGOin[i + 10 + 16], outerBGOinname[i + 10 + 16], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+12]=
        new G4PVPlacement(0, G4ThreeVector(y, x, z), outerlogicBGOin[i + 12], outerBGOinname[i + 12], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+12+16]=
        new G4PVPlacement(0, G4ThreeVector(y, x, -z), outerlogicBGOin[i + 12 + 16], outerBGOinname[i + 12 + 16], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+14]=
        new G4PVPlacement(0, G4ThreeVector(x, y, z), outerlogicBGOin[i + 14], outerBGOinname[i + 14], logicEnv, false, 0, checkOverlaps);
        // outerphyBGOin[i+14+16]=
        new G4PVPlacement(0, G4ThreeVector(x, y, -z), outerlogicBGOin[i + 14 + 16], outerBGOinname[i + 14 + 16], logicEnv, false, 0, checkOverlaps);
      }
      outerlogicBGOin[i]->SetVisAttributes(AirVisAtt);
      outerlogicBGOin[i + 16]->SetVisAttributes(AirVisAtt);
    }
    // Air_in
    G4double sizeAirX = 61.5 * mm;//内径63.5
    G4double sizeAirY = 61.5 * mm;
    G4double sizeAirZ = 150.5 * mm;//152.5-150.5
    G4Box *solidAir = new G4Box("Air", 0.5 * sizeAirX, 0.5 * sizeAirY, 0.5 * sizeAirZ);
    // G4VPhysicalVolume *innerphyAir[16];
    G4LogicalVolume *innerlogicAir[16];
    // G4VPhysicalVolume *outerphyAir[32];
    G4LogicalVolume *outerlogicAir[32];
    for (G4int i = 0; i < 16; i++)
    {
      outerlogicAir[i] = new G4LogicalVolume(solidAir, mESR, "outerAir");
      outerlogicAir[i + 16] = new G4LogicalVolume(solidAir, mESR, "outerAir");
    }
    for (G4int i = 0; i < 8; i++)
    {
      innerlogicAir[i] = new G4LogicalVolume(solidAir, mESR, "innerAir");
      innerlogicAir[i + 8] = new G4LogicalVolume(solidAir, mESR, "innerAir");
      G4double x = 0. * mm;
      G4double y = 0. * mm;
      G4double z1 = 0. * mm;
      G4double z2 = 0. * mm;
      G4ThreeVector posinnerAir1 = G4ThreeVector(x, y, z1);
      G4ThreeVector posinnerAir2 = G4ThreeVector(x, y, z2);
      new G4PVPlacement(0, posinnerAir1, innerlogicAir[i], innerAirname[i], innerlogicBGOin[i], false, 0, checkOverlaps);
      new G4PVPlacement(0, posinnerAir2, innerlogicAir[i + 8], innerAirname[i + 8], innerlogicBGOin[i + 8], false, 0, checkOverlaps);
      innerlogicAir[i]->SetVisAttributes(AirVisAtt);
      innerlogicAir[i + 8]->SetVisAttributes(AirVisAtt);
    }
    for (G4int i = 0; i < 16; i++)
    {
      outerlogicAir[i] = new G4LogicalVolume(solidAir, mESR, "outerAir");
      outerlogicAir[i + 16] = new G4LogicalVolume(solidAir, mESR, "outerAir");
      G4double x = 0. * mm;
      G4double y = 0. * mm;
      G4double z1 = 0. * mm;
      G4double z2 = 0. * mm;
      G4ThreeVector posouterAir1 = G4ThreeVector(x, y, z1);
      G4ThreeVector posouterAir2 = G4ThreeVector(x, y, z2);
      new G4PVPlacement(0, posouterAir1, outerlogicAir[i], outerAirname[i], outerlogicBGOin[i], false, 0, checkOverlaps);
      new G4PVPlacement(0, posouterAir2, outerlogicAir[i + 16], outerAirname[i + 16], outerlogicBGOin[i + 16], false, 0, checkOverlaps);
      outerlogicAir[i]->SetVisAttributes(AirVisAtt);
      outerlogicAir[i + 16]->SetVisAttributes(AirVisAtt);
    }
    // BGO_Crystal
    G4double sizeBGOX = 60. * mm;
    G4double sizeBGOY = 60. * mm;
    G4double sizeBGOZ = 120 * mm;
    G4double BGOzd =1.0 *mm;//晶体离内壳距离6*6那个面,相对的时air层64.5-63.5-61.5-60
    G4Box *solidBGO = new G4Box("BGO", 0.5 * sizeBGOX, 0.5 * sizeBGOY, 0.5 * sizeBGOZ);
    // G4VPhysicalVolume *innerphyBGO[16];
    G4LogicalVolume *innerlogicBGO[16];
    // G4VPhysicalVolume *outerphyBGO[32];
    G4LogicalVolume *outerlogicBGO[32];
    for (G4int i = 0; i < 16; i++)
    {
      outerlogicBGO[i] = new G4LogicalVolume(solidBGO, mBGO, "outerBGO");
      outerlogicBGO[i + 16] = new G4LogicalVolume(solidBGO, mBGO, "outerBGO");
    }
    for (G4int i = 0; i < 8; i++)
    {
      innerlogicBGO[i] = new G4LogicalVolume(solidBGO, mBGO, "innerBGO");
      innerlogicBGO[i + 8] = new G4LogicalVolume(solidBGO, mBGO, "innerBGO");
      G4double x = 0. * mm;
      G4double y = 0. * mm;
      G4double z1 = (sizeBGOZ - sizeAirZ) / 2 + BGOzd * mm;//=（120-152.5）/2+0.5=-15.75*mm；sizeBGOZ = 120 * mm;sizeAirZ = (sizePE_B_Z / 2) * mm=152.5 *mm;sizePE_B_Z=305. *mm
      G4double z2 = (sizeAirZ - sizeBGOZ) / 2 - BGOzd * mm;//0.5mm是晶体离内壳距离
      G4ThreeVector posinnerBGO1 = G4ThreeVector(x, y, z1);
      G4ThreeVector posinnerBGO2 = G4ThreeVector(x, y, z2);
      new G4PVPlacement(0, posinnerBGO1, innerlogicBGO[i], innerBGOname[i], innerlogicAir[i], false, 0, checkOverlaps);
      new G4PVPlacement(0, posinnerBGO2, innerlogicBGO[i + 8], innerBGOname[i + 8], innerlogicAir[i + 8], false, 0, checkOverlaps);
      innerlogicBGO[i]->SetVisAttributes(innerBGOVisAtt);
      innerlogicBGO[i + 8]->SetVisAttributes(innerBGOVisAtt);
    }
    for (G4int i = 0; i < 16; i++)
    {
      outerlogicBGO[i] = new G4LogicalVolume(solidBGO, mBGO, "outerBGO");
      outerlogicBGO[i + 16] = new G4LogicalVolume(solidBGO, mBGO, "outerBGO");
      G4double x = 0. * mm;
      G4double y = 0. * mm;
      G4double z1 = (sizeBGOZ - sizeAirZ) / 2 + BGOzd * mm;
      G4double z2 = (sizeAirZ - sizeBGOZ) / 2 - BGOzd * mm;
      G4ThreeVector posouterBGO1 = G4ThreeVector(x, y, z1);
      G4ThreeVector posouterBGO2 = G4ThreeVector(x, y, z2);
      new G4PVPlacement(0, posouterBGO1, outerlogicBGO[i], outerBGOname[i], outerlogicAir[i], false, 0, checkOverlaps);
      new G4PVPlacement(0, posouterBGO2, outerlogicBGO[i + 16], outerBGOname[i + 16], outerlogicAir[i + 16], false, 0, checkOverlaps);
      outerlogicBGO[i]->SetVisAttributes(outerBGOVisAtt);
      outerlogicBGO[i + 16]->SetVisAttributes(outerBGOVisAtt);
    }

#if 0
    // Al_back
    G4double sizeAl_backX = 60 * mm;
    G4double sizeAl_backY = 60 * mm;
    G4double sizeAl_backZ = 25 * mm;
    G4Box *solidAl_back = new G4Box("Al_back", 0.5 * sizeAl_backX, 0.5 * sizeAl_backY, 0.5 * sizeAl_backZ);
    // G4VPhysicalVolume *innerphyAl_back[16];
    G4LogicalVolume *innerlogicAl_back[16];
    // G4VPhysicalVolume *outerphyAl_back[32];
    G4LogicalVolume *outerlogicAl_back[32];
    for (G4int i = 0; i < 16; i++)
    {
      outerlogicAl_back[i] = new G4LogicalVolume(solidAl_back, mAl, "outerAl_back");
      outerlogicAl_back[i + 16] = new G4LogicalVolume(solidAl_back, mAl, "outerAl_back");
    }
    for (G4int i = 0; i < 8; i++)
    {
      innerlogicAl_back[i] = new G4LogicalVolume(solidAl_back, mAl, "innerAl_back");
      innerlogicAl_back[i + 8] = new G4LogicalVolume(solidAl_back, mAl, "innerAl_back");
      G4double x = 0. * mm;
      G4double y = 0. * mm;
      // G4double z1 = (sizeBGOZ + sizeAl_backZ) / 2 - (sizeBGOinZ - sizeBGOZ) / 2 + 1.05 * mm;//正值
      // G4double z2 = (sizeBGOinZ - sizeBGOZ) / 2 - (sizeBGOZ + sizeAl_backZ) / 2 - 1.05 * mm;//负值
      G4double z1 = (sizeAirZ-sizeAl_backZ)/2.;
      G4double z2 = -(sizeAirZ-sizeAl_backZ)/2.;
      G4ThreeVector posinnerAl_back1 = G4ThreeVector(x, y, z1);
      G4ThreeVector posinnerAl_back2 = G4ThreeVector(x, y, z2);
      new G4PVPlacement(0, posinnerAl_back1, innerlogicAl_back[i], innerAl_backname[i], innerlogicAir[i], false, 0, checkOverlaps);
      new G4PVPlacement(0, posinnerAl_back2, innerlogicAl_back[i + 8], innerAl_backname[i + 8], innerlogicAir[i + 8], false, 0, checkOverlaps);
      innerlogicAl_back[i]->SetVisAttributes(Al_backVisAtt);
      innerlogicAl_back[i + 8]->SetVisAttributes(Al_backVisAtt);
    }
    for (G4int i = 0; i < 16; i++)
    {
      outerlogicAl_back[i] = new G4LogicalVolume(solidAl_back, mAl, "outerAl_back");
      outerlogicAl_back[i + 16] = new G4LogicalVolume(solidAl_back, mAl, "outerAl_back");
      G4double x = 0. * mm;
      G4double y = 0. * mm;
      // G4double z1 = (sizeBGOZ + sizeAl_backZ) / 2 - (sizeBGOinZ - sizeBGOZ) / 2 + 1.05 * mm;
      // G4double z2 = (sizeBGOinZ - sizeBGOZ) / 2 - (sizeBGOZ + sizeAl_backZ) / 2 - 1.05 * mm;
      G4double z1 = (sizeAirZ-sizeAl_backZ)/2.;
      G4double z2 = -(sizeAirZ-sizeAl_backZ)/2.;
      G4ThreeVector posouterAl_back1 = G4ThreeVector(x, y, z1);
      G4ThreeVector posouterAl_back2 = G4ThreeVector(x, y, z2);
      new G4PVPlacement(0, posouterAl_back1, outerlogicAl_back[i], outerAl_backname[i], outerlogicAir[i], false, 0, checkOverlaps);
      new G4PVPlacement(0, posouterAl_back2, outerlogicAl_back[i + 16], outerAl_backname[i + 16], outerlogicAir[i + 16], false, 0, checkOverlaps);
      outerlogicAl_back[i]->SetVisAttributes(Al_backVisAtt);
      outerlogicAl_back[i + 16]->SetVisAttributes(Al_backVisAtt);
    }
    G4double shiftd = 0.00 * mm;//

    // // target//靶
    G4double sizeTarThickness = 1.00 * mm;
    G4double sizeTarRadians = 23.75 * mm;

    G4Tubs *solidTar = new G4Tubs("Tar", 0.0 * mm, sizeTarRadians, sizeTarThickness * 0.5, 0. * deg, 360. * deg);
    G4ThreeVector posTarVector = G4ThreeVector(0., 0., -sizeTarThickness / 2.0 * mm+shiftd);
    G4LogicalVolume *logicTar = new G4LogicalVolume(solidTar, mTa, "Tar");
    new G4PVPlacement(0, posTarVector, logicTar, "Tar", logicEnv, false, 0, checkOverlaps);
    logicTar->SetVisAttributes(tarVisAtt);


    // beam tube//靶管
    G4double sizeVaTThickness = 600. * mm;
    G4double sizeVaTRouter = 30.5 * mm;//靶管外径61mm；
    G4double sizeVaTRinner = 27. * mm;//旧模拟使用内径48mm；图纸内径54cm；
    G4ThreeVector posVaTVector = G4ThreeVector(0., 0., sizeVaTThickness / 2. - 7. * mm+shiftd);
    G4Tubs *solidVaT = new G4Tubs("VaT", sizeVaTRinner, sizeVaTRouter, sizeVaTThickness / 2., 0., 2. * M_PI);
    G4LogicalVolume *logicVaT = new G4LogicalVolume(solidVaT, mAl, "VaT");
    new G4PVPlacement(0, posVaTVector, logicVaT, "VaT", logicEnv, false, 0, checkOverlaps);
    logicVaT->SetVisAttributes(Al_tubeVisAtt);

    // beam tube2
    G4double sizeVaT2Thickness = 11.3 * mm;
    G4double sizeVaT2Router = 54./2. * mm;
    G4double sizeVaT2Rinner = 47.6/2. * mm;
    G4ThreeVector posVaT2Vector = G4ThreeVector(0., 0., sizeVaT2Thickness/2- 7. * mm + shiftd);
    G4Tubs *solidVaT2 = new G4Tubs("VaT2", sizeVaT2Rinner, sizeVaT2Router, sizeVaT2Thickness / 2., 0., 2. * M_PI);
    G4LogicalVolume *logicVaT2 = new G4LogicalVolume(solidVaT2, mAl, "VaT2");
    new G4PVPlacement(0, posVaT2Vector, logicVaT2, "VaT2", logicEnv, false, 0, checkOverlaps);
    logicVaT2->SetVisAttributes(Al_tubeVisAtt);

    // beam tube3
    G4double sizeVaT3Thickness = 2. * mm;
    G4double sizeVaT3Router = 47.6/2. * mm;
    G4double sizeVaT3Rinner = 39.5/2. * mm;
    G4ThreeVector posVaT3Vector = G4ThreeVector(0., 0.,(sizeVaT3Thickness)/2. +shiftd);
    G4Tubs *solidVaT3 = new G4Tubs("VaT3", sizeVaT3Rinner, sizeVaT3Router, sizeVaT3Thickness / 2., 0., 2. * M_PI);
    G4LogicalVolume *logicVaT3 = new G4LogicalVolume(solidVaT3, mAl, "VaT3");
    new G4PVPlacement(0, posVaT3Vector, logicVaT3, "VaT3", logicEnv, false, 0, checkOverlaps);
    logicVaT3->SetVisAttributes(Al_tubeVisAtt);


    // cold trap//冷阱
    G4double sizeCTThickness = 570. * mm;//冷阱距离靶面7.5mm
    G4double sizeCTRouter = 21. * mm;
    G4double sizeCTRinner = 19. * mm;
    G4double CTdistance =7.0 *mm;
    G4ThreeVector posCTVector = G4ThreeVector(0., 0., sizeCTThickness / 2. * mm+shiftd+CTdistance);
    G4Tubs *solidCT = new G4Tubs("CT", sizeCTRinner, sizeCTRouter, sizeCTThickness / 2., 0. * deg, 360. * deg);
    G4LogicalVolume *logicCT = new G4LogicalVolume(solidCT, mCu, "CT");
    new G4PVPlacement(0, posCTVector, logicCT, "CT", logicEnv, false, 0, checkOverlaps);
    logicCT->SetVisAttributes(CTVisAtt);


    // source
    G4double sourceThickness = 3. * mm;
    G4double sourceRadians = 12.5 * mm;
    G4Tubs *solidsource = new G4Tubs("source", 0.0 * mm, sourceRadians, sourceThickness * 0.5, 0. * deg, 360. * deg);
    G4ThreeVector possourceVector = G4ThreeVector(0., 0., sourceThickness / 2.+shiftd);
    G4LogicalVolume *logicsource = new G4LogicalVolume(solidsource, mPla, "source");
    new G4PVPlacement(0, possourceVector, logicsource, "source", logicEnv, false, 0, checkOverlaps);
    logicsource->SetVisAttributes(TaVisAtt);



    // water cooling靶后盖
    G4double shape1Z = 3. * mm;//原来是3mm
    G4double shape1R = 59/2. * mm;
    G4double shape2Z = 6.2 * mm;
    G4double shape2R = 47.5/2 * mm;
    G4double shape3Z = 1.5 * mm;//1.5
    G4double shape3R = 38/2. * mm;
    G4double shape4Z = 6. * mm;
    G4double shape4R = 42/2. * mm;
    G4double shape5Z = 10. * mm;//1.5mm
    G4double shape5R = 8./2. * mm;    
    G4Tubs *solidshape1 = new G4Tubs("shape1", 0.0 * mm, shape1R, shape1Z * 0.5, 0. * deg, 360. * deg);
    G4Tubs *solidshape2 = new G4Tubs("shape2", 0.0 * mm, shape2R, shape2Z * 0.5, 0. * deg, 360. * deg);
    G4Tubs *solidshape3 = new G4Tubs("shape3", 0.0 * mm, shape3R, shape3Z * 0.5, 0. * deg, 360. * deg);
    G4Tubs *solidshape4 = new G4Tubs("shape4", 0.0 * mm, shape4R, shape4Z * 0.5, 0. * deg, 360. * deg);
    G4Tubs *solidshape5 = new G4Tubs("shape5", 0.0 * mm, shape5R, shape5Z * 0.5, 0. * deg, 360. * deg);
    G4UnionSolid *watercooling = new G4UnionSolid("union", solidshape1, solidshape2, 0, G4ThreeVector(0., 0., (shape1Z + shape2Z) / 2.));//shape1位置为参考，shape1是大环
    G4SubtractionSolid *WCsub1 = new G4SubtractionSolid("sub1", watercooling, solidshape5, 0, G4ThreeVector(0.,19. * mm, 3. * mm));//watercooling的参考位置是watercooling的
    G4SubtractionSolid *WCsub = new G4SubtractionSolid("sub", WCsub1, solidshape5, 0, G4ThreeVector(0., -19. * mm, 3. * mm));
    G4SubtractionSolid *WCsub2 = new G4SubtractionSolid("sub2", WCsub, solidshape3, 0, G4ThreeVector(0., 0., 6.95 * mm));
    G4SubtractionSolid *WCsub3 = new G4SubtractionSolid("sub3", WCsub2, solidshape4, 0, G4ThreeVector(0., 0., 1.5 * mm));
    G4ThreeVector posWCVector = G4ThreeVector(0., 0., -7.0*mm-shape1Z/2+shiftd-0.5*mm);
    G4LogicalVolume *logicWC = new G4LogicalVolume(WCsub3, mAl, "water cooling");
    new G4PVPlacement(0, posWCVector, logicWC, "WC", logicEnv, false, 0, checkOverlaps);
    logicWC->SetVisAttributes(TaVisAtt);
#endif
    /**********************************Pb shadows are here*************************************/
    // Pb_Bottom
    G4double PbThickness = 100 * mm;
    G4double PbBottomXY = 100 * cm;
    G4Box *solidPb_B = new G4Box("Pb_B", PbBottomXY / 2, PbBottomXY / 2, PbThickness / 2);
    G4ThreeVector posPb_B = G4ThreeVector(0., -(sizePE_B_Y + PbThickness) / 2, 0.);
    G4LogicalVolume *logicPb_B = new G4LogicalVolume(solidPb_B, mPb, "Pb_B");
    G4RotationMatrix *Pb_BRot = new G4RotationMatrix();
    Pb_BRot->rotateX(-90. * deg);
    new G4PVPlacement(Pb_BRot, posPb_B, logicPb_B, "Pb_Bottom", logicEnv, false, 0, checkOverlaps);
    logicPb_B->SetVisAttributes(PbVisAtt);
    // Pb_TOP
    G4double PbToPY = 100 * cm;
    G4double PbToPX = 70 * cm;
    G4Box *solidPb_T = new G4Box("Pb_T", PbToPX / 2, PbToPY / 2, PbThickness / 2);
    G4ThreeVector posPb_T = G4ThreeVector(0., 55. * cm - sizePE_B_Y / 2, 0.);
    G4LogicalVolume *logicPb_T = new G4LogicalVolume(solidPb_T, mPb, "Pb_t");
    G4RotationMatrix *Pb_TRot = new G4RotationMatrix();
    Pb_TRot->rotateX(-90. * deg);
    new G4PVPlacement(Pb_TRot, posPb_T, logicPb_T, "Pb_ToP", logicEnv, false, 0, checkOverlaps);
    logicPb_T->SetVisAttributes(PbVisAtt);
    // Pb_left
    G4double PbleftY = 50 * cm;
    G4double PbleftX = 90 * cm;
    G4Box *solidPbleft = new G4Box("Pbleft", PbleftX / 2, PbleftY / 2, PbThickness / 2);
    G4ThreeVector posPbleft = G4ThreeVector((sizePE_B_X + PbThickness) / 2 + 1. * cm, 25. * cm - sizePE_B_Y / 2, 0.);
    G4LogicalVolume *logicPbleft = new G4LogicalVolume(solidPbleft, mPb, "Pbleft");
    G4RotationMatrix *PbleftRot = new G4RotationMatrix();
    PbleftRot->rotateY(-90. * deg);
    new G4PVPlacement(PbleftRot, posPbleft, logicPbleft, "Pbleft", logicEnv, false, 0, checkOverlaps);
    logicPbleft->SetVisAttributes(PbVisAtt);
    // Pb_Right
    G4double PbRightY = 50 * cm;
    G4double PbRightX = 90 * cm;
    G4Box *solidPbRight = new G4Box("PbRight", PbRightX / 2, PbRightY / 2, PbThickness / 2);
    G4ThreeVector posPbRight = G4ThreeVector(-(sizePE_B_X + PbThickness) / 2 - 1. * cm, 25. * cm - sizePE_B_Y / 2, 0.);
    G4LogicalVolume *logicPbRight = new G4LogicalVolume(solidPbRight, mPb, "PbRight");
    G4RotationMatrix *PbRightRot = new G4RotationMatrix();
    PbRightRot->rotateY(-90. * deg);
    new G4PVPlacement(PbRightRot, posPbRight, logicPbRight, "PbRight", logicEnv, false, 0, checkOverlaps);
    logicPbRight->SetVisAttributes(PbVisAtt);
    // Pb_Front
    G4double PbFrontY = 50. * cm;
    G4double PbFrontX = 40.75 * cm;
    G4Box *solidPbFront = new G4Box("PbFront", PbFrontX / 2, PbFrontY / 2, PbThickness / 2);
    G4ThreeVector posPbFront = G4ThreeVector(0, 25. * cm - sizePE_B_Y / 2, 35. * cm);
    // 定义钻孔的参数
    G4double Pbholesize = 65 * mm; // 铅孔的尺寸
    // 创建钻孔的实体
    G4Tubs *Pbholesolid = new G4Tubs("Pbhole", 0.0 * mm, Pbholesize/2, PbThickness * 0.5, 0. * deg, 360. * deg);
    G4SubtractionSolid *PbFrontSub = new G4SubtractionSolid("solidPbsub", solidPbFront, Pbholesolid, 0, G4ThreeVector(0, sizePE_B_Y / 2 - 25. * cm, 0));
    G4LogicalVolume *logicPbFront = new G4LogicalVolume(PbFrontSub, mPb, "PbFront");
    new G4PVPlacement(0, posPbFront, logicPbFront, "PbFront", logicEnv, false, 0, checkOverlaps);
    logicPbFront->SetVisAttributes(PbVisAtt);
    G4ThreeVector posPbBack = G4ThreeVector(0, 25. * cm - sizePE_B_Y / 2, -35. * cm);
    G4LogicalVolume *logicPbBack = new G4LogicalVolume(PbFrontSub, mPb, "PbBack");
    new G4PVPlacement(0, posPbBack, logicPbBack, "PbBack", logicEnv, false, 0, checkOverlaps);
    logicPbBack->SetVisAttributes(PbVisAtt);

    /**********************************Pb shadows are here*************************************/

    // fScoringVolume defintion
    for (G4int i = 0; i < nInnerBGO; i++)
      fScoringVolume[i] = innerlogicBGO[i];
    for (G4int i = 0; i < nOuterBGO; i++)
      fScoringVolume[i + 16] = outerlogicBGO[i];
    // always return the physical World

    return physWorld;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
