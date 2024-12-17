#include "Bkgtrack.h"
#include "TMath.h"

Bkgtrack::Bkgtrack(TString tracksfile) : fTrkid(0),fParentid(0),fPDG(11),
                                         fStepx(nullptr),fStepy(nullptr),fStepz(nullptr),
                                         fStepkE(nullptr),fStepde(nullptr),fStepproc(nullptr),
                                         hKineticE0(nullptr),hKineticE1(nullptr),hKineticE2(nullptr),hKineticE3(nullptr)
{
    f_Tf = TFile::Open(tracksfile);
    if (!f_Tf)
    {
        std::printf("[ERROR] FILE DOES NOT EXIST\n");
        throw std::runtime_error("[ERROR] FILE DOES NOT EXIST");
    }
    f_Tr = dynamic_cast<TTree*>(f_Tf->Get("teeg"));
    InitialPars();
    std::printf("==== There are %lld entries(tracks) in total(10 BX @CEPC Higgs)\n", f_Tr->GetEntries());
}


Bkgtrack::~Bkgtrack()
{
    delete fStepx, fStepy, fStepz, fStepkE, fStepde;
    delete fStepproc;
    delete f_Tr;
    delete f_Tf;
}


void Bkgtrack::InitialPars()
{
    f_Tr->SetBranchAddress("pdgcode", &fPDG);
    f_Tr->SetBranchAddress("parentid", &fParentid);
    f_Tr->SetBranchAddress("trkid", &fTrkid);
    f_Tr->SetBranchAddress("vStepx", &fStepx);
    f_Tr->SetBranchAddress("vStepy", &fStepy);
    f_Tr->SetBranchAddress("vStepz", &fStepz);
    f_Tr->SetBranchAddress("vStepkE", &fStepkE);
    f_Tr->SetBranchAddress("vStepde", &fStepde);
    f_Tr->SetBranchAddress("vStepproc", &fStepproc);
}

std::vector<size_t> Bkgtrack::CrossTPCIdx()
{
    std::vector<size_t> vCrossTPCidx;
   
    if (fStepx->size() < 1)
        return vCrossTPCidx;

    bool isCrossTPC = false;
    for (size_t mm = 0; mm < fStepx->size(); ++mm)
    {
        auto radius = std::sqrt(std::pow(fStepx->at(mm), 2) + std::pow(fStepy->at(mm), 2));
        auto posZ = fStepz->at(mm);
        if (radius >= _TPCR0 && radius <= _TPCR1 && posZ > _TPCZ0 && posZ < _TPCZ1)
        {
            vCrossTPCidx.push_back(mm);
        }
    }
    return vCrossTPCidx;
}


void Bkgtrack::PrintTrackInfo(int numofentries)
{
    if (f_Tr->GetEntries() < 1)
        return;

    for (long long ii = 0; ii < numofentries; ++ii)
    {
        f_Tr->GetEntry(ii);
        std::printf("[INFO] : PDG:%d, Trkid:%d, Parentid:%d\n", fPDG, fTrkid, fParentid);
        for (size_t step_i = 0; step_i < fStepx->size(); ++step_i)
        {
            std::printf("\t %.2f  %.2f  %.2f  %.2f  %.2f  %s\n", fStepx->at(step_i), fStepy->at(step_i), fStepz->at(step_i),
                fStepkE->at(step_i), fStepde->at(step_i), fStepproc->at(step_i).data());
        }
    }
}

TCanvas* Bkgtrack::PlotPositionXYZDistribution(int pdg, int plane, bool Isstart)
{
    if (f_Tr->GetEntries() < 1)
        return nullptr;

    TH2D* h2dxyz = nullptr;
    std::string suffix = Isstart ? "initial position of " : "end position of ";
    std::map<int, std::string> particlename{ {11,"e-;"},{-11,"e+;"},{22,"#gamma;"}};
    std::map<int, std::string> histAxis{ {0,"Z [cm];X [cm]"},{1,"Z [cm];Y [cm]"},{2,"Y [cm];X [cm]"} };
    
    auto hist_titile = suffix + particlename[pdg] + histAxis[plane];

    if (plane == 2)
    {
        h2dxyz = new TH2D(Form("h2dxy_%d_%d", pdg, plane), hist_titile.c_str(), 720/2, -180, 180, 720/2, -180, 180);
    }
    else
    {
        h2dxyz = new TH2D(Form("h2dxy_z_%d_%d", pdg, plane), hist_titile.c_str(),1200/2,-300,300,720/2, -180, 180);
    }

    for (long long ii = 0; ii < f_Tr->GetEntries(); ++ii)
    {
        f_Tr->GetEntry(ii);
        if (fPDG == pdg)
        {
            auto stepsize = fStepx->size();
            stepsize = Isstart ? 0 : stepsize - 1;

            if (plane == 0)
            {
                h2dxyz->Fill(fStepz->at(stepsize) / 10., fStepx->at(stepsize) / 10.);
            }
            else if (plane == 1)
            {
                h2dxyz->Fill(fStepz->at(stepsize) / 10., fStepy->at(stepsize) / 10.);
            }
            else if (plane == 2)
            {
                h2dxyz->Fill(fStepx->at(stepsize) / 10., fStepy->at(stepsize) / 10.);
            }
        }
    }
    h2dxyz->SetStats(kFALSE);

    auto myc = new TCanvas("Cpositionxyz", "Cpositionxyz", 800, 600);
    myc->SetGrid();
    myc->SetLogz();
    h2dxyz->Draw("COL");
    return myc;
}

void Bkgtrack::FillMapstracks()
{

    //myc->SetGrid();
    hKineticE0 = new TH1D("hKE0", "Initial kE of #gamma (interact with gas);kE [MeV];", 400, 0, 20.); // 
    hKineticE1 = new TH1D("hKE1", "The first step kE in TPC;kE [MeV];", 400, 0, 20.);
    hKineticE2 = new TH1D("hKE2", "Initial kE of #gamma, just cross TPC;kE [MeV];", 400, 0, 20.);
    hKineticE3 = new TH1D("hKE3", "Initial kE of #gamma, start in TPC;kE [MeV];", 400, 0, 20.);

    long Count1(0), Count2(0), Count3(0);
    //std::map<int,int>;
    

    for (long long ii = 0; ii < f_Tr->GetEntries(); ++ii)
    {
        f_Tr->GetEntry(ii);
        // Step I, check particle name
        if (fPDG == 22)
        {
            Count1++;
            // Step II, check position
            auto vCrosstpc = CrossTPCIdx();
            if (vCrosstpc.size() != 0)
            {
                Count2++;
                //Step III, check proc... of gamma in tpc
                int cnt_compt(0), cnt_phot(0), cnt_conv(0),cnt_init(0),cnt_transportation(0);
                for (size_t nn = 0; nn < vCrosstpc.size(); ++nn)
                {
                    std::string ProcinTPC = fStepproc->at(vCrosstpc.at(nn));
                    auto iter = std::find(_ProcName.begin(), _ProcName.end(), ProcinTPC);
                    if (iter != _ProcName.end())
                    {
                        auto proc_idx = std::distance(_ProcName.begin(), iter);
                        if (proc_idx == 2)
                        {
                            cnt_compt++;
                        }
                        else if (proc_idx == 3)
                        {
                            cnt_phot++;
                        }
                        else if (proc_idx == 4)
                        {
                            cnt_conv++;
                        }
                        else if (proc_idx == 0)
                            cnt_init++;
                        else
                            cnt_transportation++;
                    }
                    else
                    {
                        std::printf("[Warning] unknown proc....   %s and initial Kinetic Energy%.2f [MeV]\n", ProcinTPC.data(),fStepkE->at(0));
                    }
                }
                
                // Fill hists
                // case I, gamma crossed TPC gas region and interact with gas through compt phot or conv
                if (cnt_compt != 0 || cnt_conv != 0 || cnt_phot != 0)
                {
                    hKineticE0->Fill(fStepkE->at(0));
                    hKineticE1->Fill(fStepkE->at(vCrosstpc.at(0)));
                    Count3++;
                    tracks_eeg tmpeeg;
                    tmpeeg.pdg = fPDG;
                    tmpeeg.parentid = fParentid;
                    tmpeeg.vxp = (*fStepx);
                    tmpeeg.vyp = (*fStepy);
                    tmpeeg.vzp = (*fStepz);
                    tmpeeg.vde = (*fStepde);
                    fMaptracks[fTrkid].push_back(tmpeeg);
                    //vTmptrkid.push_back(fTrkid);
                }
                
                if (cnt_init != 0)
                {
                    hKineticE3->Fill(fStepkE->at(0));
                }

                if ((cnt_init==0 && cnt_transportation==vCrosstpc.size()) || (cnt_init!=0 && cnt_transportation==vCrosstpc.size()-1))
                    hKineticE2->Fill(fStepkE->at(0));
            }
        }
    }

    std::printf("[INFO]: There are %ld gamma tracks, %ld crossed TPC region, %ld loss energy in TPC region\n", Count1, Count2, Count3);
    //std::printf("[INFO]: Map size = %zu\n", fMaptracks.size());

    //fill secondary e-/e+ tracks into fMaptracks
    for (long long ii = 0; ii < f_Tr->GetEntries(); ++ii)
    {
        f_Tr->GetEntry(ii);
        auto iter = fMaptracks.find(fParentid);
        if (iter != fMaptracks.end())
        {
            //fMaptracks[fParentid].
            tracks_eeg tmpeeg;
            tmpeeg.pdg = fPDG;
            tmpeeg.parentid = fParentid;
            tmpeeg.vxp = (*fStepx);
            tmpeeg.vyp = (*fStepy);
            tmpeeg.vzp = (*fStepz);
            tmpeeg.vde = (*fStepde);
            iter->second.push_back(tmpeeg);
            //std::cout << " AAAAAAAAAAAAAAAA FIND" << std::endl;
        }
    }
}

TCanvas* Bkgtrack::PlotGammaKEDistribution()
{
    auto myc = new TCanvas("mycgamma", "mycgamma", 1000, 1000);
    myc->Divide(2, 2);

    myc->cd(1);
    gPad->SetGrid();
    if(hKineticE2)
        hKineticE2->Draw();

    myc->cd(2);
    gPad->SetGrid();
    if(hKineticE0)
        hKineticE0->Draw();

    myc->cd(3);
    gPad->SetGrid();
    if(hKineticE1)
        hKineticE1->Draw();

    myc->cd(4);
    gPad->SetGrid();
    if(hKineticE3)
        hKineticE3->Draw();


    return myc;
}