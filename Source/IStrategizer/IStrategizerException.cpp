#include "IStrategizerException.h"
using namespace std;
using namespace IStrategizer;

void Exception::To(ostream& p_out) const
{
	p_out 
		<< "[" << m_location.File
		<< " @Function: " << m_location.Function 
		<< " @Line: " << m_location.Line << "] "
		<< std::exception::what() << endl;
}


