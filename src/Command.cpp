#include "Command.hpp"

ArgumentParser::ArgumentParser (nlohmann::json defs) :
    m_definition ( defs )
{
    ArgumentParser::Arguments::iterator it;
    for (it = m_definition.begin();
            it != m_definition.end(); ++it)
    {
        ArgumentParser::Arguments opt = it.value ();
        if (opt.find("short")!=opt.end()) m_short [ opt["short"].get<std::string> () ] = it.key ();
        if (opt.find("long")!=opt.end()) m_long [ opt["long"].get<std::string> () ] = it.key ();
    }
}

void ArgumentParser::defaultArgs ( ArgumentParser::Arguments& args)
{
    ArgumentParser::Arguments::iterator it;
    for (it = m_definition.begin();
            it != m_definition.end(); ++it)
    {
        ArgumentParser::Arguments opt = it.value ();
        if ( opt["optional"].get<bool>() && ( opt["value"].find("default") != opt["value"].end () ) )
            args[it.key ()] = opt["value"]["default"];
    }
}

ArgumentParser::Arguments ArgumentParser::parseArgs ( int& index, int argc, char** argv )
{
    ArgumentParser::Arguments out = {};
    defaultArgs ( out );
    while ( (index < argc ) && ( argv[index][0] == '-' ) )
    {
        Arguments opt = parseArg ( index, argc, argv );
        if ( opt.is_null () ) break;
        out[opt.begin ().key ()] = opt.begin ().value ();
    }
    ArgumentParser::Arguments::iterator it;
    for (it = m_definition.begin();
            it != m_definition.end(); ++it)
    {
        ArgumentParser::Arguments opt = it.value ();
        if ( ! opt["optional"].get<bool>() && ( out.find(it.key ()) == out.end () ) )
            error ( "required argument " + it.key () + " not specified" );
    }
    return out;
}

ArgumentParser::Arguments ArgumentParser::parseArg ( int& index, int argc, char** argv )
{
    ArgumentParser::Arguments out = {};
    std::string name = "";
    bool isLong = false;
    if ( index >= argc ) return out;
    if ( argv[index][0] != '-' ) return out;
    if ( argv[index][1] == '-' ) isLong = true;
    std::string option = argv[index]; option = option.substr ( isLong ? 2 : 1 );
    if ( ! isLong ) {
        if ( m_short.find ( option ) == m_short.end () ) error ( "unrecognized short argument : " + option );
        name = m_short [ option ].get<std::string> ();
    } else {
        if ( m_long.find ( option ) == m_long.end () ) error ( "unrecognized long argument : " + option );
        name = m_long [ option ].get<std::string> ();
    }
    ArgumentParser::Arguments opt = m_definition[name];
    if (opt["value"].find("required") != opt["value"].end ())
    {
        if ( index+1 >= argc ) error ( "missing argument value for : " + name );
        out[name] = argv[index+1];
        index += 2;
    }
    else
    {
        out[name] = true;
        index += 1;
    }
    return out;
}

void ArgumentParser::error (std::string error)
{
    std::cerr << "Error: " << error << std::endl;
    usage ();
    exit ( -1 );
}

void ArgumentParser::usage ()
{
    Arguments::iterator it;
    for (it = m_definition.begin();
            it != m_definition.end(); ++it)
    {
        Arguments opt = it.value ();
        std::cerr << "\t";
        if ( opt.find ( "short" ) != opt.end () ) std::cerr << "-" << opt["short"].get<std::string> ();
        std::cerr << ",";
        if ( opt.find ( "long" ) != opt.end () ) std::cerr << "--" << opt["long"].get<std::string> ();
        std::cerr << " [";
        std::cerr << opt["type"].get<std::string> ();
        std::cerr << "] : ";
        std::cerr << opt["help"].get<std::string> ();
        std::cerr << std::endl;
    }
}

Command::Command (nlohmann::json defs,nlohmann::json opts) :
    ArgumentParser ( opts ),
    m_command ( defs )
{
}

void Command::usage ()
{
    std::clog << m_command["name"].get<std::string> () << ": " << m_command["help"].get<std::string> () << std::endl;
    ArgumentParser::usage ();
}