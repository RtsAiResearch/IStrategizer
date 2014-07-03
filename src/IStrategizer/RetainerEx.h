#ifndef RETAINEREX_H
#define RETAINEREX_H

#include "AbstractRetainer.h"
#include <string>

namespace IStrategizer
{
    const std::string g_CaseBasePath = "IStrategizerEx.cb";

    class RetainerEx : public AbstractRetainer
    {
    private:
        std::string _caseBasePath;
        bool _caseBaseLoaded;

    protected:
        void ExecuteCommand(const char* p_cmd);

    public:
        RetainerEx(std::string p_caseBasePath);
        void ReadCaseBase();
        void Flush();
        void Retain(CaseEx* p_case);
        ~RetainerEx();
    };
}
#endif // RETAINEREX_H
