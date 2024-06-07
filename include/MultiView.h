/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Last modified    : 2024-05-30 21:21
 * Filename         : MultiView.h
 * Description      : Import from $ROOTSYS/tutorials/eve/MultiView.C
 * Update           : 
 * ******************************************************************/
#ifndef __MULTIVIEW_H__
#define __MULTIVIEW_H__ 1

#include <iostream>

#include <TEveManager.h>

#include <TEveViewer.h>
#include <TGLViewer.h>

#include <TEveScene.h>

#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>

#include <TEveBrowser.h>
#include <TEveWindow.h>

 // MultiView
 //
 // Structure encapsulating standard views: 3D, r-phi and rho-z.
 // Includes scenes and projection managers.
 //
 // Should be used in compiled mode.



class MultiView {

    //---------------------------------------------------------------------------
public:
    MultiView(TEveManager *evem,TString viewname="multiview");

    //public methods
    void SetDepth(Float_t d);
    void ImportGeomRPhi(TEveElement* el);
    void ImportGeomRhoZ(TEveElement* el);
    void ImportEventRPhi(TEveElement* el);
    void ImportEventRhoZ(TEveElement* el);
    void DestroyEventRPhi();
    void DestroyEventRhoZ();


    TEveProjectionManager* fRPhiMgr;
    TEveProjectionManager* fRhoZMgr;

    TEveViewer* f3DView;
    TEveViewer* fRPhiView;
    TEveViewer* fRhoZView;

    TEveScene* fRPhiGeomScene;
    TEveScene* fRhoZGeomScene;
    TEveScene* fRPhiEventScene;
    TEveScene* fRhoZEventScene;
};
#endif
