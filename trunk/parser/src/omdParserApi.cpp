#include <string>
#include <iostream>
#include <fstream>
#include <boost/scoped_array.hpp>



#include "omdGrammar.h"
#include "omdParserApi.h"
#include "parseActions.h"

#include "omdNormalizer.h"


namespace omdParser
{
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