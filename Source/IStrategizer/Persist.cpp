#include "Persist.h"
#include <sstream>
//----------------------------------------------------------------------------
Persist& Persist::Instance()
{
    static Persist _instance;
    return _instance;
}
//----------------------------------------------------------------------------
void Persist::WriteToFile(string p_filePath, vector<pair<string, string>> p_data)
{
    fstream m_pen(p_filePath.c_str(), ofstream::out);

    for (vector<pair<string, string>>::const_iterator m_iterator = p_data.begin();
        m_iterator != p_data.end(); m_iterator++)
    {
        string m_line  = (*m_iterator).first.c_str();
        m_line += ':';
        m_line += (*m_iterator).second.c_str();
        m_line += '\n';

        m_pen.write(m_line.c_str(), m_line.size()* sizeof(char));
    }

    m_pen.close();
}
//----------------------------------------------------------------------------
void Persist::WriteToFile(string p_filePath, vector<string> p_data)
{
    fstream m_pen(p_filePath.c_str(), ofstream::out);

    for (vector<string>::const_iterator m_iterator = p_data.begin();
        m_iterator != p_data.end(); m_iterator++)
    {
        string m_line  = (*m_iterator).c_str();
        m_line += '\n';

        m_pen.write(m_line.c_str(), m_line.size()* sizeof(char));
    }

    m_pen.close();
}
//----------------------------------------------------------------------------
void Persist::AppendToFile(string p_filePath, string p_data)
{
    fstream m_pen(p_filePath.c_str(), ofstream::out | ofstream::app);

    string m_line  = p_data.c_str();
    m_line += '\n';

    m_pen.write(m_line.c_str(), m_line.size()* sizeof(char));

    m_pen.close();
}
//----------------------------------------------------------------------------
void Persist::WriteToFile(string p_filePath, string p_data)
{
    fstream m_pen(p_filePath.c_str(), ofstream::out | ofstream::trunc);

    string m_line  = p_data.c_str();
    m_line += '\n';

    m_pen.write(m_line.c_str(), m_line.size()* sizeof(char));

    m_pen.close();
}
//----------------------------------------------------------------------------
string Persist::ReadFile(string p_filePath)
{
    fstream m_eye(p_filePath.c_str(), ofstream::in);
    stringstream m_file;
    string  m_line;
    //char*   m_line = new char[1024];

    while (!m_eye.eof())
    {
        //m_eye.getline(m_line, sizeof(m_line));
        //m_file += m_line;
        //m_file += '\n';
        getline(m_eye, m_line, '\n');

        if(m_line.size() == 0)
            continue;

        m_file << m_line << '\n';
    }

    m_eye.close();
    return m_file.str();
}


