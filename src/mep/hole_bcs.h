/* Hole_bcs.h -- This file is part of GNU/Archimedes release 0.0.3.
   Archimedes is a simulator for Submicron 2D Silicon/GaAs
   Devices. It implements the Monte Carlo method and Hybrid MEP model
   for the simulation of the semiclassical Boltzmann equation for both
   holes and holes. It also includes the quantum effects by means
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


#ifndef ARCHIMEDES_MEP_HOLEBCS_H
#define ARCHIMEDES_MEP_HOLEBCS_H


#include "global_defines.h"
#include "mesh.h"
#include "utility.h"


// Boundary Conditions for the Hybrid MEP model
// These are completely generic boundary conditions
void MEP_hole_bcs(Mesh *mesh) {
    int nx = mesh->nx,
        ny = mesh->ny;

    // Bottom Edge
    // ===========
    for(int i = 1; i <= nx + 2; ++i) {
        // INSULATOR without potential
        if(mc_is_boundary_insulator(direction_t.BOTTOM, i)) {
            // 1 - hole density
            // 2 - x velocity
            // 3 - y velocity
            // 4 - energy
            for(int n = 1; n <= MN3; ++n) {
                h2d[i+2][0][n] = h2d[i+2][5][n];
                h2d[i+2][1][n] = h2d[i+2][4][n];
                h2d[i+2][2][n] = h2d[i+2][3][n];
            }
        }
        // SCHOTTKY or OHMIC
        else if(mc_is_boundary_contact(direction_t.BOTTOM, i)) {
            double yvel = h2d[i+2][3][3] / h2d[i+2][3][1];
            // density
            h2d[i+2][0][1] = EDGE[0][i][3];
            h2d[i+2][1][1] = EDGE[0][i][3];
            h2d[i+2][2][1] = EDGE[0][i][3];
            // x-component velocity
            h2d[i+2][0][2] = 0.;
            h2d[i+2][1][2] = 0.;
            h2d[i+2][2][2] = 0.;
            // y-component velocity
            h2d[i+2][0][3] = EDGE[0][i][3] * yvel;
            h2d[i+2][1][3] = EDGE[0][i][3] * yvel;
            h2d[i+2][2][3] = EDGE[0][i][3] * yvel;
            // energy
            h2d[i+2][0][4] = 0.5 * mstarhole * M * yvel * yvel * EDGE[0][i][3]
                           + 1.5 * KB * g_config->lattice_temp * EDGE[0][i][3];
            h2d[i+2][1][4] = 0.5 * mstarhole * M * yvel * yvel * EDGE[0][i][3]
                           + 1.5 * KB * g_config->lattice_temp * EDGE[0][i][3];
            h2d[i+2][2][4] = 0.5 * mstarhole * M * yvel * yvel * EDGE[0][i][3]
                           + 1.5 * KB * g_config->lattice_temp * EDGE[0][i][3];
        }
    }

    // Left Edge
    // =========
    for(int j = 1; j <= ny + 2; ++j) {
        // INSULATOR without potential
        if(mc_is_boundary_insulator(direction_t.LEFT, j)) {
            // 1 - hole density
            // 2 - x velocity
            // 3 - y velocity
            // 4 - energy
            for(int n = 1; n <= MN3; ++n) {
                h2d[0][j+2][n] = h2d[5][j+2][n];
                h2d[1][j+2][n] = h2d[4][j+2][n];
                h2d[2][j+2][n] = h2d[3][j+2][n];
            }
        }
        // SCHOTTKY or OHMIC
        else if(mc_is_boundary_contact(direction_t.LEFT, j)) {
            double xvel = h2d[3][j+2][2] / h2d[3][j+2][1];
            // Density
            h2d[0][j+2][1] = EDGE[3][j][3];
            h2d[1][j+2][1] = EDGE[3][j][3];
            h2d[2][j+2][1] = EDGE[3][j][3];
            // x-component velocity
            h2d[0][j+2][2] = EDGE[3][j][3] * xvel;
            h2d[1][j+2][2] = EDGE[3][j][3] * xvel;
            h2d[2][j+2][2] = EDGE[3][j][3] * xvel;
            // y-component velocity
            h2d[0][j+2][3] = 0.;
            h2d[1][j+2][3] = 0.;
            h2d[2][j+2][3] = 0.;
            // energy
            h2d[0][j+2][4] = 0.5 * mstarhole * M * xvel * xvel * EDGE[3][j][3]
                           + 1.5 * KB * g_config->lattice_temp * EDGE[3][j][3];
            h2d[1][j+2][4] = 0.5 * mstarhole * M * xvel * xvel * EDGE[3][j][3]
                           + 1.5 * KB * g_config->lattice_temp * EDGE[3][j][3];
            h2d[2][j+2][4] = 0.5 * mstarhole * M * xvel * xvel * EDGE[3][j][3]
                           + 1.5 * KB * g_config->lattice_temp * EDGE[3][j][3];
        }
    }

    // Right Edge
    // ==========
    for(int j = 1; j <= ny + 2; ++j) {
        // INSULATOR
        if(mc_is_boundary_insulator(direction_t.RIGHT, j)) {
            // 1 - hole density
            // 2 - x velocity
            // 3 - y velocity
            // 4 - energy
            for(int n = 1; n <= MN3; ++n) {
                h2d[nx+2][j+2][n] = h2d[nx+1][j+2][n];
                h2d[nx+3][j+2][n] = h2d[nx  ][j+2][n];
                h2d[nx+4][j+2][n] = h2d[nx-1][j+2][n];
            }
        }
        // SCHOTTKY or OHMIC
        else if(mc_is_boundary_contact(direction_t.RIGHT, j)) {
            double xvel = h2d[nx-1][j+2][2] / h2d[nx-1][j+2][1];
            // density
            h2d[nx+2][j+2][1] = EDGE[1][j][3];
            h2d[nx+3][j+2][1] = EDGE[1][j][3];
            h2d[nx+4][j+2][1] = EDGE[1][j][3];
            // x-component velocity
            h2d[nx+2][j+2][2] = EDGE[1][j][3] * xvel;
            h2d[nx+3][j+2][2] = EDGE[1][j][3] * xvel;
            h2d[nx+4][j+2][2] = EDGE[1][j][3] * xvel;
            // y-component velocity
            h2d[nx+2][j+2][3] = 0.;
            h2d[nx+3][j+2][3] = 0.;
            h2d[nx+4][j+2][3] = 0.;
            // energy
            h2d[nx+2][j+2][4] = 0.5 * mstarhole * M * xvel * xvel * EDGE[1][j][3]
                              + 1.5 * KB * g_config->lattice_temp * EDGE[1][j][3];
            h2d[nx+3][j+2][4] = 0.5 * mstarhole * M * xvel * xvel * EDGE[1][j][3]
                              + 1.5 * KB * g_config->lattice_temp * EDGE[1][j][3];
            h2d[nx+4][j+2][4] = 0.5 * mstarhole * M * xvel * xvel * EDGE[1][j][3]
                              + 1.5 * KB * g_config->lattice_temp * EDGE[1][j][3];
        }
    }

    // Upper Edge
    // ==========
    for(int i = 1; i <= nx + 2; ++i) {
        // INSULATOR
        if(mc_is_boundary_insulator(direction_t.TOP, i)) {
            // 1 - hole density
            // 2 - x velocity
            // 3 - y velocity
            // 4 - energy
            for(int n = 1; n <= MN3; ++n) {
                h2d[i+2][ny+2][n] = h2d[i+2][ny+1][n];
                h2d[i+2][ny+3][n] = h2d[i+2][ny  ][n];
                h2d[i+2][ny+4][n] = h2d[i+2][ny-1][n];
            }
        }
        // SCHOTTKY or OHMIC
        else if(mc_is_boundary_contact(direction_t.TOP, i)) {
            double yvel = h2d[i+2][ny-1][3] / h2d[i+2][ny-1][1];
            // density
            h2d[i+2][ny+2][1] = EDGE[2][i][3];
            h2d[i+2][ny+3][1] = EDGE[2][i][3];
            h2d[i+2][ny+4][1] = EDGE[2][i][3];
            // x-component velocity
            h2d[i+2][ny+2][2] = 0.;
            h2d[i+2][ny+3][2] = 0.;
            h2d[i+2][ny+4][2] = 0.;
            // y-component velocity
            h2d[i+2][ny+2][3] = EDGE[2][i][3] * yvel;
            h2d[i+2][ny+3][3] = EDGE[2][i][3] * yvel;
            h2d[i+2][ny+4][3] = EDGE[2][i][3] * yvel;
            // energy
            h2d[i+2][ny+2][4] = 0.5 * mstarhole * M * yvel * yvel * EDGE[2][i][3]
                              + 1.5 * KB * g_config->lattice_temp * EDGE[2][i][3];
            h2d[i+2][ny+3][4] = 0.5 * mstarhole * M * yvel * yvel * EDGE[2][i][3]
                              + 1.5 * KB * g_config->lattice_temp * EDGE[2][i][3];
            h2d[i+2][ny+4][4] = 0.5 * mstarhole * M * yvel * yvel * EDGE[2][i][3]
                              + 1.5 * KB * g_config->lattice_temp * EDGE[2][i][3];
        }
    }

}

#endif
