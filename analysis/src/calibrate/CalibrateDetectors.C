#include <TChain.h>
#include <TF1.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TString.h>

#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

TF1* BuildSinglePeakFunc(TString name, double xmin, double xmax)
{
    TF1* f = new TF1(
        name,
        "gaus(0)+[3]",
        xmin, xmax
    );
    return f;
}

bool FitPeakPosition(
    TChain& chain,
    int det,
    TString branchBase,
    double fitMin,
    double fitMax,
    int bins,
    double& peakPos
) {
    TString hname = Form("h_det%02d_%.0f_%.0f", det, fitMin, fitMax);
    TH1D* h = new TH1D(
        hname,
        Form("Detector %d;ADC channel;Counts", det),
        bins, fitMin, fitMax
    );

    TString drawExpr = Form("%s[%d]>>%s",
                            branchBase.Data(),
                            det,
                            hname.Data());
    chain.Draw(drawExpr, "nhit==1", "goff");

    if (h->GetEntries() < 100 || h->Integral() <= 0) {
        delete h;
        return false;
    }

    double maxX = h->GetBinCenter(h->GetMaximumBin());
    TF1* f = BuildSinglePeakFunc(Form("f_det%02d_%.0f_%.0f", det, fitMin, fitMax),
                                 fitMin,
                                 fitMax);

    f->SetParameter(0, h->GetMaximum());
    f->SetParameter(1, maxX);
    f->SetParameter(2, (fitMax - fitMin) / 12.0);
    f->SetParameter(3, h->GetMinimum());
    f->SetParLimits(1, fitMin, fitMax);
    f->SetParLimits(2, 0.5, fitMax - fitMin);

    int status = h->Fit(f, "RQN");
    if (status != 0) {
        delete f;
        delete h;
        return false;
    }

    peakPos = f->GetParameter(1);
    delete f;
    delete h;
    return std::isfinite(peakPos) && peakPos > 0;
}

void CalibrateDetectors(
    TString dataDir = "../../data/cal",
    TString filePattern = "run*_cal.root",
    TString branchBase = "raw",
    std::vector<double> knownEnergies = {511.0, 662.0, 1173.0, 1332.0},
    std::vector<double> fitMins = {450.0, 600.0, 1100.0, 1260.0},
    std::vector<double> fitMaxs = {560.0, 720.0, 1240.0, 1400.0},
    int bins = 2000,
    int polyOrder = 2,
    TString outputFile = "../convert/cali_factor.txt"
) {
    if (knownEnergies.size() != fitMins.size() ||
        knownEnergies.size() != fitMaxs.size()) {
        std::cerr << "knownEnergies, fitMins, and fitMaxs must have the same size\n";
        return;
    }

    if (knownEnergies.size() < 2) {
        std::cerr << "At least two calibration peaks are required\n";
        return;
    }

    TChain chain("ana");
    TString inputPattern = Form("%s/%s", dataDir.Data(), filePattern.Data());
    chain.Add(inputPattern);

    if (chain.GetEntries() <= 0) {
        std::cerr << "No entries found from " << inputPattern << "\n";
        return;
    }

    const int NDET = 48;
    std::array<std::array<double, 3>, NDET> coeff;
    std::array<int, NDET> nFitPeaks;
    for (int det = 0; det < NDET; det++) {
        coeff[det] = {{0.0, 1.0, 0.0}};
        nFitPeaks[det] = 0;
    }

    std::cout << "===== Per-detector calibration =====\n";
    std::cout << "Input: " << inputPattern << "\n";
    std::cout << "Output: " << outputFile << "\n\n";

    for (int det = 0; det < NDET; det++) {
        std::vector<double> rawPeaks;
        std::vector<double> energies;

        for (size_t peak = 0; peak < knownEnergies.size(); peak++) {
            double rawPeak = 0;
            if (!FitPeakPosition(chain,
                                 det,
                                 branchBase,
                                 fitMins[peak],
                                 fitMaxs[peak],
                                 bins,
                                 rawPeak)) {
                std::cerr << "Detector " << det
                          << " failed to fit peak near "
                          << knownEnergies[peak] << "\n";
                continue;
            }

            rawPeaks.push_back(rawPeak);
            energies.push_back(knownEnergies[peak]);

            std::cout << "Detector " << det
                      << " peak " << knownEnergies[peak]
                      << " raw = " << rawPeak << "\n";
        }

        nFitPeaks[det] = rawPeaks.size();
        if (rawPeaks.size() < 2) {
            std::cerr << "Detector " << det
                      << " has fewer than two fitted peaks; using identity calibration\n";
            continue;
        }

        TGraph graph(static_cast<int>(rawPeaks.size()), rawPeaks.data(), energies.data());
        TString fitName = Form("cal_det%02d", det);
        TString formula = (polyOrder >= 2 && rawPeaks.size() >= 3) ? "pol2" : "pol1";
        TF1* calFit = new TF1(fitName, formula, 0, 65535);
        graph.Fit(calFit, "Q");

        coeff[det][0] = calFit->GetParameter(0);
        coeff[det][1] = calFit->GetParameter(1);
        coeff[det][2] = (formula == "pol2") ? calFit->GetParameter(2) : 0.0;

        std::cout << "Detector " << det
                  << " E = " << coeff[det][0]
                  << " + " << coeff[det][1] << " * raw"
                  << " + " << coeff[det][2] << " * raw^2\n\n";

        delete calFit;
    }

    std::ofstream fout(outputFile.Data());
    if (!fout.is_open()) {
        std::cerr << "Cannot open output file: " << outputFile << "\n";
        return;
    }

    fout << "# det module channel p0 p1 p2 nfit\n";
    fout << "# E_cal = p0 + p1 * raw + p2 * raw^2\n";
    fout << std::setprecision(12);
    for (int det = 0; det < NDET; det++) {
        fout << det << " "
             << det / 16 << " "
             << det % 16 << " "
             << coeff[det][0] << " "
             << coeff[det][1] << " "
             << coeff[det][2] << " "
             << nFitPeaks[det] << "\n";
    }

    fout.close();
    std::cout << "\n>>> Per-detector calibration factors written to "
              << outputFile << "\n";
}
