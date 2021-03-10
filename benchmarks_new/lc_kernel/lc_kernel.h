#include <string>

using std::string;

#define CACHE_LINE_K 256 // CACHE_LINE_K should be bigger than size of cache string

struct cuvector {
    float x, y, z;


    /** Constructor
    **/
    cuvector(float c = 0.00) throw() { x = y = z = c; }

    /** Constructor
    **/
    cuvector(float vx, float vy, float vz) throw() {
        x = vx;
        y = vy;
        z = vz;
    }

    float length() const throw() {
        float sq = x * x + y * y + z * z;
        return sq == 0.00 ? (0.00) : (sqrt(sq));
    }

    float length2() const throw() {
        return x * x + y * y + z * z;
    }

    int normalize() throw() {
        float r = length();
        if (r == 0.00) return 1;
        x /= r;
        y /= r;
        z /= r;
        return 0;
    }

    cuvector &operator+=(const cuvector &b) throw() {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }

    cuvector &operator*=(float c) throw() {
        x *= c;
        y *= c;
        z *= c;
        return *this;
    }

    cuvector operator+(const cuvector &b) const throw() { return cuvector(x + b.x, y + b.y, z + b.z); }

    cuvector operator-(const cuvector &b) const throw() { return cuvector(x - b.x, y - b.y, z - b.z); }

    cuvector operator*(float c) const throw() { return cuvector(x * c, y * c, z * c); }

    cuvector operator/(float c) const throw() {
        /* if(c==0.00) throw v3_exc::bad_value(); */
        return cuvector(x / c, y / c, z / c);
    }

    void CP(const cuvector &a, const cuvector &b) throw() {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }
}; // struct cuvector

struct slpointers {

    /** Device pointers
        Secondary lattice (rib middles), current and previous states
        Arrays of vectors, sizes:
            x - (_plcsize.x, _plcsize.y, _plcsize.z)
            y - (_plcsize.x, _plcsize.y, _plcsize.z)
            z - (_plcsize.x, _plcsize.y, _plcsize.z)
    **/
    cuvector *slxc, *slyc, *slzc, *slxp, *slyp, *slzp;

    float *slxc_unrolled_x;
    float *slxc_unrolled_y;
    float *slxc_unrolled_z;
    float *slyc_unrolled_x;
    float *slyc_unrolled_y;
    float *slyc_unrolled_z;
    float *slzc_unrolled_x;
    float *slzc_unrolled_y;
    float *slzc_unrolled_z;
    float *slxp_unrolled_x;
    float *slxp_unrolled_y;
    float *slxp_unrolled_z;
    float *slyp_unrolled_x;
    float *slyp_unrolled_y;
    float *slyp_unrolled_z;
    float *slzp_unrolled_x;
    float *slzp_unrolled_y;
    float *slzp_unrolled_z;

    /** Device pointer
        External field values in secondary lattics
        Array of vectors, size: _plcsize
    **/
    cuvector *slxextf, *slyextf, *slzextf;

    float *slxextf_unrolled_x;
    float *slxextf_unrolled_y;
    float *slxextf_unrolled_z;
    float *slyextf_unrolled_x;
    float *slyextf_unrolled_y;
    float *slyextf_unrolled_z;
    float *slzextf_unrolled_x;
    float *slzextf_unrolled_y;
    float *slzextf_unrolled_z;

    /** Device pointer
        Volumes around each sl point for extf integration
        Array of vectors, size: _plcsize
    **/
    float *sxextfvol, *syextfvol, *szextfvol;

    /** Device pointer
        External field energy densities in secondary lattice, current and previous states
        Array of vectors, size: _plcsize
    **/
    float *sxfextfc, *sxfextfp, *syfextfc, *syfextfp, *szfextfc, *szfextfp;

    /** Device pointer
        Single easy axis
        Array of vectors, size: _plsize
    **/
    cuvector *slxsea, *slysea, *slzsea;

}; //struct slpointers

struct tlpointers {

    /** Device pointers
        Tetriary lattice (facet middles), current and previous states
        Arrays of vectors, sizes:
            xy - (_plcsize.x, _plcsize.y, _plcsize.z)
            xz - (_plcsize.x, _plcsize.y, _plcsize.z)
            yz - (_plcsize.x, _plcsize.y, _plcsize.z)
    **/
    cuvector *tlxyc, *tlxzc, *tlyzc,
            *tlxyp, *tlxzp, *tlyzp;

    float *tlxyc_unrolled_x;
    float *tlxyc_unrolled_y;
    float *tlxyc_unrolled_z;
    float *tlxzc_unrolled_x;
    float *tlxzc_unrolled_y;
    float *tlxzc_unrolled_z;
    float *tlyzc_unrolled_x;
    float *tlyzc_unrolled_y;
    float *tlyzc_unrolled_z;
    float *tlxyp_unrolled_x;
    float *tlxyp_unrolled_y;
    float *tlxyp_unrolled_z;
    float *tlxzp_unrolled_x;
    float *tlxzp_unrolled_y;
    float *tlxzp_unrolled_z;
    float *tlyzp_unrolled_x;
    float *tlyzp_unrolled_y;
    float *tlyzp_unrolled_z;

    /** Device pointer
        Misalignment level
        Arrays of float, size: _plcsize
    **/
    float *tlxymac, *tlxzmac, *tlyzmac, *tlxymap, *tlxzmap, *tlyzmap;

    /** Device pointer
        External field values in tetriary lattics
        Array of vectors, size: _plcsize
    **/
    cuvector *tlxyextf, *tlxzextf, *tlyzextf;

    float *tlxyextf_unrolled_x;
    float *tlxyextf_unrolled_y;
    float *tlxyextf_unrolled_z;
    float *tlxzextf_unrolled_x;
    float *tlxzextf_unrolled_y;
    float *tlxzextf_unrolled_z;
    float *tlyzextf_unrolled_x;
    float *tlyzextf_unrolled_y;
    float *tlyzextf_unrolled_z;

    /** Device pointer
        Volumes around each tl point for extf integration
        Array of vectors, size: _plcsize
    **/
    float *txyextfvol, *txzextfvol, *tyzextfvol;

    /** Device pointer
        External field energy densities in tetriary lattice, current and previous states
        Array of vectors, size: _plcsize
    **/
    float *txyfextfc, *txzfextfc, *tyzfextfc, *txyfextfp, *txzfextfp, *tyzfextfp;

    /** Device pointer
        Single easy axis
        Array of vectors, size: _plsize
    **/
    cuvector *tlxysea, *tlxzsea, *tlyzsea;

}; //struct tlpointers

struct cusizevector {
    size_t x, y, z;

    /** Default constructor
    **/
    cusizevector() throw() { x = y = z = 0; }


    /** Constructor
    */
    cusizevector(size_t nx, size_t ny, size_t nz) throw() {
        x = nx;
        y = ny;
        z = nz;
    }

    cusizevector operator+(const cusizevector &b) const throw() {
        return cusizevector(x + b.x, y + b.y, z + b.z);
    }

    cusizevector operator+(const size_t &c) const throw() {
        return cusizevector(x + c, y + c, z + c);
    }

    cusizevector operator*(const size_t &c) const throw() {
        return cusizevector(x * c, y * c, z * c);
    }

    /*cusizevector operator/(const size_t &c) const throw() - commented as there is no throw specification
	{
        / * catch c==0 case * /
        return cusizevector(x/c, y/c, z/c);
	}*/
};

inline int get_new_poffset(int i, int tick_x, int tick_y, int tick_z, int shift_x, int shift_y, int shift_z, cusizevector plsize)
{
    int part_qnx = (1 + tick_x + shift_x)/2;
    int part_qny = (1 + tick_y + shift_y)/2;
    int part_qnz = (1 + tick_z + shift_z)/2;

    int cubex = (1 + tick_x + shift_x) % 2, cubey = (1 + tick_y + shift_y) % 2, cubez = (1 + tick_z + shift_z) % 2;

    int cube_id = cubez * 4 + cubey * 2 + cubex;
    int cube_size = (plsize.x/2) * (plsize.y/2) * (plsize.z/2);

    return cube_size * cube_id + part_qnz * (plsize.y/2) * (plsize.x/2) + part_qny * (plsize.x/2) + part_qnx + i;
}

inline float arcavgXYZ(const float &v1x, const float &v1y, const float &v1z,
                       const float &v2x, const float &v2y, const float &v2z,
                       float *rx, float *ry, float *rz) {

    float v12 = v1x * v2x + v1y * v2y + v1z * v2z;
    float k12;

    float v2effx = v2x, v2effy = v2y, v2effz = v2z;
    if (v12 < 0.00) {
        v2effx *= -1.00;
        v2effy *= -1.00;
        v2effz *= -1.00;
        v12 *= -1.00;
    }

    if (v12 > 0.999999) {
        *rx = v1x;
        *ry = v1y;
        *rz = v1z;
    } else {
        k12 = sqrt(0.50 / (1.00 + v12));
        *rx = (v1x + v2effx) * k12;
        *ry = (v1y + v2effy) * k12;
        *rz = (v1z + v2effz) * k12;
    }
    return v12;
}

void InitSeq(float *x, cusizevector plsize, cusizevector half_plsize)
{
    for (size_t tick_x = 0; tick_x < 2; tick_x++) {
        for (size_t tick_y = 0; tick_y < 2; tick_y++) {
            for (size_t tick_z = 0; tick_z < 2; tick_z++) {
#pragma omp parallel for schedule(static)
                for (size_t i = 0; i < half_plsize.x * half_plsize.y * half_plsize.z; i++) {
                    size_t tz = i / (half_plsize.x * half_plsize.y);
                    size_t ty = (i - tz * half_plsize.x * half_plsize.y) / half_plsize.z;
                    size_t tx = (i - tz * half_plsize.x * half_plsize.y) % half_plsize.z;

                    if ((tx != (half_plsize.x - 1)) && (ty != (half_plsize.y - 1)) && (ty != (half_plsize.y - 1))) {
                        const int poffset = get_new_poffset(i, tick_x, tick_y, tick_z, 0, 0, 0, plsize);
                        x[poffset] = 1.0/double(i);
                    }
                }
            }
        }
    }
}

void InitRand(float *x, float *y, float *z, cusizevector plsize, cusizevector half_plsize)
{
    for (size_t tick_x = 0; tick_x < 2; tick_x++) {
        for (size_t tick_y = 0; tick_y < 2; tick_y++) {
            for (size_t tick_z = 0; tick_z < 2; tick_z++) {

#pragma omp parallel for schedule(static)
                for (size_t i = 0; i < half_plsize.x * half_plsize.y * half_plsize.z; i++) {
                    size_t tz = i / (half_plsize.x * half_plsize.y);
                    size_t ty = (i - tz * half_plsize.x * half_plsize.y) / half_plsize.z;
                    size_t tx = (i - tz * half_plsize.x * half_plsize.y) % half_plsize.z;

                    if ((tx != (half_plsize.x - 1)) && (ty != (half_plsize.y - 1)) && (ty != (half_plsize.y - 1))) {
                        const int poffset = get_new_poffset(i, tick_x, tick_y, tick_z, 0, 0, 0, plsize);
                        x[poffset] = 1.0/double(i);
                        y[poffset] = 1.0/double(i);
                        z[poffset] = 1.0/double(i);
                    }
                }
            }
        }
    }
}

void Init(tlpointers &tldata, slpointers &sldata, cusizevector plsize, cusizevector half_plsize)
{
    sldata.slxc_unrolled_x = (float *) malloc(sizeof(float) * plsize.x * plsize.y * plsize.z);
    sldata.slxc_unrolled_y = (float *) malloc(sizeof(float) * plsize.x * plsize.y * plsize.z);
    sldata.slxc_unrolled_z = (float *) malloc(sizeof(float) * plsize.x * plsize.y * plsize.z);

    InitRand(sldata.slxc_unrolled_x, sldata.slxc_unrolled_y, sldata.slxc_unrolled_z, plsize, half_plsize);

    sldata.slyc_unrolled_x = (float *) malloc(sizeof(float) * plsize.x * plsize.y * plsize.z);
    sldata.slyc_unrolled_y = (float *) malloc(sizeof(float) * plsize.x * plsize.y * plsize.z);
    sldata.slyc_unrolled_z = (float *) malloc(sizeof(float) * plsize.x * plsize.y * plsize.z);

    InitRand(sldata.slyc_unrolled_x, sldata.slyc_unrolled_y, sldata.slyc_unrolled_z, plsize, half_plsize);

    tldata.tlxyc_unrolled_x = (float *) malloc(sizeof(float) * plsize.x * plsize.y * plsize.z);
    tldata.tlxyc_unrolled_y = (float *) malloc(sizeof(float) * plsize.x * plsize.y * plsize.z);
    tldata.tlxyc_unrolled_z = (float *) malloc(sizeof(float) * plsize.x * plsize.y * plsize.z);

    InitRand(tldata.tlxyc_unrolled_x, tldata.tlxyc_unrolled_y, tldata.tlxyc_unrolled_z, plsize, half_plsize);

    tldata.tlxymac = (float *) malloc(sizeof(float) * plsize.x * plsize.y * plsize.z);

    InitSeq(tldata.tlxymac, plsize, half_plsize);
}

void Kernel(cusizevector plsize,
            cusizevector half_plsize,
            cusizevector tick,
            slpointers *sldata, tlpointers *tldata)
{
    tick.x = rand()%2;
    tick.y = rand()%2;
    tick.z = rand()%2;

    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < half_plsize.x * half_plsize.y * half_plsize.z; i++) {
        size_t tz = i / (half_plsize.x * half_plsize.y);
        size_t ty = (i - tz * half_plsize.x * half_plsize.y) / half_plsize.z;
        size_t tx = (i - tz * half_plsize.x * half_plsize.y) % half_plsize.z;
        if ((tx != (half_plsize.x - 1)) && (ty != (half_plsize.y - 1)) && (ty != (half_plsize.y - 1))) {
            const int poffset = get_new_poffset(i, tick.x, tick.y, tick.z, 0, 0, 0, plsize);

            const int poffsetxp = get_new_poffset(i, tick.x, tick.y, tick.z, +1, 0, 0, plsize);

            const int poffsetyp = get_new_poffset(i, tick.x, tick.y, tick.z, 0, +1, 0, plsize);


            float avg12;
            float avg1_x, avg1_y, avg1_z;
            float avg2_x, avg2_y, avg2_z;
            // 3.4.1. tlxy points
            arcavgXYZ(sldata->slyc_unrolled_x[poffset], sldata->slyc_unrolled_y[poffset],
                      sldata->slyc_unrolled_z[poffset],
                      sldata->slyc_unrolled_x[poffsetxp], sldata->slyc_unrolled_y[poffsetxp],
                      sldata->slyc_unrolled_z[poffsetxp], &avg1_x, &avg1_y, &avg1_z);
            arcavgXYZ(sldata->slxc_unrolled_x[poffset], sldata->slxc_unrolled_y[poffset],
                      sldata->slxc_unrolled_z[poffset],
                      sldata->slxc_unrolled_x[poffsetyp], sldata->slxc_unrolled_y[poffsetyp],
                      sldata->slxc_unrolled_z[poffsetyp], &avg2_x, &avg2_y, &avg2_z);
            avg12 = arcavgXYZ(avg1_x, avg1_y, avg1_z, avg2_x, avg2_y, avg2_z,
                              &tldata->tlxyc_unrolled_x[poffset],
                              &tldata->tlxyc_unrolled_y[poffset], &tldata->tlxyc_unrolled_z[poffset]);

            tldata->tlxymac[poffset] = 1.00 - avg12 * avg12;
        }
    }
}
