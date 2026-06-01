void drawsame()
{

  TFile *f1 = new TFile("run00715_cal.root"); // 红色填充谱
  TFile *f2 = new TFile("eff.root");          // 黑色谱

  TTree *t1 = (TTree *)f1->Get("ana");
  TTree *t2 = (TTree *)f2->Get("ana");

  if (t1 == nullptr || t2 == nullptr)
    return;
  /// double binnumber=600.;
  // t1->Draw("singleInner>>h1(600,100,6100)", "");
  // t2->Draw("singleInner*1.002>>h2(600,100,6100)", "");

  t1->Draw("sumInner>>h1(500,100,10100)", "");
  t2->Draw("sumInner*1.002>>h2(500,100,10100)", "");

  TF1 *f = new TF1("1", "1", 100, 20100);

  TH1D *h1 = (TH1D *)gROOT->FindObject("h1");
  TH1D *h2 = (TH1D *)gROOT->FindObject("h2");

  // h1->Multiply(f, 1);
  // h2->Multiply(f, 1);

  h1->SetLineWidth(3);
  h2->SetLineWidth(3);

  h1->SetLineColor(2);
  h1->SetFillStyle(3004);
  h1->SetFillColorAlpha(kRed, 0.1); // h1br0，用红色填充；
  h2->SetLineColor(1);

  // h1->Draw();
  h2->Draw();
  h1->Draw("same");
  // h4->Draw("same");
  h2->SetStats(0);
  // h1->SetBorderSize(0);
  TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
  // legend->AddEntry(hist3, "Simulated Spectrum", "l");
  legend->AddEntry(h1, "exp", "f");
  legend->AddEntry(h2, "sim", "l");
  legend->SetBorderSize(0);
  legend->Draw();
}
