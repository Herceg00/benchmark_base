#include <string>

using std::string;

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

template <typename AT>
void InitRand(AT *x, cusizevector plsize, cusizevector half_plsize)
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

template <typename AT>
void Init(AT *in_data, AT *out_data, cusizevector plsize, cusizevector half_plsize)
{
    InitRand(in_data, plsize, half_plsize);
    InitRand(out_data, plsize, half_plsize);
}

template <typename AT>
void Kernel_splitted(cusizevector plsize,
                     cusizevector half_plsize,
                     cusizevector tick,
                     AT *in_data, AT *out_data)
{
    tick.x = rand()%2;
    tick.y = rand()%2;
    tick.z = rand()%2;

    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < half_plsize.x * half_plsize.y * half_plsize.z; i++)
    {
        size_t tz = i / (half_plsize.x * half_plsize.y);
        size_t ty = (i - tz * half_plsize.x * half_plsize.y) / half_plsize.z;
        size_t tx = (i - tz * half_plsize.x * half_plsize.y) % half_plsize.z;
        if ((tx != (half_plsize.x - 1)) && (ty != (half_plsize.y - 1)) && (ty != (half_plsize.y - 1)))
        {
            const int poffset = get_new_poffset(i, tick.x, tick.y, tick.z, 0, 0, 0, plsize);

            const int poffsetxp = get_new_poffset(i, tick.x, tick.y, tick.z, +1, 0, 0, plsize);
            const int poffsetxm = get_new_poffset(i, tick.x, tick.y, tick.z, -1, 0, 0, plsize);
            const int poffsetyp = get_new_poffset(i, tick.x, tick.y, tick.z, 0, +1, 0, plsize);
            const int poffsetym = get_new_poffset(i, tick.x, tick.y, tick.z, 0, -1, 0, plsize);
            const int poffsetzp = get_new_poffset(i, tick.x, tick.y, tick.z, 0, 0, 1, plsize);
            const int poffsetzm = get_new_poffset(i, tick.x, tick.y, tick.z, 0, 0, -1, plsize);

            AT sum = 0;
            sum += in_data[poffset];
            sum += in_data[poffsetxp];
            sum += in_data[poffsetxm];
            sum += in_data[poffsetyp];
            sum += in_data[poffsetym];
            sum += in_data[poffsetzp];
            sum += in_data[poffsetzm];

            out_data[poffset] = sum;
        }
    }
}

template <typename AT>
void Kernel_original(cusizevector plsize,
                     cusizevector half_plsize,
                     cusizevector tick,
                     AT *in_data, AT *out_data)
{
    tick.x = rand()%2;
    tick.y = rand()%2;
    tick.z = rand()%2;

    #pragma omp parallel for schedule(static)
    for (size_t tz = 0; tz < (half_plsize.z); tz++)
    {
        for (size_t ty = 0; ty < (half_plsize.y); ty++)
        {
            for (size_t tx = 0; tx < (half_plsize.x); tx++)
            {
                const int pnx=tx*2+tick.x+1;
                const int pny=ty*2+tick.y+1;
                const int pnz=tz*2+tick.z+1;

                size_t poffset=pnz*plsize.y*plsize.x+pny*plsize.x+pnx;

                size_t poffsetxm=(pnz)*plsize.y*plsize.x+pny*plsize.x+pnx-1,
                        poffsetxp=(pnz)*plsize.y*plsize.x+pny*plsize.x+pnx+1,
                        poffsetym=pnz*plsize.y*plsize.x+(pny-1)*plsize.x+pnx,
                        poffsetyp=pnz*plsize.y*plsize.x+(pny+1)*plsize.x+pnx,
                        poffsetzm=(pnz-1)*plsize.y*plsize.x+pny*plsize.x+(pnx),
                        poffsetzp=(pnz+1)*plsize.y*plsize.x+pny*plsize.x+(pnx);

                AT sum = 0;
                sum += in_data[poffset];
                sum += in_data[poffsetxp];
                sum += in_data[poffsetxm];
                sum += in_data[poffsetyp];
                sum += in_data[poffsetym];
                sum += in_data[poffsetzp];
                sum += in_data[poffsetzm];

                out_data[poffset] = sum;
            }
        }
    }
}

template <typename AT>
void Kernel(int core_type,
            cusizevector plsize,
            cusizevector half_plsize,
            cusizevector tick,
            AT *in_data, AT *out_data)
{
    switch (core_type)
    {
        case  0:
            Kernel_original(plsize, half_plsize, tick, in_data, out_data);
            break;
        case  1:
            Kernel_splitted(plsize, half_plsize, tick, in_data, out_data);
            break;

        default: fprintf(stderr, "Wrong core type of lc_kernel_generic!\n");
    }
}