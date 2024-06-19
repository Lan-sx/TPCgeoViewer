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
//#include <TCanvas.h>

//#include <TGeoManager.h>
//#include <TGeoVolume.h>
//#include <TGeoNode.h>
//#include <TVirtualGeoTrack.h>
//#include <TGeoMaterial.h>

//#include <TEveManager.h>
//#include <TEveGeoNode.h>
//#include <TEveCompound.h>
//#include <TEveLine.h>
#include "TEveViewer.h"
//#include "TEveProjections.h"
//#include "TEveProjectionManager.h"
//#include "TEveScene.h"
//#include "TEveProjectionAxes.h"
#include <TGLViewer.h>
#include <TGTab.h>

#include <TEveGeoShape.h>

#include "geoEveViewer.h"
#include "MultiView.h"
#include "RunManager.h"
#include "GuiTable.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <exception>
using namespace std;


void extractGentle()
{
    //geoEveViewer::ExtractGentleVolume("../../geodata/TPC_ModularEndcap_o1_v01_MMCF.root", "../../geodata/TPC_ModularEndcap_o1_v01_MMCF_Extract.root",4);
    //geoEveViewer::ExtractGentleVolume("./TPC_Simple_TDR_o1_v01_mm.root", "./TPC_Simple_TDR_o1_v01_mmExtract.root", 4);
}


int main(int argc, char** argv)
{
    TApplication app("app",&argc,argv);
    //TRint app("app",&argc,argv);
    //test01();
    geoEveViewer *g = new geoEveViewer("../../geodata/TPC_ModularEndcap_o1_v01_MMCF.root", "../../geodata/TPC_ModularEndcap_o1_v01_MMCF_Extract.root", 80., kTRUE);
    g->MakeMultiViewer();
    
    GuiTable* gui = new GuiTable(g->Get_EveManager());
    
    //g->MakeLeftGUI();
    
    //TVector3 p0(0., 160., -140.), p1(0., 1., 0.);
    //auto vecX0tables1 = g->Start_Track(p0, p1, kFALSE);
    
    //GuiTable::ShowX0guiTable(vecX0tables1);

    //p1[1] = 1.; p1[2] = 2.;
    //auto vecX0table2 =g->Start_Track(p0, p1, kTRUE);
    //GuiTable::ShowX0guiTable(vecX0table2);


    //load a helix track from Garfield++ 
    //g->LoadMCHelix("./IonInfoproton_0GeV_TDR_0.0500pad_2T_0.20.root");
    //g->DrawGeoinOGL();
    

    std::printf("============================== Code End!\n");
    app.Run(kTRUE);
    //return 0;
}


