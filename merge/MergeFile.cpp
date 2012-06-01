#include "MergeFile.h"
#include "omdParserApi.h"
#include <sstream>


MergeFile::MergeFile(void)
{
}


MergeFile::~MergeFile(void)
{
}

bool MergeFile::parseFile(const std::string& fileName)
{
    // Parse the file, and get it 
    bool stuff = omdParser::raw::readFile(fileName.c_str());

    int size = omdParser::raw::getDataSize();
    if(0 == size)
    {
        std::cout << "Failed to parse the file correctly" << std::endl;
        return false;
    }


    boost::shared_ptr<unsigned char> data(new unsigned char[size]);    
    size = omdParser::raw::getData(data.get(), size);

    // Get the OMD file locally
    omdFile_.ParseFromArray(data.get(), size);

    for(int i = 0; i < omdFile_.object_model_size(); ++i)
    {
        omdParser::ObjectModel* om = omdFile_.mutable_object_model(i);

        // Load up our lookup maps
        loadMaps(om);    
    }


    for(intClassMap_t::iterator iter = classMap.begin();
        iter != classMap.end();
        ++iter)
    {
        std::cout <<  getName(*(iter->second)) << std::endl;
    }


    for(intInterMap_t::iterator iter = intMap.begin();
        iter != intMap.end();
        ++iter)
    {
        std::cout << getName(*(iter->second)) << std::endl;
    }

    return true;
}

void MergeFile::loadMaps(omdParser::ObjectModel* om)
{
    classMap.clear();
    intMap.clear();
    noteMap.clear();
    edtMap.clear();
    cdtMap.clear();
    rsMap.clear();
            

    // Go through the components, and stick pointers to them in the right maps. 
    for(int j = 0; j < om->omd_component_size(); ++j)
    {
        omdParser::OmdComponent* oc = om->mutable_omd_component(j);

        // Place the components in their corresponding maps for lookup
        switch(oc->type())
        {
            case omdParser::OmdComponent_ComponentType_class_type:
            {
                classMap[oc->class_().id().value()] = oc;
                break;
            }
            case omdParser::OmdComponent_ComponentType_interaction_type:
            {
                intMap[oc->interaction().id().value()] = oc;
                break;
            }
            case omdParser::OmdComponent_ComponentType_cdt_type:
            {
                cdtMap[oc->complex_data_type().name().value()] = oc->mutable_complex_data_type();
                break;
            }
            case omdParser::OmdComponent_ComponentType_edt_type:
            {
                edtMap[oc->enumerated_data_type().name().value()] = oc->mutable_enumerated_data_type();
                break;
            }
            case omdParser::OmdComponent_ComponentType_route_space_type:
            {
                rsMap[oc->routing_space().name().value()] = oc->mutable_routing_space();
                break;
            }
            case omdParser::OmdComponent_ComponentType_note_type:
            {
                noteMap[oc->note().note_number().value()] = oc->mutable_note();
                break;
            }
            default:
            {
                assert(false);
                break;
            }
        }
    }
}

void MergeFile::getName(const omdParser::Interaction& i, std::stringstream& ss)
{
    // Add our parent...
    if(i.has_super_interaction())
    {
        getName(intMap[i.super_interaction().value()]->interaction(), ss);
        ss << ".";
    }
    
    // Add ourselves..
    ss << i.name().value();
}

void MergeFile::getName(const omdParser::Class& c, std::stringstream& ss)
{
    // Add our parent...
    if(c.has_superclass())
    {
        getName(classMap[c.superclass().value()]->class_(), ss);
        ss << ".";
    }
    
    // Add ourselves..
    ss << c.name().value();
}

std::string MergeFile::getName(const omdParser::OmdComponent& item)
{
    std::stringstream ss;

    // Place the components in their corresponding maps for lookup
    switch(item.type())
    {
        case omdParser::OmdComponent_ComponentType_class_type:
        {
            ss << "class.";
            getName(item.class_(), ss);
            break;
        }
        case omdParser::OmdComponent_ComponentType_interaction_type:
        {
            ss << "interaction.";
            getName(item.interaction(), ss);
            break;
        }
        case omdParser::OmdComponent_ComponentType_cdt_type:
        {
            ss << "cdt." << item.complex_data_type().name().value() << std::endl;
            break;
        }
        case omdParser::OmdComponent_ComponentType_edt_type:
        {
            ss << "edt." << item.enumerated_data_type().name().value() << std::endl;
            break;
        }
        case omdParser::OmdComponent_ComponentType_route_space_type:
        {
            ss << "routing_space." << item.routing_space().name().value() << std::endl;
            break;
        }
        case omdParser::OmdComponent_ComponentType_note_type:
        {
            ss << "note." << item.note().note_number().value() << std::endl;
            break;
        }
        default:
        {
            assert(false);
            break;
        }
    }

    return ss.str();
}