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
 * CUDA kernel to calculate velocity
 * @param p_in    - particles (input)
 * @param p_out   - particles (output)
 * @param N       - Number of particles
 * @param dt      - Size of the time step
 */
__global__ void calculate_velocity(t_particles p_in, t_particles p_out, int N, float dt)
{
  extern __shared__ float shared_memory[];

  // Setup pointers to shared memory
  t_particles cache {
    .weights = shared_memory,
    .positions_x = &shared_memory[blockDim.x],
    .positions_y = &shared_memory[blockDim.x * 2],
    .positions_z = &shared_memory[blockDim.x * 3],
    .velocities_x = &shared_memory[blockDim.x * 4],
    .velocities_y = &shared_memory[blockDim.x * 5],
    .velocities_z = &shared_memory[blockDim.x * 6],
  };

  unsigned int g_id = threadIdx.x + blockIdx.x * blockDim.x;

  // Setup registers to avoid repeated access into global memory
  float F, r, dx, dy, dz;

  float vx = 0;
  float vy = 0;
  float vz = 0;

  float p1_w = p_in.weights[g_id];
  float p1_x = p_in.positions_x[g_id];
  float p1_y = p_in.positions_y[g_id];
  float p1_z = p_in.positions_z[g_id];
  float p1_vx = p_in.velocities_x[g_id];
  float p1_vy = p_in.velocities_y[g_id];
  float p1_vz = p_in.velocities_z[g_id];

  unsigned int offset;
  unsigned int tile_size = blockDim.x;
  unsigned int tile_cnt = ceil(float(N) / tile_size);

  // Loop over tiles
  for (unsigned int tile_id = 0; tile_id < tile_cnt; tile_id++) {
    offset = threadIdx.x + tile_id * blockDim.x;

    // Load data into shared memory
    cache.weights[threadIdx.x] = (offset < N) ? p_in.weights[offset] : 0.0f;
    cache.positions_x[threadIdx.x] = (offset < N) ? p_in.positions_x[offset] : 0.0f;
    cache.positions_y[threadIdx.x] = (offset < N) ? p_in.positions_y[offset] : 0.0f;
    cache.positions_z[threadIdx.x] = (offset < N) ? p_in.positions_z[offset] : 0.0f;
    cache.velocities_x[threadIdx.x] = (offset < N) ? p_in.velocities_x[offset] : 0.0f;
    cache.velocities_y[threadIdx.x] = (offset < N) ? p_in.velocities_y[offset] : 0.0f;
    cache.velocities_z[threadIdx.x] = (offset < N) ? p_in.velocities_z[offset] : 0.0f;

    __syncthreads();

    // Loop over particles in tile
    for (unsigned p_id = 0; p_id < tile_size; p_id++) {
      dx = p1_x - cache.positions_x[p_id];
      dy = p1_y - cache.positions_y[p_id];
      dz = p1_z - cache.positions_z[p_id];

      r = sqrt(dx*dx + dy*dy + dz*dz);

      // Gravitation velocity
      F = -G * p1_w * cache.weights[p_id] / (r * r + FLT_MIN);

      vx += (r > COLLISION_DISTANCE) ? ((F * dx / (r + FLT_MIN)) * dt / p1_w) : 0.0f;
      vy += (r > COLLISION_DISTANCE) ? ((F * dy / (r + FLT_MIN)) * dt / p1_w) : 0.0f;
      vz += (r > COLLISION_DISTANCE) ? ((F * dz / (r + FLT_MIN)) * dt / p1_w) : 0.0f;

      // Collision velocity
      vx += (r > 0.0f && r < COLLISION_DISTANCE) ? (((p1_w * p1_vx - cache.weights[p_id] * p1_vx + 2 * cache.weights[p_id] * cache.velocities_x[p_id]) /
            (p1_w + cache.weights[p_id])) - p1_vx) : 0.0f;
      vy += (r > 0.0f && r < COLLISION_DISTANCE) ? (((p1_w * p1_vy - cache.weights[p_id] * p1_vy + 2 * cache.weights[p_id] * cache.velocities_y[p_id]) /
            (p1_w + cache.weights[p_id])) - p1_vy) : 0.0f;
      vz += (r > 0.0f && r < COLLISION_DISTANCE) ? (((p1_w * p1_vz - cache.weights[p_id] * p1_vz + 2 * cache.weights[p_id] * cache.velocities_z[p_id]) /
            (p1_w + cache.weights[p_id])) - p1_vz) : 0.0f;
    }
    __syncthreads();
  }

  // Update particles
  if (g_id < N) {
    p_out.velocities_x[g_id] = p1_vx + vx;
    p_out.velocities_y[g_id] = p1_vy + vy;
    p_out.velocities_z[g_id] = p1_vz + vz;

    p_out.positions_x[g_id] = p1_x + p_out.velocities_x[g_id] * dt;
    p_out.positions_y[g_id] = p1_y + p_out.velocities_y[g_id] * dt;
    p_out.positions_z[g_id] = p1_z + p_out.velocities_z[g_id] * dt;
  }
}// end of calculate_velocity
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
  extern __shared__ float shared_memory[];

  // Setup pointers to shared memory
  t_particles cache {
    .weights = shared_memory,
    .positions_x = &shared_memory[blockDim.x],
    .positions_y = &shared_memory[blockDim.x * 2],
    .positions_z = &shared_memory[blockDim.x * 3],
  };

  unsigned int g_id = (blockIdx.x * blockDim.x + threadIdx.x);

  // Initialize values in shared memory
  cache.positions_x[threadIdx.x] = p.positions_x[g_id];
  cache.positions_y[threadIdx.x] = p.positions_y[g_id];
  cache.positions_z[threadIdx.x] = p.positions_z[g_id];
  cache.weights[threadIdx.x] = p.weights[g_id];

  float dx, dy, dz, dw;

  __syncthreads();

  // Reduction within a block
  for (unsigned int i = 0; i < blockDim.x; i++) {
    if (threadIdx.x < i) {
      dx = cache.positions_x[i] - cache.positions_x[threadIdx.x];
      dy = cache.positions_y[i] - cache.positions_y[threadIdx.x];
      dz = cache.positions_z[i] - cache.positions_z[threadIdx.x];
      dw = ((cache.weights[i] + cache.weights[threadIdx.x]) > 0.0f) 
           ? (cache.weights[i] / (cache.weights[i] + cache.weights[threadIdx.x])) : 0.0f;

      cache.positions_x[threadIdx.x] += dx * dw;
      cache.positions_y[threadIdx.x] += dy * dw;
      cache.positions_z[threadIdx.x] += dz * dw;
      cache.weights[threadIdx.x] += cache.weights[i];
    }
    __syncthreads();
  }

  // Global reduction
  if (threadIdx.x == 0 && g_id < N) {
    // Lock mutex before entering the critical section
    while (atomicCAS(lock, 0, 1) != 0);

    dx = cache.positions_x[0] - *comX;
    dy = cache.positions_y[0] - *comY;
    dz = cache.positions_z[0] - *comZ;
    dw = ((cache.weights[0] + *comW) > 0.0f)
         ? (cache.weights[0] / (cache.weights[0] + *comW)) : 0.0f;

    *comX += dx * dw;
    *comY += dy * dw;
    *comZ += dz * dw;
    *comW += cache.weights[0];

    // Unlock mutex
    atomicExch(lock, 0);
  }
}// end of centerOfMass
//----------------------------------------------------------------------------------------------------------------------

/**
 * CPU implementation of the Center of Mass calculation
 * @param particles - All particles in the system
 * @param N         - Number of particles
 */
__host__ float4 centerOfMassCPU(MemDesc& memDesc)
{
  float4 com = {0, 0, 0, 0};

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
