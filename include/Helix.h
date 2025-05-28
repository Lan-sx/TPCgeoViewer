/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Created          : 2025-05-28 16:22
 * Filename         : Helix.h
 * Description      : 
 * Update           : 
 * ******************************************************************/
#ifndef __Helix_H__
#define __Helix_H__ 1
//std
#include <iostream>
#include <vector>

//ROOT CERN
#include "TMath.h"
#include "TVector3.h"

//Users
#include "Bkgtrack.h"

constexpr int Npoints = 400;

class Helix
{
public:
	Helix(double Momentum, double Theta, double Phi, double MagB);
	~Helix() {};

	void SetTrkColor(decltype(kRed) trkcolor) { m_trkColor = trkcolor; }
	void GenerateAtrack();
	const std::vector<TVector3>& GetMCHelixTrk() const { return m_vtrkpoints; }
	inline const double GetRho();
	const decltype(kRed) GetTrkColor() const { return m_trkColor; }

private:
	decltype(kRed) m_trkColor;
	double m_momentum;
	double m_x0, m_y0, m_z0, m_drho0, m_dz;
	double m_theta, m_phi;
	double m_magB;
	std::vector<TVector3> m_vtrkpoints;
};

inline const double Helix::GetRho()
{
	if (TMath::Abs(m_momentum) < 1.e-4) // 1 GeV/c * 1e-4 = 0.1 MeV/c
	{
		std::printf("[warning] too small momentum\n");
		return 1.11;
	}

	double Pt = m_momentum * TMath::Sin(m_theta * TMath::DegToRad());
	return Pt * (1. / (TMath::C() * m_magB / 1.e+9));
}

#endif
