/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Last modified    : 2024-05-30 21:23
 * Filename         : geoEveViewer.cpp
 * Description      : src file of geoEveViewer
 * Update           : 
 * ******************************************************************/

#include "geoEveViewer.h"
#include "MCMagB.h"

#include <TEveGeoNode.h>
#include <TEveGeoShapeExtract.h>
#include <TVirtualGeoTrack.h>
#include <TGLViewer.h>
#include <TRegexp.h>
#include <TEveLine.h>
#include <TGTab.h>
#include <TGButton.h>


#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>
#include <TCanvas.h>

#include <fstream>


geoEveViewer::geoEveViewer(TString rawGeofile, TString geoGentlefile, double voltransparency, Bool_t ismapWindow) :
    geo_manager(nullptr), evem(nullptr), cmp(nullptr),fmultiview(nullptr), 
    fmapWindow(ismapWindow), viewer("ogl"), fvolTransparency(voltransparency), ftrackColor(kMagenta),
    fgentleGeoName(geoGentlefile)
{
    //Rmove old cache file
    //gSystem->Exec("del *.lnk");
    //Initial Geomanager
    geo_manager = TGeoManager::Import(rawGeofile.Data());
    if (!geo_manager)
    {
        std::printf("====> % s can not found!\n", rawGeofile.Data());
        gSystem->Exit(-1);
    }

    //Create Eve manager
    fevemWidth = 1200;
    fevemHeight = 800;
    evem = new TEveManager(fevemWidth,fevemHeight,fmapWindow);

    //Set cache file path
    TFile::SetCacheFileDir(".");

    auto cachefile = TFile::Open(fgentleGeoName.Data(), "CACHEREAD");
    if (!cachefile)
    {
        std::printf("====> %s can not found!\n",fgentleGeoName.Data());
        gSystem->Exit(-1);
    }

    auto gse = (TEveGeoShapeExtract*)cachefile->Get("Gentle");
    if (!gse)
    {
        std::printf("====> Extracted file needed! invoke geoViewer::ExtractGentleVolume\n");
        gSystem->Exit(-1);
    }

    fgentlegeoshape = TEveGeoShape::ImportShapeExtract(gse, 0);
    
    cachefile->Close();
    delete cachefile;
    evem->AddGlobalElement(fgentlegeoshape);

    //Create track compound
    cmp = new TEveCompound("Tracks");
    cmp->SetMainColor(kOrange - 3);
    evem->AddElement(cmp);

}

geoEveViewer::~geoEveViewer()
{
    delete geo_manager;
    delete evem;
};

void geoEveViewer::Print_Nodes(TObjArray* vlist)
{
    if ( vlist==0 ) vlist=geo_manager->GetListOfNodes();
    vlist->Print();
    for(Int_t i=0; i<vlist->GetEntriesFast(); i++){
        TGeoNode* node=(TGeoNode*)vlist->At(i);
        if( node->GetNdaughters() > 0 )Print_Nodes(node->GetNodes());	//recursize call
    }
};

void geoEveViewer::Draw(TString pviewer)
{
    //viewers: eve,pad,ogl,x3d 
    viewer=pviewer;
    if( viewer != "eve" ){
        GetTopVolume()->Draw(viewer);
        geo_manager->DrawTracks();
        return;
    }else if( !evem ){ 	// Make Eve Window Manager
        evem = new TEveManager(1000,800);
        TGeoNode *node=geo_manager->GetTopNode();
        TEveGeoTopNode* its = new TEveGeoTopNode(geo_manager, node);
        its->SetVisOption(1);
        its->UseNodeTrans();
        evem->AddGlobalElement(its);
        
        cmp = new TEveCompound("Tracks");
        cmp->SetMainColor(kGreen);
        evem->AddElement(cmp);

    }
    //evem->GetDefaultGLViewer()->UpdateScene();
};

void geoEveViewer::DrawTrack(TVirtualGeoTrack *track)
{
    //Process tracks
    cmp->OpenCompound();
    TEveLine *line=new TEveLine;
    line->SetMainColor(ftrackColor);
    line->SetLineStyle(1);
    line->SetLineWidth(1);
    line->SetMarkerColor(kGreen);
    line->SetRnrPoints(true);

    for(Int_t j=0; j<track->GetNpoints(); j++){
        Double_t x,y,z,t;
        track->GetPoint(j, x, y, z, t);
        line->SetNextPoint(x, y, z);
    }
    cmp->AddElement(line);

    cmp->CloseCompound();

    this->UpdateProjectedView();

}

//**********************************************************************

void geoEveViewer::Raytrace()
{
    GetTopVolume()->Raytrace();
};


//**********************************************************************
void geoEveViewer::LoadMCHelix(TString filename)
{
    auto file = TFile::Open(filename.Data());
    if (!file) return;

    TTree* myTreeIn = (TTree*)file->Get("IonEleInfo");
    int vi_Entry = myTreeIn->GetEntries();

    std::cout << "======================= " << vi_Entry << std::endl;

    int vi_trkid = 0;
    int vi_clusid = 0;
    int vi_ne = 0;
    auto vi_vecXe = new std::vector<double>();
    auto vi_vecYe = new std::vector<double>();
    auto vi_vecZe = new std::vector<double>();

    myTreeIn->SetBranchAddress("vecxe", &vi_vecXe);
    myTreeIn->SetBranchAddress("vecye", &vi_vecYe);
    myTreeIn->SetBranchAddress("vecze", &vi_vecZe);



    cmp->OpenCompound();
    TEveLine* line = new TEveLine;
    line->SetMainColor(ftrackColor);
    line->SetLineColor(ftrackColor);
    line->SetMarkerColor(ftrackColor);
    line->SetLineStyle(1);
    line->SetLineWidth(1);
    line->SetRnrPoints(true);

    for (int i = 0; i < vi_Entry; i++)
    {
        myTreeIn->GetEntry(i);
        for (int ii = 0; ii < vi_vecXe->size(); ++ii)
        {
            line->SetNextPoint(vi_vecXe->at(ii), vi_vecYe->at(ii), vi_vecZe->at(ii));
        }
    }

    cmp->AddElement(line);
    cmp->CloseCompound();
    this->UpdateProjectedView();
    file->Close();
    delete file;
    //evem->Redraw3D(kFALSE);

}


//**********************************************************************

std::vector<X0tables> geoEveViewer::Start_Track(TVector3 pstart, TVector3 dirstart, Bool_t verbose)
{
    using namespace std;
    cout.precision(8);

    TGeoNode *nextnode=geo_manager->InitTrack(
                                              pstart.x(), pstart.y(), pstart.z(),
                                              dirstart.x(), dirstart.y(), dirstart.z());
    Int_t id(0);
    Int_t track_index=geo_manager->AddTrack(id, 22, 0);
    TVirtualGeoTrack *track=geo_manager->GetTrack(track_index);
    Double_t time(0.);
    track->AddPoint(pstart.x(), pstart.y(), pstart.z(), time);
    const char *path = geo_manager->GetPath();
    TString name=nextnode->GetName();

    if(verbose) 
        cout<<"Start Node "<<name<<" Current path is: "<<path
            <<" Start Location("<< pstart.x()<< "," << pstart.y() << ","<< pstart.z() << ")" << endl;
    
    double sumofMaterial = 0.;
    double preMaterialRadLen = nextnode->GetMedium()->GetMaterial()->GetRadLen();
    auto preMaterialName = nextnode->GetMedium()->GetMaterial()->GetName();
    TVector3 prePoint = pstart;

    X0tables x0table;
    std::vector<X0tables> vecX0table;

    while( (nextnode=geo_manager->FindNextBoundaryAndStep()) )
    {
        name=nextnode->GetName();
        TVector3 point=geo_manager->GetCurrentPoint();
        track->AddPoint(point.x(), point.y(), point.z(), time);
        time++;
        auto cur_material = nextnode->GetMedium()->GetMaterial();
        auto cur_matRadlen = cur_material->GetRadLen();
        //Double_t step = geo_manager->GetStep();
        Double_t safety = geo_manager->GetSafeDistance();

        //check geo_manager->GetStep() and distance between pre_point and pos_point
        double d_step = (point - prePoint).Mag();
        //if (TMath::Abs(d_step - step) > 1.e-4)
        //    std::printf("===========Flags1\n");

        auto fracMatRadLen = d_step / preMaterialRadLen;
        sumofMaterial += fracMatRadLen;

        x0table.matName = preMaterialName;
        x0table.arrContent[0] = d_step;
        x0table.arrContent[1] = preMaterialRadLen;
        x0table.arrContent[2] = fracMatRadLen * 100.;

        vecX0table.push_back(x0table);

        if(verbose){
            cout<<"StepLength" 	<<setw(12) << d_step << "\t"
                <<"Mat "        <<setw(12) << preMaterialName 
                <<" -> "        <<setw(16) << fracMatRadLen << " X0 "
                <<"Safety" 	    <<setw(8)<< safety << "\t";
            cout<<"NextNode " 	<<setw(12)<< name << "\t";
            cout<<"CurLocation: ("<<point.x()<< "," << point.y() << "," << point.z() << ") "<<endl;
        }
        preMaterialName = cur_material->GetName();
        preMaterialRadLen = cur_matRadlen;
        prePoint = point;
    }

    if(verbose)
        cout << "$$$$$$$$$$$ Material along the track correspods to : " << sumofMaterial << endl;
    
    
    DrawTrack(track);
    return vecX0table;
};

//**********************************************************************

//transparency is integer percent 0/opacque 100/invisiable
//void geoEveViewer::SetTransparency(Char_t ptransparency)
//{
//    transparency=ptransparency;
//    TIter materials(geo_manager->GetListOfMaterials());
//    while (TGeoMaterial *m = (TGeoMaterial *)materials())m->SetTransparency(transparency);
//    Draw(viewer);
//    return;
//};

//**********************************************************************
bool geoEveViewer::ExtractGentleVolume(TString inputfileName, TString outputfileName, int level, double voltransp)
{
    auto evem = TEveManager::Create(kFALSE);
    //auto geoman = TGeoManager::Import(inputfileName.Data());
    auto geoman = evem->GetGeometry(inputfileName.Data());
    if (!geoman)
        return kFALSE;
    std::cout << "==================" << std::endl;

    auto topnode = geoman->GetTopVolume()->FindNode("TPC_envelope_0");
    auto evetopnode = new TEveGeoTopNode(geoman, topnode);
    evetopnode->SetVisLevel(level);
    evem->AddGlobalElement(evetopnode);

    evetopnode->ExpandIntoListTreesRecursively();

    evetopnode->SaveExtract(outputfileName.Data(), "Gentle", kFALSE);

    std::cout << outputfileName << " Created!\n" << std::endl;
    delete evem;
    return kTRUE;
}

//**********************************************************************
void geoEveViewer::MakeMultiViewer()
{
    fmultiview = new MultiView(evem);
    fmultiview->f3DView->GetGLViewer()->SetStyle(TGLRnrCtx::kOutline);

    fmultiview->SetDepth(-10);
    fmultiview->ImportGeomRPhi(fgentlegeoshape);
    fmultiview->ImportGeomRhoZ(fgentlegeoshape);
    fmultiview->SetDepth(0);

    evem->GetViewers()->SwitchColorSet();
    evem->GetDefaultGLViewer()->SetStyle(TGLRnrCtx::kOutline);

    evem->Redraw3D(kTRUE);
}

//**********************************************************************
void geoEveViewer::MakeLeftGUI()
{
    evem->GetBrowser()->GetTabRight()->SetTab(1);

    auto browser = evem->GetBrowser();
    browser->StartEmbedding(TRootBrowser::kLeft);

    auto frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
    frmMain->SetWindowName("GeoViewer GUI");
    frmMain->SetCleanup(kDeepCleanup);

    auto hframe = new TGHorizontalFrame(frmMain);

    TGTextButton* EXIT = new TGTextButton(hframe, "&Exit", "gSystem->Exit(-1)");
    hframe->AddFrame(EXIT, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

    frmMain->AddFrame(hframe);

    frmMain->MapSubwindows();
    frmMain->Resize();
    frmMain->MapWindow();

    browser->StopEmbedding();
    browser->SetTabTitle("Viewer Control", 0);

}

//**********************************************************************
void geoEveViewer::SetVolumeTransparency(TGeoManager* geoman,double voltransparency)
{
	geoman->SetNsegments(40);
	TIterator* volumes = geoman->GetListOfVolumes()->MakeIterator();
	TGeoVolume* volume;
	while ((volume = (TGeoVolume*)(volumes->Next()))) {
		//volume->SetLineColor(color);
		TString volName = volume->GetName();
		if (volName.Contains("lowerlayer") || volName.Contains("upperlayer"))
		{
			volume->SetInvisible();
		}
		else
			volume->SetTransparency(voltransparency);
        
	}

    std::printf("======> End Set Transparency!\n");
}

//**********************************************************************
void geoEveViewer::UpdateProjectedView()
{
    //Update projected views
    fmultiview->DestroyEventRPhi();
    fmultiview->ImportEventRPhi(cmp);

    fmultiview->DestroyEventRhoZ();
    fmultiview->ImportEventRhoZ(cmp);

    evem->Redraw3D(kFALSE, kTRUE);
}

//**********************************************************************
void geoEveViewer::DrawGeoinOGL(TString vieweropt) 
{
    new TCanvas("geo", "geo", 800, 800);
    geo_manager->SetNsegments(40);
    //geo_manager->SetMaxVisNodes(5000);
    TIterator* volumes = geo_manager->GetListOfVolumes()->MakeIterator();
    TGeoVolume* volume;
    while ((volume = (TGeoVolume*)(volumes->Next()))) {
        //volume->SetLineColor(color);
        TString volName = volume->GetName();
        if (volName.Contains("lowerlayer") || volName.Contains("upperlayer"))
        {
            volume->SetInvisible();
        }
        else
            volume->SetTransparency(fvolTransparency);

    }

    Draw(vieweropt);
};

//**********************************************************************
TEveTrack* geoEveViewer::Make_Helixtrack(TEveTrackPropagator* prop, TVector3 pstart, TVector3 pend, int sign)
{
    // Make track with given propagator.
    // Add to math-marks to test fit.

    auto rc = new TEveRecTrackD();
    rc->fV.Set(0.028558, -0.000918, 3.691919);
    rc->fP.Set(0.767095, -2.400006, -0.313103);
    rc->fSign = sign;

    auto track = new TEveTrack(rc, prop);
    track->SetName(Form("Charge %d", sign));
    // daughter 0
    auto pm1 = new TEvePathMarkD(TEvePathMarkD::kDaughter);
    pm1->fV.Set(pstart.x(),pstart.y(),pstart.z());
    track->AddPathMark(*pm1);
    // daughter 1
    auto pm2 = new TEvePathMarkD(TEvePathMarkD::kDaughter);
    pm2->fV.Set(pend.x(),pend.y(),pend.z());
    track->AddPathMark(*pm2);

    return track;
}


void geoEveViewer::GenMCHelixTrack(TVector3 pstart, TVector3 pend, double magB , bool isRungKutta)
{
    auto list = new TEveTrackList();
    TEveTrackPropagator* prop = list->GetPropagator();
    prop->SetFitDaughters(kFALSE);
    prop->SetMaxZ(300);
    prop->SetMaxR(180);

    if (isRungKutta)
    {
        prop->SetStepper(TEveTrackPropagator::kRungeKutta);
        list->SetName("RK Propagator");
    }
    else
        list->SetName("Helix Propagator");

    //Set magnetic field
    prop->SetMagFieldObj(new MCMagB(magB));
    list->SetElementName(Form("%s, const B", list->GetElementName()));

    auto MCtrack = this->Make_Helixtrack(prop, pstart, pend,-1);

    list->SetLineColor(ftrackColor);
    MCtrack->SetLineColor(ftrackColor);
    
    cmp->OpenCompound();
    cmp->AddElement(list);
    list->AddElement(MCtrack);
    cmp->CloseCompound();

    MCtrack->MakeTrack();
    this->UpdateProjectedView();


}
