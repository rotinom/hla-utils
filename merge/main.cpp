#include "omdParserApi.h"
#include "omdParser.pb.h"
#include "boost/shared_ptr.hpp"

#include <iostream>

#include "MergeFile.h"

int main(int argc, char* argv[])
{
    //if(argc != 2)
    //{
    //    std::cout << "Please specify file to beautify on the command line..." << std::endl;
    //    exit(-1);
    //}

    //omdParser::raw::setDebug(true);

    MergeFile m;
    m.parseFile("test_files/RPR-FOM.omd");

	return 0;
}