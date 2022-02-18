/**
 * @file      nbody.cpp
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

#include <math.h>
#include <cfloat>
#include "nbody.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Declare following structs / classes                                          //
//                                  If necessary, add your own classes / routines                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Compute velocity
void calculate_velocity(const Particles& p_in,
                        Particles&       p_out,
                        const int        N,
                        const float      dt)
{
  // Loop over particles
  #pragma acc parallel loop present(p_in, p_out)
  for (unsigned p1_id = 0; p1_id < N; p1_id++)
  {
    float p2_w, F, r, dx, dy, dz;

    float vx = 0;
    float vy = 0;
    float vz = 0;

    float p1_w = p_in.weights[p1_id];
    float p1_x = p_in.positions_x[p1_id];
    float p1_y = p_in.positions_y[p1_id];
    float p1_z = p_in.positions_z[p1_id];
    float p1_vx = p_in.velocities_x[p1_id];
    float p1_vy = p_in.velocities_y[p1_id];
    float p1_vz = p_in.velocities_z[p1_id];

    // Loop over particles
    for (unsigned int p2_id = 0; p2_id < N; p2_id++) {
      p2_w = p_in.weights[p2_id];

      dx = p1_x - p_in.positions_x[p2_id];
      dy = p1_y - p_in.positions_y[p2_id];
      dz = p1_z - p_in.positions_z[p2_id];

      r = sqrt(dx*dx + dy*dy + dz*dz);

      // Gravitation velocity
      F = -G * p1_w * p2_w / (r * r + FLT_MIN);

      vx += (r > COLLISION_DISTANCE) ? ((F * dx / (r + FLT_MIN)) * dt / p1_w) : 0.0f;
      vy += (r > COLLISION_DISTANCE) ? ((F * dy / (r + FLT_MIN)) * dt / p1_w) : 0.0f;
      vz += (r > COLLISION_DISTANCE) ? ((F * dz / (r + FLT_MIN)) * dt / p1_w) : 0.0f;

      // Collision velocity
      vx += (r > 0.0f && r < COLLISION_DISTANCE) ? (((p1_w * p1_vx - p2_w * p1_vx + 2 * p2_w * p_in.velocities_x[p2_id]) /
            (p1_w + p2_w)) - p1_vx) : 0.0f;
      vy += (r > 0.0f && r < COLLISION_DISTANCE) ? (((p1_w * p1_vy - p2_w * p1_vy + 2 * p2_w * p_in.velocities_y[p2_id]) /
            (p1_w + p2_w)) - p1_vy) : 0.0f;
      vz += (r > 0.0f && r < COLLISION_DISTANCE) ? (((p1_w * p1_vz - p2_w * p1_vz + 2 * p2_w * p_in.velocities_z[p2_id]) /
            (p1_w + p2_w)) - p1_vz) : 0.0f;
    }

    // Update particles
    p_out.velocities_x[p1_id] = p1_vx + vx;
    p_out.velocities_y[p1_id] = p1_vy + vy;
    p_out.velocities_z[p1_id] = p1_vz + vz;

    p_out.positions_x[p1_id] = p1_x + p_out.velocities_x[p1_id] * dt;
    p_out.positions_y[p1_id] = p1_y + p_out.velocities_y[p1_id] * dt;
    p_out.positions_z[p1_id] = p1_z + p_out.velocities_z[p1_id] * dt;
  }
}// end of calculate_velocity
//----------------------------------------------------------------------------------------------------------------------

/// Compute center of gravity
float4 centerOfMassGPU(const Particles& p,
                       const int        N)
{

  return {0.0f, 0.0f, 0.0f, 0.0f};
}// end of centerOfMassGPU
//----------------------------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Compute center of mass on CPU
float4 centerOfMassCPU(MemDesc& memDesc)
{
  float4 com = {0 ,0, 0, 0};

  for(int i = 0; i < memDesc.getDataSize(); i++)
  {
    // Calculate the vector on the line connecting points and most recent position of center-of-mass
    const float dx = memDesc.getPosX(i) - com.x;
    const float dy = memDesc.getPosY(i) - com.y;
    const float dz = memDesc.getPosZ(i) - com.z;

    // Calculate weight ratio only if at least one particle isn't massless
    const float dw = ((memDesc.getWeight(i) + com.w) > 0.0f)
                          ? ( memDesc.getWeight(i) / (memDesc.getWeight(i) + com.w)) : 0.0f;

    // Update position and weight of the center-of-mass according to the weight ration and vector
    com.x += dx * dw;
    com.y += dy * dw;
    com.z += dz * dw;
    com.w += memDesc.getWeight(i);
  }
  return com;
}// end of centerOfMassCPU
//----------------------------------------------------------------------------------------------------------------------
