#include "MergeFile.h"
#include "omdParserApi.h"
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <cctype>

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

    // Go through the object model(s), and load them into our id-based maps
    for(int i = 0; i < omdFile_.object_model_size(); ++i)
    {
        omdParser::ObjectModel* om = omdFile_.mutable_object_model(i);
        loadMaps(om);    
    }

    // Ok.  So now our maps are loaded with the ID's of the classes and interactions.

    // We need to now go through them *again*, to get their fully qualified names
    // (which is what their "real" unique identifier is)


    // Go through our classes
    for(intClassMap_t::iterator iter = classMap.begin();
        iter != classMap.end();
        ++iter)
    {
        strClassMap_[getName(*iter->second)] = iter->second->mutable_class_();

        // test our scoring function
        std::cout << iter->second->class_().name().value() << ": " << getScore(iter->second->class_()) << std::endl;
    }


    // Go through our interactions
    for(intInterMap_t::iterator iter = intMap.begin();
        iter != intMap.end();
        ++iter)
    {
        strInterMap_[getName(*iter->second)] = iter->second->mutable_interaction();
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
            

    // Go through the components, and stick pointers to them in the appropriate maps. 
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


int scoreDescription(const std::string& desc)
{
    int ret = 0;

    std::string DESC = boost::to_upper_copy(desc);

    // Did we find "placeholder" ?  If so, we probably aren't the real item
    if(std::string::npos != DESC.find("PLACEHOLDER"))
    {
        ret -= 50;
    }

    // Let's find all the words in the string
    bool found_letter = false;
    for(unsigned int i = 0; i < DESC.length(); ++i)
    {
        if(!isspace(DESC[i]))
        {
            found_letter = true;
        }

        // So, we found a space.  Did we find a previous letter?
        else if(found_letter)
        {
            // Reset finding a letter
            found_letter = false;
            ret += 1;
        }
    }

    return ret;
}

int MergeFile::getScore(const omdParser::Class& cls)
{
    int ret = 0;

    if(cls.has_mom_class())
    {
        ret++;
    }

    if(cls.has_ps_capabilities())
    {
        ret++;
    }

    if(cls.has_description())
    {
        ret++;
        ret += scoreDescription(cls.description().value());
    }

    return ret;
}


void MergeFile::merge(const MergeFile& rhs)
{
    omdParser::ObjectModel dest;
    mergeClass(dest, rhs);
}

void MergeFile::mergeClass(omdParser::ObjectModel& dest, const MergeFile& rhs)
{
    // Iterate through all of the classes in the lhs
    for(strClassMap_t::const_iterator rhs_iter = rhs.strClassMap_.begin();
        rhs_iter != rhs.strClassMap_.end();
        ++rhs_iter)
    {
        // Does the class exist in both the left and right sides?
        strClassMap_t::iterator lhs_iter = strClassMap_.find(rhs_iter->first);
        if(lhs_iter != strClassMap_.end())
        {
            // Score both classes
            int lhs_score = getScore(*(lhs_iter->second));
            int rhs_score = getScore(*(rhs_iter->second));

            // Take the lhs
            if(lhs_score > rhs_score)
            {
//                dest.a
            }

            // Take the rhs
            else if(rhs_score > lhs_score)
            {
//                *(lhs_iter->second)->
            }

            // Some default behavior here...
            else
            {
            }
        }
    }
}