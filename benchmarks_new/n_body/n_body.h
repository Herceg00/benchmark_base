#include <iostream>
#include <iomanip>
#include <fstream>
#include <random>
#include <cmath>
#include <omp.h>

using namespace std;


static mt19937 eng;

void SphericToCartesian(base_type cartesianVector[3], const base_type sphericVector[3])
{
    cartesianVector[0] = sphericVector[0] * cos(sphericVector[1]) * sin(sphericVector[2]);
    cartesianVector[1] = sphericVector[0] * sin(sphericVector[1]) * sin(sphericVector[2]);
    cartesianVector[2] = sphericVector[0] * cos(sphericVector[2]);
}

struct Problem {
    const base_type mG = 6.674e-11;
    base_type mMass;
    base_type mInverseMass;
    base_type mDt;
    unsigned mNumParticles;

    base_type *pos_x;
    base_type *pos_y;
    base_type *pos_z;

    base_type *vel_x;
    base_type *vel_y;
    base_type *vel_z;

    Problem(base_type Mass, base_type dt, unsigned numParticles);
    ~Problem();

    void integrate();
    void integrate_blocked();
};

Problem::Problem(base_type Mass, base_type dt, unsigned numParticles)
        :
        mMass(Mass),
        mInverseMass(1 / Mass),
        mDt(dt),
        mNumParticles(numParticles)
{
    pos_x = new base_type[numParticles];
    pos_y = new base_type[numParticles];
    pos_z = new base_type[numParticles];
    vel_x = new base_type[numParticles];
    vel_y = new base_type[numParticles];
    vel_z = new base_type[numParticles];

    for(int i = 0; i < numParticles; i++)
    {
        const base_type maxVelocity = 5000.0;

        uniform_real_distribution<base_type> dist_radius(0.0, 1.0);
        uniform_real_distribution<base_type> dist_psi(0.0, 2 * M_PI);
        uniform_real_distribution<base_type> dist_phi(0.0, M_PI);

        base_type positionSpheric[3] = {dist_radius(eng), dist_psi(eng), dist_phi(eng)};
        base_type local_pos[3];

        SphericToCartesian(local_pos, positionSpheric);

        pos_x[i] = local_pos[0];
        pos_y[i] = local_pos[1];
        pos_z[i] = local_pos[2];


        base_type mod = sqrt(pos_x[i] * pos_x[i] + pos_y[i] * pos_y[i] + pos_z[i] * pos_z[i]);

        vel_x[i] = pos_x[i] * maxVelocity / mod;
        vel_y[i] = pos_y[i] * maxVelocity / mod;
        vel_z[i] = pos_z[i] * maxVelocity / mod;
    }
}

Problem::~Problem()
{
    delete [] pos_x;
    delete [] pos_y;
    delete [] pos_z;

    delete [] vel_x;
    delete [] vel_y;
    delete [] vel_z;
}

void Problem::integrate()
{
    const base_type Const = mG * mMass * mMass;

    #pragma omp parallel for
    for (int pi = 0; pi < mNumParticles; pi++) {

        base_type force_x = 0, force_y = 0, force_z = 0;

        // Calculate total force
        for (int pj = 0; pj < mNumParticles; pj++) {

            if (pj != pi) {
                base_type dx = pos_x[pj] - pos_x[pi];
                base_type dy = pos_y[pj] - pos_y[pi];
                base_type dz = pos_z[pj] - pos_z[pi];

                const base_type dist2 = dx * dx + dy * dy + dz * dz;

                const base_type ConstDist2 = Const / dist2;
                const base_type idist = 1.0 / sqrt(dist2);

                // F = C * m * m / ||x2 - x1||^2 * (x2 - x1) / ||x2 - x1||
                force_x += ConstDist2 * dx * idist;
                force_y += ConstDist2 * dy * idist;
                force_z += ConstDist2 * dz * idist;
            }
        }

        // dv / dt = a = F / m
        vel_x[pi] = force_x * mInverseMass * mDt;
        vel_y[pi] = force_y * mInverseMass * mDt;
        vel_z[pi] = force_z * mInverseMass * mDt;
    }

    // Update pos this should be done after all forces/velocities have being computed
    #pragma omp parallel for
    for (int pi = 0; pi < mNumParticles; pi++)
    {
        // dx / dt = v
        pos_x[pi] += vel_x[pi] * mDt;
        pos_y[pi] += vel_y[pi] * mDt;
        pos_z[pi] += vel_z[pi] * mDt;
    }
}

#define BLOCK_SIZE 32

void Problem::integrate_blocked()
{
    const base_type Const = mG * mMass * mMass;

    #pragma omp parallel for
    for (int pi_start = 0; pi_start < mNumParticles; pi_start += BLOCK_SIZE)
    {
        for (int pj_start = 0; pj_start < mNumParticles; pj_start += BLOCK_SIZE)
        {
            #pragma unroll(BLOCK_SIZE)
            for(int i = 0; i < BLOCK_SIZE; i++)
            {
                int pi = pi_start + i;
                base_type force_x = 0, force_y = 0, force_z = 0;

                #pragma simd
                #pragma ivdep
                #pragma unroll(BLOCK_SIZE)
                for(int j = 0; j < BLOCK_SIZE; j++)
                {
                    int pj = pj_start + j;
                    if (pj != pi) {
                        base_type dx = pos_x[pj] - pos_x[pi];
                        base_type dy = pos_y[pj] - pos_y[pi];
                        base_type dz = pos_z[pj] - pos_z[pi];

                        const base_type dist2 = dx * dx + dy * dy + dz * dz;

                        const base_type ConstDist2 = Const / dist2;
                        const base_type idist = 1.0 / sqrt(dist2);

                        // F = C * m * m / ||x2 - x1||^2 * (x2 - x1) / ||x2 - x1||
                        force_x += ConstDist2 * dx * idist;
                        force_y += ConstDist2 * dy * idist;
                        force_z += ConstDist2 * dz * idist;
                    }
                }

                // dv / dt = a = F / m
                vel_x[pi] = force_x * mInverseMass * mDt;
                vel_y[pi] = force_y * mInverseMass * mDt;
                vel_z[pi] = force_z * mInverseMass * mDt;
            }
        }
    }

    // Update pos this should be done after all forces/velocities have being computed
    #pragma omp parallel for
    for (int pi = 0; pi < mNumParticles; pi++)
    {
        // dx / dt = v
        pos_x[pi] += vel_x[pi] * mDt;
        pos_y[pi] += vel_y[pi] * mDt;
        pos_z[pi] += vel_z[pi] * mDt;
    }
}

void Kernel(int core_type, Problem &problem, int nTimeSteps)
{
    for (int ts = 0; ts < nTimeSteps; ts++)
    {
        switch (core_type)
        {
            case  0:
                problem.integrate();
                break;
            case  1:
                problem.integrate_blocked();
                break;

            default: fprintf(stderr, "Wrong core type of random generator!\n");
        }
    }
}