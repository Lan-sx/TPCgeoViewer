/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Last modified    : 2024-06-04 10:19
 * Filename         : MCMagB.h
 * Description      : magnetic field description class 
 * Update           : 
 * ******************************************************************/
#ifndef __MCMagB_H__
#define __MCMagB_H__ 1

#include <TEveTrackPropagator.h>
#include <TEveVector.h>

class MCMagB : public TEveMagField
{
private:
    double fmagB;
    bool fmagnetIsOn;

public:
    MCMagB(double magB);
    ~MCMagB() override {};

    virtual Double_t    GetMaxFieldMagD() { return fmagB; }

    TEveVectorD GetFieldD(Double_t /*x*/, Double_t /*y*/, Double_t z) const override
    {
        return TEveVectorD(0, 0, fmagB);
        //if (TMath::Abs(z) < 600) return TEveVectorD(0, 0, 0);
    }
};




#endif
