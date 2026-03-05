void histdeductsim()
{
	// 定义要处理的文件列表
	TString filenames[] = {
		"12Cag_740-0keVsim.root",
		"B11ag_606keVR_sim.root",
		"108h_JUNAbkg.root",
		"Cnat.root"};

	int nfiles = sizeof(filenames) / sizeof(filenames[0]);

	// 总谱参数
	int nbins2, binw = 300;
	int Emin2 = 7000;
	int Emax2 = 14000;

	nbins2 = (Emax2 - Emin2) / binw;

	// 创建总谱直方图数组
	TH1D *hist_sim_sum[nfiles];

	for (int idx = 0; idx < nfiles; idx++)
	{
		TString sumspec_name = "hist_sim_sum_" + TString(Form("%d", idx));
		hist_sim_sum[idx] = new TH1D(sumspec_name, ";E[keV];Counts", nbins2, Emin2, Emax2);
	}

	// 循环处理每个文件
	for (int idx = 0; idx < nfiles; idx++)
	{
		TFile *ipf = new TFile(filenames[idx]); // 输入文件
		if (ipf->IsZombie())
		{
			cout << "Error opening file: " << filenames[idx] << endl;
			continue;
		}

		TTree *tree = (TTree *)ipf->Get("ana"); // 获取树

		if (!tree)
		{
			cout << "Tree 'ana' not found in file: " << filenames[idx] << endl;
			ipf->Close();
			continue;
		}

		int nentries = tree->GetEntries();
		cout << "Processing file: " << filenames[idx] << ", Entries: " << nentries << endl;

		Double_t sum;

		// 设置分支地址
		tree->SetBranchAddress("sum", &sum);

		Double_t exp_cal = 1.0; // 根据需要调整校准因子

		// 添加进度条
		int updateInterval = max(1, nentries / 100); // 每1%更新一次

		for (long k = 0; k < nentries; k++)
		{
			tree->GetEntry(k);

			sum = sum * exp_cal;

			// 填充总谱
			if (sum < Emax2 && sum > Emin2)
			{
				hist_sim_sum[idx]->Fill(sum);
			}

			// 显示进度条
			if ((k + 1) % updateInterval == 0)
			{
				int percent = (int)((double)(k + 1) / nentries * 100);
				int barWidth = 50;
				cout << "\r[" << idx + 1 << "/" << nfiles << "] File: " << filenames[idx] << " [";

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

		cout << endl; // 完成当前文件后换行

		ipf->Close();
	}

	// 输出结果到ROOT文件
	TFile *file = TFile::Open("sim.root", "RECREATE");
	file->cd();

	for (int idx = 0; idx < nfiles; idx++)
	{
		double count = hist_sim_sum[idx]->Integral();
		std::cout << "idx" << idx << "=" << count << std::endl;
		hist_sim_sum[idx]->Write();
	}

	TCanvas *c1 = new TCanvas("c1", "", 1600, 2000);
	c1->Divide(2, 2);
	for (int i = 1; i < nfiles + 1; i++)
	{
		c1->cd(i);
		hist_sim_sum[i - 1]->Draw();
	}

	file->Close();
}