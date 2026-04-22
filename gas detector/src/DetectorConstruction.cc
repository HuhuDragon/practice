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

  const double pressure = cfg_.gas_pressure_mbar * bar / 1000.0;
  const double temperature = 273.15 * kelvin;
  auto* gas = new G4Material("DetectorGas", density, base, kStateGas, temperature, pressure);
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

  // 以“气体分段体积”表示 E1~E4 信号区，确保可统计该区能量沉积。
  const double seg_lengths[4] = {100.0 * mm, 80.0 * mm, 60.0 * mm, 130.0 * mm};
  const char* seg_names[4] = {"AnodeE1", "AnodeE2", "AnodeE3", "AnodeE4"};

  double z_cursor = -chamber_z / 2;
  for (int i = 0; i < 4; ++i) {
    const double seg_z = seg_lengths[i];
    const double z_center = z_cursor + seg_z / 2.0;

    auto* solid_seg = new G4Box(seg_names[i], chamber_x / 2, chamber_y / 2, seg_z / 2);
    auto* logic_seg = new G4LogicalVolume(solid_seg, gas_mat, seg_names[i]);
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, z_center), logic_seg, seg_names[i], logic_chamber, false, i);

    z_cursor += seg_z;
  }

  // 保留阴极与栅极几何（结构展示用，不用于 E1~E4 分段计数）。
  const double electrode_t = 0.5 * mm;
  auto* solid_cathode = new G4Box("CathodeEtotal", chamber_x / 2, electrode_t / 2, chamber_z / 2);
  auto* logic_cathode = new G4LogicalVolume(solid_cathode, al, "CathodeEtotal");
  new G4PVPlacement(nullptr,
                    G4ThreeVector(0, -chamber_y / 2 + electrode_t / 2, 0),
                    logic_cathode,
                    "CathodeEtotal",
                    logic_chamber,
                    false,
                    0);

  const double grid_t = 0.1 * mm;
  auto* solid_grid = new G4Box("GridLayer", chamber_x / 2, grid_t / 2, chamber_z / 2);
  auto* logic_grid = new G4LogicalVolume(solid_grid, al, "GridLayer");
  new G4PVPlacement(nullptr, G4ThreeVector(0, chamber_y / 2 - 2.0 * mm, 0), logic_grid, "GridLayer", logic_chamber, false, 0);

  return phys_world;
}
