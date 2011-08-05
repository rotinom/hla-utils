#pragma once

#ifdef _WIN32
    #pragma warning( push )
    #pragma warning( disable : 4267 )
    #pragma warning( disable : 4503 )
#endif

#include <fstream>

// Redirect our debug output to cerr.  Preprocessors make this really effing annoying
#define BOOST_SPIRIT_DEBUG_OUT std::cerr



// Define these prior to including boost spirit
#define PHOENIX_LIMIT 6
#define BOOST_SPIRIT_CLOSURE_LIMIT 6
#define BOOST_SPIRIT_DEBUG




#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/classic_file_iterator.hpp>
#include <boost/spirit/include/phoenix1_binders.hpp>
#include <boost/spirit/include/classic_actor.hpp>



#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/lambda/bind.hpp>


#include "parseActions.h"



#ifdef _WIN32
    #pragma warning( pop )
#endif


namespace omdParser
{
    
    using namespace boost::spirit::classic;
    using namespace phoenix;
    typedef file_iterator<char> IteratorT;    

/**
    \brief The definition of the grammar we are using.
**/
struct OMDGrammar : boost::spirit::classic::grammar<OMDGrammar>
{

public:

    OMDGrammar()
    {
    }

    virtual ~OMDGrammar()
    {
    }

    /**
        \brief Required structure where all of the grammar definitions are set up
    **/
    template <typename ScannerT>
    struct definition
    {
        typedef boost::spirit::classic::rule<ScannerT> ScannerRule_t;
        typedef std::map<std::string, std::string> StrStrMap_t;

        StrStrMap_t ObjectModelMap;
        StrStrMap_t::value_type om_item;

        /**
            \brief Constructor where all of the grammar definitions are set up
        **/
        definition(OMDGrammar const& self)
        {



        /**
        *******************************************
            GENERIC REUSABLE SUB-RULES
        *******************************************
        **/

            date = 
                lexeme_d[ repeat_p(1,2)[digit_p] ] &&
                '/'               &&
                lexeme_d[ repeat_p(1,2)[digit_p] ] &&
                '/'               &&
                lexeme_d[ repeat_p(1,4)[digit_p] ];
            BOOST_SPIRIT_DEBUG_NODE(date);

            escaped_string = confix_p('"', *c_escape_ch_p, '"');
            BOOST_SPIRIT_DEBUG_NODE(escaped_string);


            boolean = 
                ( 
                    str_p("TRUE")   | 
                    str_p("Yes")    |
                    str_p("FALSE")  |
                    str_p("No")
                );
            BOOST_SPIRIT_DEBUG_NODE(boolean);



        /**
        *******************************************
            GENERIC REUSABLE RULES
        *******************************************
        **/
            name = 
                (
                    '('                                           && 
                    str_p("Name")       [&assign_ak]        && 
                        escaped_string  [&assign_av]  &&
                        !note_reference                           &&
                    ch_p(')')
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(name);

            description =
                (
                    '('                                              && 
                        str_p("Description")[&assign_ak]       &&
                        escaped_string      [&assign_av] && 
                        !note_reference                              &&
                    ch_p(')')
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(description);

            msg_ordering =
                (
                    ch_p('(')                               &&
                        str_p("MessageOrdering")[&assign_ak]   &&
                        escaped_string          [&assign_av]   &&
                        !note_reference                     &&
                    ch_p(')')
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(msg_ordering);


            cardinality =
                (
                    ch_p('(')                           &&
                        str_p("Cardinality")[&assign_ak]   &&
                        escaped_string      [&assign_av]   &&
                        !note_reference                         &&
                    ch_p(')')
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(cardinality);

            routing_space =
                (
                    ch_p('(')                                &&
                        str_p("RoutingSpace")   [&assign_ak] &&
                        escaped_string          [&assign_av] &&
                        !note_reference                      &&
                    ch_p(')') 
                )[&insert_am];

            id =
                (
                    ch_p('(')                    &&
                        str_p("ID") [&assign_ak] &&
                        uint_p      [&assign_av_int] &&
                        !note_reference           &&
                    ch_p(')')
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(id);

            data_type =
                (
                    ch_p('(')                             &&
                        str_p("DataType")   [&assign_ak]     &&
                        escaped_string      [&assign_av] &&
                        !note_reference                   &&
                    ch_p(')')
                )[&insert_am];


            accuracy =
                (
                    ch_p('(')                             &&
                        str_p("Accuracy")   [&assign_ak]     &&
                        escaped_string      [&assign_av] &&
                        !note_reference                   &&
                    ch_p(')')
                )[&insert_am];

            accuracy_condition =
                (
                    ch_p('(')                                   &&
                        str_p("AccuracyCondition")  [&assign_ak]   &&
                        escaped_string              [&assign_av]   &&
                        !note_reference                         &&
                    ch_p(')')
                )[&insert_am];

            units =
                (
                    ch_p('(')                       &&
                        str_p("Units")  [&assign_ak]   &&
                        escaped_string  [&assign_av]   &&
                        !note_reference             &&
                    ch_p(')')
                )[&insert_am];

            resolution =
                (
                    ch_p('(')                           &&
                        str_p("Resolution" )[&assign_ak]   &&
                        escaped_string      [&assign_av]   &&
                        !note_reference                 &&
                    ch_p(')')
                )[&insert_am];

            delivery_cat =
                (
                    ch_p('(')                                   &&
                        str_p("DeliveryCategory")   [&assign_ak]   &&
                        escaped_string              [&assign_av]   &&
                        !note_reference                         &&
                    ch_p(')')
                )[&insert_am];


            note_reference =
                ch_p('[')                                   &&
                    uint_p [&assign_note]  %
                    ','                                     &&
                ']';






        /**
        *******************************************
            FILE HEADER RULES
        *******************************************
        **/


			/**
				This defines file as a header and one or more object_model,
				followed by and end of line
			**/
            file =  header && +object_model && end_p;
            BOOST_SPIRIT_DEBUG_NODE(file);


			/**
				This defines a header, as something like:
				(OMDT v1.2.3.4)
				and sends OMDT to the setKey action, and
				the version string to the setVal_str action.
			**/
            header = 
                    '(' && 
                        str_p("OMDT")                   && 
                        lexeme_d
                        [
                            ch_p('v')                   && 
                            uint_p && ch_p('.')         &&
                            uint_p && ch_p('.')         &&
                            uint_p && ch_p('.')         &&
                            uint_p
                        ][&set_omdtVersion]				&&
                    ')';
            BOOST_SPIRIT_DEBUG_NODE(header);



        /**
        *******************************************
            OBJECT MODEL RULES
        *******************************************
        **/
            object_model =
                (
                    '(' &&
                        str_p("ObjectModel") [&allocObjectModel] &&
                        name                                     &&
                        *
                        (
                            om_qstring                       |
                            om_type                          |
                            om_mod_date
                        )                                    &&
                        *omd_component                       &&
                    ')'
                )[&commitObjectModel];
            BOOST_SPIRIT_DEBUG_NODE(object_model);

            /**
                This is a simplification of grammar.  This rule is meant
                to match an open parenthesis, one of the "names", and then
                a quoted string.  Since there are so many, it made sense
                to make this singular rule match all of the similar ones
            **/
            om_qstring =
                (
                    ch_p('(')                                    &&
                        (
                            str_p("VersionNumber")               |
                            str_p("Purpose")                     |
                            str_p("ApplicationDomain")           |
                            str_p("SponsorOrgName")              |
                            str_p("POCHonorificName")            |
                            str_p("POCFirstName")                |
                            str_p("POCLastName")                 |
                            str_p("POCOrgName")                  |
                            str_p("POCPhone")                    |
                            str_p("POCEmail")                    |
                            str_p("MOMVersion")                  |
                            str_p("FEDname")
                        )[assign_a(om_k)]                        &&
                        escaped_string[assign_a(om_v.value)]     &&
                    ch_p(')') 
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(om_qstring);

            /**
                This rule matches the object model "Type" field.
                Valid values are "FOM", "SOM", or "OTHER"
            **/
            om_type =
                (
                    '('                                     &&
                        str_p("Type") [assign_a(om_k)]   && 
                        (
                            str_p("FOM")                    |
                            str_p("SOM")                    | 
                            str_p("OTHER")
                        )[assign_a(om_v.value)]          &&
                    ch_p(')')
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(om_type);


            om_mod_date =
                (
                    '(' &&
                        str_p("ModificationDate")   [assign_a(om_k)]       && 
                        date                        [assign_a(om_v.value)] &&
                    ch_p(')')
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(om_mod_date);




            omd_component = (edt | cdt | cls | interaction | note | rspace);





        /**
        *******************************************
            ENUMERATED DATA TYPES
        *******************************************
        **/
            edt =
                (
                    ch_p('(') &&
                        str_p("EnumeratedDataType") [&allocEnumeratedDataType]  &&
                        name                  &&
                        +
                        (
                            edt_mom           |                        
                            edt_rep           |
                            description       |
                            edt_start_value   |
                            edt_autosequence
                        )                     &&
                        + edt_enum            &&
                    ')'
                )[&commitEnumeratedDataType];
            BOOST_SPIRIT_DEBUG_NODE(edt);

            edt_mom =
                (
                    '('                                                     &&
                        str_p("MOMEnumeratedDataType")[assign_a(c_k)]       &&
                        boolean                       [assign_a(c_v.value)] &&
                    ')'
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(edt_mom);

            edt_autosequence =
                (
                    '('                                             &&
                        str_p("AutoSequence") [assign_a(c_k)]       &&
                        boolean               [assign_a(c_v.value)] &&
                    ')'
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(edt_autosequence);

            edt_start_value =
                (
                    '('                                     &&
                        str_p("StartValue") [assign_a(c_k)] &&
                        int_p               [&assign_a_int] &&
                    ')'
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(edt_start_value);

            edt_enum = 
                (
                    '('                                                             &&
                        str_p("Enumeration")[&allocEnumeratedDataTypeEnumeration]   &&
                        edt_enum_enum                                               &&
                        edt_enum_rep                                                &&
                    ')'
                )[&commitEnumeratedDataTypeEnumeration];
            BOOST_SPIRIT_DEBUG_NODE(edt_enum);

            edt_enum_enum = 
                (
                    '('                                             &&
                        str_p("Enumerator") [assign_a(sc_k)]        &&
                        escaped_string      [assign_a(sc_v.value)]  &&
                     ')'
                 )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(edt_enum_enum);

            edt_enum_rep = 
                (
                    '('                                          &&
                        str_p("Representation") [assign_a(sc_k)] &&
                        int_p                   [&assign_a_int]  &&
                     ')'
                 )[&insert_am];

        /**
        -------------------------------------------
            COMPLEX DATA TYPES
        -------------------------------------------
        **/
            cdt =
                (
                    ch_p('(') &&
                        str_p("ComplexDataType")[&allocComplexDataType]  &&
                        name                                             &&                    
                        *
                        (
                            description           ||
                            cdt_mom_complex_dt
                        )                         &&
                        +
                        (
                            cdt_complex_component
                        )                         &&
                    ch_p(')')
                )[&commitComplexDataType];
            BOOST_SPIRIT_DEBUG_NODE(cdt);

            cdt_mom_complex_dt =
                (
                    ch_p('(') &&
                        str_p("MOMComplexDataType") [assign_a(c_k)]    &&
                        boolean                     [assign_a(c_v.value)]    &&
                    ')'
                )[&insert_am];

            cdt_complex_component =
                (
                    ch_p('(')                                       &&
                        str_p("ComplexComponent")   [&allocComplexDataTypeComplexComponent]      &&
                        cdt_field_name  &&
                        +
                        (
                            data_type            ||
                            accuracy             ||
                            accuracy_condition   ||
                            units                ||
                            resolution           ||
                            cardinality          ||
                            description         
                        ) &&
                    ')'
                )[&commitComplexDataTypeComplexComponent];

            cdt_field_name =
                (
                    ch_p('(')                           &&
                        str_p("FieldName")  [assign_a(sc_k)]  &&
                        escaped_string      [assign_a(sc_v.value)]   &&
                        !note_reference                 &&
                    ')'
                )[&insert_am];





        /**
        *******************************************
            CLASSES
        *******************************************
        **/
            cls = 
                (
                    ch_p('(')                           &&
                        str_p("Class")  [&allocClass]   &&
                        id                              &&
                        name                            &&
                        *(
                            cls_ps_capabilities         ||
                            description                 ||
                            cls_momclass
                        )                               &&
                        !cls_superclass                 &&
                        *(
                            cls_attribute
                        )                               &&
                    ')'
                )[&commitClass];
            BOOST_SPIRIT_DEBUG_NODE(cls);

            cls_ps_capabilities =
                (
                    ch_p('(')                               &&
                        str_p("PSCapabilities") [assign_a(c_k)]   &&
                        (
                            str_p("PS")                     ||
                            "N"                             ||
                            "S"                             ||
                            "P"
                        )[assign_a(c_v.value)]                      &&
                        !note_reference                     &&
                    ')'
                )[&insert_am];
             BOOST_SPIRIT_DEBUG_NODE(cls_ps_capabilities);




            cls_attribute =
                (
                    ch_p('(')                           &&
                        str_p("Attribute")  [&allocClassAttribute]  &&
                        name                            &&
                        +(
                            data_type                   ||
                            cardinality                 ||
                            description                 ||
                            units                       ||
                            resolution                  ||
                            accuracy                    ||
                            accuracy_condition          ||
                            delivery_cat                ||
                            msg_ordering                ||
                            routing_space               ||
                            cls_update_type             ||
                            cls_update_cond             ||
                            cls_transfer_accpt          ||
                            cls_update_reflect          
                        )                               &&
                        !note_reference                 &&
                    ')'
                )[&commitClassAttribute];


            cls_update_type =
                (
                    ch_p('(')                           &&
                        str_p("UpdateType") [assign_a(sc_k)]   &&
                        (
                            str_p("Static")             ||
                            "Periodic"                  ||
                            "Conditional"
                        )                   [assign_a(sc_v.value)]   &&
                        !note_reference                 &&
                    ')'
                )[&insert_am];

            cls_update_cond =
                (
                    ch_p('(')                               &&
                        str_p("UpdateCondition")[assign_a(sc_k)]   &&
                        escaped_string          [assign_a(sc_v.value)]   &&
                        !note_reference                     &&
                    ')'
                )[&insert_am];

            cls_transfer_accpt =
                (
                    ch_p('(')                               &&
                        str_p("TransferAccept") [assign_a(sc_k)]   &&
                        (
                            str_p("TA")                     ||
                            "T"                             ||
                            "A"                             ||
                            "N"
                        )                       [assign_a(sc_v.value)]   &&
                        !note_reference                     &&
                    ')'
                )[&insert_am];
             BOOST_SPIRIT_DEBUG_NODE(cls_transfer_accpt);

            cls_update_reflect =
                (
                    ch_p('(')                               &&
                        str_p("UpdateReflect")  [assign_a(sc_k)]   &&
                        (
                            str_p("UR")                     ||
                            "U"                             ||
                            "R"
                        )                       [assign_a(sc_v.value)]   &&
                        !note_reference                     &&
                    ')'
                )[&insert_am];
             BOOST_SPIRIT_DEBUG_NODE(cls_update_reflect);


            cls_superclass =
                (
                    ch_p('(')                               &&
                        str_p("SuperClass") [assign_a(c_k)]       &&
                        uint_p              [&assign_a_int]  &&
                    ')'
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(cls_superclass);

            cls_momclass =
                (
                    ch_p('(')           &&
                        str_p("MOMClass") [assign_a(c_k)]     &&
                        boolean           [assign_a(c_v.value)]        &&
                    ')'
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(cls_momclass);



        /**
        *******************************************
            INTERACTIONS
        *******************************************
        **/
            interaction =
                (
                    ch_p('(')                                    &&
                        str_p("Interaction") [&allocInteraction] &&
                        id                                       &&
                        name                                     &&
                        *
                        (
                            description            ||
                            delivery_cat           ||
                            msg_ordering           ||
                            int_isr_type           ||
                            int_mom_inter          ||
                            routing_space   
                        )                          &&
                        *
                        (
                            int_parameter          ||
                            int_super_inter 
                        )                          &&
                    ')'
                )[&commitInteraction];
            BOOST_SPIRIT_DEBUG_NODE(interaction);


            int_isr_type =
                (
                    ch_p('(')                   &&
                        str_p("ISRType")[assign_a(c_k)]              &&
                        (
                            str_p("IS")     ||
                            "IR"            ||
                            'I'             ||
                            'S'             ||
                            'R'             ||
                            'N' 
                        )[assign_a(c_v.value)]                     &&
                        !note_reference         &&
                    ')'
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(int_isr_type);

            int_mom_inter =
                (
                    ch_p('(')                                  &&
                        str_p("MOMInteraction")[assign_a(c_k)] &&
                        boolean                [assign_a(c_v.value)] &&
                    ')'
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(int_mom_inter);

            int_super_inter =
                (
                    ch_p('(')                                    &&
                        str_p("SuperInteraction")[assign_a(c_k)] &&
                        uint_p                   [&assign_a_uint] &&
                    ')'
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(int_super_inter);

            int_parameter =
                (
                ch_p('(')                   &&
                    str_p("Parameter") [&allocInteractionParameter]  &&
                    name                    &&
                    +
                    (
                        data_type          ||
                        cardinality        ||
                        description        ||
                        units              ||
                        resolution         ||
                        accuracy           ||
                        accuracy_condition
                    )                       &&
                ')')[&commitInteractionParameter];
            BOOST_SPIRIT_DEBUG_NODE(int_parameter);




        /**
        *******************************************
            NOTES
        *******************************************
        **/

            note =
                (
                    ch_p('(')                       &&
                        str_p("Note")[&allocNote]   &&
                        note_number                 &&
                        note_text                   &&
                    ')'
                )[&commitNote];
            BOOST_SPIRIT_DEBUG_NODE(note);

            note_number =
                (
                    ch_p('(')               &&
                        str_p("NoteNumber")[assign_a(c_k)] &&
                        uint_p             [&assign_av_int] &&
                    ')'
                )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(note_number);

            note_text =
            (
                ch_p('(')                                   &&
                    str_p("NoteText")[assign_a(c_k)]       &&
                    escaped_string   [assign_a(c_v.value)] &&
                    !note_reference                         &&
                ')'
            )[&insert_am];
            BOOST_SPIRIT_DEBUG_NODE(note_text);





        /**
        *******************************************
            ROUTING SPACES
        *******************************************
        **/
            rspace =
                (
                    ch_p('(')                               &&
                        str_p("RoutingSpace")[&allocRoutingSpace] &&
                        name                                &&
                        !description                        &&
                        *rspace_dim                         &&
                    ')'
                )[&commitRoutingSpace];

            rspace_dim =
                (
                    ch_p('(')                           &&
                        str_p("Dimension")[&allocRoutingSpaceDimension] &&
                        name                            &&
                        rspace_dim_type                 &&
                        rspace_dim_min                  &&
                        rspace_dim_max                  &&
                        rspace_interval_type            &&
                        rspace_range_set                &&
                        rspace_norm_fun                 &&
                    ')'
                )[&commitRoutingSpaceDimension];

            
            rspace_dim_type =
                (
                    ch_p('(')               &&
                        str_p("DimensionType")[assign_a(sc_k)]     &&
                        escaped_string [assign_a(sc_v.value)]     &&
                        !note_reference     &&
                    ')'
                )[&insert_am];

            rspace_dim_min =
                (
                    ch_p('(')               &&
                        str_p("DimensionMinimum")[assign_a(sc_k)]  &&
                        escaped_string  [assign_a(sc_v.value)]    &&
                        !note_reference     &&
                    ')'
                )[&insert_am];

            rspace_dim_max =
                (
                    ch_p('(')               &&
                        str_p("DimensionMaximum")[assign_a(sc_k)]  &&
                        escaped_string  [assign_a(sc_v.value)]    &&
                        !note_reference     &&
                    ')'
                )[&insert_am];

            rspace_interval_type =
                (
                    ch_p('(')                               &&
                        str_p("IntervalType")[assign_a(sc_k)]   &&
                        (
                            str_p("Open")                   ||
                            "Closed"
                        )                       [assign_a(sc_v.value)]   &&
                        !note_reference                     &&
                    ')'
                )[&insert_am];

            rspace_range_set =
                (
                    ch_p('(')                               &&
                        str_p("RangeSetUnits")[assign_a(sc_k)]   &&
                        escaped_string          [assign_a(sc_v.value)]   &&
                        !note_reference                     &&
                    ')'
                )[&insert_am];

            rspace_norm_fun =
                (
                    ch_p('(')                                                   &&
                        str_p("NormalizationFunction")[assign_a(sc_k)]          &&
                        escaped_string                [assign_a(sc_v.value)]    &&
                        !note_reference                                         &&
                    ')'
                )[&insert_am];
        }

        /**
            \brief The function used to retrieve the starting rule
        **/
        ScannerRule_t const& start() const 
        {
            return file; 
        }    

private:

        ScannerRule_t file,
                      omd_component;

        ScannerRule_t object_model;
                                      
                                      

        ScannerRule_t name,
                      note_reference,
                      boolean,
                      escaped_string, 
                      date,
                      description,
                      note_ref,
                      cardinality,
                      id,
                      data_type,
                      units,
                      resolution,
                      msg_ordering,
                      delivery_cat,
                      routing_space,
                      accuracy,
                      accuracy_condition;

        ScannerRule_t om_qstring,
                      om_type,
                      om_mod_date;

        ScannerRule_t edt;

        ScannerRule_t edt_mom,
                      edt_enum,
                      edt_enum_enum,
                      edt_enum_rep,
                      edt_rep,
                      edt_start_value,
                      edt_autosequence;
                                      
        ScannerRule_t cdt;

        ScannerRule_t cdt_mom_complex_dt,
                      cdt_complex_component,
                      cdt_field_name;                                      
                                      
        ScannerRule_t cls,
                      cls_ps_capabilities,
                      cls_attribute,
                      cls_update_type,
                      cls_update_cond,
                      cls_transfer_accpt,
                      cls_update_reflect,
                      cls_superclass,
                      cls_momclass;

        ScannerRule_t interaction,
                      int_isr_type,
                      int_parameter,
                      int_super_inter,
                      int_mom_inter;

        ScannerRule_t note,
                      note_number,
                      note_text;

        ScannerRule_t rspace,
                      rspace_dim,
                      rspace_dim_min,
                      rspace_dim_max,
                      rspace_interval_type,
                      rspace_range_set,
                      rspace_dim_type,
                      rspace_norm_fun;


        ScannerRule_t header;
    };
};

}