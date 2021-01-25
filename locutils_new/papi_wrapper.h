#ifndef PAPI_WRAPPER_H
#define PAPI_WRAPPER_H

#include <stdio.h>
#include <papi.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <stdint.h>
#include <iostream>
#include <sstream>

#include "utils.h"

typedef long long int counter_type;

namespace locality {

    struct SPapiSet
    {
        int event_set;

        std::vector<std::string> names;
        int count;

        SPapiSet& Add(std::string name, int id)
        {
            int papi_error = PAPI_OK;

            names.push_back(name);

            if((papi_error = PAPI_add_event(event_set, id)) != PAPI_OK)
                throw std::runtime_error("PAPI add error: " + utils::ToString(papi_error) + " in "+ name);

            count++;

            return *this;
        }

        SPapiSet():
                event_set(PAPI_NULL),
                count(0)
        {
            int papi_error = PAPI_OK;

            if((papi_error = PAPI_create_eventset(&event_set)) != PAPI_OK)
            {
                throw std::runtime_error("PAPI event create error: " + utils::ToString(papi_error));
            }
        }
    };

    class PapiController
    {
    public:
        static void Start(const SPapiSet& set)
        {
            int papi_error = PAPI_OK;

            if((papi_error = PAPI_start(set.event_set)) != PAPI_OK)
            {
                throw std::runtime_error("PAPI library start error: " + utils::ToString(papi_error));
            }
        }

        static counter_type* Stop(const SPapiSet& set)
        {
            int papi_error = PAPI_OK;

            counter_type* values = new counter_type[set.count];

            if((papi_error = PAPI_stop(set.event_set, values)) != PAPI_OK)
            {
                throw std::runtime_error("PAPI library stop error: " + utils::ToString(papi_error));
            }

            return values;
        }

        static counter_type* Read(const SPapiSet& set)
        {
            int papi_error = PAPI_OK;

            counter_type* values = new counter_type[set.count];

            if((papi_error = PAPI_read(set.event_set, values)) != PAPI_OK)
            {
                throw std::runtime_error("PAPI library read error: " + utils::ToString(papi_error));
            }

            return values;
        }

        static void Init()
        {
            int retval = PAPI_library_init(PAPI_VER_CURRENT);

            if(retval != PAPI_VER_CURRENT)
            {
                std::cerr << "papi error code: " << retval << std::endl;
                throw std::runtime_error("PAPI library init error");
            }

            PAPI_thread_init(locality::utils::GetThreadID);
        }
    };

} /*namespace*/

#endif /*PAPI_WRAPPER_H*/