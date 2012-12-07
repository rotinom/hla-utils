#pragma once

#include "boost/shared_ptr.hpp"
#include "omdParser.pb.h"

#include <string>


class MergeFile
{
public:
    

    MergeFile();
    ~MergeFile();

    bool parseFile(const std::string& fileName);

private:

    typedef std::map<int, omdParser::OmdComponent*> intClassMap_t;
    typedef std::map<int, omdParser::OmdComponent*> intInterMap_t;
    typedef std::map<int, omdParser::Note*> intNoteMap_t;
    typedef std::map<std::string, omdParser::EnumeratedDataType*> strEdtMap_t;
    typedef std::map<std::string, omdParser::ComplexDataType*> strCdtMap_t;
    typedef std::map<std::string, omdParser::RoutingSpace*> rsMap_t;

    intClassMap_t classMap;
    intInterMap_t intMap;
    intNoteMap_t  noteMap;
    strEdtMap_t   edtMap;
    strCdtMap_t   cdtMap;
    rsMap_t       rsMap;

    void loadMaps(omdParser::ObjectModel* om);

    std::string getName(const omdParser::OmdComponent& item);
    void getName(const omdParser::Class& c, std::stringstream& ss);
    void getName(const omdParser::Interaction& i, std::stringstream& ss);

    omdParser::OmdFile omdFile_;
};
