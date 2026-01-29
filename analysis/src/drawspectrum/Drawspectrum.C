#include <TChain.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TCut.h>
#include <iostream>

void Drawspectrum(
    TString filePattern = "run*_cal.root",
    TString branch = "sumInner",
    int bins = 5000,
    double xmin = 0,
    double xmax = 10000
) {


    TChain chain("ana");
    for (int run = 1; run <= 50; run++) {
        TString fname = Form("../../data/cal/run%05d_cal.root", run);
        chain.Add(fname);
    }


    std::cout << ">>> Total entries = " << chain.GetEntries() << std::endl;

    TH1D* h = new TH1D(
        "h",
        Form("%s spectrum;Energy;Counts", branch.Data()),
        bins, xmin, xmax
    );

    // 条件
    TCut cut = "nhit==1";

    chain.Draw(Form("%s>>h", branch.Data()), cut, "goff");

    TCanvas* c = new TCanvas("c", "spectrum", 900, 700);
    h->Draw();

    //c->SaveAs("spectrum.png");
}
