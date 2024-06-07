/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Last modified    : 2024-05-30 21:24
 * Filename         : MultiView.cpp
 * Description      : src file of MultiView class
 * Update           :
 * ******************************************************************/
#include "MultiView.h"

MultiView::MultiView(TEveManager* evem, TString viewname)
{
	// Constructor --- creates required scenes, projection managers
 // and GL viewers.

 // Scenes
 //========

	fRPhiGeomScene = evem->SpawnNewScene("RPhi Geometry",
		"Scene holding projected geometry for the RPhi view.");
	fRhoZGeomScene = evem->SpawnNewScene("RhoZ Geometry",
		"Scene holding projected geometry for the RhoZ view.");
	fRPhiEventScene = evem->SpawnNewScene("RPhi Event Data",
		"Scene holding projected event-data for the RPhi view.");
	fRhoZEventScene = evem->SpawnNewScene("RhoZ Event Data",
		"Scene holding projected event-data for the RhoZ view.");


	// Projection managers
	//=====================

	fRPhiMgr = new TEveProjectionManager(TEveProjection::kPT_RPhi);
	gEve->AddToListTree(fRPhiMgr, kFALSE);
	{
		TEveProjectionAxes* a = new TEveProjectionAxes(fRPhiMgr);
		a->SetMainColor(kWhite);
		a->SetTitle("R-Phi");
		a->SetTitleSize(0.05);
		a->SetTitleFont(102);
		a->SetLabelSize(0.025);
		a->SetLabelFont(102);
		fRPhiGeomScene->AddElement(a);
	}

	fRhoZMgr = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
	gEve->AddToListTree(fRhoZMgr, kFALSE);
	{
		TEveProjectionAxes* a = new TEveProjectionAxes(fRhoZMgr);
		a->SetMainColor(kWhite);
		a->SetTitle("Rho-Z");
		a->SetTitleSize(0.05);
		a->SetTitleFont(102);
		a->SetLabelSize(0.025);
		a->SetLabelFont(102);
		fRhoZGeomScene->AddElement(a);
	}


	// Viewers
	//=========

	TEveWindowSlot* slot = nullptr;
	TEveWindowPack* pack = nullptr;

	slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
	pack = slot->MakePack();
	pack->SetElementName("Multi View");
	pack->SetHorizontal();
	pack->SetShowTitleBar(kFALSE);
	pack->NewSlot()->MakeCurrent();
	f3DView = gEve->SpawnNewViewer("3D View", "");
	f3DView->AddScene(gEve->GetGlobalScene());
	f3DView->AddScene(gEve->GetEventScene());

	pack = pack->NewSlot()->MakePack();
	pack->SetShowTitleBar(kFALSE);
	pack->NewSlot()->MakeCurrent();
	fRPhiView = gEve->SpawnNewViewer("RPhi View", "");
	fRPhiView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
	fRPhiView->AddScene(fRPhiGeomScene);
	fRPhiView->AddScene(fRPhiEventScene);

	pack->NewSlot()->MakeCurrent();
	fRhoZView = gEve->SpawnNewViewer("RhoZ View", "");
	fRhoZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
	fRhoZView->AddScene(fRhoZGeomScene);
	fRhoZView->AddScene(fRhoZEventScene);
}

void MultiView::SetDepth(Float_t d)
{
	// Set current depth on all projection managers.
	fRPhiMgr->SetCurrentDepth(d);
	fRhoZMgr->SetCurrentDepth(d);
}

void MultiView::ImportGeomRPhi(TEveElement* el)
{
	fRPhiMgr->ImportElements(el, fRPhiGeomScene);
}

void MultiView::ImportGeomRhoZ(TEveElement* el)
{
	fRhoZMgr->ImportElements(el, fRhoZGeomScene);
}

void MultiView::ImportEventRPhi(TEveElement* el)
{
	fRPhiMgr->ImportElements(el, fRPhiEventScene);
}

void MultiView::ImportEventRhoZ(TEveElement* el)
{
	fRhoZMgr->ImportElements(el, fRhoZEventScene);
}

void MultiView::DestroyEventRPhi()
{
	fRPhiEventScene->DestroyElements();
}

void MultiView::DestroyEventRhoZ()
{
	fRhoZEventScene->DestroyElements();
}