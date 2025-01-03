#include "Bkgtrack.h"
#include "TMath.h"
#include "TAxis.h"
#include "TVector3.h"

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
    delete fStepx;
    delete fStepy; 
    delete fStepz; 
    delete fStepkE; 
    delete fStepde;
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

bool Bkgtrack::IsinTPCgasRegion(float xx, float yy, float zz)
{
    auto radius = std::sqrt(std::pow(xx, 2) + std::pow(yy, 2));
    if (radius >= _TPCR0 && radius <= _TPCR1 && zz > _TPCZ0 && zz < _TPCZ1)
    {
        return true;
    }
    else
        return false;
}

std::vector<size_t> Bkgtrack::CrossTPCIdx()
{
    std::vector<size_t> vCrossTPCidx;
   
    if (fStepx->size() < 1)
        return vCrossTPCidx;

    bool isCrossTPC = false;
    for (size_t mm = 0; mm < fStepx->size(); ++mm)
    {
        if (IsinTPCgasRegion(fStepx->at(mm), fStepy->at(mm), fStepz->at(mm)))
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

TCanvas* Bkgtrack::PlotParticleType()
{
    std::map<int, int> particleInTPC{ {22,0},  {11,0},  {-11,0},
                                      {13,0},  {-13,0}, {211,0},
                                      {-211,0},{321,0}, {-321,0},
                                      {2212,0}, {2112,0}};
    auto hParticleType = new TH1D("hParticleType", ";;Cnts", 11, 0, 11);
    //int count_unknown = 0;
    for (long long ii = 0; ii < f_Tr->GetEntries(); ++ii)
    {
        f_Tr->GetEntry(ii);
        if (ii % 200000 == 0)
            std::printf("[INFO]: %lld entries read!\n", ii);
        
        // Step I, checking is current track cross TPC
        auto vCrosstpc = this->CrossTPCIdx();
        if (vCrosstpc.size() > 0)
        {
            particleInTPC[fPDG]++;
            //if (iter != particleInTPC.end())
            //{
            //    iter->second++;
            //}
            //else
            //    count_unknown++;
        }

    }

    const char* particlename[11] = { "#gamma","e^{-}","e^{+}","#mu^{-}","#mu^{+}",
                                     "#pi^{+}","#pi^{-}","K^{+}","K^{-}","#it{p}","#it{n}"};

    hParticleType->Fill(particlename[0], particleInTPC[22]);
    hParticleType->Fill(particlename[1], particleInTPC[11]);
    hParticleType->Fill(particlename[2], particleInTPC[-11]);
    hParticleType->Fill(particlename[9], particleInTPC[2212]);
    hParticleType->Fill(particlename[10], particleInTPC[2112]);

    //std::printf("[WARNING]: %d tracks unknown!\n", count_unknown);
    auto myc = new TCanvas("CparticleType", "CparticleType", 800, 600);
    myc->SetGrid();
    myc->SetLogy();
    hParticleType->SetStats(0);
    hParticleType->GetXaxis()->SetLabelSize(0.08);
    hParticleType->Draw("HIST");

    return myc;
}

TCanvas* Bkgtrack::PlotGammaPositionDistribution(bool IsAll)
{
    auto hEvsZ = new TH2D("hPosEvsPz", ";Z [cm];kE of #gamma [MeV];", 1000, -500, 500, 200, 0, 10);
    hEvsZ->SetStats(0);
    auto hPosz = new TH1D("hPosz", ";Z [cm];", 1000, -500, 500);
    //hPosz->SetStats(0);
    if (!IsAll)
    {
        if (fMaptracks.size() == 0)
        {
            std::printf("[WARNING]: fMaptracks size==0, FilleegtrackMap first\n");
            return nullptr;
        }
        int tmpcnt = 0;
        for (const auto itemmap : fMaptracks)
        {
            auto iter = std::next(itemmap.second.begin(), 1);
            tmpcnt++;
            if (iter != itemmap.second.end())
            {
                hPosz->Fill(iter->vzp.at(0) / 10.);
                //hPosxy->Fill(iter->vxp.at(0) / 10., iter->vyp.at(0) / 10.);
                hEvsZ->Fill(iter->vzp.at(0) / 10., iter->e0);
                if (tmpcnt < 40)
                    std::printf("[DEBUG]:  pdg:%d, trkid:%d, parentid:%d\n", iter->pdg, iter->trkid, iter->parentid);
            }
        }
    }
    else
    {
        for (long long ii = 0; ii < f_Tr->GetEntries(); ++ii)
        {
            if (ii % 200000 == 0)
                std::printf("[INFO]: %lld entries read!\n", ii);
            f_Tr->GetEntry(ii);
            
            auto vcrosstpc = CrossTPCIdx();
            if (vcrosstpc.size() > 0 && fPDG==22)
            {
                hPosz->Fill(fStepz->at(0) / 10.);
                hEvsZ->Fill(fStepz->at(0) / 10., fStepkE->at(0));
            }
        }
    }

    auto myc = new TCanvas("Cgammaposition", "Cgammaposition", 1200, 600);
    myc->Divide(2, 1);
    myc->cd(1);
    gPad->SetLogz();
    gPad->SetGrid();
    hEvsZ->Draw("COL");
    myc->cd(2);
    gPad->SetGrid();
    hPosz->Draw();

    return myc;

}

TCanvas* Bkgtrack::PlotGammaDirectionDistribution(bool IsAll)
{
    auto hCosvsZ = new TH2D("hCosEvsPz", ";Z [cm];cos#theta;", 1000, -500, 500, 40, -1, 1);
    hCosvsZ->SetStats(0);
    auto hPhivsZ = new TH2D("hPhivsZ", ";Z [cm]; #Phi [Degree]", 1000, -500, 500, 360, -180, 180);
    //auto hPhi = new TH1D("hPhi", ";#Phi [Degree];Cnts", 360, -180, 180);
    //hPhi->SetStats(0);

    if (!IsAll)
    {
        if (fMaptracks.size() == 0)
        {
            std::printf("[WARNING]: fMaptracks size==0, FilleegtrackMap first\n");
            return nullptr;
        }

        int tmpcnt = 0;
        for (const auto itemmap : fMaptracks)
        {
            auto iter = std::next(itemmap.second.begin(), 1);
            tmpcnt++;
            if (iter != itemmap.second.end())
            {
                if (iter->vxp.size() >= 2)
                {
                    auto xp0 = iter->vxp.at(0);
                    auto yp0 = iter->vyp.at(0);
                    auto zp0 = iter->vzp.at(0);
                    TVector3 p0(xp0, yp0, zp0);
                    auto xp1 = iter->vxp.at(1);
                    auto yp1 = iter->vyp.at(1);
                    auto zp1 = iter->vzp.at(1);
                    TVector3 p1(xp1, yp1, zp1);

                    auto vdirect = p1 - p0;
                    auto costheta = vdirect.z() / vdirect.Mag();

                    auto phi = TMath::ATan2(vdirect.y(), vdirect.x())*TMath::RadToDeg();

                    hCosvsZ->Fill(zp0 / 10., costheta);
                    hPhivsZ->Fill(zp0 / 10, phi);
                    //hPhi->Fill(phi);
                    if (tmpcnt < 20)
                        std::printf("[DEBUG]:  pdg:%d, trkid:%d, parentid:%d costheta=%.2f\n", iter->pdg, iter->trkid, iter->parentid, costheta);
                }
                //hEvsZ->Fill(iter->vzp.at(0) / 10., iter->e0);

            }
        }
    }
    else
    {
        for (long long ii = 0; ii < f_Tr->GetEntries(); ++ii)
        {
            if (ii % 200000 == 0)
                std::printf("[INFO]: %lld entries read!\n", ii);
            f_Tr->GetEntry(ii);

            auto vcrosstpc = CrossTPCIdx();
            if (vcrosstpc.size() >=2 && fPDG == 22)
            {
                TVector3 p0(fStepx->at(0), fStepy->at(0), fStepz->at(0));
                TVector3 p1(fStepx->at(1), fStepy->at(1), fStepz->at(1));

                auto vdirect = p1 - p0;
                auto costheta = vdirect.z() / vdirect.Mag();

                hCosvsZ->Fill(fStepz->at(0) / 10., costheta);
                auto phi = TMath::ATan2(vdirect.y(), vdirect.x()) * TMath::RadToDeg();
                hPhivsZ->Fill(fStepz->at(0) / 10., phi);
                //hPhi->Fill(phi);
            }
        }
    }


    auto myc = new TCanvas("Ccosvsz", "Ccosvsz", 1200, 400);
    myc->Divide(2, 1);
    myc->cd(1);
    gPad->SetGrid();
    //myc->SetLogz();
    hCosvsZ->Draw("COL");
    myc->cd(2);
    gPad->SetGridx();
    gPad->SetLogz();
    //hPhi->Draw();
    hPhivsZ->Draw("COL");
    return myc;
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
        h2dxyz = new TH2D(Form("h2dxy_%d_%d", pdg, plane), hist_titile.c_str(), 400, -200, 200, 400, -200, 200);
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

void Bkgtrack::FilleegtrackMap()
{
    hKineticE0 = new TH1D("hKE0", "Initial kE of #gamma (interact with gas);kE [MeV];", 400, 0, 20.); // 
    hKineticE1 = new TH1D("hKE1", "The first step kE in TPC;kE [MeV];", 400, 0, 20.);
    hKineticE2 = new TH1D("hKE2", "Initial kE of #gamma, just cross TPC;kE [MeV];", 400, 0, 20.);
    hKineticE3 = new TH1D("hKE3", "Initial kE of #gamma, start in TPC;kE [MeV];", 400, 0, 20.);

    long Count1(0), Count2(0), Count3(0);

    for (long long ii = 0; ii < f_Tr->GetEntries(); ++ii)
    {
        if (ii % 200000 == 0)
            std::printf("[INFO]: %lld entries read!\n", ii);
        f_Tr->GetEntry(ii);
        // Step I, check particle name
        if (fPDG == 22)
        {
            Count1++;
            // Step II, check position
            auto vCrosstpc = this->CrossTPCIdx();
            if (vCrosstpc.size() != 0)
            {
                Count2++;
                //Step III, check proc... of gamma in tpc
                int cnt_compt(0), cnt_phot(0), cnt_conv(0),cnt_init(0),cnt_transportation(0),cnt_rayl(0);
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
                        else if (proc_idx == 5)
                            cnt_rayl++;
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
                if (cnt_compt != 0 || cnt_conv != 0 || cnt_phot != 0 || cnt_rayl !=0)
                {
                    hKineticE0->Fill(fStepkE->at(0));
                    hKineticE1->Fill(fStepkE->at(vCrosstpc.at(0)));
                    Count3++;
                    tracks_eeg tmpeeg;
                    tmpeeg.pdg = fPDG;
                    tmpeeg.parentid = fParentid;
                    tmpeeg.trkid = fTrkid;
                    tmpeeg.e0 = fStepkE->at(0);
                    tmpeeg.vxp = (*fStepx);
                    tmpeeg.vyp = (*fStepy);
                    tmpeeg.vzp = (*fStepz);
                    tmpeeg.vde = (*fStepde);
                    fMaptracks[fTrkid].push_back(tmpeeg);
                    fMapParentTrkofGamma[fParentid] = fTrkid;
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

    //fill secondary e-/e+ tracks into fMaptracks
    for (long long ii = 0; ii < f_Tr->GetEntries(); ++ii)
    {
        f_Tr->GetEntry(ii);
        auto iter = fMaptracks.find(fParentid);
        if (iter != fMaptracks.end())
        {
            tracks_eeg tmpeeg;
            tmpeeg.pdg = fPDG;
            tmpeeg.trkid = fTrkid;
            tmpeeg.parentid = fParentid;
            tmpeeg.e0 = fStepkE->at(0);
            tmpeeg.vxp = (*fStepx);
            tmpeeg.vyp = (*fStepy);
            tmpeeg.vzp = (*fStepz);
            tmpeeg.vde = (*fStepde);
            iter->second.push_back(tmpeeg);
        }
        
        auto iter1 = fMapParentTrkofGamma.find(fTrkid);
        if (iter1 != fMapParentTrkofGamma.end())
        {
            int gammatrkid = iter1->second;
            tracks_eeg tmpeeg1;
            tmpeeg1.pdg = fPDG;
            tmpeeg1.trkid = fTrkid;
            tmpeeg1.parentid = fParentid;
            tmpeeg1.e0 = fStepkE->at(0);
            tmpeeg1.vxp = (*fStepx);
            tmpeeg1.vyp = (*fStepy);
            tmpeeg1.vzp = (*fStepz);
            tmpeeg1.vde = (*fStepde);
            fMaptracks[gammatrkid].push_front(tmpeeg1);
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

float Bkgtrack::GetEDepbyelectronInTPC(int BX)
{
    float sumEdep = 0.;
    if (fMaptracks.size() == 0)
    {
        std::printf("[WARNING]: fMaptracks size==0, FilleegtrackMap first\n");
        return 0.;
    }
    for (auto mapiter : fMaptracks)
    {
        auto iter_trks = mapiter.second.begin();
        //skip parent particle
        iter_trks++;
        for (; iter_trks != mapiter.second.end(); ++iter_trks)
        {
            if ((iter_trks->pdg) != 22)
            {
                for (size_t ipoint = 0; ipoint < (iter_trks->vxp).size(); ++ipoint)
                {
                    if (IsinTPCgasRegion((iter_trks->vxp).at(ipoint), (iter_trks->vyp).at(ipoint), (iter_trks->vzp).at(ipoint)))
                    {
                        sumEdep += (iter_trks->vde).at(ipoint);
                    }
                }
            }
        }
    }

    return sumEdep / BX;
}

const std::map<int, std::list<tracks_eeg>> Bkgtrack::GetPrimaryParticleMaps()
{
    std::map<int, std::list<tracks_eeg>> PrieeMaps;
    for (long long ii = 0; ii < f_Tr->GetEntries(); ++ii)
    {
        if (ii % 200000 == 0)
            std::printf("[INFO]: %lld entries read!\n", ii);
        f_Tr->GetEntry(ii);
        if (fParentid == 0)
        {
            tracks_eeg tmpeeg;
            tmpeeg.pdg = fPDG;
            tmpeeg.parentid = fParentid;
            tmpeeg.trkid = fTrkid;
            tmpeeg.e0 = fStepkE->at(0);
            tmpeeg.vxp = (*fStepx);
            tmpeeg.vyp = (*fStepy);
            tmpeeg.vzp = (*fStepz);
            tmpeeg.vde = (*fStepde);
            PrieeMaps[fTrkid].push_back(tmpeeg);
        }
    }

    return PrieeMaps;
}
