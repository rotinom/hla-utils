#include "omdParserApi.h"
#include "omdParser.pb.h"
#include "boost/shared_ptr.hpp"

#include <iostream>

#include "MergeFile.h"

int main(int argc, char* argv[])
{
    MergeFile m1;
    m1.parseFile("../test_files/merge1.omd");
    
    MergeFile m2;
    m2.parseFile("../test_files/merge2.omd");

    m1.merge(m2);
	return 0;
}