void convert16()
{
    Double_t Ecut = 10;
    Double_t raw[16];
    Double_t E_raw[16];
    Double_t single16[16];
    Double_t times[16];
    Double_t ts[16];
    Double_t sum = 0;
    Double_t sumf = 0; // 前8个探测器的能量和
    Double_t sumb = 0; // 后8个探测器的能量和
    Int_t ret;
    int nhit = 0;
    Double_t tscoin[4];
    Double_t fbcoin = 0;

    TString str_tmp;
    str_tmp = str_tmp.Format("out.root");
    TFile *f_input = new TFile(str_tmp.Data());
    if (f_input->IsZombie())
    {
        cout << "open file " << str_tmp.Data() << " error!" << endl;
        delete f_input;
        return;
    }

    cout << "*** start process file " << str_tmp.Data() << " ********" << endl;
    TTree *tr_data = (TTree *)f_input->Get("tr");
    if (tr_data == NULL)
    {
        cout << " Get the tree error in file" << str_tmp.Data() << endl;
        f_input->Close();
        return;
    }

    tr_data->SetBranchAddress("de", raw);
    tr_data->SetBranchAddress("tTs", times);

    str_tmp = str_tmp.Format("eff.root");
    TFile *f_output = new TFile(str_tmp.Data(), "RECREATE", "ana");

    TTree *ntree = new TTree("ana", "ana");
    ntree->Branch("E_raw", E_raw, "E_raw[16]/D");
    ntree->Branch("single16", single16, "single16[16]/D");
    ntree->Branch("ts", ts, "ts[16]/D");
    ntree->Branch("sum", &sum, "sum/D");
    ntree->Branch("sumf", &sumf, "sumf/D");
    ntree->Branch("sumb", &sumb, "sumb/D");
    ntree->Branch("nhit", &nhit, "nhit/I");
    ntree->Branch("tscoin", tscoin, "tscoin[4]/D");
    ntree->Branch("fbcoin", &fbcoin, "fbcoin/D"); // 修正变量名

    int nentries = tr_data->GetEntries();
    for (int i = 0; i < nentries; i++)
    {
        if (i % 1000 == 0)
        {
            printf("%9d/%9d %3.0f%%\r", i, nentries, 100.0 * (double)i / (double)nentries);
            fflush(stdout);
        }

        ret = tr_data->GetEntry(i);
        if (ret < 1)
        {
            cout << "read entry " << i << " error!!" << endl;
            continue;
        }

        // 初始化
        sum = 0;
        sumf = 0;
        sumb = 0;
        nhit = 0;
        fbcoin = 0;

        // 初始化数组
        for (int j = 0; j < 16; j++)
        {
            ts[j] = 0;
            E_raw[j] = raw[j];
            single16[j] = 0;
        }
        for (int j = 0; j < 4; j++)
        {
            tscoin[j] = 0;
        }

        // 处理数据
        for (int j = 0; j < 16; j++)
        {
            if (raw[j] > Ecut)
            {
                single16[j] = raw[j];
                ts[j] = times[j];
                sum += raw[j];

                // 分别计算前8个和后8个探测器的能量和
                if (j < 8)
                {
                    sumf += raw[j];
                }
                else
                {
                    sumb += raw[j];
                }
                nhit++;
            }
        }

        // 在循环结束后判断整个事件是否满足条件
        if (abs(sumf - 511) < 75 && abs(sumb - 511) < 75)
        {
            // 保存第一个满足能量阈值的探测器时间
            for (int j = 0; j < 16; j++)
            {
                if (raw[j] > Ecut)
                {
                    fbcoin = ts[j];
                    break; // 只取第一个
                }
            }
        }

        // for (int j = 0, k = 0; j < 8; j += 2, k++)
        // {
        //     int i = j + 12 > 15 ? j - 8 : j;
        //     if (abs(single16[j] - 511) < 75 && abs(single16[i + 12] - 511) < 75)
        //     {
        //         tscoin[k] = ts[j];
        //     }
        // }

        // 设置tscoin数组
        int pairs[4][2] = {{0, 12}, {2, 14}, {4, 8}, {6, 10}};

        for (int k = 0; k < 4; k++)
        {
            int idx1 = pairs[k][0];
            int idx2 = pairs[k][1];
            if (TMath::Abs(single16[idx1] - 511) < 75 && TMath::Abs(single16[idx2] - 511) < 75)
            {
                tscoin[k] = ts[idx1];
            }
        }

        ntree->Fill();
    }

    f_output->Write();
    f_input->Close();
    f_output->Close();
    cout << "**** finish *******" << endl;
}