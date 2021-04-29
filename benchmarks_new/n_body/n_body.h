#include <iostream>
#include <iomanip>
#include <fstream>
#include <random>
#include <cmath>
#include <omp.h>

using namespace std;

#ifdef __USE_KUNPENG__
#include <arm_neon.h>
#endif

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

    base_type * __restrict__ pos_x;
    base_type * __restrict__ pos_y;
    base_type * __restrict__ pos_z;

    base_type * __restrict__ vel_x;
    base_type * __restrict__ vel_y;
    base_type * __restrict__ vel_z;

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

        #ifdef __USE_KUNPENG__
        float posx_pi[4] = {pos_x[pi], pos_x[pi], pos_x[pi], pos_x[pi]};
        float32x4_t x_pi = vld1q_f32(posx_pi);
        float posy_pi[4] = {pos_y[pi], pos_y[pi], pos_y[pi], pos_y[pi]};
        float32x4_t y_pi = vld1q_f32(posy_pi);
        float posz_pi[4] = {pos_z[pi], pos_z[pi], pos_z[pi], pos_z[pi]};
        float32x4_t z_pi = vld1q_f32(posz_pi);

        float32x4_t sum_x = {0, 0, 0, 0};
        float32x4_t sum_y = {0, 0, 0, 0};
        float32x4_t sum_z = {0, 0, 0, 0};

        // Calculate total force
        for (int pj = 0; pj < mNumParticles; pj += 4) {
            if(pj/4 == pi/4)
            {
                for (int i = 0; i < 4; i++)
                {
                    if( (i + pj) != pi)
                    {
                        base_type dx = pos_x[pj + i] - pos_x[pi];
                        base_type dy = pos_y[pj + i] - pos_y[pi];
                        base_type dz = pos_z[pj + i] - pos_z[pi];

                        const base_type dist2 = dx * dx + dy * dy + dz * dz;

                        const base_type ConstDist2 = Const / dist2;
                        const base_type idist = 1.0 / sqrt(dist2);

                        // F = C * m * m / ||x2 - x1||^2 * (x2 - x1) / ||x2 - x1||
                        force_x += ConstDist2 * dx * idist;
                        force_y += ConstDist2 * dy * idist;
                        force_z += ConstDist2 * dz * idist;
                    }
                }
            }
            else
            {
                float32x4_t posx =  vld1q_f32(&pos_x[pj]);
                float32x4_t posy =  vld1q_f32(&pos_y[pj]);
                float32x4_t posz =  vld1q_f32(&pos_z[pj]);

                float32x4_t dx = vsubq_f32(posx, x_pi);
                float32x4_t dy = vsubq_f32(posy, y_pi);
                float32x4_t dz = vsubq_f32(posz, z_pi);

                float32x4_t distx = vmulq_f32(dx,dx);
                float32x4_t disty = vmulq_f32(dy,dy);
                float32x4_t distz = vmulq_f32(dz,dz);

                float32x4_t dist2 = vaddq_f32(distx, disty);
                dist2 = vaddq_f32(dist2, distz);

                float32x4_t cnst = vmovq_n_f32(Const);
                float32x4_t cnst_1 = vmovq_n_f32(1.0);

                float32x4_t reciprocal = vrecpeq_f32(dist2);
                reciprocal = vmulq_f32(vrecpsq_f32(dist2, reciprocal), reciprocal);
                reciprocal = vmulq_f32(vrecpsq_f32(dist2, reciprocal), reciprocal);
                float32x4_t ConstDist2 = vmulq_f32(cnst,reciprocal);

                reciprocal = vrsqrteq_f32(dist2);
                reciprocal = vmulq_f32(vrsqrtsq_f32(dist2, reciprocal), reciprocal);
                reciprocal = vmulq_f32(vrsqrtsq_f32(dist2, reciprocal), reciprocal);
                float32x4_t idist = vmulq_f32(cnst_1,reciprocal);

                /*float temp1[4];
                vst1q_f32(temp1, idist);
                for(int i = 0; i < 4; i++)
                {
                    base_type dx = pos_x[pj + i] - pos_x[pi];
                    base_type dy = pos_y[pj + i] - pos_y[pi];
                    base_type dz = pos_z[pj + i] - pos_z[pi];

                    const base_type dist2 = dx * dx + dy * dy + dz * dz;

                    const base_type ConstDist2 = Const / dist2;
                    const base_type idist_c = 1.0 / sqrt(dist2);

                    #pragma omp critical
                    {
                        if (idist_c != temp1[i])
                            cout << idist_c << " " << temp1[i] << endl;
                    }
                }*/

                dx = vmulq_f32(ConstDist2, dx);
                dx = vmulq_f32(dx, idist);
                dy = vmulq_f32(ConstDist2, dy);
                dy = vmulq_f32(dy, idist);
                dz = vmulq_f32(ConstDist2, dz);
                dz = vmulq_f32(dz, idist);

                sum_x = vaddq_f32(sum_x, dx);
                sum_y = vaddq_f32(sum_y, dy);
                sum_z = vaddq_f32(sum_z, dz);
            }
        }

        float temp[4];
        vst1q_f32(temp, sum_x);
        for (int iter = 0; iter < 4; iter++){
            force_x += temp[iter];
        }
        vst1q_f32(temp, sum_y);
        for (int iter = 0; iter < 4; iter++){
            force_y += temp[iter];
        }
        vst1q_f32(temp, sum_z);
        for (int iter = 0; iter < 4; iter++){
            force_z += temp[iter];
        }
        #else
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
        #endif

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

void Kernel(int core_type, Problem &problem, int nTimeSteps)
{
    for (int ts = 0; ts < nTimeSteps; ts++)
    {
        problem.integrate();
    }
}