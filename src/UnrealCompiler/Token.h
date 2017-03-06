#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <vector>

extern int EOFTypeId;

namespace Unreal
{
    enum
    {
        TOKEN_Unrecognized = 0,
        TOKEN_Error = 1,
    };

    struct UTokenType
    {
        // the id of the token type
        int Id;

        // name representing the token type
        std::string Name;

        // token can be ignored by the lexical analyzer (comment, whitespace, etc...)
        bool IsIgnored;

        // token is unique in lexeme and is not reserved token in the same time (=, <, (,etc...)
        bool IsSpecial;

        // token should be added to the symbol table
        bool IsIdentifier;

        // token lexeme can compose a reserved work (if, then, else, etc ...)
        bool IsComposite;

        // token identifies an error
        bool IsError;

        // token identifies a reserved language keyword
        bool IsReserved;

        UTokenType(int p_id, const std::string& p_name) :
            Id(p_id), Name(p_name), IsError(false), IsSpecial(false), IsIdentifier(false), IsComposite(false), IsIgnored(false), IsReserved(false)
        {}
    };

    const int MaxTokenTypes = 128;


    struct TokenValue
    {
        // the lexeme that represents the token type pattern
        std::string      Lexeme;

        // the row in which the beginning of the lexeme resides
        int         Row;

        // the column in which the beginning of the lexeme resides
        int         Column;

        // this can be any extra 32-bit data saved with the token, such as index, pointer, etc..
        unsigned    Data;

        TokenValue(const std::string& p_lexeme, int p_row, int p_column) : Lexeme(p_lexeme), Row(p_row), Column(p_column), Data(0)
        {}
    };

    struct Token
    {
        int             TypeId;
        TokenValue*     Value;

        Token(int p_tokenTypeId, TokenValue* p_value)
        {
            TypeId = p_tokenTypeId;
            Value = p_value;
        }

        ~Token()
        {
            delete Value;
        }

        bool IsEOF() { return TypeId == EOFTypeId; }
    };
}

extern std::vector<Unreal::UTokenType*> g_TokenTypesMap;

#endif // TOKEN_H
