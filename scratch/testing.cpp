#include "myco/myco.h"

class Testing : public myco::Application {

};

MYCO_RUN(Testing, (myco::WindowOpenParams{
        .title = "Testing",
        .size = {800, 600},
        .flags = myco::WindowFlags::centered
}))
