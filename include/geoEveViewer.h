/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Last modified    : 2024-05-30 21:22
 * Filename         : geoEveViewer.h
 * Description      : A geomerty viewer class , Ref. ROOT forum
 * Update           : 
 * ******************************************************************/
#ifndef __geoEveViewer_H__
#define __geoEveViewer_H__ 1
#include <iostream>
#include <vector>
#include <array>
#include <Riostream.h>

#include <TString.h>
#include <TSystem.h>
#include <TEveGeoShape.h>
#include <TGeoManager.h>
#include <TVector3.h>
#include <TEveManager.h>
#include <TEveCompound.h>
#include <TEveTrackPropagator.h>
#include <TEveTrack.h>

#include "MultiView.h"

struct X0tables
{
    std::string matName;
    std::array<double,3> arrContent; // arr[0] -> Distance
                                     // arr[1] -> material RadLen (X0)
                                     // arr[2] -> Distance*100/X0 [%] 
};


class geoEveViewer{
public:
   
    /**
     * @brief 
     * Constructor
     *
     * @param rawGeofile -> exported from DD4hep
     * @param geoGentlefile -> gentle geo file extract from rawgeo file
     */
    geoEveViewer(TString rawGeofile ,TString geoGentlefile, double voltransparency, Bool_t ismapWindow=kTRUE);

    /**
     * @brief 
     * Destructor
     */
    virtual ~geoEveViewer();

    /**
     * @brief 
     * Get get the TGeoManager
     * @return TGeoManager*  
     */
    TGeoManager* Get_Manager(){return geo_manager; };

    /**
     * @brief 
     * Get the top volume of the geometry
     * @return TGeoVolume* 
     */
    TGeoVolume* GetTopVolume(){return geo_manager->GetTopVolume(); };

    /**
     * @brief 
     * Print the geometry volume tree
     */
    void Print_Volumes(){geo_manager->GetListOfVolumes()->Print(); };

    /**
     * @brief 
     * Export the geometry volume tree to a root file
     */
    void Export(){ geo_manager->Export(frawGeoName(0,frawGeoName.Length()-5)+".root","manager","vg"); };

    /**
     * @brief 
     * Print the node tree
     */
    void Print_Nodes(TObjArray* vlist=0);

    /**
     * @brief 
     * Shoot a vector from a given point in a given direction through the geometry
     * and optional print the encounter volumes
     * @param TVector3 pstart 	Starting point
     * @param TVector3 dirstart	Starting direction (direction cosines)
     * @param Bool_t verbose		kTRUE/print out encountered volumes
     * @return std::vector<TString> 	A vector of encountered volume names
     */
    std::vector<X0tables>  Start_Track(TVector3 pstart=	TVector3(0.,0.,0.),
                                      TVector3 dirstart=	TVector3(0.,0.,1.),
                                      Bool_t verbose=	kTRUE);

    /**
     * @brief 
     * Draw the geometry
     * @param TString pviewer The type of viewer to use for display
     * Types are eve, pad, ogl, and x3d 
     */
    void Draw(TString pviewer="eve");

    /**
     * @brief 
     * Display the geometry using raytrace
     */
    void Raytrace();

    /**
     * @brief 
     * Shoot a vector from a given point in a given direction through the geometry
     * @param Char_t ptransparency Set the geometry tranparency 0/clear 100/opake
     */
    //void SetTransparency(Char_t ptransparency=50);
    
    /**
     * @brief
     * Make MultiView: 3D view / RhoPhi view  / RhoZ view 
     */
    void MakeMultiViewer();

    /**
     * @brief 
     * Make a GUI at Left Side
     */
    void MakeLeftGUI();

    /**
     * @brief
     * Load a Helix track: read external file. Default generated by Garfield++ (from Yue)  
     * @param TString filename, external file name
     */
    void LoadMCHelix(TString filename);

    /**
     * @brief
     * Draw Geo in OGL view 
     * @param TString vieweropt OGL/X3D/PAD
     */
    void DrawGeoinOGL(TString vieweropt="ogl");

    /**
     * @brief
     * Extract gentle volume obj
     * @param TString inputfileName, raw geo file exported from DD4hep
     * @param TString outputfileName, gentle geo file name
     * @param int level, vis level
     * @param double voltransparency
     */
    static bool ExtractGentleVolume(TString inputfileName,TString outputfileName, int level, double voltransp=50.);

    /**
    * @brief
    * Generate a helix track, ref $ROOTSYS/tutorials/eve/track.C
    * 
    */
    void GenMCHelixTrack(TVector3 pstart, TVector3 pend, double magB=-2.,bool isRungKutta=true);

protected:
    TEveTrack* Make_Helixtrack(TEveTrackPropagator* prop, TVector3 pstart, TVector3 pend ,int sign);
    void DrawTrack(TVirtualGeoTrack *track);
    static void SetVolumeTransparency(TGeoManager* geoman,double voltransparency);
    void UpdateProjectedView();
    
private:
    TGeoManager* geo_manager;
    TEveManager* evem;
    TEveCompound* cmp;

    MultiView* fmultiview;
    TEveGeoShape* fgentlegeoshape;

    UInt_t fevemWidth;
    UInt_t fevemHeight;
    Bool_t fmapWindow;
    TString viewer;
    Color_t ftrackColor;
    //Char_t transparency;

    double fvolTransparency;
    TString fgentleGeoName;
    TString frawGeoName;
};






#endif
