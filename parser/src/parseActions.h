#pragma once

#include "omdParser.pb.h"

#include "boost/shared_ptr.hpp"

#include <list>
#include <map>


namespace omdParser
{


	/**
		So, what is all this stuff?  Well, when Boost::Spirit parses
		a file, the scope is all messy (short version).  So, to avoid
		unwanted side effects, you need to have the parse actions be 
		global.

		This Private namespace is used to attempt to limit the view 
		of the variables within this scope.

		Beware, dragons be here.

		You're probably wondering how this works.  Well, when an OMD 
		file is parsed "raw", the parser understands that an OMD file
		can be conceptually broken down into 'Collections' and 'Elements'.

		N elements live within a collection.

		An element has a key, value, and notes.  Notes are the numeric
		ID's of the notes which are parsed out of the file.  The key is
		the name of the element, the 'value' is the value of that element.

		Take the following Interaction snippet
		(Interaction (Name "Foo" [1,2,3])
		    (Parameter (Name "Bar")
		    ...
			)
		)

		The above, would conceptually be broken down into (excuse the pseudo-XML):

		<Collection type="Interaction">
			<Element Key="Name" Value="Foo" Notes=[1,2,3] />
			<Collection type="Parameter">
				<Element Key="Name" Value="Bar"/>
			</Colllection>
		</Collection>

		Where, the above XML shows that a collection is composed of 0 or more Elements,
		followed by 0 or more Collections.

		Theorhetically this could be indefinitely deep, but in reality, the OMD 
		specification denotes the following Collection/Sub-Collection relationships.

		One Enumerated Data Type will have N Enumerations
		One Complex Data Type will have N Complex Components
		One Interaction will have N Parameters
		One Class will have N Attribute

		Where N is >= 0

		Now, the reason why we actually parse it in this manner, is because certain 
		classes of problems requre that we maintain the order of elements.

		The intent, is to make helper functions which provide the user with a 
		mechanism to allow them to access a more fully-featured version of the 
		data from the OMD file, or a more "Raw" outlook on the file.
	**/

        typedef std::list<int> noteList_t;

        struct omdValue
        {
            std::string value;
            noteList_t  notes;
        };
    
        typedef std::map<std::string, omdValue> strMap_t;


        OmdFile& getRoot();
        void reset();

		/**
			Note on naming of these variables:

			XXX_m is the map of name->value
			XXX_k is the key of an item in the map
			XXX_v is the actual value of the item to be inserted into the map
		**/



        // Object model map information
        extern strMap_t om_m;
        extern strMap_t::key_type om_k;
        extern strMap_t::mapped_type om_v;


        // Component map information (class/interaction/edt/...)
        extern strMap_t c_m;
        extern strMap_t::key_type c_k;
        extern strMap_t::mapped_type c_v;


        // Sub-component map information (attribute, parameter, ...)
        extern strMap_t sc_m;
        extern strMap_t::key_type sc_k;
        extern strMap_t::mapped_type sc_v;

		// "Active" pointers.  These get swapped based on what we are
		// currently parsing, since some of the elements in the grammar
		// are reused.  These are used to allow them to update the appropriate
		// component
        extern strMap_t* a_m;
        extern strMap_t::key_type* a_k;
        extern strMap_t::mapped_type* a_v;


        void disableCerr();
        void enableCerr();


    
    void assign_ak(const char* first, const char* last);
    void assign_av(const char* first, const char* last);
    void assign_av_int(const int value);
    void insert_am(const char* first, const char* last);

    void assign_a_int(const int value);
    void assign_a_uint(const unsigned int value);
    void assign_note(const int value);

	void set_omdtVersion(const char * first, const char * last);

    void allocObjectModel(const char* first, const char* last);
    void commitObjectModel(const char* first, const char* last);

    void allocClass(const char* first, const char* last);
    void commitClass(const char* first, const char* last);

    void allocClassAttribute(const char* first, const char* last);
    void commitClassAttribute(const char* first, const char* last);


	void allocEnumeratedDataType(const char* first, const char* last);
    void commitEnumeratedDataType(const char* first, const char* last);

    void allocEnumeratedDataTypeEnumeration(const char* first, const char* last);
    void commitEnumeratedDataTypeEnumeration(const char* first, const char* last);

	void allocComplexDataType(const char* first, const char* last);
    void commitComplexDataType(const char* first, const char* last);

    void allocComplexDataTypeComplexComponent(const char* first, const char* last);
    void commitComplexDataTypeComplexComponent(const char* first, const char* last);

	void allocInteraction(const char* first, const char* last);
    void commitInteraction(const char* first, const char* last);

    void allocInteractionParameter(const char* first, const char* last);
    void commitInteractionParameter(const char* first, const char* last);

	void allocNote(const char* first, const char* last);
    void commitNote(const char* first, const char* last);

	void allocRoutingSpace(const char* first, const char* last);
    void commitRoutingSpace(const char* first, const char* last);

	void allocRoutingSpaceDimension(const char* first, const char* last);
    void commitRoutingSpaceDimension(const char* first, const char* last);

	/**
		This method sets the key of an element

		\param first First character of the matched key
		\param last Last character of the matched key
	**/
	/*
    void setKey(const char * first, const char * last);

    void setVal_str(const char * first, const char * last);

    void setVal_int(const int value) ;

    void setVal_uint(const unsigned int value) ;
    void setVal_bool(const char * first, const char * last);

    void setNote(const unsigned int note) ;

    void addElement(const char* first, const char* last);

    void popCollection(const char * first, const char * last);

    void setType(const char * first, const char * last);
	*/

	/*
    void setFileHeader(const char * first, const char * last);
    void setObjectModelValues(const char * first, const char * last);
    void addNoteList(const unsigned int value);

    //void setOmdNameNote(const char * first, const char * last);
    void setEdtValues(const char * first, const char * last);
    void setEnumValues(const char * first, const char * last);

    void setCdtValues(const char * first, const char * last);
    void setComplexComponentValues(const char * first, const char * last);

    void setClassValues(const char * first, const char * last);
    void setAttributeValues(const char * first, const char * last);

    void setInteractionValues(const char * first, const char * last);
    void setParameterValues(const char * first, const char * last);
	*/

}