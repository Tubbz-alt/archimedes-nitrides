/* electron_relaxation.c -- This file is part of GNU/Archimedes 0.1.0
   This code is a simulator for Submicron 2D III-V semiconductor
   Devices. It implements the Monte Carlo method and Hybrid MEP model
   for the simulation of the semiclassical Boltzmann equation for both
   electrons and holes. It also includes the quantum effects by means 
   of effective potential method. It is now able to simulate applied
   magnetic fields along with self consistent Faraday equation.

   Copyright (C) 2004, 2005, 2006, 2007 Jean Michel Sellier <sellier@dmi.unict.it>
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


// =================================================================
// Date of Creation : 12 Aug.2004, Siracusa, Italy, Jean Michel Sellier
// Last Revision : 09 Sep.2007, Siracusa, Italy, Jean Michel Sellier.
// System of Measure : M.K.S.C. System
// =================================================================

// This part is for the simulation of the relaxation part
// of the Hybrid MEP model. This is a parabolic model
// which is based on the Maximum Entropy Principle.
// For more informations and references see the manual.

void electron_relaxation_step(void)
{
 int ND=2;
 register int i,j,c;
 real taup,t;
 real ktaup;
 
// Source Term for the Hybrid MEP model
// ====================================
// We take into account all the relevant scattering
// effects in Silicon. For this purpose, we
// use the same relaxation scattering model used
// in BBW (Baccarani et al.) model.

// This is a simple explicit Euler step.
 for(c=1;c<=50;c++)
  for(i=ND;i<=g_mesh->nx+ND;i++)
   for(j=ND;j<=g_mesh->ny+ND;j++){
     int material = g_mesh->nodes[i][j].material->id;
     t=(2./3.)*(u2d[i][j][4]/u2d[i][j][1])/KB;
     ktaup=M*g_materials[material].cb.mstar[1]*MIU0*g_config->lattice_temp/Q;
     taup=ktaup/t;
     u2d[i][j][4]+=-g_config->dt/50.*(Q*(u2d[i][j][2]*g_mesh->nodes[i-1][j-1].efield.x
                  +u2d[i][j][3]*g_mesh->nodes[i-1][j-1].efield.y)
                  +u2d[i][j][1]*1.5*KB*(t-g_config->lattice_temp)/tauwi(1.5*KB*t));
     u2d[i][j][2]+=-g_config->dt/50.*(u2d[i][j][1]*Q*g_mesh->nodes[i-1][j-1].efield.x
                  /(M*g_materials[material].cb.mstar[1])
                  +u2d[i][j][2]/taup);
     u2d[i][j][3]+=-g_config->dt/50.*(u2d[i][j][1]*Q*g_mesh->nodes[i-1][j-1].efield.y
                  /(M*g_materials[material].cb.mstar[1])
                  +u2d[i][j][3]/taup);
   }
}
// ================================================
