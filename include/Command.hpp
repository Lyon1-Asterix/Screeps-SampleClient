#ifndef Command_Hpp
#define Command_Hpp

#include <string>
#include "nlohmann/json.hpp"
#include "ScreepsApi/ApiManager.hpp"

class ArgumentParser
{
public:
    typedef nlohmann::json Arguments;
    ArgumentParser (nlohmann::json defs);
    void defaultArgs ( Arguments& args);
    Arguments parseArgs ( int& index, int argc, char** argv );
    Arguments parseArg ( int& index, int argc, char** argv );
    virtual void error (std::string error);
    virtual void usage ();
    nlohmann::json m_definition;
    nlohmann::json m_short;
    nlohmann::json m_long;
};

class Command : public ArgumentParser
{
public:
    Command (nlohmann::json defs,nlohmann::json opts);
    virtual bool process ( std::shared_ptr < ScreepsApi::Api > client, Arguments args ) = 0;
    virtual void usage ();
    nlohmann::json m_command;
};

class Program
{
public:
    class ProgramData {};
    Program ( nlohmann::json mainOptions ) {}
    void addCommand ( nlohmann::json def, nlohmann::json options, std::function<bool()> callback) {}
};

#endif
