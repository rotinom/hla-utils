#pragma once

#include <map>
#include <vector>
#include "omdParser.pb.h"

namespace omdParser
{

class omdNormalizer
{
public:
    omdNormalizer();
    ~omdNormalizer();

    bool parse(const void* protoBuffer, const int protoBufferSize);

private:
    typedef std::map<int, omdParser::Class> ClassMap_t;
    ClassMap_t classMap_;

    void parseCollection(const omdParser::CollectionNode& collection);

    void parseRoot(const omdParser::CollectionNode& collection);

    omdParser::OmdFile omdFile_;
};

}