#pragma once

#include "Config.hh"

#include <G4VUserDetectorConstruction.hh>

class G4Material;
class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction {
 public:
  explicit DetectorConstruction(const SimConfig& cfg);

  G4VPhysicalVolume* Construct() override;

 private:
  G4Material* BuildGasMaterial() const;

  SimConfig cfg_;
};
