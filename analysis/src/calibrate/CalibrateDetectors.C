#include <TChain.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TCut.h>
#include <TMath.h>
#include <iostream>
#include <vector>

TF1* BuildSinglePeakFunc(double xmin, double xmax)
{
    TF1* f = new TF1(
        "f_single",
        "gaus(0)+[3]",
        xmin, xmax
    );
    return f;
}


void CalibrateDetectors(
    TString dataDir     = "../../data/cal",
    TString filePattern = "run*_cal.root",
    TString branchBase  = "raw",
    double fitMin       = 600,                      //设置拟合范围，即用哪个峰位归一
    double fitMax       = 720,
    int bins            = 2000
) {
    // 1. 累加多 run

    TChain chain("ana");
    for (int run = 1; run <= 50; run++) {
        TString fname = Form("../../data/cal/run%05d_cal.root", run);
        chain.Add(fname);
    }



    const int NDET = 48;
    std::vector<double> peakPos(NDET, 0);


    // 2. 循环每块探测器
    for (int det = 0; det < NDET; det++) {

        TString hname = Form("h_det%d", det);
        TString htitle = Form(
            "Detector %d spectrum;Energy;Counts", det
        );

        TH1D* h = new TH1D(
            hname, htitle,
            bins, fitMin, fitMax
        );

        TString drawExpr = Form("%s[%d]>>%s",
                                branchBase.Data(),
                                det,
                                hname.Data());

        TCut cut = "nhit==1";
        chain.Draw(drawExpr, cut, "goff");

        if (h->GetEntries() < 100) {
            std::cerr << " Detector " << det
                      << " statistics too low\n";
            continue;
        }


        // 3. 拟合
        TF1* f = BuildSinglePeakFunc(fitMin, fitMax);

        f->SetParameter(0, h->GetMaximum());
        f->SetParameter(1, h->GetMean());
        f->SetParameter(2, 5.0);
        f->SetParameter(3, h->GetMinimum());

        f->SetParLimits(2, 0.5, 100.0);

        h->Fit(f, "RQ");  // Quiet + Range

        peakPos[det] = f->GetParameter(1);

        std::cout << "Det " << det
                  << " peak = " << peakPos[det] << std::endl;
    }


    // 4. 归一到 det 0
    double refPeak = peakPos[0];

    std::cout << "\n===== Normalization factors =====\n";

    for (int det = 0; det < NDET; det++) {
        if (peakPos[det] <= 0) continue;

        double norm = refPeak / peakPos[det];

        std::cout << "Detector " << det
                  << "  peak = " << peakPos[det]
                  << "  norm = " << norm << std::endl;
    }


    // 5. 输出系数到 txt
    std::ofstream fout("alig_factor.txt");

    if (!fout.is_open()) {
        std::cerr << " Cannot open output file!" << std::endl;
        return;
    }

    for (int det = 0; det < NDET; det++) {
        if (peakPos[det] > 0) {
            double norm = refPeak / peakPos[det];
            fout << norm << "\n";
        } else {
            fout << 0 << "\n";  // 拟合失败的探测器，占位
        }
    }

    fout.close();

    std::cout << "\n>>> Calibration factors written to alig_factor.txt\n";
}
