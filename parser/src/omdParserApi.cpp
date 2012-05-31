#include <string>
#include <iostream>
#include <fstream>
#include <boost/scoped_array.hpp>
#include <set>



#include "omdGrammar.h"
#include "omdParserApi.h"
#include "parseActions.h"


namespace omdParser
{

#if 0
    namespace logical
    {
        OmdFile root;
        ObjectModel* l_om;
               
        typedef std::map<int, Class*> intClassMap_t;
        typedef std::map<int, Interaction*> intInterMap_t;
        typedef std::map<int, Note*> intNoteMap_t;
        typedef std::map<std::string, EnumeratedDataType*> strEdtMap_t;
        typedef std::map<std::string, ComplexDataType*> strCdtMap_t;
        typedef std::map<std::string, RoutingSpace*> rsMap_t;

        // These are our destination maps
        intClassMap_t dest_classMap;
        intInterMap_t dest_intMap;
        intNoteMap_t  dest_noteMap;
        strEdtMap_t   dest_edtMap;
        strCdtMap_t   dest_cdtMap;
        rsMap_t       dest_rsMap;


        void addClassItem(const int& key, Class& val, intClassMap_t& classMap)
        {
            // If we've already added this item, return
            if(dest_classMap.find(key) != dest_classMap.end())
            {
                return;
            }



            // Add ourselves
            Class* c = l_om->add_classes();
            dest_classMap[key] = c;
        }



        // These are our source maps
        intClassMap_t classMap;
        intInterMap_t intMap;
        intNoteMap_t  noteMap;
        strEdtMap_t   edtMap;
        strCdtMap_t   cdtMap;
        rsMap_t       rsMap;

        void loadMaps(ObjectModel* om)
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
                OmdComponent* oc = om->mutable_omd_component(j);

                // Place the components in their corresponding maps for lookup
                switch(oc->type())
                {
                    case omdParser::OmdComponent_ComponentType_class_type:
                    {
                        classMap[oc->class_().id().value()] = oc->mutable_class_();
                        break;
                    }
                    case omdParser::OmdComponent_ComponentType_interaction_type:
                    {
                        intMap[oc->interaction().id().value()] = oc->mutable_interaction();
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


        OMDP_DECLSPEC bool readFile(const char* fileName)
        {
            // Parse the file raw, and bail out if it isn't right
            bool retValue = omdParser::raw::readFile(fileName);

            if(!retValue)
            {
                return retValue;
            }



            

            // Now, we iterate through all of the items we parsed, and put them in the
            // corresponding map

            // Yes, we want a copy here, not a reference...
            OmdFile omdFile = getRoot();

        

            // Loop through the object models
            assert(2 > omdFile.object_model_size());    // We don't handle more than one object model yet
            for(int i = 0; i < omdFile.object_model_size(); ++i)
            {


                ObjectModel* om = omdFile.mutable_object_model(i);

                // Create our logical object model
                l_om = root.add_object_model();
                l_om->set_view_type(omdParser::ObjectModel_ViewType_logical);

                // Load up our lookup maps
                loadMaps(om);




                // Sweet.  So, now we have a lookup to every item based on their unique
                // identifying feature.  Whether that's a numerical ID, or a textual name.
                // Lets begin to build our logical data representations.

                // Classes first:
                for(intClassMap_t::iterator iter = classMap.begin();
                    iter != classMap.end();
                    ++iter)
                {
                    addItem<Class>(iter->first, *(iter->second), classMap, classSet);
                }
                

            }


            return true;
        }

        
    }
#endif

	namespace raw
	{
        /**
            typedef of a boost scoped array
        **/
		typedef boost::scoped_array<char> CharArrayPtr_t;

        /**
            typedef of a file iterator for a parse info object
        **/
        typedef boost::spirit::classic::parse_info< boost::spirit::classic::file_iterator <char> > parseInfo;

        /**
            Declare our OMDGrammar boost::spirit object
        **/
		static omdParser::OMDGrammar omdg;

		static std::string raw_output;

        static bool debugEnabled = false;


        OMDP_DECLSPEC void setDebug(const bool& dbg)
        {
            debugEnabled = dbg;
        }

		OMDP_DECLSPEC bool readFile(const char* fileName)
		{

            assert(fileName);

            bool retValue = false;

            if(!debugEnabled)
            {
                disableCerr();
            }

            // Reset our internal variables
			reset();
			raw_output.clear();			

			// Open up the file
			std::ifstream input(fileName, std::istream::in);
			if(!input.is_open())
			{
				std::cout << "Could not open " << fileName << ".  Exiting..." << std::endl;
				return retValue;
			}

			// Get the length of the file        
			input.seekg(0, std::ios::end);
			int length = (int)(input.tellg());
			input.seekg(0, std::ios::beg);

			// Add one to length for the end of string null
			length +=1;

			// Read the file into our buffer
			CharArrayPtr_t buffer_(new char[length]);
			memset(buffer_.get(), 0, length);
			input.read(buffer_.get(), length);
			input.close();
	        
            // Read the string
            retValue = read(buffer_.get());

            if(!debugEnabled)
            {
                enableCerr();
            }

            return retValue;
        }

        OMDP_DECLSPEC bool read(const char* data)
        {
            assert(data);

            if(!debugEnabled)
            {
                disableCerr();
            }

            bool retValue = false;

			// Parse the file, and check to see if it succeeded.
            parseInfo pi = boost::spirit::classic::parse(data, omdg, boost::spirit::classic::space_p);
			retValue = pi.full;

			// If we didn't get a full parse, throw away anything parsed
			if(!retValue)
			{
				reset();
				raw_output.clear();
			}
			else
			{
                const OmdFile &omdFile = getRoot();

                try
                {
                    retValue = omdFile.SerializeToString(&raw_output);
                }
                catch(google::protobuf::FatalException& e)
                {
                    std::cout << e.message() << std::endl;
                    retValue = false;
                }
			}

            // Re-enable cerr
            if(!debugEnabled)
            {
                enableCerr();
            }

			return retValue;
		}

		unsigned int getDataSize()
		{
			return raw_output.length();
		}


		unsigned int getData(void* data, const unsigned int dataLen)
		{
            // Verify the passed-in pointer
			if(NULL == data)
			{
				return 0;
			}

            // Copy the data to the destination.
			unsigned int out_bytes = std::min<unsigned int>(raw_output.size(), dataLen);
			memcpy(data, raw_output.c_str(), out_bytes);

            // Return the amount of data written
			return out_bytes;
		}

	}
}