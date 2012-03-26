#ifndef OBJECTFORMATTER_H
#include "ObjectFormatter.h"
#endif

#include <fstream>
#include <sstream>
#include <cassert>

#ifndef FILEMANAGER_H
#include "FileManager.h"
#endif

#ifndef TYPETABLE_H
#include "TypeTable.h"
#endif

#ifndef TYPERESOLVER_H
	#include "TypeResolver.h"
#endif

string g_TypeInfoPath;
string g_TypeLexerFilePath;
string g_TypeNamesFilePath;
string g_SerializationTag;

ObjectFormatter::ObjectFormatter() : m_lexer(NULL), m_parser(NULL), m_buffer(NULL)
{
    g_TypeInfoPath                  = "TypeInfo\\";
    g_TypeNamesFilePath             = g_TypeInfoPath + "TypeNames.list";
    g_TypeLexerFilePath             = g_TypeInfoPath + "TypeLexer.dfa";
    g_SerializationTag              = "///> [Serializable]";

    m_headerNameMap["class"]        = HATTR_Class;
    m_headerNameMap["type"]         = HATTR_Type;
    m_headerNameMap["parent"]       = HATTR_Parent;
    m_headerNameMap["alias"]        = HATTR_Alias;

    InitializeParsing();
}
//----------------------------------------------------------------------------------------------
ObjectFormatter::~ObjectFormatter()
{
    Toolbox::MemoryClean(m_lexer);
    Toolbox::MemoryClean(m_parser);
} 
//----------------------------------------------------------------------------------------------
void ObjectFormatter::WriteTypeTable(const string& p_sourceCodeDir)
{
    ReadSourceFiles(p_sourceCodeDir.c_str());

    TypeTable typeTable;
    for(vector<string>::iterator fileNameItr = m_sourceFiles.begin();
        fileNameItr != m_sourceFiles.end();
        ++fileNameItr)
    {
        ExtractTypes(*fileNameItr, typeTable);
    }

    g_TypeResolver.Resolve(typeTable);

    for(TypeTable::iterator typeItr = typeTable.begin();
        typeItr != typeTable.end();
        ++typeItr)
    {
        if(typeTable[typeItr->first].IsAlias)
            continue;

        typeTable[typeItr->first].Write(g_TypeInfoPath);
        m_typeNames.push_back(typeItr->first);
    }

    WriteTypeNames();
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::ReadSourceFiles(const string& p_sourceCodeDir)
{
    m_sourceFiles = g_FileManager.GetFiles(p_sourceCodeDir.c_str(), ".h");
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::InitializeParsing()
{
    m_lexer     = new LexicalAnalyzer(g_TypeLexerFilePath);
    m_parser    = new TypeDeclarationParser(m_lexer);
    m_buffer    = new CharacterBuffer();
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::ExtractTypes(string& p_sourceFileName, TypeTable& p_typeTable)
{
    string      line;
    string      format;
    fstream     eye;
    TypeNode*   lasteRoot = NULL;

    eye.open(p_sourceFileName.c_str());
    assert(eye.is_open());

    int i = -1;
    while(!eye.eof())
    {
        getline(eye, line);
        ++i;

        if(i == 0 && line != g_SerializationTag)
            break;
        else if(i == 0 && line == g_SerializationTag)
            continue;

        if(line.empty())
            continue;

        if(IsFormatLine(line, format))
        {
            ParseFormat(format, p_typeTable, lasteRoot);
        }
    }
    eye.close();
}
//----------------------------------------------------------------------------------------------
bool ObjectFormatter::IsFormatLine(string& p_line, string& p_format)
{
    bool isFormat = false;
    stringstream stream;
    int state = 0;
    int cursor = -1;
    char c;

    stream.unsetf(ios::skipws);
    stream.str(p_line);

    while(stream >> c)
    {
        cursor++;
        switch(state)
        {
        case 0:
            if(c == '/') state = 1;
            else if(c == ' ' || c == '\t') state = 0;
            else state = 5;
            break;
        case 1:
            if(c == '/') state = 2;
            else state = 5;
            break;
        case 2:
            if(c == '/') state = 3;
            else state = 5;
            break;
        case 3:
            if(c == '>') state = 4;
            else state = 5;
            break;
        case 4:
            p_format = p_line.substr(cursor, p_line.size() - cursor);
            return true;
        case 5:
            return false;
        }
    }
    return false;
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::ParseFormat(string& p_formatLine, TypeTable& p_typeTable, TypeNode*& p_lastTypeRoot)
{
    Attributes  attributes;

    GetAttributes(p_formatLine, attributes);

    if (attributes.find(HATTR_Type) != attributes.end())
    {
        GetMemberNode(attributes, p_typeTable, p_lastTypeRoot);
    }
    else if(attributes.find(HATTR_Class) != attributes.end())
    {
        GetNewTypeNode(attributes, p_typeTable, p_lastTypeRoot);
    }
    else if(attributes.find(HATTR_Parent) != attributes.end())
    {
        GetParentNode(attributes, p_typeTable, p_lastTypeRoot);
    }
    else if(attributes.find(HATTR_Alias) != attributes.end())
    {
        GetAliasNode(attributes,  p_typeTable, p_lastTypeRoot);
    }
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::GetAttributes(string& p_line, Attributes& p_attributes)
{
    vector<string> pairs;
    vector<string> tokens;

    Split(p_line, ' ', pairs);
    assert(pairs.size() >= 1);

    for(int i = 0, size = pairs.size(); i < size; ++i)
    {
        string& attribute = pairs[i];

        tokens.clear();
        Split(attribute, '=', tokens);
        // assuming a key=value structure
        assert(tokens.size() == 2);

        string& key = tokens[0];
        string& value = tokens[1];

        HeaderAttribute attr = m_headerNameMap[key];
        assert(p_attributes.find(attr) == p_attributes.end());
        p_attributes[attr] = value;
    }
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::Split(string& p_str, char p_delim, vector<string>& p_tokens)
{
    stringstream str;
    str.str(p_str);
    string token;

    while(getline(str, token, p_delim))
    {
        if(token.empty())
            continue;
        p_tokens.push_back(token);
    }
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::GetMemberNode(Attributes& p_attributes, TypeTable& p_typeTable, TypeNode*& p_lastTypeRoot)
{
    assert(p_attributes.find(HATTR_Type) != p_attributes.end());

    Toolbox::GetCharacterBuffer(p_attributes[HATTR_Type], m_buffer);
    m_lexer->SetCodeBuffer(m_buffer);

    m_parser->Reset();
    m_parser->Parse();

    TypeNode* specializedType = m_parser->TypeGraph();
    specializedType->SetTemplateArguments(p_lastTypeRoot->TemplateArguments);
    p_lastTypeRoot->Children.push_back(TypeChild(specializedType));
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::GetNewTypeNode(Attributes& p_attributes,  TypeTable& p_typeTable, TypeNode*& p_lastTypeRoot)
{
    TypeNode* newType = NULL;

    Toolbox::GetCharacterBuffer(p_attributes[HATTR_Class], m_buffer);
    m_lexer->SetCodeBuffer(m_buffer);

    m_parser->Reset();
    m_parser->Parse();

    newType = m_parser->TypeGraph();
    newType->SetTemplateArguments(newType->TemplateArguments);

    /*vector<string> parents;
    if(p_attributes.find(HATTR_Parent) != p_attributes.end())
    {
        Split(p_attributes[HATTR_Parent], ',', parents);
    }
    */

    assert(p_typeTable.find(newType->UserDefinedType) == p_typeTable.end());

    TypeData& tableEntry = p_typeTable[newType->UserDefinedType];
    tableEntry.TypeGraph = newType;

    p_lastTypeRoot = newType;
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::GetParentNode(Attributes& p_attributes,  TypeTable& p_typeTable, TypeNode*& p_lastTypeRoot)
{
    assert(p_attributes.find(HATTR_Parent) != p_attributes.end());

    TypeNode* parent = NULL;

    Toolbox::GetCharacterBuffer(p_attributes[HATTR_Parent], m_buffer);
    m_lexer->SetCodeBuffer(m_buffer);

    m_parser->Reset();
    m_parser->Parse();

    parent = m_parser->TypeGraph();
    parent->SetTemplateArguments(p_lastTypeRoot->TemplateArguments);

    TypeData& childTableEntry = p_typeTable[p_lastTypeRoot->UserDefinedType];
    childTableEntry.Parents.push_back(parent);
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::GetAliasNode(Attributes& p_attributes,  TypeTable& p_typeTable, TypeNode*& p_lastTypeRoot)
{
    Toolbox::GetCharacterBuffer(p_attributes[HATTR_Alias], m_buffer);
    m_lexer->SetCodeBuffer(m_buffer);

    m_parser->Reset();
    m_parser->Parse();

    TypeNode* aliasNode = m_parser->TypeGraph();

    assert(p_typeTable.find(aliasNode->UserDefinedType) == p_typeTable.end());

    if(p_lastTypeRoot != NULL && !p_lastTypeRoot->TemplateArguments.empty())
        aliasNode->SetTemplateArguments(p_lastTypeRoot->TemplateArguments);

    TypeData& tableEntry = p_typeTable[aliasNode->UserDefinedType];
    tableEntry.TypeGraph = aliasNode;
    tableEntry.IsAlias = true;
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::WriteTypeNames()
{
    fstream pen;
    int     size = m_typeNames.size();
    char    buffer[MaxTypeNameLength + 1];

    pen.open(g_TypeNamesFilePath.c_str(), ios::binary | ios::out);
    assert(pen.is_open());

    pen.write(reinterpret_cast<char*>(&size), sizeof(int));

    for(int i = 0; i < m_typeNames.size(); ++i)
    {
        assert(m_typeNames[i].size() <= MaxTypeNameLength);
        strcpy(buffer, m_typeNames[i].c_str());
        pen.write(reinterpret_cast<char*>(buffer), sizeof(char) * (MaxTypeNameLength + 1));
    }

    pen.close();
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::ReadTypeTable(TypeTable& p_typeTable)
{
    ReadTypeNames();
    for(vector<string>::iterator typeNameItr = m_typeNames.begin();
        typeNameItr != m_typeNames.end();
        ++typeNameItr)
    {
        TypeData data;
        data.Read(g_TypeInfoPath + *typeNameItr);
        p_typeTable[data.TypeGraph->UserDefinedType] = data;
    }
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::ReadTypeNames()
{
    fstream eye;
    int     size = m_typeNames.size();
    char    buffer[MaxTypeNameLength + 1];

    eye.open(g_TypeNamesFilePath.c_str(), ios::binary | ios::in);
    assert(eye.is_open());

    eye.read(reinterpret_cast<char*>(&size), sizeof(int));

    m_typeNames.resize(size);
    for(int i = 0; i < m_typeNames.size(); ++i)
    {
        eye.read(reinterpret_cast<char*>(buffer), sizeof(char) * (MaxTypeNameLength + 1));
        m_typeNames[i] = buffer;
    }

    eye.close();
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::FinalizeTypeTable(TypeTable& p_typeTable, ObjectTable& p_objectTable)
{
    // 1. Collect template type specialization
    CollectTemplateSpecialization(p_typeTable, p_objectTable);
}
//----------------------------------------------------------------------------------------------
void ObjectFormatter::CollectTemplateSpecialization(TypeTable& p_typeTable, ObjectTable& p_objectTable)
{
    for(ObjectTable::iterator objItr = p_objectTable.begin();
        objItr != p_objectTable.end();
        ++objItr)
    {
        string typeName = objItr->second->TypeName();
        if(p_typeTable.find(typeName) != p_typeTable.end())
        {
            TypeData& typeTemplate = p_typeTable[typeName];
            // type has template arguments and is for sure it is specialized
            if(!typeTemplate.TypeGraph->TemplateArguments.empty())
            {
                // 1. extract object specialization info
                string specializedTypeName = g_ObjectFactory.FromCName(objItr->second->CName());
                assert(p_typeTable.find(specializedTypeName) == p_typeTable.end());

                // 2. parse specialization string and get specialized type graph
                Toolbox::GetCharacterBuffer(specializedTypeName, m_buffer);
                m_lexer->SetCodeBuffer(m_buffer);
                m_parser->Reset();
                m_parser->Parse();

                // 3. specialize a copy of the type template using the specialization info
                g_TypeResolver.Specialize(m_parser->TypeGraph(), typeTemplate, p_typeTable);

                delete m_parser->TypeGraph();
            }
        }
    }
}
