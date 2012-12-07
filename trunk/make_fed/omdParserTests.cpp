/**
 * \file 
 *
 * \brief 
 *
 * Tests 
*/

#include "OMDParser.h"

int main(int argc, char*argv[])
{
    omd_parser::OMDParser omdp;
    omd_parser::OMDParser::parseInfo pi;
    pi = omdp.parse("../f18.omd");

    if(!pi.full)
    {
        std::cout << "Failure!!" << std::endl;

        return 1;
    }

    omd_parser::omdCollection::shared_ptr res = omdp.getRoot();


    unsigned int foo = sizeof(res);

    std::cout << toString(res);

}
