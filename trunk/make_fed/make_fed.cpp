#include "omdParserApi.h"
#include "omdParser.pb.h"
#include "boost/shared_ptr.hpp"
#include "boost/format.hpp"

#include <iostream>
#include <map>
#include <list>

typedef std::map<unsigned int, std::string> intStrMap_t;
intStrMap_t indentMap;

const unsigned int indentStepSize = 1;

// Lookup map for a class' PS Capabilities;
typedef std::map<omdParser::Class_PSCapabilities, std::string> psStringMap_t;
psStringMap_t psStringMap;

// Declare lists of our objects 


typedef std::list<omdParser::RoutingSpace *> spacePtrList_t;






struct ClassStruct;
struct InteractionStruct;




typedef std::list<ClassStruct*> classPtrList_t;
typedef std::list<InteractionStruct*> interPtrList_t;


spacePtrList_t spaceList;
interPtrList_t interList;
classPtrList_t classList;

struct ClassStruct
{
    omdParser::Class* cls;
    classPtrList_t children;
    ClassStruct* parent;

    ClassStruct():
        cls(NULL),
        parent(NULL)
    {
    }

    ClassStruct(omdParser::Class* c):
        cls(c),
        parent(NULL)
    {
    }

    ClassStruct(const ClassStruct& c):
        cls(c.cls),
        parent(c.parent),
        children(c.children)
    {
    }
};



struct InteractionStruct
{
    omdParser::Interaction* inter;
    interPtrList_t children;
    InteractionStruct* parent;

    InteractionStruct():
        inter(NULL),
        parent(NULL)
        {
        }

    InteractionStruct(omdParser::Interaction* i):
        inter(i),
        parent(NULL)
    {
    }

    InteractionStruct(const InteractionStruct& i):
        inter(i.inter),
        parent(i.parent)
    {
    }
};



typedef std::map<const int, ClassStruct> classMap_t;
typedef std::map<const int, InteractionStruct> interMap_t;


classMap_t classMap;
interMap_t interMap;






void fillMaps(omdParser::ObjectModel& model)
{
    // We need to fill up our lookup maps
    for(int i = 0; i < model.omd_component_size(); i++)
    {
        omdParser::OmdComponent *obj = model.mutable_omd_component(i);

        switch(obj->type())
        {
            case omdParser::OmdComponent_ComponentType_class_type:
            {
                // make a class struct, and add it to the map
                ClassStruct cs(obj->mutable_class_());
                classMap[cs.cls->id().value()] = cs;
                break;
            }
            case omdParser::OmdComponent_ComponentType_interaction_type:
            {
                InteractionStruct is(obj->mutable_interaction());
                interMap[is.inter->id().value()] = is;
                break;
            }
            //case omdParser::OmdComponent_ComponentType_route_space_type:
            //{
            //    spaceList.push_back(obj.routing_space());
            //    break;
            //}
        }
    }

    // Go through the map now, and set the parent/child relationships
    for(classMap_t::iterator iter = classMap.begin();
        iter != classMap.end();
        ++iter)
    {
        ClassStruct& cs = iter->second;

        if(cs.cls->has_superclass())
        {
            // Add the parent to the child and child to parent
            cs.parent = &classMap[cs.cls->superclass().value()];
            cs.parent->children.push_back(&cs);
        }
        else
        {
            // Push the roots onto our list
            classList.push_back(&cs);
        }
    }

    // Go through the interaction map and set the parent/child relationships
    for(interMap_t::iterator iter = interMap.begin();
        iter != interMap.end();
        ++iter)
    {
        InteractionStruct& is = iter->second;

        if(is.inter->has_super_interaction())
        {
            is.parent = &interMap[is.inter->super_interaction().value()];
            is.parent->children.push_back(&is);
        }
        else
        {
            interList.push_back(&is);
        }

    }
}

//void printInteraction(InteractionStruct* is, int level=1)
//{
//    // Print ourselves
//    std::cout << 
////        indentMap[level] << 
//        " (class " << cs->cls->name().value() << std::endl;
//
//    // Print our attributes
//    for(int i = 0; i < cs->cls->attributes_size(); ++i)
//    {
//        std::cout << 
////            indentMap[level]    << 
//            "     (attribute "   << 
//                cs->cls->attributes(i).name().value() << " "    <<
//                cs->cls->attributes(i).delivery_category().value() << " " <<
//                cs->cls->attributes(i).message_ordering().value()   <<
//            ")" <<
//            std::endl;
//    }
//
//    // Print our child classes
//    for(classPtrList_t::iterator iter = cs->children.begin();
//        iter != cs->children.end();
//        ++iter)
//    {
//        printClass((*iter), level+1);
//    }
//
//    std::cout << 
////        indentMap[level] << 
//        " )" << std::endl;
//
//}
//
//    for(classPtrList_t::iterator iter = classList.begin();
//        iter != classList.end();
//        ++iter)
//    {
//        printClass(*iter);
//    }



void printClass(ClassStruct* cs, int level=1)
{
    // Print ourselves
    std::cout << 
//        indentMap[level] << 
        " (class " << cs->cls->name().value() << std::endl;

    // Print our attributes
    for(int i = 0; i < cs->cls->attributes_size(); ++i)
    {
        std::cout << 
//            indentMap[level]    << 
            "     (attribute "   << 
                cs->cls->attributes(i).name().value() << " "    <<
                cs->cls->attributes(i).delivery_category().value() << " " <<
                cs->cls->attributes(i).message_ordering().value()   <<
            ")" <<
            std::endl;
    }

    // Print our child classes
    for(classPtrList_t::iterator iter = cs->children.begin();
        iter != cs->children.end();
        ++iter)
    {
        printClass((*iter), level+1);
    }

    std::cout << 
//        indentMap[level] << 
        " )" << std::endl;

}


void printFile(omdParser::OmdFile * file)
{
    std::cout << "(FED" << std::endl;


    for(int i = 0; i < file->object_model_size(); i++)
    {
        std::cout << "(Federation " << file->object_model(i).fed_name().value() << ")" << std::endl;
        std::cout << "(FEDversion v" << file->object_model(i).mom_version().value() << ")" << std::endl;
        fillMaps(*file->mutable_object_model(i));
    }

    std::cout << " (spaces " << std::endl;
    std::cout << " )" << std::endl;

    std::cout << " (objects " << std::endl;
    std::cout << "    (class ObjectRoot " << std::endl;
    std::cout << "       (attribute privilegeToDelete  reliable timestamp)" << std::endl;
    std::cout << "       (class RTIprivate)" << std::endl;
    for(classPtrList_t::iterator iter = classList.begin();
        iter != classList.end();
        ++iter)
    {
        printClass(*iter);
    }

    for(interPtrList_t::iterator iter = interList.begin();
        iter != interList.end();
        ++iter)
    {
        //printInteraction(*iter);
    }

    std::cout << ")" << std::endl;
}


int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "Please specify file to beautify on the command line..." << std::endl;
        exit(-1);
    }

    //omdParser::raw::setDebug(true);

    bool stuff = omdParser::raw::readFile(argv[1]);

    int size = omdParser::raw::getDataSize();

    boost::shared_ptr<unsigned char> data(new unsigned char[size]);
    
    size = omdParser::raw::getData(data.get(), size);

    if(0 == size)
    {
        std::cout << "Failed to parse the file correctly" << std::endl;
        exit(-1);
    }

    omdParser::OmdFile omdFile;
    omdFile.ParseFromArray(data.get(), size);


    char one_indent[indentStepSize+1] = {NULL};
    memset(one_indent, ' ', indentStepSize);
    one_indent[indentStepSize] = '\0';

    // Create a map of indentations.
    std::string indentString;
    for(int i = 0; i < 10; i++)
    {      
        indentMap[indentStepSize*i] = indentString;
        indentString.append(one_indent);
    }


    printFile(&omdFile);

	return 0;
}

