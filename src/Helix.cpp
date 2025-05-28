/*********************************************************************
 * Author           : Lan-sx
 * Email            : shexin@ihep.ac.cn
 * Created          : 2025-05-28 16:22
 * Filename         : Helix.cpp
 * Description      : 
 * Update           : 
 * ******************************************************************/
#include "Helix.h"

Helix::Helix(double Momentum, double Theta, double Phi, double MagB) : m_momentum(Momentum), m_theta(Theta), m_phi(Phi), m_magB(MagB)
{
	m_x0 = 0.;
	m_y0 = 0.;
	m_z0 = 0.;
	m_drho0 = 0.;
	m_dz = 0.;
	m_trkColor = kMagenta;
}

void Helix::GenerateAtrack()
{
	double rho = this->GetRho();
	double phip0 = m_phi * TMath::DegToRad() - TMath::PiOver2();
	double costheta = TMath::Cos(m_theta * TMath::DegToRad());
	double tanlambda = costheta / TMath::Sqrt(1. - costheta * costheta);
	double dphi = 2*TMath::Pi() / Npoints;

	for (int point_i = 0; point_i < Npoints; ++point_i)
	{
		double phi_i = 0. + point_i * dphi; 
		phi_i = rho > 0. ? -1 * phi_i : phi_i;

		double xx_ii = m_x0 + m_drho0*TMath::Cos(phip0) + rho * (TMath::Cos(phip0) - TMath::Cos(phip0 + phi_i));
		double yy_ii = m_y0 + m_drho0*TMath::Sin(phip0) + rho * (TMath::Sin(phip0) - TMath::Sin(phip0 + phi_i));
		double zz_ii = m_z0 + m_dz - rho * tanlambda*phi_i;
		xx_ii *= 100; // m -> cm
		yy_ii *= 100;
		zz_ii *= 100;

		if (TMath::Sqrt(xx_ii * xx_ii + yy_ii * yy_ii) > 180. || TMath::Abs(zz_ii) > 290.)
			break;
		TVector3 xyzPoint(xx_ii, yy_ii, zz_ii);
		m_vtrkpoints.push_back(xyzPoint);
	}
}
