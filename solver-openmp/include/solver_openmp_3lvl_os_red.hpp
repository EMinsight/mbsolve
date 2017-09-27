/*
 * mbsolve: Framework for solving the Maxwell-Bloch/-Lioville equations
 *
 * Copyright (c) 2016, Computational Photonics Group, Technical University of
 * Munich.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef MBSOLVE_SOLVER_OPENMP_3LVL_OS_RED_H
#define MBSOLVE_SOLVER_OPENMP_3LVL_OS_RED_H

#include <iostream>
#include <omp.h>
#include <solver.hpp>
#include <internal/common_fdtd_2lvl.hpp>
#include <internal/copy_list_entry.hpp>

namespace mbsolve {

const unsigned int num_lvl = 3;
const unsigned int num_adj = num_lvl * num_lvl - 1;

class sim_constants_3lvl_os
{
public:
    /* constant propagators */
    Eigen::Matrix<complex, num_adj, num_adj> B_1;
    Eigen::Matrix<complex, num_adj, num_adj> B_2;

    /* backup: without diagonalization */
    Eigen::Matrix<real, num_adj, num_adj> A_0;
    Eigen::Matrix<complex, num_adj, num_adj> P;

    /* required for polarization calc ? */
    Eigen::Matrix<real, num_adj, num_adj> M_0;
    Eigen::Matrix<real, num_adj, num_adj> U;
    /* dipole moments */
    /* TODO generalize */
    real dipole_moments[3];

    /* diagonalized interaction propagator */
    /* TODO: special type for diagonal matrix? */
    /* TODO: vector would do, right? */
    Eigen::Array<complex, num_adj, 1> L;

    /* electromagnetic constants */
    real M_CE;
    real M_CH;
    real M_CP;
    real sigma;

    /* simulation constants */
    real d_x_inv;
    real d_t;

    /* initialization constants */
    real inversion_init;

};

class solver_openmp_3lvl_os_red : public solver_int
{
public:
    solver_openmp_3lvl_os_red(std::shared_ptr<const device> dev,
                              std::shared_ptr<scenario> scen);

    ~solver_openmp_3lvl_os_red();

    const std::string& get_name() const;

    void run() const;

private:
    /* TODO: rule of three. make copy constructor etc. private?
     * or implement correctly
     */

    /*
     * Position-dependent density matrix in adjoint representation.
     */
    Eigen::Matrix<real, num_adj, 1> **m_d;

    std::vector<Eigen::Matrix<complex, num_lvl, num_lvl>  > m_generators;

    real **m_h;
    real **m_e;

    real *m_result_scratch;

    unsigned int m_scratch_size;

    real *m_source_data;

    unsigned int **m_mat_indices;

#ifdef XEON_PHI_OFFLOAD
    copy_list_entry_dev *l_copy_list;
#else
    copy_list_entry *l_copy_list;
#endif
    sim_source *l_sim_sources;
    sim_constants_3lvl_os *l_sim_consts;

    std::vector<sim_constants_3lvl_os> m_sim_consts;

    std::vector<sim_source> m_sim_sources;

    std::vector<copy_list_entry> m_copy_list;
};

}

#endif