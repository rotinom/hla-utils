#include "omdParserApi.h"
#include "omdParser.pb.h"
#include "boost/shared_ptr.hpp"
#include "boost/format.hpp"

#include <iostream>
#include <map>

typedef std::map<unsigned int, std::string> intStrMap_t;
intStrMap_t indentMap;

const unsigned int indentStepSize = 2;

// Lookup map for a class' PS Capabilities;
typedef std::map<omdParser::Class_PSCapabilities, std::string> psStringMap_t;
psStringMap_t psStringMap;



void printElement(const omdParser::ElementNode * element, const unsigned int indent=0)
{
    std::string indentString = indentMap[indent];

    std::string notes;
    if(element->note_size() > 0)
    {
        notes += " [";
        
        unsigned int numNotes = element->note_size();
        for(unsigned int i = 0; i < numNotes; i++)
        {
            // This is a safer, cross-platform alternative to sprintf.
            // No, I'm not terribly thrilled with the syntax.  *shrugs*
            notes += boost::str(boost::format("%i") % element->note(i));

            // Don't put a comma after the last note
            if(i < numNotes-1)
            {
                notes += ",";
            }
        }

        notes += "]";
    }
	std::cout << indentString << "(" << element->key() << " " << element->value() << notes << ")";
}

void printCollection(const omdParser::CollectionNode * coll, const unsigned int indent=0)
{
    std::string indentString = indentMap[indent];
    std::string prevIndentString;
    
	// If we are the first node, then set our previous indent to
	// the current indentation level
    if(0 == indent)
    {
        prevIndentString = indentMap[0];
    }
    else
    {
        prevIndentString = indentMap[indent-indentStepSize];
    }

	// Print out the node type
    if(omdParser::RootEnum != coll->type())
    {
        std::string strType;
        switch(coll->type())
        {
            case omdParser::ClassEnum:
            {
                strType = "Class";
                break;
            }
            case omdParser::AttributeEnum:
            {
                strType = "Attribute";
                break;
            }
            case omdParser::InteractionEnum:
            {
                strType = "Interaction";
                break;
            }
            case omdParser::ParameterEnum:
            {
                strType = "Parameter";
                break;
            }
            case omdParser::CDTEnum:
            {
                strType = "ComplexDataType";
                break;
            }
            case omdParser::ComplexComponentEnum:
            {
                strType = "ComplexComponent";
                break;
            }
            case omdParser::EDTEnum:
            {
                strType = "EnumeratedDataType";
                break;
            }
            case omdParser::EnumerationEnum:
            {
                strType = "Enumeration";
                break;
            }
            case omdParser::RoutingSpaceEnum:
            {
                strType = "RoutingSpace";
                break;
            }
            case omdParser::DimensionEnum:
            {
                strType = "Dimension";
                break;
            }
            case omdParser::NoteEnum:
            {
                strType = "Note";
                break;
            }
            case omdParser::ObjectModelEnum:
            {
                strType = "ObjectModel";
                break;
            }
            default:
            {
                strType = "UNKNOWN";
                break;
            }
        }
        std::cout << prevIndentString << "(" << strType << " ";
    }

	// Print out each element in this collection
    unsigned int size = coll->element_size();
    for(unsigned int i = 0; i < size; i++)
    {
        const omdParser::ElementNode* element = &coll->element(i);
        if(omdParser::RootEnum != coll->type() && i == 0)
        {
            printElement(element, 0);
        }
        else
        {
            printElement(element, indent);
        }

        // This whole block of code, is due to the display of the
        // official RPR-FOM.  Notes and Enumerations are displayed
        // inconsistantly and what not.
        bool lastItem = (i == (size-1));
        if(lastItem && (omdParser::NoteEnum == coll->type()))
        {
            continue;
        }
        else if(lastItem && (omdParser::EnumerationEnum == coll->type()))
        {
            continue;
        }
        std::cout << std::endl;
    }

	// Print out any child collections
    for(int i = 0; i < coll->collection_size(); i++)
    {
        printCollection(&coll->collection(i), indent+indentStepSize);
        
    }
    
    // For notes, there is an extra endline.  This formatting is based on
    // the RPR-FOM.  This whole thing ought to be a bit more configurable.
    if(omdParser::NoteEnum == coll->type())
    {
        std::cout << ")" << std::endl;
        std::cout << std::endl;
    }
    else if(omdParser::RootEnum != coll->type())
    {
        std::cout << prevIndentString << ")" << std::endl;
    }
}

std::string getNoteString(const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >& notes)
{
    if(0 == notes.size())
    {
        return "";
    }

    std::string retValue = " [";
    for(int i = 0; i < notes.size(); i++)
    {
        retValue += boost::str(boost::format("%i") % notes.Get(i));

        if(i != (notes.size() - 1))
        {
            retValue += ", ";
        }
    }
    retValue += "]";

    return retValue;
}

void printEDT(const omdParser::EnumeratedDataType & edt)
{
    std::cout << boost::str(boost::format("  (EnumeratedDataType (Name \"%s\"%s)\n") % edt.name().value() % getNoteString(edt.name().notes()));

    if(edt.has_description())
    {
        std::cout << boost::str(boost::format("        (Description \"%s\")\n") % edt.description().value());
    }
    if(edt.has_mom_data_type())
    {
        std::cout << boost::str(boost::format("        (MOMEnumeratedDataType %s)\n") % edt.mom_data_type().value());
    }
    if(edt.has_auto_sequence())
    {
        std::cout << boost::str(boost::format("        (AutoSequence %s)\n") % (edt.auto_sequence().value() ? "Yes" : "No") );
    }
    if(edt.has_start_value())
    {
        std::cout << boost::str(boost::format("        (StartValue %s)\n") % edt.start_value().value());
    }
    for(int i = 0; i < edt.enumerations_size(); i++)
    {
        const omdParser::EnumeratedDataType_Enumeration& ee = edt.enumerations(i);
        std::cout << boost::str(boost::format("        (Enumeration (Enumerator \"%s\")") % ee.enumerator().value()) << std::endl;
        std::cout << boost::str(boost::format("                     (Representation %i))") % ee.representation().value()) << std::endl;
    }
    std::cout << "  )" << std::endl;
}

void printCDT(const omdParser::ComplexDataType & cdt)
{
    std::cout << boost::str(boost::format("  (ComplexDataType (Name \"%s\"%s)\n") % cdt.name().value() % getNoteString(cdt.name().notes()));

    if(cdt.has_description())
    {
        std::cout << boost::str(boost::format("       (Description \"%s\")\n") % cdt.description().value());
    }
    if(cdt.has_mom_data_type())
    {
        std::cout << boost::str(boost::format("             (MOMComplexDataType %s)\n") % cdt.mom_data_type().value());
    }
    for(int i = 0; i < cdt.complex_component_size(); i++)
    {
        const omdParser::ComplexDataType_ComplexComponent& cc = cdt.complex_component(i);
        std::cout << boost::str(boost::format("       (ComplexComponent (FieldName \"%s\")") % cc.field_name().value()) << std::endl;
        if(cc.has_data_type())
        {
            std::cout << boost::format("               (DataType \"%s\"%s)\n") % cc.data_type().value() % getNoteString(cc.data_type().notes());
        }
        if(cc.has_cardinality())
        {
            std::cout << boost::format("                         (Cardinality \"%s\")\n") % cc.cardinality().value();
        }
        if(cc.has_units())
        {
            std::cout << boost::format("                         (Units \"%s\"%s)\n") % cc.units().value() % getNoteString(cc.units().notes());
        }
        if(cc.has_resolution())
        {
            std::cout << boost::format("                         (Resolution \"%s\")\n") % cc.resolution().value();
        }
        if(cc.has_accuracy())
        {
            std::cout << boost::format("                         (Accuracy \"%s\")\n") % cc.accuracy().value();
        }
        if(cc.has_accuracy_condition())
        {
            std::cout << boost::format("                         (AccuracyCondition \"%s\")\n") % cc.accuracy_condition().value();
        }
        if(cc.has_description())
        {
            std::cout << boost::format("                         (Description \"%s\")\n") % cc.description().value();
        }
        std::cout << "       )" << std::endl;
    }
    std::cout << "  )" << std::endl;
}

void printClass(const omdParser::Class& class_)
{
    std::cout << boost::format("  (Class (ID %i)\n") % class_.id().value();
    std::cout << boost::format("     (Name \"%s\"%s)\n") % class_.name().value() % getNoteString(class_.name().notes());
    if(class_.has_ps_capabilities())
    {
        // Only populate the map once
        if(psStringMap.size() == 0)
        {
            psStringMap[omdParser::Class_PSCapabilities_N]  = "N";
            psStringMap[omdParser::Class_PSCapabilities_P]  = "P";
            psStringMap[omdParser::Class_PSCapabilities_S]  = "S";
            psStringMap[omdParser::Class_PSCapabilities_PS] = "PS";
        }

        std::cout << boost::format("     (PSCapabilities %s)\n") % psStringMap[class_.ps_capabilities().value()];
    }
    if(class_.has_description())
    {
        std::cout << boost::format("     (Description \"%s\")\n") % class_.description().value();
    }
    if(class_.has_superclass())
    {
        std::cout << boost::format("     (SuperClass %s)\n") % class_.superclass().value();
    }

    for(int i = 0; i < class_.attributes_size(); i++)
    {
        const omdParser::Class_Attribute& attrib = class_.attributes(i);
        std::cout << boost::str(boost::format("     (Attribute (Name \"%s\"%s)\n") % attrib.name().value() % getNoteString(attrib.name().notes()));

        if(attrib.has_data_type())
        {
            std::cout << boost::str(boost::format("                (DataType \"%s\")\n") % attrib.data_type().value());
        }
        if(attrib.has_cardinality())
        {
            std::cout << boost::str(boost::format("                (Cardinality \"%s\")\n") % attrib.cardinality().value());
        }
        if(attrib.has_units())
        {
            std::cout << boost::str(boost::format("                (Units \"%s\")\n") % attrib.units().value());
        }
        if(attrib.has_resolution())
        {
            std::cout << boost::str(boost::format("                (Resolution \"%s\")\n") % attrib.resolution().value());
        }
        if(attrib.has_accuracy())
        {
            std::cout << boost::str(boost::format("                (Accuracy \"%s\")\n") % attrib.accuracy().value());
        }
        if(attrib.has_accuracy_condition())
        {
            std::cout << boost::str(boost::format("                (AccuracyCondition \"%s\")\n") % attrib.accuracy_condition().value());
        }
        if(attrib.has_update_type())
        {
            std::string val;
            switch(attrib.update_type().value())
            {
                case omdParser::Class_Attribute_UpdateType_Conditional:
                {
                    val = "Conditional";
                    break;
                }
                case omdParser::Class_Attribute_UpdateType_Periodic:
                {
                    val = "Periodic";
                    break;
                }
                case omdParser::Class_Attribute_UpdateType_Static:
                {
                    val = "Static";
                    break;
                }
            }
            std::cout << boost::str(boost::format("                (UpdateType %s)\n") % val);
        }
        if(attrib.has_update_condition())
        {
            std::cout << boost::str(boost::format("                (UpdateCondition \"%s\"%s)\n") % attrib.update_condition().value() % getNoteString(attrib.update_condition().notes()));
        }
        if(attrib.has_transfer_accept())
        {
            std::string val;
            switch(attrib.transfer_accept().value())
            {
                case omdParser::Class_Attribute_TransferAccept_A:
                {
                    val = "A";
                    break;
                }
                case omdParser::Class_Attribute_TransferAccept_T:
                {
                    val = "T";
                    break;
                }
                case omdParser::Class_Attribute_TransferAccept_TA:
                {
                    val = "TA";
                    break;
                }
                case omdParser::Class_Attribute_TransferAccept_N:
                {
                    val = "N";
                    break;
                }
            }
            std::cout << boost::str(boost::format("                (TransferAccept %s%s)\n") % val % getNoteString(attrib.transfer_accept().notes()));
        }
        if(attrib.has_update_reflect())
        {
            std::string val;
            switch(attrib.update_reflect().value())
            {
                case omdParser::Class_Attribute_UpdateReflect_R:
                {
                    val = "R";
                    break;
                }
                case omdParser::Class_Attribute_UpdateReflect_U:
                {
                    val = "U";
                    break;
                }
                case omdParser::Class_Attribute_UpdateReflect_UR:
                {
                    val = "UR";
                    break;
                }
            }
            std::cout << boost::str(boost::format("                (UpdateReflect %s%s)\n") % val % getNoteString(attrib.update_reflect().notes()));
        }
        if(attrib.has_description())
        {
            std::cout << boost::str(boost::format("                (Description \"%s\"%s)\n") % attrib.description().value() % getNoteString(attrib.description().notes()));
        }
        if(attrib.has_delivery_category())
        {
            std::cout << boost::str(boost::format("                (DeliveryCategory \"%s\"%s)\n") % attrib.delivery_category().value() % getNoteString(attrib.delivery_category().notes()));
        }
        if(attrib.has_message_ordering())
        {
            std::cout << boost::str(boost::format("                (MessageOrdering \"%s\"%s)\n") % attrib.message_ordering().value() % getNoteString(attrib.message_ordering().notes()));
        }

        std::cout << "     )" << std::endl;
    }

    std::cout << "  )" << std::endl;
}

typedef std::map<omdParser::Interaction_ISR_Type, std::string> isrStringMap_t;
isrStringMap_t isrStringMap_;

void printInteraction(const omdParser::Interaction& inter)
{
    std::cout << boost::str(boost::format("  (Interaction (ID %i)\n") % inter.id().value());
    std::cout << boost::str(boost::format("     (Name \"%s\"%s)\n") % inter.name().value() % getNoteString(inter.name().notes()));
    if(inter.has_isr_type())
    {
        if(0 == isrStringMap_.size())
        {
            isrStringMap_[omdParser::Interaction_ISR_Type_IS] = "IS";
            isrStringMap_[omdParser::Interaction_ISR_Type_IR] = "IR";
            isrStringMap_[omdParser::Interaction_ISR_Type_I]  = "I";
            isrStringMap_[omdParser::Interaction_ISR_Type_S]  = "S";
            isrStringMap_[omdParser::Interaction_ISR_Type_R]  = "R";
            isrStringMap_[omdParser::Interaction_ISR_Type_N]  = "N";
        }

        std::cout << boost::format("     (ISRType %s%s)\n") % isrStringMap_[inter.isr_type().value()] % getNoteString(inter.isr_type().notes());
    }

    if(inter.has_description())
    {
        std::cout << boost::str(boost::format("     (Description \"%s\")\n") % inter.description().value());
    }
    if(inter.has_delivery_category())
    {
        std::cout << boost::str(boost::format("     (DeliveryCategory \"%s\"%s)\n") % inter.delivery_category().value() % getNoteString(inter.delivery_category().notes()));
    }
    if(inter.has_message_ordering())
    {
        std::cout << boost::str(boost::format("     (MessageOrdering \"%s\"%s)\n") % inter.message_ordering().value() % getNoteString(inter.message_ordering().notes()));
    }
    if(inter.has_super_interaction())
    {
        std::cout << boost::str(boost::format("     (SuperInteraction %s)\n") % inter.super_interaction().value());
    }
    for(int i = 0; i < inter.parameters_size(); i++)
    {
        const omdParser::Interaction_Parameter& parm = inter.parameters(i);
        std::cout << boost::str(boost::format("     (Parameter (Name \"%s\"%s)\n") % parm.name().value() % getNoteString(parm.name().notes()));

        if(parm.has_data_type())
        {
            std::cout << boost::str(boost::format("                (DataType \"%s\")\n") % parm.data_type().value());
        }
        if(parm.has_cardinality())
        {
            std::cout << boost::str(boost::format("                (Cardinality \"%s\")\n") % parm.cardinality().value());
        }
        if(parm.has_units())
        {
            std::cout << boost::str(boost::format("                (Units \"%s\")\n") % parm.units().value());
        }
        if(parm.has_resolution())
        {
            std::cout << boost::str(boost::format("                (Resolution \"%s\")\n") % parm.resolution().value());
        }
        if(parm.has_accuracy())
        {
            std::cout << boost::str(boost::format("                (Accuracy \"%s\")\n") % parm.accuracy().value());
        }
        if(parm.has_accuracy_condition())
        {
            std::cout << boost::str(boost::format("                (AccuracyCondition \"%s\")\n") % parm.accuracy_condition().value());
        }
        if(parm.has_description())
        {
            std::cout << boost::str(boost::format("                (Description \"%s\")\n") % parm.description().value());
        }

        std::cout << "     )" << std::endl;
    }

    std::cout << "  )" << std::endl;
}

void printNote(const omdParser::Note& note)
{
    std::cout << boost::str(boost::format("  (Note (NoteNumber %i)\n") % note.note_number().value());
    std::cout << boost::str(boost::format("        (NoteText \"%s\"%s))\n\n") % note.note_text().value() % getNoteString(note.note_text().notes()));
}


typedef std::map<omdParser::RoutingSpace_Dimension_IntervalType, std::string> intervalTypeStringMap_t;
intervalTypeStringMap_t itsm_;
void printRoutingSpace(const omdParser::RoutingSpace& rs)
{
    std::cout << boost::format("  (RoutingSpace (Name \"%s\" %s)\n") % rs.name().value() % getNoteString(rs.name().notes());
    if(rs.has_description())
    {
        std::cout << boost::format("    (Description \"%s\" %s)\n") % rs.description().value() % getNoteString(rs.description().notes());
    }

    for(int i = 0; i < rs.dimensions_size(); ++i)
    {
        const omdParser::RoutingSpace::Dimension& dim = rs.dimensions(i);

        std::cout << boost::format("    (Dimension (Name \"%s\" %s)\n") % dim.name().value() % getNoteString(dim.name().notes());
        if(dim.has_dimension_type())
        {
            std::cout << boost::format("      (DimensionType \"%s\" %s)\n") % dim.dimension_type().value() % getNoteString(dim.dimension_type().notes());
        }

        if(dim.has_dimension_minimum())
        {
            std::cout << boost::format("      (DimensionMinimum \"%s\" %s)\n") % dim.dimension_minimum().value() % getNoteString(dim.dimension_minimum().notes());
        }

        if(dim.has_dimension_maximum())
        {
            std::cout << boost::format("      (DimensionMaximum \"%s\" %s)\n") % dim.dimension_maximum().value() % getNoteString(dim.dimension_maximum().notes());
        }

        if(dim.has_interval_type())
        {
            if(0 == itsm_.size())
            {
                itsm_[omdParser::RoutingSpace_Dimension_IntervalType_Closed] = "Closed";
                itsm_[omdParser::RoutingSpace_Dimension_IntervalType_Open] = "Open";
            }

            std::cout << boost::format("      (IntervalType %s %s)\n") % itsm_[dim.interval_type().value()] % getNoteString(dim.interval_type().notes());
        }

        if(dim.has_range_set_units())
        {
            std::cout << boost::format("      (RangeSetUnits \"%s\" %s)\n") % dim.range_set_units().value() % getNoteString(dim.range_set_units().notes());
        }

        if(dim.has_normalization_function())
        {
            std::cout << boost::format("      (NormalizationFunction \"%s\" %s)\n") % dim.normalization_function().value() % getNoteString(dim.normalization_function().notes());
        }
        std::cout << "    )" << std::endl;
    }
std::cout << "  )" << std::endl;
}

void printObjectModel(const omdParser::ObjectModel& model)
{
    std::cout << boost::str(boost::format("(ObjectModel (Name \"%s\")\n") % model.name().value());

    if(model.has_version_number())
    {
        std::cout << boost::str(boost::format("             (VersionNumber \"%s\")\n") % model.version_number().value());
    }

    if(model.has_type())
    {
        std::cout << boost::str(boost::format("             (Type %s)\n") % model.type().value());
    }

    if(model.has_purpose())
    {
        std::cout << boost::str(boost::format("             (Purpose \"%s\")\n") % model.purpose().value());
    }

    if(model.has_application_domain())
    {
        std::cout << boost::str(boost::format("             (ApplicationDomain \"%s\")\n") % model.application_domain().value());
    }

    if(model.has_sponsor_org_name())
    {
        std::cout << boost::str(boost::format("             (SponsorOrgName \"%s\")\n") % model.sponsor_org_name().value());
    }

    if(model.has_poc_honorific_name())
    {
        std::cout << boost::str(boost::format("             (POCHonorificName \"%s\")\n") % model.poc_honorific_name().value());
    }

    if(model.has_poc_first_name())
    {
        std::cout << boost::str(boost::format("             (POCFirstName \"%s\")\n") % model.poc_first_name().value());
    }

    if(model.has_poc_last_name())
    {
        std::cout << boost::str(boost::format("             (POCLastName \"%s\")\n") % model.poc_last_name().value());
    }

    if(model.has_poc_org_name())
    {
        std::cout << boost::str(boost::format("             (POCOrgName \"%s\")\n") % model.poc_org_name().value());
    }

    if(model.has_poc_phone())
    {
        std::cout << boost::str(boost::format("             (POCPhone \"%s\")\n") % model.poc_phone().value());
    }

    if(model.has_poc_email())
    {
        std::cout << boost::str(boost::format("             (POCEmail \"%s\")\n") % model.poc_email().value());
    }

    if(model.has_modification_date())
    {
        std::cout << boost::str(boost::format("             (ModificationDate %s)\n") % model.modification_date().value());
    }

    if(model.has_mom_version())
    {
        std::cout << boost::str(boost::format("             (MOMVersion \"%s\")\n") % model.mom_version().value());
    }

    if(model.has_fed_name())
    {
        std::cout << boost::str(boost::format("             (FEDname \"%s\")\n") % model.fed_name().value());
    }

    for(int i = 0; i < model.omd_component_size(); i++)
    {
        const omdParser::OmdComponent &obj = model.omd_component(i);

        switch(obj.type())
        {
            case omdParser::OmdComponent_ComponentType_edt_type:
            {
                printEDT(obj.enumerated_data_type());
                break;
            }
            case omdParser::OmdComponent_ComponentType_cdt_type:
            {
                printCDT(obj.complex_data_type());
                break;
            }
            case omdParser::OmdComponent_ComponentType_class_type:
            {
                printClass(obj.class_());
                break;
            }
            case omdParser::OmdComponent_ComponentType_interaction_type:
            {
                printInteraction(obj.interaction());
                break;
            }
            case omdParser::OmdComponent_ComponentType_note_type:
            {
                printNote(obj.note());
                break;
            }
            case omdParser::OmdComponent_ComponentType_route_space_type:
            {
                printRoutingSpace(obj.routing_space());
                break;
            }
        }        
    }
    std::cout << ")" << std::endl;
}

void printFile(const omdParser::OmdFile * file)
{
    std::cout << boost::str(boost::format("(OMDT %s)\n") % file->version());

    for(int i = 0; i < file->object_model_size(); i++)
    {
        printObjectModel(file->object_model(i));
    }

    
}


int main(int argc, char* argv[])
{
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

    std::string indentString;
    for(int i = 0; i < 10; i++)
    {      
        indentMap[indentStepSize*i] = indentString;
        indentString.append(one_indent);
    }


    printFile(&omdFile);

	return 0;
}

