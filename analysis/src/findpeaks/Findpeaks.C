#include <TChain.h>
#include <TH1D.h>
#include <TSpectrum.h>
#include <iostream>
#include <vector>

void Findpeaks(
    TString filePattern = "RunName*_cal.root",
    TString branch = "sumInner",
    int bins = 5000,
    double xmin = 0,
    double xmax = 10000,
    double sigma = 2.0,
    double threshold = 0.03
) {
    TChain chain("ana");
    for (int run = 1; run <= 50; run++) {
        TString fname = Form("../../data/cal/run%05d_cal.root", run);
        chain.Add(fname);
    }




    TH1D* h = new TH1D(
        "h_peak",
        "Summed spectrum;Energy;Counts",
        bins, xmin, xmax
    );

    chain.Draw(Form("%s>>h_peak", branch.Data()), "nhit==1", "goff"); //注意条件

    // 寻峰
    TSpectrum spec(50);
    int nfound = spec.Search(h, sigma, "", threshold);

    std::cout << ">>> Found " << nfound << " peaks" << std::endl;

    double* xpeaks = spec.GetPositionX();
    for (int i = 0; i < nfound; i++) {
        std::cout << "Peak " << i
                  << " at E = " << xpeaks[i] << std::endl;
    }


    TCanvas* c = new TCanvas("c_peak", "Peaks", 900, 700);
    h->Draw();
    //c->SaveAs("peaks.png");
}
