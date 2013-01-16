// vim: set et ts=4 shiftwidth=4 :

#pragma once

#include <string>

#ifdef OMD_PARSER_EXPORT
    #define OMDP_DECLSPEC __declspec(dllexport)
    #pragma message("******* Exporting OMD Parser Symbols *******")
#else
    #define OMDP_DECLSPEC __declspec(dllimport)
    #pragma message("******* Importing OMD Parser Symbols *******")
#endif


namespace omdParser
{



    /**
        For the record of my thoughts..

        The current concept of having a "logical" versus "raw" representation simply 
        doesn't make sense with dealing with protocol buffers.  Since you cannot make
        a pointer from one protocol buffer element to another, you end up having massive 
        redundancy of data when trying to create a logical represtation of the data.

        The "best" (tm) way to do this, is on the application side (not the parser side)
        with a generic utility class, awwwwwwww fuck.

        Let's do this.  It's going to be expensive, but worth it....

        Ugh.... 
    **/ 


    /**
        This namespace encompasses logical parsing of an OMD file.  Most users 
        will want to use the functions in this namespace, as the object returned
        will be laid out in a heiarchical fashion, corresponding to their 
        logical relationship to each other.

        Please see the functions in the raw namespace, if you wish to process
        the file the way that it is physically laid out in the file
    **/
    namespace logical
    {
        //OMDP_DECLSPEC 
        //    unsigned int getDataFromRaw(
        //        const void * const input, 
        //        unsigned int inputLen, 
        //        void * output,
        //        unsigned int outputLen);


        /**
            \brief This function will read an OMD-formatted file.

            \param[in] fileName The name of the file which will be read

            \returns true if the function was successful, false if 
                the file does not exist, or an error occurred in parsing.

            \note After this function is called sucessfully, the internal
                state will contain a Google ProtocolBuffer with a 
                representation of the parsed file
        **/
        OMDP_DECLSPEC bool readFile(const char* fileName);

        /**
            \brief This function is used to get the raw representation of
            the OMD file.  The data is serialized using the ProtocolBuffers
            definition.

            \param[out] data The location where the data will be written
            \param[out] dataLen The amount of memory pointed to by data

            \returns The number of bytes written to data
        **/
        OMDP_DECLSPEC unsigned int getData(void* data, const unsigned int dataLen);

        /**
            \brief This function is used to determine how much data will be 
            returned by getData

            \returns The size, in bytes, of the amount of data that a subsequent
                call to getData will return
        **/
        OMDP_DECLSPEC unsigned int getDataSize();
    }

    namespace raw
    {
        OMDP_DECLSPEC void setDebug(const bool& dbg);

        /**
            \brief This function will parse an OMD file from a "C" style
                null-delimited string.

            \param[in] data A "C" style null-delmited string which is the 
                OMD-formatted data to be parsed.

            \returns true if the function was successful, false if 
                an error occurred in parsing.

            \note After this function is called sucessfully, the internal
                state will contain a Google ProtocolBuffer with a 
                representation of the parsed file.  This data can be 
                retrieved using getDataSize and getData
        **/
        OMDP_DECLSPEC bool read(const char* data);
        
        /**
            \brief This function will read an OMD-formatted file.

            \param[in] fileName The name of the file which will be read

            \returns true if the function was successful, false if 
                the file does not exist, or an error occurred in parsing.

            \note After this function is called sucessfully, the internal
                state will contain a Google ProtocolBuffer with a 
                representation of the parsed file
        **/
        OMDP_DECLSPEC bool readFile(const char* fileName);

        

        /**
            \brief This function is used to get the raw representation of
            the OMD file.  The data is serialized using the ProtocolBuffers
            definition.

            \param[out] data The location where the data will be written
            \param[in] dataLen The size of the buffer point to by data.

            \returns The number of bytes actually written to data
        **/
        OMDP_DECLSPEC unsigned int getData(void* data, const unsigned int dataLen);


        /**
            \brief This function is used to determine how much data will be 
            returned by getData

            \returns The size, in bytes, of the amount of data that a subsequent
                call to getData will return
        **/
        OMDP_DECLSPEC int getDataSize();

    }

}
