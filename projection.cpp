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
#include <TRegexp.h>
#include <TList.h>
#include <TSystem.h>
#include <TCanvas.h>

#include <TGeoManager.h>
#include <TGeoVolume.h>
#include <TGeoNode.h>
#include <TVirtualGeoTrack.h>
#include <TGeoMaterial.h>

#include <TEveManager.h>
#include <TEveGeoNode.h>
#include <TEveCompound.h>
#include <TEveLine.h>
#include "TEveViewer.h"
#include "TEveProjections.h"
#include "TEveProjectionManager.h"
#include "TEveScene.h"
#include "TEveProjectionAxes.h"
#include <TGLViewer.h>

#include <TEveGeoShape.h>

#include "geoEveViewer.h"
#include "MultiView.h"
#include "RunManager.h"
#include "GuiTable.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
using namespace std;


void test01()
{
    geoEveViewer::ExtractGentleVolume("./TPC_ModularEndcap_o1_v01_MMCF.root", "./TPC_ModularEndcap_o1_v01_MMCF_Extract.root",4);
    //geoEveViewer::ExtractGentleVolume("./TPC_Simple_TDR_o1_v01_mm.root", "./TPC_Simple_TDR_o1_v01_mmExtract.root", 4);
}

int main(int argc, char** argv)
{
    TApplication app("app",&argc,argv);
    //TRint app("app",&argc,argv);
    
    //RunManager run1(0);
    //std::printf("=======> Input Mode!\n");
    //int mode;
    //std::scanf("%d", &mode);
    //cout << "========> " << mode << endl;
    //run1.RunMode(mode);


    geoEveViewer *g = new geoEveViewer("./TPC_ModularEndcap_o1_v01_MMCF.root", "./TPC_ModularEndcap_o1_v01_MMCF_Extract.root", 80.);
    g->MakeMultiViewer();
    
    TVector3 p0(0., 120., 280.), p1(0., 0., 1.);
    auto vecX0tables = g->Start_Track(p0, p1, kTRUE);

    //std::printf("=========================================\n ");
    //p1[1] = 1.; p1[2] = 0.;
    //p0[1] = 0.; p0[2] = 145.;
    //g->Start_Track(p0, p1, kTRUE);


    double totalDistance = 0.;
    double totalX0 = 0.;
    auto sumDistanceandX0 = [&](X0tables& x0table) 
    { 
         totalDistance += x0table.arrContent[0];
         totalX0 += x0table.arrContent[2];
    };
    
    std::for_each(vecX0tables.begin(), vecX0tables.end(), sumDistanceandX0);

    GuiTable* te = new GuiTable();

    for (int i = 0; i < vecX0tables.size(); i++) {
        te->AddRow(Form("%s", vecX0tables.at(i).matName.c_str()), 
                   Form("%.4f",vecX0tables.at(i).arrContent[0]), 
                   Form("%.4f",vecX0tables.at(i).arrContent[1]),
                   Form("%.4f",vecX0tables.at(i).arrContent[2])
                   );
    }
    TString cont1 = Form("total material budget = %.4f", totalX0);
    auto cont = cont1 + "[%]";
    te->AddRow("Summary", Form("totalDistance = %.4f [cm]", totalDistance), "", cont.Data());

    te->Show();

    //std::printf("=========================================\n ");
    //p0[0] = 65.*TMath::Cos(0.015); p0[1] = 65.*TMath::Sin(-0.015); p0[2] = 145.;
    //p1[0] = 0.; p1[2] = 0.; p1[2] = 1.;
    //g->Start_Track(p0, p1, kTRUE);

    //auto myc = new TCanvas("myc", "myc", 800, 600);
    //auto geoMan = g->Get_Manager();
    //TVector3 p_check(65., 0., 10.);
    //geoMan->CheckPoint(p_check.x(), p_check.y(), p_check.z());

    //g->LoadMCHelix("./IonInfoproton_0GeV_TDR_0.0500pad_2T_0.20.root");
    //g->DrawGeoinOGL();
    

    std::printf("============================== Code End!\n");
    app.Run(kTRUE);
    return 0;
}

