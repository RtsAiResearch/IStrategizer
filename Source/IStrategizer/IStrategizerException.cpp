#include "IStrategizerException.h"
using namespace std;
using namespace IStrategizer;

void Exception::To(ostream& p_out)
{
	p_out 
		<< "[" << m_location.File
		<< " @Function: " << m_location.Function 
		<< " @Line: " << m_location.Line << "] "
		<< what() << endl;
}
