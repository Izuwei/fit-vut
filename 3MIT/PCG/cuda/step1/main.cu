/**
 * @File main.cu
 *
 * The main file of the project
 *
 * Paralelní programování na GPU (PCG 2021)
 * Projekt c. 1 (cuda)
 * Login: xsadil07
 */

#include <sys/time.h>
#include <cstdio>
#include <cmath>

#include "nbody.h"
#include "h5Helper.h"

/**
 * Main rotine
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv)
{
  // Time measurement
  struct timeval t1, t2;

  if (argc != 10)
  {
    printf("Usage: nbody <N> <dt> <steps> <threads/block> <write intesity> <reduction threads> <reduction threads/block> <input> <output>\n");
    exit(1);
  }

  // Number of particles
  const int N           = std::stoi(argv[1]);
  // Length of time step
  const float dt        = std::stof(argv[2]);
  // Number of steps
  const int steps       = std::stoi(argv[3]);
  // Number of thread blocks
  const int thr_blc     = std::stoi(argv[4]);
  // Write frequency
  int writeFreq         = std::stoi(argv[5]);
  // number of reduction threads
  const int red_thr     = std::stoi(argv[6]);
  // Number of reduction threads/blocks
  const int red_thr_blc = std::stoi(argv[7]);

  // Size of the simulation CUDA gird - number of blocks
  const size_t simulationGrid = (N + thr_blc - 1) / thr_blc;
  // Size of the reduction CUDA grid - number of blocks
  const size_t reductionGrid  = (red_thr + red_thr_blc - 1) / red_thr_blc;

  // Log benchmark setup
  printf("N: %d\n", N);
  printf("dt: %f\n", dt);
  printf("steps: %d\n", steps);
  printf("threads/block: %d\n", thr_blc);
  printf("blocks/grid: %lu\n", simulationGrid);
  printf("reduction threads/block: %d\n", red_thr_blc);
  printf("reduction blocks/grid: %lu\n", reductionGrid);

  const size_t recordsNum = (writeFreq > 0) ? (steps + writeFreq - 1) / writeFreq : 0;
  writeFreq = (writeFreq > 0) ?  writeFreq : 0;


  t_particles particles_cpu;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                            FILL IN: CPU side memory allocation (step 0)                                          //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  float *host_particles = (float *)malloc(N * T_PAR_MEM_CNT * sizeof(float));

  particles_cpu.weights = host_particles;
  particles_cpu.positions_x = &host_particles[N];
  particles_cpu.positions_y = &host_particles[N * 2];
  particles_cpu.positions_z = &host_particles[N * 3];
  particles_cpu.velocities_x = &host_particles[N * 4];
  particles_cpu.velocities_y = &host_particles[N * 5];
  particles_cpu.velocities_z = &host_particles[N * 6];

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                              FILL IN: memory layout descriptor (step 0)                                          //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /*
   * Caution! Create only after CPU side allocation
   * parameters:
   *                      Stride of two               Offset of the first
   *  Data pointer        consecutive elements        element in floats,
   *                      in floats, not bytes        not bytes
  */
  MemDesc md(
        particles_cpu.positions_x,    1,  0,          // Postition in X
        particles_cpu.positions_y,    1,  0,          // Postition in Y
        particles_cpu.positions_z,    1,  0,          // Postition in Z
        particles_cpu.velocities_x,   1,  0,          // Velocity in X
        particles_cpu.velocities_y,   1,  0,          // Velocity in Y
        particles_cpu.velocities_z,   1,  0,          // Velocity in Z
        particles_cpu.weights,        1,  0,          // Weight
        N,                                            // Number of particles
        recordsNum);                                  // Number of records in output file

  // Initialisation of helper class and loading of input data
  H5Helper h5Helper(argv[8], argv[9], md);

  try
  {
    h5Helper.init();
    h5Helper.readParticleData();
  }
  catch (const std::exception& e)
  {
    std::cerr<<e.what()<<std::endl;
    return -1;
  }


  //t_particles particles_gpu;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                  FILL IN: GPU side memory allocation (step 0)                                    //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  std::vector<t_particles> particles_gpu(2);

  float *device_particles;
  for (unsigned int i = 0; i < particles_gpu.size(); i++) {
    cudaMalloc<float>(&device_particles, N * T_PAR_MEM_CNT * sizeof(float));

    particles_gpu[i].weights = device_particles;
    particles_gpu[i].positions_x = &device_particles[N];
    particles_gpu[i].positions_y = &device_particles[N * 2];
    particles_gpu[i].positions_z = &device_particles[N * 3];
    particles_gpu[i].velocities_x = &device_particles[N * 4];
    particles_gpu[i].velocities_y = &device_particles[N * 5];
    particles_gpu[i].velocities_z = &device_particles[N * 6];
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                       FILL IN: memory transfers (step 0)                                         //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for (unsigned int i = 0; i < particles_gpu.size(); i++) {
    cudaMemcpy(particles_gpu[i].weights, host_particles, N * T_PAR_MEM_CNT * sizeof(float), cudaMemcpyHostToDevice);
  }
  
  gettimeofday(&t1, 0);

  dim3 dimGrid(simulationGrid);
  dim3 dimBlock(thr_blc);

  for(int s = 0; s < steps; s++)
  {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                       FILL IN: kernels invocation (step 0)                                     //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    calculate_velocity<<<dimGrid, dimBlock>>>(particles_gpu[s & 1], particles_gpu[(s+1) & 1], N, dt);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                          FILL IN: synchronization  (step 4)                                    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if (writeFreq > 0 && (s % writeFreq == 0))
    {
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //                          FILL IN: synchronization and file access logic (step 4)                             //
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //              FILL IN: invocation of center-of-mass kernel (step 3.1, step 3.2, step 4)                           //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  cudaDeviceSynchronize();

  gettimeofday(&t2, 0);

  // Approximate simulation wall time
  double t = (1000000.0 * (t2.tv_sec - t1.tv_sec) + t2.tv_usec - t1.tv_usec) / 1000000.0;
  printf("Time: %f s\n", t);


  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                             FILL IN: memory transfers for particle data (step 0)                                 //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  float4 comOnGPU = {};
  cudaMemcpy(host_particles, particles_gpu[steps & 1].weights, N * T_PAR_MEM_CNT * sizeof(float), cudaMemcpyDeviceToHost);

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                        FILL IN: memory transfers for center-of-mass (step 3.1, step 3.2)                         //
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  float4 comOnCPU = centerOfMassCPU(md);

  std::cout << "Center of mass on CPU:" << std::endl
            << comOnCPU.x <<", "
            << comOnCPU.y <<", "
            << comOnCPU.z <<", "
            << comOnCPU.w
            << std::endl;

  std::cout << "Center of mass on GPU:" << std::endl
            << comOnGPU.x<<", "
            << comOnGPU.y<<", "
            << comOnGPU.z<<", "
            << comOnGPU.w
            << std::endl;

  // Writing final values to the file
  h5Helper.writeComFinal(comOnGPU.x, comOnGPU.y, comOnGPU.z, comOnGPU.w);
  h5Helper.writeParticleDataFinal();

  return 0;
}// end of main
//----------------------------------------------------------------------------------------------------------------------
