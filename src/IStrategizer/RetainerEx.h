#ifndef RETAINEREX_H
#define RETAINEREX_H

#include "AbstractRetainer.h"
#include <string>

namespace IStrategizer
{
#define  CASEBASE_FILENAME "IStrategizerEx.cb"

    class RetainerEx : public AbstractRetainer
    {
    private:
        std::string m_caseBasePath;
        bool m_caseBaseLoaded;

    protected:
        void ExecuteCommand(const char* p_cmd);

    public:
        RetainerEx();
        void ReadCaseBase();
        void Flush();
        void Retain(_In_ CaseEx* pCase);
        ~RetainerEx();
    };
}
#endif // RETAINEREX_H
