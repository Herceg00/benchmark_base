#include <iostream>
#include <iomanip>
#include <fstream>
#include <random>
#include <cmath>
#include <omp.h>

using namespace std;

struct Particle {
    static mt19937 eng;
    double pos[3];
    double vel[3];
    Particle();
};

mt19937 Particle::eng(static_cast<mt19937::result_type>(12345));

void SphericToCartesian(double cartesianVector[3], const double sphericVector[3])
{
    cartesianVector[0] = sphericVector[0] * cos(sphericVector[1]) * sin(sphericVector[2]);
    cartesianVector[1] = sphericVector[0] * sin(sphericVector[1]) * sin(sphericVector[2]);
    cartesianVector[2] = sphericVector[0] * cos(sphericVector[2]);
}

Particle::Particle()
{
    const double maxVelocity = 5000.0;

    uniform_real_distribution<double> dist_radius(0.0, 1.0);
    uniform_real_distribution<double> dist_psi(0.0, 2 * M_PI);
    uniform_real_distribution<double> dist_phi(0.0, M_PI);

    double positionSpheric[3] = {dist_radius(eng), dist_psi(eng), dist_phi(eng)};

    SphericToCartesian(pos, positionSpheric);

    vel[0] = pos[0];
    vel[1] = pos[1];
    vel[2] = pos[2];
    double mod = sqrt(pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2]);
    for (auto& v : vel)
        v *= maxVelocity / mod;
}

struct Problem {
    const double mG = 6.674e-11;
    double mMass;
    double mInverseMass;
    double mDt;
    unsigned mNumParticles;
    Particle * mParticles;
    Problem(double Mass, double dt, unsigned numParticles);
    ~Problem();
    void integrate();
    void write_csv(unsigned ts);
    void write_console(unsigned ts);
};

Problem::Problem(double Mass, double dt, unsigned numParticles)
        :
        mMass(Mass),
        mInverseMass(1 / Mass),
        mDt(dt),
        mNumParticles(numParticles)
{
    mParticles = new Particle[numParticles];
}

Problem::~Problem()
{
    delete [] mParticles;
}

void Problem::write_csv(unsigned ts)
{
    std::stringstream ss;
    ss << "particles_" << ts << ".csv";
    ofstream outFile;
    outFile.open(ss.str());

    outFile << "x,y,z,vx,vy,vz" << endl;
    outFile << scientific ;
    for (int pi = 0; pi < mNumParticles; pi++) {
        outFile << mParticles[pi].pos[0] << ","
                << mParticles[pi].pos[1] << ","
                << mParticles[pi].pos[2] << ","
                << mParticles[pi].vel[0] << ","
                << mParticles[pi].vel[1] << ","
                << mParticles[pi].vel[2] << endl;
    }
    outFile.close();
}

void Problem::write_console(unsigned ts)
{
    cout << scientific ;
    for (int pi = 0; pi < mNumParticles; pi++) {
        for (auto& p : mParticles[pi].pos)
            cout << setw(15) << p << ",";
        for (auto& v : mParticles[pi].vel)
            cout << setw(15) << v << ",";
        cout << endl;
    }
}

void Problem::integrate()
{
    const double Const = mG * mMass * mMass;

    #pragma omp parallel for
    for (int pi = 0; pi < mNumParticles; pi++) {

        double force[3] = {};

        // Calculate total force
        for (int pj = 0; pj < mNumParticles; pj++) {

            if (pj != pi) {
                const double dij[3] = {
                        mParticles[pj].pos[0] - mParticles[pi].pos[0],
                        mParticles[pj].pos[1] - mParticles[pi].pos[1],
                        mParticles[pj].pos[2] - mParticles[pi].pos[2]};

                const double dist2 = dij[0] * dij[0] + dij[1] * dij[1] + dij[2] * dij[2];
                const double ConstDist2 = Const / dist2;
                const double idist = 1 / sqrt(dist2);

                // F = C * m * m / ||x2 - x1||^2 * (x2 - x1) / ||x2 - x1||
                force[0] += ConstDist2 * dij[0] * idist;
                force[1] += ConstDist2 * dij[1] * idist;
                force[2] += ConstDist2 * dij[2] * idist;
            }
        }

        // dv / dt = a = F / m
        mParticles[pi].vel[0] += force[0] * mInverseMass * mDt;
        mParticles[pi].vel[1] += force[1] * mInverseMass * mDt;
        mParticles[pi].vel[2] += force[2] * mInverseMass * mDt;
    }

    // Update pos this should be done after all forces/velocities have being computed
    #pragma omp parallel for
    for (int pi = 0; pi < mNumParticles; pi++) {
        // dx / dt = v
        mParticles[pi].pos[0] += mParticles[pi].vel[0] * mDt;
        mParticles[pi].pos[1] += mParticles[pi].vel[1] * mDt;
        mParticles[pi].pos[2] += mParticles[pi].vel[2] * mDt;
    }
}

void Kernel(Problem &problem, int nTimeSteps)
{
    for (int ts = 0; ts < nTimeSteps; ts++)
    {
        problem.integrate();
    }
}