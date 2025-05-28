/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Last modified    : 2024-05-30 21:22
 * Filename         : projection.cpp
 * Description      : geomerty viewer for dd4hep geo
 * Update           :
 * ******************************************************************/
#include <Riostream.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TRint.h>
#include "TFile.h"
#include <TVector3.h>
#include <TList.h>
#include <TSystem.h>
#include <TCanvas.h>

#include "geoEveViewer.h"
#include "MultiView.h"
#include "RunManager.h"
#include "GuiTable.h"
#include "Bkgtrack.h"
#include "Helix.h"
#include "Lansxlogon.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <exception>
using namespace std;


void extractGentle()
{
    //geoEveViewer::ExtractGentleVolume("../../geodata/TDR_Ecal.root", "../../geodata/TDR_Ecal_Extract.root",1);
    geoEveViewer::ExtractGentleVolume("../../geodata/CepcTPCModularEndplatev02.root", "../../geodata/CepcTPCModularEndplatev02_Extract.root", 6);
}


int main(int argc, char** argv)
{
    TApplication app("app",&argc,argv);
    //TRint app("app",&argc,argv);
    //extractGentle();
    LansxFormat::myStyle();
    //geoEveViewer *g = new geoEveViewer("../../geodata/TPC_ModularEndcap_o1_v01_MM_CF_bugfix.root", "../../geodata/TPC_ModularEndcap_o1_v01_MM_CF_bugfix_Extract.root", 80., kTRUE);
    geoEveViewer* g = new geoEveViewer("../../geodata/CepcTPCModularEndplatev02.root", "../../geodata/CepcTPCModularEndplatev02_Extract.root", 80., kTRUE);
    g->MakeMultiViewer();
    Helix trk(1.16, 44.3, 90., 3.);
    trk.GenerateAtrack();
    g->PlotHelixTrack(&trk);
    
    Helix trk1(-0.93, 180-60.4, 90., 3.);
    trk1.GenerateAtrack();
    trk1.SetTrkColor(kGreen);
    g->PlotHelixTrack(&trk1);

    //g->DrawGeoinOGL("ogl");
    //auto geoman = g->Get_GeoManager();
    //auto node1 = geoman->FindNode(0, 120, 145.);
    //std::cout <<" Volume: "<< node1->GetVolume()->GetName()<<" material: "
    //          << node1->GetMedium()->GetMaterial()->GetName() << std::endl;

    //TString logdir = "D:\\Data\\simulation\\CEPCSWBkgSimu\\MDIopt241226-250102\\Higgs_250415To250427\\SSinside_250421\\";
    //TString logrootfile = logdir + "AlltracksSSinSide_Higgs_250421_10BX.root";
    //Bkgtrack bkgtracks(logrootfile.Data());
    //bkgtracks.PrintTrackInfo(2);
    //bkgtracks.PlotPositionXYZDistribution(11, 0, true);
    //bkgtracks.PlotParticleType();
    //bkgtracks.FilleegtrackMap();
    //bkgtracks.PlotGammaPositionDistribution();
    //auto selectedtracks = bkgtracks.GeteegTrackMaps();
    //std::printf("[INFO]: Map size =%zu\n", selectedtracks.size());
    //g->PlotTracks(selectedtracks, 10);
    //bkgtracks.PlotGammaDirectionDistribution();
    //auto primaryeetracks = bkgtracks.GetPrimaryParticleMaps();
    //std::printf("[INFO]: Map size =%zu\n", primaryeetracks.size());
    
    //auto eventmap = bkgtracks.GetEventsMap();
    //int ii = 0;
    //for (auto event : eventmap)
    //{
    //    if (ii >= 1)
    //        break;
    //    std::cout << event.first <<"\t"<<event.second.size() << std::endl;
    //    auto trks_iter = event.second.begin();
    //    for (; trks_iter != event.second.end(); trks_iter++)
    //    {
    //        auto pdgcode = trks_iter->pdg;
    //        auto parent_id = trks_iter->parentid;
    //        if (parent_id == 0)
    //        {
    //            size_t trk_size = trks_iter->vxp.size();
    //            auto xp0 = trks_iter->vxp.at(trk_size-1)/10.;
    //            auto yp0 = trks_iter->vyp.at(trk_size-1)/10.;
    //            auto zp0 = trks_iter->vzp.at(trk_size-1)/10.;
    //            
    //            auto thisnode = geoman->FindNode(double(xp0), double(yp0), double(zp0));
    //            if (!thisnode)
    //            {
    //                std::printf("[Error] Can not find node!!!\n");
    //                continue;
    //            }
    //            auto volname = thisnode->GetVolume()->GetName();
    //            auto materialname = thisnode->GetMedium()->GetName();
    //            std::cout << pdgcode << " P0= (" << xp0 << " ,"
    //                << yp0 << " ,"
    //                << zp0 << " )"
    //                <<" vol: "<< volname
    //                <<" material: "<< materialname
    //                << std::endl;

    //        }
    //    }
    //    ++ii;
    //}
    //g->PlotTracks(eventmap, 1);

    //print secondary e- (caused by gamma) energy deposit
    //std::printf("[INFO]: Total Energy Deposit=%.4f [MeV/BX]\n", bkgtracks.GetEDepbyelectronInTPC(10));
    //bkgtracks.PlotPositionXYZDistribution(22, 2, true);
    //bkgtracks.PlotGammaKEDistribution();

    //auto listCan = gROOT->GetListOfCanvases();
    //auto cc1 = dynamic_cast<TCanvas*>(listCan->FindObject("Cgammaposition"));
    //if (cc1)
    //{
    //    TString C_name = logdir + "Cgammaposition.root";
    //    cc1->Print(C_name.Data());
    //}


    //g->GenMCHelixTrack(p0,p1,-3);
    //TVector3 p0(0., 0., 0.), p1(0., 1., 0.);
    //double xx = 0;
    //auto vecX0tables1 = g->Start_Track(p0, p1, kFALSE,xx);
    //GuiTable::ShowX0guiTable(vecX0tables1);
    //std::cout << "======> " << xx << std::endl;
    //g->TurnOffDrawTrack();
    //auto h1 = g->GetXoverX0vsPhi();
    //auto myc = new TCanvas("myc", "myc", 800, 600);
    //myc->SetGrid();
    //h1->SetStats(kFALSE);
    ////h1->SetTitle("TPC material budget, accumulated projection along #theta");
    //h1->SetTitle("TPC material budget, accumulated projection along #theta");
    ////TColor col1(9001, 134. / 255, 211. / 255, 174. / 255);
    //int hexcol = TColor::GetColor("#86d3ae");
    //LansxFormat::FormatAll(h1, "%a %g", hexcol, hexcol);
    //h1->Draw("HIST");

    //gROOT->GetListOfCanvases()->ls();
    //load a helix track from Garfield++ 
    //g->LoadMCHelix("../../geodata/TPCtracks00000.root");
    //g->DrawGeoinOGL();
    
    std::printf("============================== Code End!\n");
    app.Run(kTRUE);
    //return 0;
}


