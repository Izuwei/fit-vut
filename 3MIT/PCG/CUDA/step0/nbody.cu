/**
 * @File nbody.cu
 *
 * Implementation of the N-Body problem
 *
 * Paralelní programování na GPU (PCG 2021)
 * Projekt c. 1 (cuda)
 * Login: xsadil07
 */

#include <cmath>
#include <cfloat>
#include "nbody.h"

/**
 * CUDA kernel to calculate gravitation velocity
 * @param p       - particles
 * @param tmp_vel - temp array for velocities
 * @param N       - Number of particles
 * @param dt      - Size of the time step
 */
__global__ void calculate_gravitation_velocity(t_particles p, t_velocities tmp_vel, int N, float dt)
{
  unsigned int g_id = threadIdx.x + blockIdx.x * blockDim.x;

  if (g_id >= N) {
    return;
  }

  // Setup registers to avoid repeated access into global memory
  float F, r, dx, dy, dz;

  float vx = 0;
  float vy = 0;
  float vz = 0;

  float p1_w = p.weights[g_id];
  float p1_x = p.positions_x[g_id];
  float p1_y = p.positions_y[g_id];
  float p1_z = p.positions_z[g_id];

  // Loop over particles
  for (unsigned int p_id = 0; p_id < N; p_id++) {
    dx = p1_x - p.positions_x[p_id];
    dy = p1_y - p.positions_y[p_id];
    dz = p1_z - p.positions_z[p_id];

    r = sqrt(dx*dx + dy*dy + dz*dz);

    F = -G * p1_w * p.weights[p_id] / (r * r + FLT_MIN);

    vx += (r > COLLISION_DISTANCE) ? ((F * dx / (r + FLT_MIN)) * dt / p1_w) : 0.0f;
    vy += (r > COLLISION_DISTANCE) ? ((F * dy / (r + FLT_MIN)) * dt / p1_w) : 0.0f;
    vz += (r > COLLISION_DISTANCE) ? ((F * dz / (r + FLT_MIN)) * dt / p1_w) : 0.0f;
  }

  // Store velocity
  tmp_vel.x[g_id] += vx;
  tmp_vel.y[g_id] += vy;
  tmp_vel.z[g_id] += vz;
}// end of calculate_gravitation_velocity
//----------------------------------------------------------------------------------------------------------------------

/**
 * CUDA kernel to calculate collision velocity
 * @param p       - particles
 * @param tmp_vel - temp array for velocities
 * @param N       - Number of particles
 * @param dt      - Size of the time step
 */
__global__ void calculate_collision_velocity(t_particles p, t_velocities tmp_vel, int N, float dt)
{ 
  unsigned int g_id = threadIdx.x + blockIdx.x * blockDim.x;

  if (g_id >= N) {
    return;
  }

  // Setup registers to avoid repeated access into global memory
  float p2_w, r, dx, dy, dz;
    
  float vx = 0;
  float vy = 0;
  float vz = 0;

  float p1_w = p.weights[g_id];
  float p1_x = p.positions_x[g_id];
  float p1_y = p.positions_y[g_id];
  float p1_z = p.positions_z[g_id];
  float p1_vx = p.velocities_x[g_id];
  float p1_vy = p.velocities_y[g_id];
  float p1_vz = p.velocities_z[g_id];

  // Loop over particles
  for (unsigned int p_id = 0; p_id < N; p_id++) {
    p2_w = p.weights[p_id];

    dx = p1_x - p.positions_x[p_id];
    dy = p1_y - p.positions_y[p_id];
    dz = p1_z - p.positions_z[p_id];

    r = sqrt(dx*dx + dy*dy + dz*dz);
    
    vx += (r > 0.0f && r < COLLISION_DISTANCE) ? (((p1_w * p1_vx - p2_w * p1_vx + 2 * p2_w * p.velocities_x[p_id]) /
            (p1_w + p2_w)) - p1_vx) : 0.0f;
    vy += (r > 0.0f && r < COLLISION_DISTANCE) ? (((p1_w * p1_vy - p2_w * p1_vy + 2 * p2_w * p.velocities_y[p_id]) /
            (p1_w + p2_w)) - p1_vy) : 0.0f;
    vz += (r > 0.0f && r < COLLISION_DISTANCE) ? (((p1_w * p1_vz - p2_w * p1_vz + 2 * p2_w * p.velocities_z[p_id]) /
            (p1_w + p2_w)) - p1_vz) : 0.0f;
  }

  // Store velocity
  tmp_vel.x[g_id] += vx;
  tmp_vel.y[g_id] += vy;
  tmp_vel.z[g_id] += vz;
}// end of calculate_collision_velocity
//----------------------------------------------------------------------------------------------------------------------

/**
 * CUDA kernel to update particles
 * @param p       - particles
 * @param tmp_vel - temp array for velocities
 * @param N       - Number of particles
 * @param dt      - Size of the time step
 */
__global__ void update_particle(t_particles p, t_velocities tmp_vel, int N, float dt)
{
  unsigned int g_id = threadIdx.x + blockIdx.x * blockDim.x;

  if (g_id < N) {
    p.velocities_x[g_id] += tmp_vel.x[g_id];
    p.velocities_y[g_id] += tmp_vel.y[g_id];
    p.velocities_z[g_id] += tmp_vel.z[g_id];

    p.positions_x[g_id] += p.velocities_x[g_id] * dt;
    p.positions_y[g_id] += p.velocities_y[g_id] * dt;
    p.positions_z[g_id] += p.velocities_z[g_id] * dt;
  }
}// end of update_particle
//----------------------------------------------------------------------------------------------------------------------

/**
 * CUDA kernel to update particles
 * @param p       - particles
 * @param comX    - pointer to a center of mass position in X
 * @param comY    - pointer to a center of mass position in Y
 * @param comZ    - pointer to a center of mass position in Z
 * @param comW    - pointer to a center of mass weight
 * @param lock    - pointer to a user-implemented lock
 * @param N       - Number of particles
 */
__global__ void centerOfMass(t_particles p, float* comX, float* comY, float* comZ, float* comW, int* lock, const int N)
{

}// end of centerOfMass
//----------------------------------------------------------------------------------------------------------------------

/**
 * CPU implementation of the Center of Mass calculation
 * @param particles - All particles in the system
 * @param N         - Number of particles
 */
__host__ float4 centerOfMassCPU(MemDesc& memDesc)
{
  float4 com = {0 ,0, 0, 0};

  for(int i = 0; i < memDesc.getDataSize(); i++)
  {
    // Calculate the vector on the line connecting current body and most recent position of center-of-mass
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
}// enf of centerOfMassCPU
//----------------------------------------------------------------------------------------------------------------------
