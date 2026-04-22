#include "RunAction.hh"

#include <G4Run.hh>
#include <G4SystemOfUnits.hh>

RunAction::RunAction(const SimConfig& cfg) : cfg_(cfg) {}

RunAction::~RunAction() {
  if (out_.is_open()) out_.close();
}

void RunAction::BeginOfRunAction(const G4Run*) {
  out_.open(cfg_.output_csv, std::ios::out);
  out_ << "event,e1_MeV,e2_MeV,e3_MeV,e4_MeV,etotal_MeV\n";
}

void RunAction::EndOfRunAction(const G4Run*) {
  if (out_.is_open()) out_.close();
}

void RunAction::RecordEvent(double e1, double e2, double e3, double e4, double etotal) {
  static int event_id = 0;
  out_ << event_id++ << "," << e1 / MeV << "," << e2 / MeV << "," << e3 / MeV << "," << e4 / MeV << ","
       << etotal / MeV << "\n";
}
