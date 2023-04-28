#ifndef MYCO_MYCO_H
#define MYCO_MYCO_H

#include "myco/core/engine.h"

#include "myco/util/log.h"
#include "myco/util/rnd.h"
#include "myco/util/sops.h"

#include "fmt/format.h"

#define MYCO_RUN(application, window_open_params)  \
    int main(int, char *[]) {                      \
        auto e = std::make_shared<myco::Engine>(); \
        e->run<application>(window_open_params);   \
    }

#endif//MYCO_MYCO_H
