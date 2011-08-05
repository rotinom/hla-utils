

namespace omdParser
{
    namespace logical
    {
		typedef boost::scoped_array<char> CharArrayPtr;
		typedef boost::spirit::parse_info< boost::spirit::file_iterator <char> > parseInfo;

		static omdParser::OMDGrammar omdg;
		static std::string raw_output;

		OMDP_DECLSPEC bool readFile(const char* fileName)
		{
			// Disable std::cout
			std::streambuf* old_rdbuff = std::cout.rdbuf();
			std::cout.rdbuf(NULL);

			Private::reset();
			raw_output.clear();

			bool retValue = false;
			CharArrayPtr buffer_;

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
			buffer_.reset(new char[length]);
			memset(buffer_.get(), 0, length);
			input.read(buffer_.get(), length);
			input.close();
	           
			// Parse the file, and check to see if it succeeded.
			parseInfo pi = boost::spirit::parse(buffer_.get(), omdg, boost::spirit::space_p);
			retValue = pi.full;

			// If we didn't get a full parse, throw away anything parsed
			if(!retValue)
			{
				Private::reset();
				raw_output.clear();
			}
			else
			{
				Private::g_root->SerializeToString(&raw_output);
			}

			// Re-enable cout
			std::cout.rdbuf(old_rdbuff);
			old_rdbuff = NULL;

			return retValue;
		}

		unsigned int getDataSize()
		{
			return raw_output.length();
		}

		unsigned int getData(void* data, const unsigned int dataLen)
		{
			if(NULL == data)
			{
				return 0;
			}

			unsigned int out_bytes = std::min<unsigned int>(raw_output.size(), dataLen);
			memcpy(data, raw_output.c_str(), out_bytes);

			return out_bytes;
		}
        unsigned int getDataFromRaw(
            const void * const input, 
            unsigned int inputLen, 
            void * output,
            unsigned int outputLen)
        {
            return true;
        }
    }
}