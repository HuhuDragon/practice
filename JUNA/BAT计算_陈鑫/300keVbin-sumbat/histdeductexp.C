void histdeductexp()
{

	int nbins2, binw = 300;
	int Emin2 = 7000;
	int Emax2 = 14000;

	nbins2 = (Emax2 - Emin2) / binw;
	TH1D *hist_exp_sum = new TH1D("hist_exp_sum", ";E[keV];Counts", nbins2, Emin2, Emax2);

	TFile *ipf = new TFile("740kev.root"); // input file
	if (ipf->IsZombie())
	{
		throw "error  to read file";
	}

	TTree *tree = (TTree *)ipf->Get("ana"); // get the tree

	int nentries;
	nentries = tree->GetEntries();
	cout << "Entries: " << nentries << endl;

	Double_t sum;

	tree->SetBranchAddress("sum", &sum);

	Double_t exp_cal = 1.000;

	// 添加进度条
	int updateInterval = max(1, nentries / 100); // 每1%更新一次

	for (long k = 0; k < nentries; k++)
	{
		tree->GetEntry(k);
		sum = sum * exp_cal;
		if (sum < Emax2 && sum > Emin2)
		{
			hist_exp_sum->Fill(sum);
		}

		// 显示进度条
		if ((k + 1) % updateInterval == 0)
		{
			int percent = (int)((double)(k + 1) / nentries * 100);
			int barWidth = 50;
			cout << "\rProgress: [";

			int pos = barWidth * percent / 100;
			for (int i = 0; i < barWidth; ++i)
			{
				if (i < pos)
					cout << "=";
				else if (i == pos)
					cout << ">";
				else
					cout << " ";
			}
			cout << "] " << percent << "%";
			cout.flush();
		}
	}

	cout << endl; // 完成后换行

	TFile *file = TFile::Open("exp.root", "RECREATE");
	file->cd();
	hist_exp_sum->Write();

	file->Close();
}