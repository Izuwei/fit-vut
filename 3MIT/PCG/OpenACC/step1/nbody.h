/**
 * @file      nbody.h
 *
 * @author    Jakub Sadilek \n
 *            Faculty of Information Technology \n
 *            Brno University of Technology \n
 *            xsadil07@stud.fit.vutbr.cz
 *
 * @brief     PCG Assignment 2
 *            N-Body simulation in ACC
 *
 * @version   2021
 *
 * @date      11 November  2020, 11:22 (created) \n
 * @date      15 November  2021, 14:10 (revised) \n
 *
 */

#ifndef __NBODY_H__
#define __NBODY_H__

#include <cstdlib>
#include <cstdio>
#include  <cmath>
#include "h5Helper.h"

/// Gravity constant
constexpr float G = 6.67384e-11f;

/// Collision distance threshold
constexpr float COLLISION_DISTANCE = 0.01f;

/**
 * @struct float4
 * Structure that mimics CUDA float4
 */
struct float4
{
  float x;
  float y;
  float z;
  float w;
};

/**
 * @struct float3
 * Structure that mimics CUDA float3
 */
struct float3
{
  float x;
  float y;
  float z;
};

/// Define sqrtf from CUDA libm library
#pragma acc routine(sqrtf) seq

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Declare following structs / classes                                          //
//                                  If necessary, add your own classes / routines                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Structure with particle data
 */
struct Particles
{
  // Fill the structure holding the particle/s data
  // It is recommended to implement constructor / destructor and copyToGPU and copyToCPU routines
  unsigned N;
  float *positions_x;
  float *positions_y;
  float *positions_z;
  float *weights;

  float *velocities_x;
  float *velocities_y;
  float *velocities_z;

  // Constructor
  Particles(unsigned count) : N(count)
  {
    positions_x = new float[count];
    positions_y = new float[count];
    positions_z = new float[count];
    weights = new float[count];
    velocities_x = new float[count];
    velocities_y = new float[count];
    velocities_z = new float[count];

    #pragma acc enter data copyin(this)
    #pragma acc enter data create(positions_x[N])
    #pragma acc enter data create(positions_y[N])
    #pragma acc enter data create(positions_z[N])
    #pragma acc enter data create(weights[N])
    #pragma acc enter data create(velocities_x[N])
    #pragma acc enter data create(velocities_y[N])
    #pragma acc enter data create(velocities_z[N])
  }

  // Copy data from CPU to GPU
  void copyToGPU()
  {
    #pragma acc update device(positions_x[N])
    #pragma acc update device(positions_y[N])
    #pragma acc update device(positions_z[N])
    #pragma acc update device(weights[N])
    #pragma acc update device(velocities_x[N])
    #pragma acc update device(velocities_y[N])
    #pragma acc update device(velocities_z[N])
  }

  // Copy data from GPU to CPU
  void copyToCPU()
  {
    #pragma acc update host(positions_x[N])
    #pragma acc update host(positions_y[N])
    #pragma acc update host(positions_z[N])
    #pragma acc update host(weights[N])
    #pragma acc update host(velocities_x[N])
    #pragma acc update host(velocities_y[N])
    #pragma acc update host(velocities_z[N])
  }

  // Destructor
  ~Particles()
  {
    #pragma acc exit data delete(positions_x)
    #pragma acc exit data delete(positions_y)
    #pragma acc exit data delete(positions_z)
    #pragma acc exit data delete(weights)
    #pragma acc exit data delete(velocities_x)
    #pragma acc exit data delete(velocities_y)
    #pragma acc exit data delete(velocities_z)
    #pragma acc exit data delete(this)

    delete[] positions_x;
    delete[] positions_y;
    delete[] positions_z;
    delete[] weights;
    delete[] velocities_x;
    delete[] velocities_y;
    delete[] velocities_z;
  }
};// end of Particles
//----------------------------------------------------------------------------------------------------------------------

/**
 * @struct Velocities
 * Velocities of the particles
 */
struct Velocities
{
  // Fill the structure holding the particle/s data
  // It is recommended to implement constructor / destructor and copyToGPU and copyToCPU routines
  unsigned N;
  float *x;
  float *y;
  float *z;

  // Constructor
  Velocities(unsigned count) : N(count)
  {
    x = new float[count];
    y = new float[count];
    z = new float[count];

    #pragma acc enter data copyin(this)
    #pragma acc enter data create(x[count])
    #pragma acc enter data create(y[count])
    #pragma acc enter data create(z[count])
  }

  // Copy data from CPU to GPU
  void copyToGPU()
  {
    #pragma acc update device(x[N])
    #pragma acc update device(y[N])
    #pragma acc update device(z[N])
  }

  // Copy data from GPU to CPU
  void copyToCPU()
  {
    #pragma acc update host(x[N])
    #pragma acc update host(y[N])
    #pragma acc update host(z[N])
  }

  // Destructor
  ~Velocities()
  {
    #pragma acc exit data delete(x)
    #pragma acc exit data delete(y)
    #pragma acc exit data delete(z)
    #pragma acc exit data delete(this)

    delete[] x;
    delete[] y;
    delete[] z;
  }
};// end of Velocities
//----------------------------------------------------------------------------------------------------------------------

/**
 * Compute gravitation velocity
 * @param [in]  p        - Particles
 * @param [out] tmp_vel  - Temporal velocity
 * @param [in ] N        - Number of particles
 * @param [in]  dt       - Time step size
 */
void calculate_gravitation_velocity(const Particles& p,
                                    Velocities&      tmp_vel,
                                    const int        N,
                                    const float      dt);

/**
 * Calculate collision velocity
 * @param [in]  p        - Particles
 * @param [out] tmp_vel  - Temporal velocity
 * @param [in ] N        - Number of particles
 * @param [in]  dt       - Time step size
 */
void calculate_collision_velocity(const Particles& p,
                                  Velocities&      tmp_vel,
                                  const int        N,
                                  const float      dt);

/**
 * Update particle position
 * @param [in]  p        - Particles
 * @param [out] tmp_vel  - Temporal velocity
 * @param [in ] N        - Number of particles
 * @param [in]  dt       - Time step size
 */
void update_particle(const Particles& p,
                     Velocities&      tmp_vel,
                     const int        N,
                     const float      dt);

/**
 * Compute center of gravity - implement in steps 3 and 4.
 * @param [in] p - Particles
 * @param [in] N - Number of particles
 * @return Center of Mass [x, y, z] and total weight[w]
 */
float4 centerOfMassGPU(const Particles& p,
                       const int        N);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Compute center of mass on CPU
 * @param memDesc
 * @return centre of gravity
 */
float4 centerOfMassCPU(MemDesc& memDesc);

#endif /* __NBODY_H__ */
