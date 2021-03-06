/* holemep2d.c -- This file is part of GNU/Archimedes 0.0.3
   This code is a simulator for Submicron 2D Silicon/GaAs
   Devices. It implements the Monte Carlo method and Hybrid MEP model
   for the simulation of the semiclassical Boltzmann equation for both
   electrons and holes. It also includes the quantum effects by means
   of effective potential method.

   Copyright (C) 2004, 2005, 2006, 2007 Jean Michel Sellier <sellier@dmi.unict.it>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
*/


#ifndef ARCHIMEDES_MEP_HOLE_H
#define ARCHIMEDES_MEP_HOLE_H


#include "mesh.h"


// This part is for the simulation of the ballistic part
// of the Hybrid MEP model for holes. This is a parabolic model
// which is based on the Maximum Entropy Principle.
// See the paper:
// "Parabolic Hydrodynamical Model for Bipolar Semiconductor Devices
//  and Low Field Hole Mobility", G.Mascali, V.Romano, J.M.Sellier
void MEP_hole(Mesh *mesh, double theta) {
/*
************************************************************
* INPUT : nx, ny :   # of cells in x-, y-direction
*         dx, dy :   step size  in x-, y-direction
*         tf : final time to stop iteration;
*         to : current time
*         h2d  : conservative variables at time to
*              entries of "h" needed : h[3:nx+3][3:ny+3][4]
* OUTPUT : h2d : conservative variable after 2-stage time iteration
* REMARK : Reset parameters "NXM","NYM" to adjust dimension of
*          arrays, Modify boundary conditions below when necessary.
* CAUTION : h2d[i][j][*] when io=1 is on the cell with solid-line
*           i.e. meaning h2d[i+1/2][j+1/2][*]
************************************************************
*/
    const int ND = 2;

    int nx = mesh->nx,
        ny = mesh->ny;
    double dx = mesh->dx,
           dy = mesh->dy;

    int NXE = nx + ND,
        NYE = ny + ND;
    double dt     = 0.,
           dtodx2 = 0.,
           dtody2 = 0.;

    // reset everything
    memset(bufx2d,0,sizeof(&bufx2d));
    memset(bufy2d,0,sizeof(&bufy2d));
    memset(ux2d,0,sizeof(&ux2d));
    memset(uy2d,0,sizeof(&uy2d));
    memset(f2d,0,sizeof(&f2d));
    memset(g2d,0,sizeof(&g2d));
    memset(fx2d,0,sizeof(&fx2d));
    memset(gy2d,0,sizeof(&gy2d));

    // Start a 2-stage time iteration
    for(int io = 0; io <= 1; ++io) {
        MEP_hole_bcs(mesh);

        // Compute Numerical Slopes for u in x-, y-directions
        // (denoted as "ux", "uy", resp.)
        for(int m = 1; m <= MN3; ++m) {
            for(int j = ND - 2; j <= NYE + 2; ++j) {
                for(int i = ND - 2; i <= NXE + 2; ++i) {
                    bufx2d[i][j] = h2d[i+1][j  ][m] - h2d[i][j][m];
                    bufy2d[i][j] = h2d[i  ][j+1][m] - h2d[i][j][m];
                }
            }
            for(int j = ND; j <= NYE + 1; ++j) {
                for(int i = ND; i <= NXE + 1; ++i) {
                    ux2d[i][j][m] = MM2(theta, bufx2d[i-1][j  ], bufx2d[i][j]);
                    uy2d[i][j][m] = MM2(theta, bufy2d[i  ][j-1], bufy2d[i][j]);
                }
            }
        }

        // Compute the fluxes f and g
        for(int j = ND - 2; j <= NYE + 3; ++j) {
            for(int i = ND - 2; i <= NXE + 3; ++i) {
                double den = h2d[i][j][1];
                double xmt = h2d[i][j][2];
                double ymt = h2d[i][j][3];
                double eng = h2d[i][j][4];
                double vex = xmt / den;
                double vey = ymt / den;
                f2d[i][j][1] = xmt;
                f2d[i][j][2] = 2. / (3. * mstarhole * M) * eng;
                f2d[i][j][3] = 0.;
                f2d[i][j][4] = 4. / 3. * eng * vex;
                g2d[i][j][1] = ymt;
                g2d[i][j][2] = 0.0;
                g2d[i][j][3] = 2. / (3. * mstarhole * M) * eng;
                g2d[i][j][4] = 4. / 3. * eng * vey;
            }
        }

        // Compute numerical slopes for f, g in x-, y-direction
        // (denoted as "fx", "gy", resp.)
        for(int m = 1; m <= MN3; ++m ) {
            for(int j = ND - 2; j <= NYE + 2; ++j) {
                for(int i = ND - 2; i <= NXE + 2; ++i) {
                    bufx2d[i][j] = f2d[i+1][j  ][m] - f2d[i][j][m];
                    bufy2d[i][j] = g2d[i  ][j+1][m] - g2d[i][j][m];
                }
            }
            for(int j = ND; j <= NYE + 1; ++j) {
                for(int i = ND; i <= NXE + 1; ++i) {
                    fx2d[i][j][m] = MM2(theta, bufx2d[i-1][j  ], bufx2d[i][j]);
                    gy2d[i][j][m] = MM2(theta, bufy2d[i  ][j-1], bufy2d[i][j]);
                }
            }
        }

        // Compute the flux values of f, g and h at the center of the four faces
        for(int j = ND; j <= NYE + 1; ++j) {
            for(int i = ND; i <= NXE + 1; ++i) {
                double den = h2d[i][j][1] - dtodx2 * fx2d[i][j][1] - dtody2 * gy2d[i][j][1];
                double xmt = h2d[i][j][2] - dtodx2 * fx2d[i][j][2] - dtody2 * gy2d[i][j][2];
                double ymt = h2d[i][j][3] - dtodx2 * fx2d[i][j][3] - dtody2 * gy2d[i][j][3];
                double eng = h2d[i][j][4] - dtodx2 * fx2d[i][j][4] - dtody2 * gy2d[i][j][4];
                double vex = xmt / den;
                double vey = ymt / den;
                f2d[i][j][1] = xmt;
                f2d[i][j][2] = 2. / (3. * mstarhole * M) * eng;
                f2d[i][j][3] = 0.0;
                f2d[i][j][4] = 4. / 3. * eng * vex;
                g2d[i][j][1] = ymt;
                g2d[i][j][2] = 0.0;
                g2d[i][j][3] = 2. / (3. * mstarhole * M) * eng;
                g2d[i][j][4] = 4. / 3. * eng * vey;
            }
        }

        // Compute time step size
        if(io == 0) {
            dt = g_config->dt;
            if((g_config->time + 2. * dt) >= g_config->tf) {
                dt = 0.5 * (g_config->tf - g_config->time);
            }
        }
        dtodx2 = 0.5 * dt / dx;
        dtody2 = 0.5 * dt / dy;

        // Compute the values of "u2d" at the next time level
        for(int m = 1; m <= MN3; ++m) {
            for(int j = ND + 1 - io; j <= NYE - io; ++j) {
                for(int i = ND + 1 - io; i <= NXE - io; ++i) {
                    bufx2d[i][j] = 0.25 * (h2d[i][j  ][m] + h2d[i+1][j  ][m] +
                                           h2d[i][j+1][m] + h2d[i+1][j+1][m])
                                 + 0.0625 * (ux2d[i][j  ][m] - ux2d[i+1][j  ][m] +
                                             ux2d[i][j+1][m] - ux2d[i+1][j+1][m] +
                                             uy2d[i][j  ][m] + uy2d[i+1][j  ][m] -
                                             uy2d[i][j+1][m] - uy2d[i+1][j+1][m])
                                 + dtodx2 * (f2d[i][j  ][m] - f2d[i+1][j  ][m] +
                                             f2d[i][j+1][m] - f2d[i+1][j+1][m])
                                 + dtody2 * (g2d[i][j  ][m] + g2d[i+1][j  ][m] -
                                             g2d[i][j+1][m] - g2d[i+1][j+1][m]);
                }
            }
            for(int j = ND + 1; j <= NYE; ++j) {
                for(int i = ND + 1; i <= NXE; ++i) {
                    h2d[i][j][m] = bufx2d[i-io][j-io];
                }
            }
        }

    } // End of the 2-stage time iteration

}

#endif
