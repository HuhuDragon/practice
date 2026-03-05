#define COMPILER (!defined(__CINT__) && !defined(__CLING__))

#if defined(__MAKECINT__) || defined(__ROOTCLING__) || COMPILER

#include <BAT/BCAux.h>
#include <BAT/BCGaussianPrior.h>
#include <BAT/BCLog.h>
#include <BAT/BCMTF.h>
#include <BAT/BCMTFChannel.h>
#include <BAT/BCParameter.h>

#include <TFile.h>
#include <TH1D.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#endif

void fit()
{
    // ---- set style and open log files ---- //

    // open log file
    BCLog::OpenLog("log.txt", BCLog::detail, BCLog::detail);

    // set nicer style for drawing than the ROOT default
    // BCAux::SetStyle();

    // ---- read histograms from a file ---- //

    // open file
    std::string fname = "sim.root";
    TFile *file = TFile::Open(fname.c_str(), "READ");

    // check if file is open
    if (!file || !file->IsOpen())
    {
        BCLog::OutError(Form("Could not open file %s.", fname.c_str()));
        BCLog::OutError("Run macro CreateHistograms.C in Root to create the file.");
        return;
    }

    // read data histograms
    const int num_histograms = 4;
    TH1D *hist_sim[num_histograms];

    for (int i = 0; i < num_histograms; ++i)
    {
        hist_sim[i] = (TH1D *)file->Get(Form("hist_sim_sum_%d", i));
    }

    TF1 *faddseg = new TF1("1", "1", 0, 50000);
    float eff_sim[num_histograms];
    for (int i = 0; i < num_histograms; i++)
    {
        hist_sim[i]->Add(faddseg, 0.000001);
        eff_sim[i] = hist_sim[i]->Integral() / 1000000.;
        std::cout << "eff_sim[" << i << "] = " << eff_sim[i] << std::endl;
        hist_sim[i]->Scale(1. / hist_sim[i]->Integral());
    }

    std::string fname1 = "exp.root";
    TFile *file1 = TFile::Open(fname1.c_str(), "READ");

    TH1D *hist_exp_sum = (TH1D *)file1->Get("hist_exp_sum");

    // ---- perform fitting ---- //

    // create new fitter object
    BCMTF *m = new BCMTF("");

    // add channels
    m->AddChannel("channel1");

    // add processes
    m->AddProcess("branch1", 1, 1e3);                   //
    m->AddProcess("branch2", 1, 1e3);                   //
    m->AddProcess("branch3", 1e6 * 0.977, 1e6 * 0.977); //
    m->AddProcess("branch4", 1e4, 5e4);                 //

    m->SetData("channel1", *hist_exp_sum);

    int nbranch = 4;

    for (int i = 0; i < nbranch; i++)
    {
        m->SetTemplate("channel1", Form("branch%d", i + 1), *hist_sim[i], eff_sim[i]);
    }

    m->GetParameter("branch1").SetPriorConstant();
    m->GetParameter("branch2").SetPriorConstant();
    m->GetParameter("branch3").SetPriorConstant();
    m->GetParameter("branch4").SetPriorConstant();

    // m->GetParameter("systematic1").SetPrior(new BCGaussianPrior(0., 1.));

    // set precision
    // m->SetPrecision(BCEngineMCMC::kQuick);
    m->SetPrecision(BCEngineMCMC::kMedium);
    // m->SetPrecision(BCEngineMCMC::kHigh);

    // marginalize
    m->MarginalizeAll(BCIntegrate::kMargMetropolis);

    cout << "debug" << endl;

    // find global mode
    m->FindMode(m->GetBestFitParameters());

    // print all marginalized distributions
    m->PrintAllMarginalized("marginalized");

    // print results of the analysis into a text file
    m->PrintSummary();

    // print templates and stacks
    // print templates and stacks
    for (int i = 0; i < m->GetNChannels(); ++i)
    {
        BCMTFChannel *channel = m->GetChannel(i);
        m->PrintStack(i, m->GetBestFitParameters(), channel->GetSafeName() + "_stack.C");

        channel->PrintTemplates(channel->GetSafeName() + "_templates.pdf");
        m->PrintStack(i, m->GetBestFitParameters(), channel->GetSafeName() + "_stack.pdf");
    }

    // ---- clean up ---- //

    // free memory
    delete m;
}
