#include "DetectorConstruction.hh"

#include <G4Box.hh>
#include <G4LogicalVolume.hh>
#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>

DetectorConstruction::DetectorConstruction(const SimConfig& cfg) : cfg_(cfg) {}

G4Material* DetectorConstruction::BuildGasMaterial() const {
  auto* nist = G4NistManager::Instance();
  auto* base = nist->FindOrBuildMaterial(cfg_.gas_material);

  // 将标准材料按指定气压缩放近似为低压气体密度。
  constexpr double reference_pressure_mbar = 1013.25;
  const double scale = cfg_.gas_pressure_mbar / reference_pressure_mbar;
  const double density = base->GetDensity() * scale;

  auto* gas = new G4Material("DetectorGas", density, base);
  gas->SetState(kStateGas);
  return gas;
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
  auto* nist = G4NistManager::Instance();
  auto* world_mat = nist->FindOrBuildMaterial("G4_Galactic");
  auto* gas_mat = BuildGasMaterial();
  auto* mylar = nist->FindOrBuildMaterial("G4_MYLAR");
  auto* al = nist->FindOrBuildMaterial("G4_Al");

  const double world_x = 500.0 * mm;
  const double world_y = 300.0 * mm;
  const double world_z = 800.0 * mm;

  auto* solid_world = new G4Box("World", world_x / 2, world_y / 2, world_z / 2);
  auto* logic_world = new G4LogicalVolume(solid_world, world_mat, "World");
  auto* phys_world = new G4PVPlacement(nullptr, {}, logic_world, "World", nullptr, false, 0);

  // 探测器气室长度与阴极板一致：373 mm。
  const double chamber_x = 65.0 * mm;
  const double chamber_y = 40.0 * mm;
  const double chamber_z = 373.0 * mm;

  auto* solid_chamber = new G4Box("GasChamber", chamber_x / 2, chamber_y / 2, chamber_z / 2);
  auto* logic_chamber = new G4LogicalVolume(solid_chamber, gas_mat, "GasChamber");
  new G4PVPlacement(nullptr, {}, logic_chamber, "GasChamber", logic_world, false, 0);

  // 入射窗：65mm x 40mm，厚度 2.5um 的 Mylar 膜。
  const double window_t = 2.5 * um;
  auto* solid_window = new G4Box("EntranceWindow", chamber_x / 2, chamber_y / 2, window_t / 2);
  auto* logic_window = new G4LogicalVolume(solid_window, mylar, "EntranceWindow");
  new G4PVPlacement(nullptr,
                    G4ThreeVector(0, 0, -chamber_z / 2 + window_t / 2),
                    logic_window,
                    "EntranceWindow",
                    logic_chamber,
                    false,
                    0);

  // 四个阳极板长度：100, 80, 60, 130 mm；一个阴极板长度 373 mm。
  const double anode_lengths[4] = {100.0 * mm, 80.0 * mm, 60.0 * mm, 130.0 * mm};
  const char* anode_names[4] = {"AnodeE1", "AnodeE2", "AnodeE3", "AnodeE4"};
  const double electrode_t = 0.5 * mm;

  double z_cursor = -chamber_z / 2;
  for (int i = 0; i < 4; ++i) {
    const double seg_z = anode_lengths[i];
    const double z_center = z_cursor + seg_z / 2.0;

    auto* solid = new G4Box(anode_names[i], chamber_x / 2, electrode_t / 2, seg_z / 2);
    auto* logic = new G4LogicalVolume(solid, al, anode_names[i]);
    new G4PVPlacement(nullptr,
                      G4ThreeVector(0, chamber_y / 2 - electrode_t / 2, z_center),
                      logic,
                      anode_names[i],
                      logic_chamber,
                      false,
                      i);

    z_cursor += seg_z;
  }

  auto* solid_cathode = new G4Box("CathodeEtotal", chamber_x / 2, electrode_t / 2, chamber_z / 2);
  auto* logic_cathode = new G4LogicalVolume(solid_cathode, al, "CathodeEtotal");
  new G4PVPlacement(nullptr,
                    G4ThreeVector(0, -chamber_y / 2 + electrode_t / 2, 0),
                    logic_cathode,
                    "CathodeEtotal",
                    logic_chamber,
                    false,
                    0);

  // 简化栅极层。
  const double grid_t = 0.1 * mm;
  auto* solid_grid = new G4Box("GridLayer", chamber_x / 2, grid_t / 2, chamber_z / 2);
  auto* logic_grid = new G4LogicalVolume(solid_grid, al, "GridLayer");
  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logic_grid, "GridLayer", logic_chamber, false, 0);

  return phys_world;
}
