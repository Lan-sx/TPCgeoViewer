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
#include "Lansxlogon.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <exception>
using namespace std;


void extractGentle()
{
    //geoEveViewer::ExtractGentleVolume("../../geodata/TPC_Simple_o1_v01_MM_CF_bugfix.root", "../../geodata/TPC_Simple_o1_v01_MM_CF_bugfix_Extract.root",6);
    //geoEveViewer::ExtractGentleVolume("./TPC_Simple_TDR_o1_v01_mm.root", "./TPC_Simple_TDR_o1_v01_mmExtract.root", 4);
}


int main(int argc, char** argv)
{
    TApplication app("app",&argc,argv);
    //TRint app("app",&argc,argv);
    //extractGentle();
    LansxFormat::myStyle();
    geoEveViewer *g = new geoEveViewer("../../geodata/TPC_ModularEndcap_o1_v01_MM_CF_bugfix.root", "../../geodata/TPC_ModularEndcap_o1_v01_MM_CF_bugfix_Extract.root", 80., kTRUE);
    g->MakeMultiViewer();

    Bkgtrack bkgtracks("../../geodata/AlltracksWholeH241204.root");
    //bkgtracks.PrintTrackInfo(2);
    //bkgtracks.PlotPositionXYZDistribution(11, 0, true);
    bkgtracks.FilleegtrackMap();
    auto selectedtracks = bkgtracks.GeteegTrackMaps();
    std::printf("[INFO]: Map size =%zu\n", selectedtracks.size());
    g->PlotTracks(selectedtracks, 20);
    
    //print secondary e- (caused by gamma) energy deposit
    std::printf("[INFO]: Total Energy Deposit=%.4f [MeV/BX]\n", bkgtracks.GetEDepbyelectronInTPC());
    
    //bkgtracks.PlotPositionXYZDistribution(22, 2, true);
    //bkgtracks.PlotGammaKEDistribution();
    // TVector3 p0(0., 120., 1.), p1(0.,0.,1.);
    //g->GenMCHelixTrack(p0,p1,-3);
    //double xx = 0;
    //auto vecX0tables1 = g->Start_Track(p0, p1, kFALSE,xx);
    //GuiTable::ShowX0guiTable(vecX0tables1);
    //std::cout << "======> " << xx << std::endl;
    //g->TurnOffDrawTrack();
    //auto h1 = g->GetXoverX0vsTheta();
    //auto myc = new TCanvas("myc", "myc", 800, 600);
    //myc->SetGrid();
    //h1->SetStats(kFALSE);
    //LansxFormat::FormatAll(h1, "%a %g", kMagenta, kMagenta);
    //h1->Draw("HIST");


    //load a helix track from Garfield++ 
    //g->LoadMCHelix("../../geodata/TPCtracks00000.root");
    //g->DrawGeoinOGL();
    
    std::printf("============================== Code End!\n");
    app.Run(kTRUE);
    //return 0;
}


