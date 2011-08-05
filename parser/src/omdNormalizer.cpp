#include "omdNormalizer.h"

#include "omdParser.pb.h"

namespace omdParser
{

    omdNormalizer::omdNormalizer()
    {
    }

    omdNormalizer::~omdNormalizer()
    {
    }

    bool omdNormalizer::parse(const void* protoBuffer, const int protoBufferSize)
    {
        // Suck in the "raw" protocol buffer 
        omdParser::CollectionNode rawOmdFile;
        bool results = rawOmdFile.ParseFromArray(protoBuffer, protoBufferSize);



        omdParser::OmdFile omdFile;

        parseCollection(rawOmdFile);

        return results;
    }

    void omdNormalizer::parseCollection(const omdParser::CollectionNode& collection)
    {
        switch(collection.type())
        {
            case RootEnum:
            {
                parseRoot(collection);
                break;
            }
            case ObjectModelEnum:
            {
                break;
            }
            case ClassEnum:
            {
                break;
            }
            case AttributeEnum:
            {
                break;
            }
            case InteractionEnum:
            {
                break;
            }
            case ParameterEnum:
            {
                break;
            }
            case CDTEnum:
            {
                break;
            }
            case ComplexComponentEnum:
            {
                break;
            }
            case EDTEnum:
            {
                break;
            }
            case EnumerationEnum:
            {
                break;
            }
            case RoutingSpaceEnum:
            {
                break;
            }
            case DimensionEnum:
            {
                break;
            }
            case NoteEnum:
            {
                break;
            }
            default:
            {
                break;
            }
        }

        for(int i = 0; i < collection.collection_size(); i++)
        {
            parseCollection(collection.collection(i));
        }
    }

    void omdNormalizer::parseRoot(const omdParser::CollectionNode& collection)
    {
        for(int i = 0; i < collection.element_size(); i++)
        {
            const ElementNode& elem = collection.element(i);


        }
    }

}
