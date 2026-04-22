#include <TCanvas.h>
#include <TFile.h>
#include <TH2D.h>
#include <TStyle.h>
#include <TTree.h>

#include <array>
#include <iostream>
#include <string>

namespace {
struct PairDef {
  const char* x;
  const char* y;
  const char* title;
};
}

void draw_energy_pairs(const char* input_root = "events.root", const char* tree_name = "gas") {
  gStyle->SetOptStat(0);

  TFile fin(input_root, "READ");
  if (fin.IsZombie()) {
    std::cerr << "[ERROR] cannot open file: " << input_root << '\n';
    return;
  }

  auto* tree = dynamic_cast<TTree*>(fin.Get(tree_name));
  if (!tree) {
    std::cerr << "[ERROR] cannot find tree: " << tree_name << '\n';
    return;
  }

  double e1 = 0.0, e2 = 0.0, e3 = 0.0, e4 = 0.0;
  tree->SetBranchAddress("e1_MeV", &e1);
  tree->SetBranchAddress("e2_MeV", &e2);
  tree->SetBranchAddress("e3_MeV", &e3);
  tree->SetBranchAddress("e4_MeV", &e4);

  const std::array<PairDef, 6> pairs{{
      {"e1", "e2", "E1 vs E2"},
      {"e1", "e3", "E1 vs E3"},
      {"e1", "e4", "E1 vs E4"},
      {"e2", "e3", "E2 vs E3"},
      {"e2", "e4", "E2 vs E4"},
      {"e3", "e4", "E3 vs E4"},
  }};

  std::array<TH2D*, 6> h2{};
  for (size_t i = 0; i < pairs.size(); ++i) {
    const std::string hname = std::string("h2_") + pairs[i].x + "_" + pairs[i].y;
    h2[i] = new TH2D(hname.c_str(), pairs[i].title, 300, 0.0, 30.0, 300, 0.0, 30.0);
    h2[i]->GetXaxis()->SetTitle((std::string(pairs[i].x) + " (MeV)").c_str());
    h2[i]->GetYaxis()->SetTitle((std::string(pairs[i].y) + " (MeV)").c_str());
  }

  const auto nentries = tree->GetEntries();
  for (Long64_t i = 0; i < nentries; ++i) {
    tree->GetEntry(i);
    h2[0]->Fill(e1, e2);
    h2[1]->Fill(e1, e3);
    h2[2]->Fill(e1, e4);
    h2[3]->Fill(e2, e3);
    h2[4]->Fill(e2, e4);
    h2[5]->Fill(e3, e4);
  }

  TCanvas c("c_pairs", "Energy Pair Spectra", 1800, 1000);
  c.Divide(3, 2);
  for (size_t i = 0; i < h2.size(); ++i) {
    c.cd(static_cast<int>(i + 1));
    h2[i]->Draw("COLZ");
  }

  c.Update();
  std::cout << "[OK] 2D spectra are drawn on canvas c_pairs." << '\n';
  std::cout << "[INFO] Use c_pairs->SaveAs(\"your_name.png\") manually if needed." << '\n';
}
