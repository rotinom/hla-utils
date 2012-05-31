#include "parseActions.h"

#include <iostream>
#include "boost/lexical_cast.hpp"

namespace omdParser
{

        /// Protocol buffer object representing the OMD file
        OmdFile omdFile;

        ObjectModel* objectModel;

        OmdComponent* component;

        Class::Attribute* attrib;
        Interaction::Parameter* p;
        RoutingSpace::Dimension * dim;
        EnumeratedDataType::Enumeration* e;
        ComplexDataType::ComplexComponent* cc;

        /**
            Function which retrieves the root of the OMD file

            \returns pointer to an OmdFile
        **/
        OmdFile& getRoot()
        {
            return omdFile;
		}


        static std::streambuf* old_rdbuff = NULL;

        void enableCerr()
        {
            std::cerr.rdbuf(old_rdbuff);
            old_rdbuff = NULL;
        }

        void disableCerr()
        {
            
			// Disable std::cout
			old_rdbuff = std::cerr.rdbuf();
			std::cerr.rdbuf(NULL);
        }

        void reset()
        {
            // Clear our protocol buffer pointers
            objectModel = NULL;
            component = NULL;
            attrib = NULL;
            p = NULL;
            dim = NULL;
            e = NULL;
            cc = NULL;

            // Clear our active pointers
            a_m = &om_m;
            a_k = &om_k;
            a_v = &om_v;

            // Clear our value maps
            om_m.clear();
            c_m.clear();
            sc_m.clear();

            omdFile.Clear();
        }

        // Object model map information
        strMap_t om_m;
        strMap_t::key_type om_k;
        strMap_t::mapped_type om_v;


        // Component map information (class/interaction/edt/...)
        strMap_t c_m;
        strMap_t::key_type c_k;
        strMap_t::mapped_type c_v;


        // Sub-component map information (attribute, parameter, ...)
        strMap_t sc_m;
        strMap_t::key_type sc_k;
        strMap_t::mapped_type sc_v;


        strMap_t* a_m               = &om_m;
        strMap_t::key_type* a_k     = &om_k;
        strMap_t::mapped_type* a_v  = &om_v;

         bool enableDebug = false;



    void assign_ak(const char* first, const char* last)
    {
        a_k->assign(first, last);
    }

    void assign_av(const char* first, const char* last)
    {
        a_v->value = std::string(first, last);
    }

    void assign_av_int(const int value)
    {
        a_v->value = boost::lexical_cast<std::string>(value);
    }

    void insert_am(const char* first, const char* last)
    {
        a_m->insert(strMap_t::value_type(*a_k, *a_v));
        a_v->notes.clear();
    }

    void assign_note(const int value)
    {
        a_v->notes.push_back(value);
    }

    template <typename T>
    void setNotes(T* t, noteList_t& notes)
    {
        for(noteList_t::iterator i = notes.begin();
            i != notes.end();
            ++i)
        {
            t->add_notes(*i);
        }
    }

    inline bool toBoolean(const std::string& str)
    {
        return str == "TRUE" || str == "Yes" ? true : false;
    }

    void set_omdtVersion(const char* first, const char* last)
	{
        omdFile.set_version(std::string(first, last));
	}

    std::string& stripQuotes(std::string& str)
    {
        // If we are an escaped string, then remove the leading/trailing quotes
        if(!str.empty() &&
            *str.begin() == '\"' &&
            *str.rbegin() == '\"')
        {
            str = str.substr(1, str.size()-2);
        }
        return str;
    }

    void allocObjectModel(const char* first, const char* last)
    {

        a_m = &om_m;
        a_k = &om_k;
        a_v = &om_v;

        objectModel = omdFile.add_object_model();
        objectModel->set_view_type(omdParser::ObjectModel_ViewType_raw);
    }

	void commitObjectModel(const char* first, const char* last)
    {
        ObjectModel* om = objectModel;



            
        // Iterate through the map, and insert the string value into the appropriate location
        for(strMap_t::iterator iter = om_m.begin();
            iter != om_m.end();
            ++iter)
        {
            std::string name = iter->first;
            std::string val  = stripQuotes(iter->second.value);

            if("Name" == name)
            {
                om->mutable_name()->set_value(val);
            }
            else if("VersionNumber" == name)
            {
                om->mutable_version_number()->set_value(val);
            }
            else if("Purpose" == name)
            {
                om->mutable_purpose()->set_value(val);
            }
            else if("ApplicationDomain" == name)
            {
                om->mutable_application_domain()->set_value(val);
            }
            else if("SponsorOrgName" == name)
            {
                om->mutable_sponsor_org_name()->set_value(val);
            }
            else if("POCHonorificName" == name)
            {
                om->mutable_poc_honorific_name()->set_value(val);
            }
            else if("POCFirstName" == name)
            {
                om->mutable_poc_first_name()->set_value(val);
            }
            else if("POCLastName" == name)
            {
                om->mutable_poc_last_name()->set_value(val);
            }
            else if("POCOrgName" == name)
            {
                om->mutable_poc_org_name()->set_value(val);
            }
            else if("POCPhone" == name)
            {
                om->mutable_poc_phone()->set_value(val);
            }
            else if("POCEmail" == name)
            {
                om->mutable_poc_email()->set_value(val);
            }
            else if("MOMVersion" == name)
            {
                om->mutable_mom_version()->set_value(val);
            }
            else if("FEDname" == name)
            {
                om->mutable_fed_name()->set_value(val);
            }
            else if("Type" == name)
            {
                om->mutable_type()->set_value(val);
            }
            else if("ModificationDate" == name)
            {
                om->mutable_modification_date()->set_value(val);
            }
            else
            {
                assert(false);
            }
            
        }

        om_m.clear();
    }


       
	void allocEnumeratedDataType(const char* first, const char* last)
	{
		// Allocate our EDT, and set its type
		component = objectModel->add_omd_component();
		component->set_type(omdParser::OmdComponent_ComponentType_edt_type);

		// Set our active pointers to the component
		a_m = &c_m;
		a_k = &c_k;
		a_v = &c_v;
	}

    void commitEnumeratedDataType(const char* first, const char* last)
    {
        EnumeratedDataType* edt = component->mutable_enumerated_data_type();
		
        // Iterate through the component map, and handle the values
		// the string value into the appropriate location
        for(strMap_t::iterator iter = c_m.begin();
            iter != c_m.end();
            ++iter)
        {
            std::string name = iter->first;
            std::string val  = stripQuotes(iter->second.value);

            if("Name" == name)
            {
                edt->mutable_name()->set_value(val);
                setNotes<omdParser::stringValue>(edt->mutable_name(), iter->second.notes);
            }
            else if("Description" == name)
            {
                edt->mutable_description()->set_value(val);
                setNotes<omdParser::stringValue>(edt->mutable_description(), iter->second.notes);
            }
            else if("MOMEnumeratedDataType" == name)
            {
                edt->mutable_mom_data_type()->set_value(toBoolean(val));
                setNotes<omdParser::boolValue>(edt->mutable_mom_data_type(), iter->second.notes);
            }
            else if("AutoSequence" == name)
            {
                edt->mutable_auto_sequence()->set_value(toBoolean(val));
                setNotes<omdParser::boolValue>(edt->mutable_auto_sequence(), iter->second.notes);
            }
            else if("StartValue" == name)
            {
                edt->mutable_start_value()->set_value(boost::lexical_cast<int>(val));
                setNotes<omdParser::int32Value>(edt->mutable_start_value(), iter->second.notes);
            }
            else
            {
                assert(false);
            }
        }

        c_m.clear();

        // Reset active pointers to the object model
        a_m = &om_m;
        a_k = &om_k;
        a_v = &om_v;
    }



    void allocEnumeratedDataTypeEnumeration(const char* first, const char* last)
    {
        e = component->mutable_enumerated_data_type()->add_enumerations();

        // Set our active pointers to the subcomponents
        a_m = &sc_m;
        a_k = &sc_k;
        a_v = &sc_v;
    }


    void commitEnumeratedDataTypeEnumeration(const char* first, const char* last)
    {  

	    for(strMap_t::iterator iter = sc_m.begin();
            iter != sc_m.end();
            ++iter)
	    {
		    std::string name = iter->first;
            std::string val  = stripQuotes(iter->second.value);

            if("Enumerator" == name)
            {
                e->mutable_enumerator()->set_value(val);
                setNotes<omdParser::stringValue>(e->mutable_enumerator(), iter->second.notes);
            }
            else if("Representation" == name)
            {
                e->mutable_representation()->set_value(boost::lexical_cast<unsigned int>(val));
                setNotes<omdParser::int32Value>(e->mutable_representation(), iter->second.notes);
            }
            else
            {
                assert(false);
            }
        }

        sc_m.clear();

        // Set our active pointers to the subcomponents
        a_m = &c_m;
        a_k = &c_k;
        a_v = &c_v;
    }




	void allocComplexDataType(const char* first, const char* last)
	{
		// Allocate our CDT, and set its type
		component = objectModel->add_omd_component();
		component->set_type(omdParser::OmdComponent_ComponentType_cdt_type);

		// Set our active pointers to the component
		a_m = &c_m;
		a_k = &c_k;
		a_v = &c_v;
	}

    void commitComplexDataType(const char* first, const char* last)
    {
        ComplexDataType* cdt = component->mutable_complex_data_type();
		
        // Iterate through the component map, and handle the values
		// the string value into the appropriate location
        for(strMap_t::iterator iter = c_m.begin();
            iter != c_m.end();
            ++iter)
        {
            std::string name = iter->first;
            std::string val  = stripQuotes(iter->second.value);

            if("Name" == name)
            {
                cdt->mutable_name()->set_value(val);
                setNotes<omdParser::stringValue>(cdt->mutable_name(), iter->second.notes);
            }
            else if("Description" == name)
            {
                cdt->mutable_description()->set_value(val);
                setNotes<omdParser::stringValue>(cdt->mutable_description(), iter->second.notes);
            }
            else if("MOMEnumeratedDataType" == name)
            {
                cdt->mutable_mom_data_type()->set_value(toBoolean(val));
                setNotes<omdParser::boolValue>(cdt->mutable_mom_data_type(), iter->second.notes);
            }
            else
            {
                assert(false);
            }
        }

        c_m.clear();

        // Reset active pointers to the object model
        a_m = &om_m;
        a_k = &om_k;
        a_v = &om_v;
    }


    void allocComplexDataTypeComplexComponent(const char* first, const char* last)
    {
        cc = component->mutable_complex_data_type()->add_complex_component();

        // Set our active pointers to the subcomponents
        a_m = &sc_m;
        a_k = &sc_k;
        a_v = &sc_v;
    }


    void commitComplexDataTypeComplexComponent(const char* first, const char* last)
    {  

	    for(strMap_t::iterator iter = sc_m.begin();
            iter != sc_m.end();
            ++iter)
	    {
		    std::string name = iter->first;
            std::string val  = stripQuotes(iter->second.value);

            if("FieldName" == name)
            {
                cc->mutable_field_name()->set_value(val);
                setNotes<omdParser::stringValue>(cc->mutable_field_name(), iter->second.notes);
            }
            else if("DataType" == name)
            {
                cc->mutable_data_type()->set_value(val);
                setNotes<omdParser::stringValue>(cc->mutable_data_type(), iter->second.notes);
            }
            else if("Accuracy" == name)
            {
                cc->mutable_accuracy()->set_value(val);
                setNotes<omdParser::stringValue>(cc->mutable_accuracy(), iter->second.notes);
            }
            else if("AccuracyCondition" == name)
            {
                cc->mutable_accuracy_condition()->set_value(val);
                setNotes<omdParser::stringValue>(cc->mutable_accuracy_condition(), iter->second.notes);
            }
            else if("Units" == name)
            {
                cc->mutable_units()->set_value(val);
                setNotes<omdParser::stringValue>(cc->mutable_units(), iter->second.notes);
            }
            else if("Resolution" == name)
            {
                cc->mutable_resolution()->set_value(val);
                setNotes<omdParser::stringValue>(cc->mutable_resolution(), iter->second.notes);
            }
            else if("Cardinality" == name)
            {
                cc->mutable_cardinality()->set_value(val);
                setNotes<omdParser::stringValue>(cc->mutable_cardinality(), iter->second.notes);
            }
            else if("Description" == name)
            {
                cc->mutable_description()->set_value(val);
                setNotes<omdParser::stringValue>(cc->mutable_description(), iter->second.notes);
            }
            else
            {
                assert(false);
            }
        }

        sc_m.clear();

        // Set our active pointers to the subcomponents
        a_m = &c_m;
        a_k = &c_k;
        a_v = &c_v;
    }



    void allocClass(const char* first, const char* last)
    {
        // Allocate our class, and set its type
        component = objectModel->add_omd_component();
        component->set_type(omdParser::OmdComponent_ComponentType_class_type);

        // Set our active pointers to the component
        a_m = &c_m;
        a_k = &c_k;
        a_v = &c_v;
    }


    // Set up our PS capabilities map
    typedef std::map<std::string, omdParser::Class_PSCapabilities> psMap_t;
	static psMap_t psMap;

    void commitClass(const char* first, const char* last)
    {
		// Populate on first call only...
        if(psMap.size() == 0)
        {
            psMap["PS"] = omdParser::Class_PSCapabilities_PS;
            psMap["N"]  = omdParser::Class_PSCapabilities_N;
            psMap["S"]  = omdParser::Class_PSCapabilities_S;
            psMap["P"]  = omdParser::Class_PSCapabilities_P;
        }

        Class* c = component->mutable_class_();
		
        // Iterate through the component map, and handle the values
		// the string value into the appropriate location
        for(strMap_t::iterator iter = c_m.begin();
            iter != c_m.end();
            ++iter)
        {
            std::string name = iter->first;
            std::string val  = stripQuotes(iter->second.value);
            if(name == "Name")
            {
                c->mutable_name()->set_value(val);
                setNotes<omdParser::stringValue>(c->mutable_name(), iter->second.notes);
            }
			else if(name == "ID")
			{
				c->mutable_id()->set_value(boost::lexical_cast<unsigned int>(val));
				setNotes<omdParser::int32Value>(c->mutable_id(), iter->second.notes);
			}
            else if(name == "PSCapabilities")
            {
                c->mutable_ps_capabilities()->set_value(psMap[val]);
                setNotes<omdParser::Class_PSCapabilitiesValue>(c->mutable_ps_capabilities(), iter->second.notes);
            }
            else if(name == "Description")
            {
                c->mutable_description()->set_value(val);
                setNotes<omdParser::stringValue>(c->mutable_description(), iter->second.notes);
            }            
            else if(name == "MOMClass")
            {
                c->mutable_mom_class()->set_value(toBoolean(val));
                setNotes<omdParser::boolValue>(c->mutable_mom_class(), iter->second.notes);
            }
            else if(name == "SuperClass")
            {
				c->mutable_superclass()->set_value(boost::lexical_cast<unsigned int>(val.c_str()));
                setNotes<omdParser::int32Value>(c->mutable_superclass(), iter->second.notes);
            }
            else
            {
                assert(false);
            }
        }

        // Clear the component map
        c_m.clear();

        // Reset active pointers to the object model
        a_m = &om_m;
        a_k = &om_k;
        a_v = &om_v;
    }



    void allocClassAttribute(const char* first, const char* last)
    {
		attrib = component->mutable_class_()->add_attributes();

        // Set our active pointers to the subcomponents
        a_m = &sc_m;
        a_k = &sc_k;
        a_v = &sc_v;
    }

    typedef std::map<std::string, omdParser::Class_Attribute_UpdateType> utMap_t;
    utMap_t utMap;

    typedef std::map<std::string, omdParser::Class_Attribute_TransferAccept> taMap_t;
    taMap_t taMap;

    typedef std::map<std::string, omdParser::Class_Attribute_UpdateReflect> urMap_t;
    urMap_t urMap;


    void commitClassAttribute(const char* first, const char* last)
    {    
		// These maps are used to convert a string to an enumeration.
        if(utMap.size() == 0)
        {
            utMap["Static"] = omdParser::Class_Attribute_UpdateType_Static;
            utMap["Periodic"]  = omdParser::Class_Attribute_UpdateType_Periodic;
            utMap["Conditional"]  = omdParser::Class_Attribute_UpdateType_Conditional;
        }

        if(taMap.size() == 0)
        {
            taMap["TA"] = omdParser::Class_Attribute_TransferAccept_TA;
            taMap["T"] = omdParser::Class_Attribute_TransferAccept_T;
            taMap["A"] = omdParser::Class_Attribute_TransferAccept_A;
            taMap["N"] = omdParser::Class_Attribute_TransferAccept_N;
        }

        if(urMap.size() == 0)
        {
            urMap["U"] = omdParser::Class_Attribute_UpdateReflect_U;
            urMap["R"] = omdParser::Class_Attribute_UpdateReflect_R;
            urMap["UR"] = omdParser::Class_Attribute_UpdateReflect_UR;
        }


		Class::Attribute * a = attrib;

		for(strMap_t::iterator iter = sc_m.begin();
            iter != sc_m.end();
            ++iter)
		{
			std::string name = iter->first;
            std::string val  = stripQuotes(iter->second.value);

            if("Name" == name)
            {
                a->mutable_name()->set_value(val);
                setNotes<omdParser::stringValue>(a->mutable_name(), iter->second.notes);
            }
			else if("DataType" == name)
			{
				a->mutable_data_type()->set_value(val);
				setNotes<omdParser::stringValue>(a->mutable_data_type(), iter->second.notes);
			}
			else if("Cardinality" == name)
			{
				a->mutable_cardinality()->set_value(val);
				setNotes<omdParser::stringValue>(a->mutable_cardinality(), iter->second.notes);
			}
			else if("Description" == name)
			{
				a->mutable_description()->set_value(val);
				setNotes<omdParser::stringValue>(a->mutable_description(), iter->second.notes);
			}
            else if("Units" == name)
            {
                a->mutable_units()->set_value(val);
                setNotes<omdParser::stringValue>(a->mutable_units(), iter->second.notes);
            }
            else if("Resolution" == name)
            {
                a->mutable_resolution()->set_value(val);
                setNotes<omdParser::stringValue>(a->mutable_resolution(), iter->second.notes);
            }
            else if("Accuracy" == name)
            {
                a->mutable_accuracy()->set_value(val);
                setNotes<omdParser::stringValue>(a->mutable_accuracy(), iter->second.notes);
            }
            else if("AccuracyCondition" == name)
            {
                a->mutable_accuracy_condition()->set_value(val);
                setNotes<omdParser::stringValue>(a->mutable_accuracy_condition(), iter->second.notes);
            }
            else if("DeliveryCategory" == name)
            {
                a->mutable_delivery_category()->set_value(val);
                setNotes<omdParser::stringValue>(a->mutable_delivery_category(), iter->second.notes);
            }
            else if("MessageOrdering" == name)
            {
                a->mutable_message_ordering()->set_value(val);
                setNotes<omdParser::stringValue>(a->mutable_message_ordering(), iter->second.notes);
            }
            else if("RoutingSpace" == name)
            {
                a->mutable_routing_space()->set_value(val);
                setNotes<omdParser::stringValue>(a->mutable_routing_space(), iter->second.notes);
            }
            else if("UpdateType" == name)
            {
                a->mutable_update_type()->set_value(utMap[val]);
                setNotes<omdParser::Class_Attribute_UpdateTypeValue>(a->mutable_update_type(), iter->second.notes);
            }
            else if("UpdateCondition" == name)
            {
                a->mutable_update_condition()->set_value(val);
                setNotes<omdParser::stringValue>(a->mutable_update_condition(), iter->second.notes);
            }
            else if("TransferAccept" == name)
            {
                a->mutable_transfer_accept()->set_value(taMap[val]);
                setNotes<omdParser::Class_Attribute_TransferAcceptValue>(a->mutable_transfer_accept(), iter->second.notes);
            }
            else if("UpdateReflect" == name)
            {
                a->mutable_update_reflect()->set_value(urMap[val]);
                setNotes<omdParser::Class_Attribute_UpdateReflectValue>(a->mutable_update_reflect(), iter->second.notes);
            }
            else
            {
                assert(false);
            }
		}
		
        // Clear the subcomponent map
        sc_m.clear();

        // Reset our active pointers to the class
        a_m = &c_m;
        a_k = &c_k;
        a_v = &c_v;
    }




	void allocInteraction(const char* first, const char* last)
    {
        // Allocate our class, and set its type
        component = objectModel->add_omd_component();
        component->set_type(omdParser::OmdComponent_ComponentType_interaction_type);

        // Set our active pointers to the component
        a_m = &c_m;
        a_k = &c_k;
        a_v = &c_v;
    }

    typedef std::map<std::string, omdParser::Interaction_ISR_Type> isrMap_t;
    isrMap_t isrMap;

    void commitInteraction(const char* first, const char* last)
    {
        if(0 == isrMap.size())
        {
            isrMap["IS"] = omdParser::Interaction_ISR_Type_IS;
            isrMap["IR"] = omdParser::Interaction_ISR_Type_IR;
            isrMap["I"] = omdParser::Interaction_ISR_Type_I;
            isrMap["S"] = omdParser::Interaction_ISR_Type_S;
            isrMap["R"] = omdParser::Interaction_ISR_Type_R;
            isrMap["N"] = omdParser::Interaction_ISR_Type_N;
        }


        Interaction* i = component->mutable_interaction();
		
        // Iterate through the component map, and handle the values
		// the string value into the appropriate location
        for(strMap_t::iterator iter = c_m.begin();
            iter != c_m.end();
            ++iter)
        {
            std::string name = iter->first;
            std::string val  = stripQuotes(iter->second.value);
            if("Name" == name)
            {
                i->mutable_name()->set_value(val);
                setNotes<omdParser::stringValue>(i->mutable_name(), iter->second.notes);
            }
			else if("ID" == name)
			{
				i->mutable_id()->set_value(boost::lexical_cast<unsigned int>(val));
				setNotes<omdParser::int32Value>(i->mutable_id(), iter->second.notes);
			}
            else if("Description" == name)
			{
                i->mutable_description()->set_value(val);
				setNotes<omdParser::stringValue>(i->mutable_description(), iter->second.notes);
			}
            else if("Description" == name)
			{
                i->mutable_description()->set_value(val);
				setNotes<omdParser::stringValue>(i->mutable_description(), iter->second.notes);
			}
            else if("DeliveryCategory" == name)
			{
                i->mutable_delivery_category()->set_value(val);
				setNotes<omdParser::stringValue>(i->mutable_delivery_category(), iter->second.notes);
			}
            else if("MessageOrdering" == name)
			{
                i->mutable_message_ordering()->set_value(val);
				setNotes<omdParser::stringValue>(i->mutable_message_ordering(), iter->second.notes);
			}
            else if("ISRType" == name)
			{
                i->mutable_isr_type()->set_value(isrMap[val]);
                setNotes<omdParser::Interaction_ISR_TypeValue>(i->mutable_isr_type(), iter->second.notes);
			}
            else if("MOMEnumeratedDataType" == name)
            {
                i->mutable_mom_interaction()->set_value(toBoolean(val));
                setNotes<omdParser::boolValue>(i->mutable_mom_interaction(), iter->second.notes);
            }
            else if("RoutingSpace" == name)
			{
                i->mutable_routing_space()->set_value(val);
				setNotes<omdParser::stringValue>(i->mutable_routing_space(), iter->second.notes);
			}
            else if("SuperInteraction" == name)
			{
                i->mutable_super_interaction()->set_value(boost::lexical_cast<unsigned int>(val));
				setNotes<omdParser::int32Value>(i->mutable_super_interaction(), iter->second.notes);
			}
            else
            {
                assert(false);
            }
        }

        // Set our active pointers to the object model
        a_m = &om_m;
        a_k = &om_k;
        a_v = &om_v;

        c_m.clear();
    }

    void allocInteractionParameter(const char* first, const char* last)
    {
        p = component->mutable_interaction()->add_parameters();

        // Set our active pointers to the subcomponents
        a_m = &sc_m;
        a_k = &sc_k;
        a_v = &sc_v;
    }

    void commitInteractionParameter(const char* first, const char* last)
    {
        for(strMap_t::iterator iter = sc_m.begin();
            iter != sc_m.end();
            ++iter)
		{
			std::string name = iter->first;
            std::string val  = stripQuotes(iter->second.value);

            if("Name" == name)
            {
                p->mutable_name()->set_value(val);
                setNotes<omdParser::stringValue>(p->mutable_name(), iter->second.notes);
            }
			else if("DataType" == name)
			{
				p->mutable_data_type()->set_value(val);
				setNotes<omdParser::stringValue>(p->mutable_data_type(), iter->second.notes);
			}
			else if("Cardinality" == name)
			{
				p->mutable_cardinality()->set_value(val);
				setNotes<omdParser::stringValue>(p->mutable_cardinality(), iter->second.notes);
			}
            else if("Description" == name)
			{
				p->mutable_description()->set_value(val);
				setNotes<omdParser::stringValue>(p->mutable_description(), iter->second.notes);
			}
            else if("Units" == name)
			{
				p->mutable_units()->set_value(val);
				setNotes<omdParser::stringValue>(p->mutable_units(), iter->second.notes);
			}
            else if("Resolution" == name)
			{
				p->mutable_resolution()->set_value(val);
				setNotes<omdParser::stringValue>(p->mutable_resolution(), iter->second.notes);
			}
            else if("Accuracy" == name)
			{
				p->mutable_accuracy()->set_value(val);
				setNotes<omdParser::stringValue>(p->mutable_accuracy(), iter->second.notes);
			}
            else if("AccuracyCondition" == name)
			{
				p->mutable_accuracy_condition()->set_value(val);
				setNotes<omdParser::stringValue>(p->mutable_accuracy_condition(), iter->second.notes);
			}
            else
            {
                assert(false);
            }
        }

        // Reset our active pointers to the component
        a_m = &c_m;
        a_k = &c_k;
        a_v = &c_v;

        // Clar the subcomponent map
        sc_m.clear();
    }

	void allocNote(const char* first, const char* last)
    {
        // Allocate our class, and set its type
        component = objectModel->add_omd_component();
        component->set_type(omdParser::OmdComponent_ComponentType_note_type);

        // Set our active pointers to the component
        a_m = &c_m;
        a_k = &c_k;
        a_v = &c_v;
    }

    void commitNote(const char* first, const char* last)
    {
        Note * n = component->mutable_note();

		for(strMap_t::iterator iter = c_m.begin();
            iter != c_m.end();
            ++iter)
		{
			std::string name = iter->first;
            std::string val  = stripQuotes(iter->second.value);

            if("NoteNumber" == name)
            {
                n->mutable_note_number()->set_value(boost::lexical_cast<unsigned int>(val));
                setNotes<omdParser::int32Value>(n->mutable_note_number(), iter->second.notes);
            }
            else if("NoteText" == name)
            {
                n->mutable_note_text()->set_value(val);
                setNotes<omdParser::stringValue>(n->mutable_note_text(), iter->second.notes);
            }
        }

        // Set our active pointers to the object model
        a_m = &om_m;
        a_k = &om_k;
        a_v = &om_v;

        c_m.clear();
    }


	void allocRoutingSpace(const char* first, const char* last)
    {
        // Allocate our class, and set its type
        component = objectModel->add_omd_component();
        component->set_type(omdParser::OmdComponent_ComponentType_route_space_type);

        // Set our active pointers to the component
        a_m = &c_m;
        a_k = &c_k;
        a_v = &c_v;
    }

    void commitRoutingSpace(const char* first, const char* last)
    { 
        RoutingSpace * r = component->mutable_routing_space();


		for(strMap_t::iterator iter = c_m.begin();
            iter != c_m.end();
            ++iter)
		{
			std::string name = iter->first;
            std::string val  = stripQuotes(iter->second.value);

            if("Name" == name)
            {
                r->mutable_name()->set_value(val);                
                setNotes<omdParser::stringValue>(r->mutable_name(), iter->second.notes);
            }
            else if("Description" == name)
            {
                r->mutable_description()->set_value(val);
                setNotes<omdParser::stringValue>(r->mutable_name(), iter->second.notes);
            }
        }

        // Set our active pointers to the object model
        a_m = &om_m;
        a_k = &om_k;
        a_v = &om_v;

        c_m.clear();
    }


    void allocRoutingSpaceDimension(const char* first, const char* last)
    {
        dim = component->mutable_routing_space()->add_dimensions();

        // Set our active pointers to the subcomponents
        a_m = &sc_m;
        a_k = &sc_k;
        a_v = &sc_v;
    }


    typedef std::map<std::string, omdParser::RoutingSpace_Dimension_IntervalType> itMap_t;
    itMap_t itMap;

    void commitRoutingSpaceDimension(const char* first, const char* last)
    {  
		// These maps are used to convert a string to an enumeration.
        if(itMap.size() == 0)
        {
            itMap["Open"]   = omdParser::RoutingSpace_Dimension_IntervalType_Open;
            itMap["Closed"] = omdParser::RoutingSpace_Dimension_IntervalType_Closed;
        }

	    for(strMap_t::iterator iter = sc_m.begin();
            iter != sc_m.end();
            ++iter)
	    {
		    std::string name = iter->first;
            std::string val  = stripQuotes(iter->second.value);

            if("Name" == name)
            {
                dim->mutable_name()->set_value(val);
                setNotes<omdParser::stringValue>(dim->mutable_name(), iter->second.notes);
            }
            else if("DimensionType" == name)
            {
                dim->mutable_dimension_type()->set_value(val);
                setNotes<omdParser::stringValue>(dim->mutable_dimension_type(), iter->second.notes);
            }
            else if("DimensionMinimum" == name)
            {
                dim->mutable_dimension_minimum()->set_value(val);
                setNotes<omdParser::stringValue>(dim->mutable_dimension_minimum(), iter->second.notes);
            }
            else if("DimensionMaximum" == name)
            {
                dim->mutable_dimension_maximum()->set_value(val);
                setNotes<omdParser::stringValue>(dim->mutable_dimension_maximum(), iter->second.notes);
            }
            else if("IntervalType" == name)
            {
                dim->mutable_interval_type()->set_value(itMap[val]);
                setNotes<omdParser::RoutingSpace_Dimension_IntervalTypeValue>(dim->mutable_interval_type(), iter->second.notes);
            }
            else if("RangeSetUnits" == name)
            {
                dim->mutable_range_set_units()->set_value(val);
                setNotes<omdParser::stringValue>(dim->mutable_range_set_units(), iter->second.notes);
            }
            else if("NormalizationFunction" == name)
            {
                dim->mutable_normalization_function()->set_value(val);
                setNotes<omdParser::stringValue>(dim->mutable_normalization_function(), iter->second.notes);
            }

        }

        sc_m.clear();

        // Set our active pointers to the components
        a_m = &c_m;
        a_k = &c_k;
        a_v = &c_v;
    }


    void assign_a_int(const int value)
    {
        a_v->value = boost::lexical_cast<std::string>(value);
    }

    void assign_a_uint(const unsigned int value)
    {
        a_v->value = boost::lexical_cast<std::string>(value);
    }

}