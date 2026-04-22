#include "EventAction.hh"

#include "RunAction.hh"

#include <G4Event.hh>
#include <G4RunManager.hh>

#include <iomanip>
#include <iostream>
#include <string>

EventAction::EventAction(int total_events) : total_events_(total_events) {}

void EventAction::BeginOfEventAction(const G4Event*) {
  anode_edep_.fill(0.0);
  total_edep_ = 0.0;
}

void EventAction::EndOfEventAction(const G4Event*) {
  auto* run_action = const_cast<RunAction*>(dynamic_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction()));
  if (run_action) {
    run_action->RecordEvent(anode_edep_[0], anode_edep_[1], anode_edep_[2], anode_edep_[3], total_edep_);
  }

  ++finished_events_;
  PrintProgress();
}

void EventAction::AddToAnode(int idx, double edep) {
  if (idx >= 0 && idx < 4) anode_edep_[idx] += edep;
}

void EventAction::AddToTotal(double edep) { total_edep_ += edep; }

void EventAction::PrintProgress() {
  if (total_events_ <= 0) return;

  const int percent = static_cast<int>(100.0 * finished_events_ / total_events_);
  if (percent == last_percent_ && finished_events_ != total_events_) return;
  last_percent_ = percent;

  constexpr int bar_width = 40;
  const int filled = (bar_width * percent) / 100;

  std::string bar;
  bar.reserve(bar_width);
  for (int i = 0; i < bar_width; ++i) {
    bar.push_back(i < filled ? '=' : ' ');
  }

  std::cout << "\rProgress [" << bar << "] " << std::setw(3) << percent << "% (" << finished_events_ << "/"
            << total_events_ << ")" << std::flush;

  if (finished_events_ == total_events_) {
    std::cout << std::endl;
  }
}
