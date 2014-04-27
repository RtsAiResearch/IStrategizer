#ifndef ISTRATEGIZEREXCEPTIONS_H
#define ISTRATEGIZEREXCEPTIONS_H

#include <stdexcept>
#include <ostream>
#include <sstream>

namespace IStrategizer
{
    class ExceptionLocation
    {
    public:
        const char* Function;
        const char* File;
        int Line;
        ExceptionLocation(const char* p_file, const char* p_function, int p_line) :
        File(p_file), Function(p_function), Line(p_line) {}
    };

#define XcptHere ExceptionLocation(__FILE__, __FUNCTION__, __LINE__)

/*
Use DEBUG_THROW macro to control thrown exceptions behavior between really throwing it or replace it with assert
If DEBUG_ISTRATEGIZER_EXCEPTION is defined, DEBUG_THROW replaces the throw call with _ASSERTE(!<exception-string>)
*/
#define DEBUG_ISTRATEGIZER_EXCEPTION

#ifdef DEBUG_ISTRATEGIZER_EXCEPTION
#define DEBUG_THROW(X) _ASSERTE(!#X);
#else
#define DEBUG_THROW(X) throw X
#endif

    class Exception : public std::exception
    {
    public:
        Exception(ExceptionLocation p_location)
            : std::exception("Exception"), m_location(p_location) 
        {
            Init();
        }

        Exception(ExceptionLocation p_location, const char* p_pWhat)
            : std::exception(p_pWhat), m_location(p_location) 
        {
            Init();
        }

        void To(std::ostream& p_out) const;
        const char* what() const { return m_formattedXcpt.c_str(); }

    protected:
        void Init()
        {
            std::stringstream sstream;
            To(sstream);

            m_formattedXcpt = sstream.str();
        }

        ExceptionLocation m_location;
        std::string         m_formattedXcpt;
    };

    class InvalidParameterException : public Exception
    {
    public:
        InvalidParameterException(ExceptionLocation p_location)
            : Exception(p_location, "InvalidParameterException") {}
    };

    class NotImplementedException : public Exception
    {
    public:
        NotImplementedException(ExceptionLocation p_location)
            : Exception(p_location, "NotImplementedException") {}
    };

    class ItemNotFoundException : public IStrategizer::Exception
    {
    public:
        ItemNotFoundException(ExceptionLocation p_location)
            : Exception(p_location, "ItemNotFoundException") {}
    };

    class ItemAlreadyExistsException : public IStrategizer::Exception
    {
    public:
        ItemAlreadyExistsException(ExceptionLocation p_location)
            : Exception(p_location, "ItemAlreadyExistsException") {}
    };
}

#endif // ISTRATEGIZEREXCEPTIONS_H
