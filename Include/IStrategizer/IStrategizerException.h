#ifndef ISTRATEGIZEREXCEPTIONS_H
#define ISTRATEGIZEREXCEPTIONS_H

#include <stdexcept>
#include <ostream>

namespace IStrategizer
{
	class ExceptionLocation
	{
	public:
		const char* Function;
		const char* File;
		int			Line;
		ExceptionLocation(const char* p_file, const char* p_function, int p_line) :
		File(p_file), Function(p_function), Line(p_line) {}
	};

#define XcptHere ExceptionLocation(__FILE__, __FUNCTION__, __LINE__)

	class Exception : public std::exception
	{
	public:
		Exception(ExceptionLocation p_location)
			: std::exception("Exception"), m_location(p_location) {}

		Exception(ExceptionLocation p_location, const char* p_pWhat)
			: std::exception(p_pWhat), m_location(p_location) {}

		void To(std::ostream& p_out);

	protected:
		ExceptionLocation	m_location;
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