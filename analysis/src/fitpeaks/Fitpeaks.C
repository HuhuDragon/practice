#include <TChain.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TCut.h>
#include <iostream>
#include <vector>
#include <TMath.h>


TF1* BuildMultiPeakFunc(
    int nPeak,
    double xmin,
    double xmax
) {
    TString formula = "";

    // 高斯峰
    for (int i = 0; i < nPeak; i++) {
        formula += Form("gaus(%d)", i * 3);
        if (i != nPeak - 1) formula += "+";
    }

    // 线性本底
    formula += Form("+[%d]+[%d]*x", nPeak * 3, nPeak * 3 + 1);

    TF1* f = new TF1("f_multi", formula, xmin, xmax);
    return f;
}


void FitMultiPeak(
    TString dataDir      = "../../data/cal",
    TString filePattern  = "run*_cal.root",
    TString branch       = "sumInner",
    double fitMin        = 500,
    double fitMax        = 1500,
    std::vector<double> peakGuess = {662, 1173, 1332},
    int bins             = 4000
) {

    TChain chain("ana");
    for (int run = 1; run <= 50; run++) {                                //累加哪些轮次
        TString fname = Form("../../data/cal/run%05d_cal.root", run);
        chain.Add(fname);
    }



    TH1D* h = new TH1D(
        "h_fit",
        Form("%s spectrum;Energy;Counts", branch.Data()),
        bins, fitMin, fitMax
    );

    TCut cut = "nhit==1";                                        //条件
    chain.Draw(Form("%s>>h_fit", branch.Data()), cut, "goff");


    int nPeak = peakGuess.size();
    TF1* f = BuildMultiPeakFunc(nPeak, fitMin, fitMax);

    //参数初值 
    for (int i = 0; i < nPeak; i++) {
        int p = i * 3;
        f->SetParameter(p,     h->GetMaximum()); // amplitude
        f->SetParameter(p + 1, peakGuess[i]);    // mean
        f->SetParameter(p + 2, 3.0);              // sigma

        f->SetParLimits(p + 2, 0.5, 20.0);         // sigma 合理限制
    }

    // 本底初值
    f->SetParameter(nPeak * 3,     10);   // p0
    f->SetParameter(nPeak * 3 + 1, 0);    // p1


    h->Fit(f, "R");


    TCanvas* c = new TCanvas("c_fit", "peaks fit", 900, 700);
    h->Draw();
    f->SetLineColor(kRed);
    f->Draw("same");

    c->SaveAs("fitpeaks.png");


    std::cout << "\n===== Fit Results (Background subtracted) =====\n";

    for (int i = 0; i < nPeak; i++) {
        int p = i * 3;

        double A     = f->GetParameter(p);
        double Aerr  = f->GetParError(p);

        double mean  = f->GetParameter(p + 1);
        double merr  = f->GetParError(p + 1);

        double sigma = f->GetParameter(p + 2);
        double serr  = f->GetParError(p + 2);

        // 扣本底后净峰面积
        double area = A * sigma * sqrt(2.0 * TMath::Pi());

        //误差传播
        double area_err = area * sqrt(
            pow(Aerr / A, 2) +
            pow(serr / sigma, 2)
        );

        std::cout << "Peak " << i << "\n"
                << "  Mean  = " << mean  << " ± " << merr  << "\n"
                << "  Sigma = " << sigma << " ± " << serr  << "\n"
                << "  Net Area (bkg subtracted) = "
                << area << " ± " << area_err << "\n\n";
    }

}