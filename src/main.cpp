#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include <boost/filesystem.hpp>

#include "ScreepsApi/ApiManager.hpp"
#include "ScreepsApi/Web.hpp"

#include "simple-web-server/client_http.hpp"
#include "simple-websocket-server/client_ws.hpp"

#include "nlohmann/json.hpp"

#include "Command.hpp"

namespace fs = boost::filesystem;

/*
 *
 * string utilities
 *
 */

 extern void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace);

std::string toString ( std::istream& stream )
{
    /**/
    std::string ret;
    char buffer[4096];
    while (stream.read(buffer, sizeof(buffer)))
        ret.append(buffer, sizeof(buffer));
    ret.append(buffer, stream.gcount());
    return ret;
}

/*
 *
 * encapsulation of Web::Client inside a ScreepsApi::Web::Client
 *
 */

class WebClient : public ScreepsApi::Web::Client
{
public:
    WebClient ( std::string host_port ) : m_web ( host_port ) {}
    virtual void connect ()
    {
        m_web.connect ();
    }
    virtual void close()
    {
        m_web.close ();
    }
    virtual ScreepsApi::Web::Reply request ( ScreepsApi::Web::RoutingMethod method, std::string uri, std::string content = "", ScreepsApi::Web::Header header = ScreepsApi::Web::Header () )
    {
        ScreepsApi::Web::Reply out;
        std::string meth = "";
        switch ( method )
        {
        case ScreepsApi::Web::RoutingMethod::HttpGet:
            meth = "GET";
            break;
        case ScreepsApi::Web::RoutingMethod::HttpPost:
            meth = "POST";
            break;
        default:
            break;
        }
        if ( meth != "" )
        {
            std::shared_ptr<SimpleWeb::Client<SimpleWeb::HTTP>::Response> reply = m_web.request ( meth, uri, content, header.m_data );
            for(auto it = reply->header.begin(); it != reply->header.end(); it++) {
                out.m_header.m_data[it->first] = it->second;
            }
            out.m_content = toString ( reply->content );
        }
        return out;
    }
protected:
    SimpleWeb::Client<SimpleWeb::HTTP> m_web;
};

/*
 *
 * encapsulation of Web::Client inside a ScreepsApi::Web::Client
 *
 */

void error ( std::string message )
{
    std::cout << "Error: " << message << std::endl;
    exit ( -1 );
}

nlohmann::json gServerOptions = {
        { "serverIP", {
            { "short", "s" },
            { "long", "server" },
            { "type", "string" },
            { "optional", true },
            { "help", "hostname / hostip of the private screeps server" },
            {"value", {
                { "default", "localhost" },
                { "required", true }
            } }
        } },
        { "serverPort", {
            { "short", "p" },
            { "long", "port" },
            { "type", "int" },
            { "optional", true },
            { "help", "port opened on the private screeps server" },
            { "value", {
                { "default", "21025" },
                { "required", true }
            } }
        } },
        { "username", {
            { "short", "u" },
            { "long", "username" },
            { "type", "string" },
            { "optional", false },
            { "help", "username on the server" },
            { "value", {
                { "required", true }
            } }
        } },
        { "password", {
            { "short", "w" },
            { "long", "password" },
            { "type", "string" },
            { "optional", false },
            { "help", "paswword for the account on the server" },
            { "value", {
                { "required", true }
            } }
        } }
    };

nlohmann::json gPullOptions = {
        { "branch", {
            { "short", "b" },
            { "long", "branch" },
            { "type", "string" },
            { "optional", true },
            { "help", "name of the branch to pull" },
            {"value", {
                { "default", "default" },
                { "required", true }
            } }
        } },
        { "output", {
            { "short", "o" },
            { "long", "output" },
            { "type", "path" },
            { "optional", true },
            { "help", "destination directory for code modules" },
            { "value", {
                { "default", "." },
                { "required", true }
            } }
        } },
        { "force", {
            { "short", "f" },
            { "long", "force" },
            { "type", "boolean" },
            { "optional", true },
            { "help", "force creation of destination directory or pull in non empty directory" },
            { "value", {
                { "default", false }
            } }
        } }
    };

nlohmann::json gPullCommandDef = {
        { "name", "pull" },
        { "type", "system" },
        { "processor", "Pull" },
        { "help", "retrieve code modules for specific branch on the server and save them locally" }
    };

nlohmann::json gPushOptions = {
        { "branch", {
            { "short", "b" },
            { "long", "branch" },
            { "type", "string" },
            { "optional", true },
            { "help", "name of the branch to pull" },
            {"value", {
                { "default", "default" },
                { "required", true }
            } }
        } },
        { "input", {
            { "short", "i" },
            { "long", "input" },
            { "type", "path" },
            { "optional", true },
            { "help", "input directory for code modules" },
            { "value", {
                { "default", "." },
                { "required", true }
            } }
        } },
        { "addNewFiles", {
            { "short", "a" },
            { "long", "add" },
            { "type", "boolean" },
            { "optional", true },
            { "help", "add code module for new files present in input directory" },
            { "value", {
                { "default", false }
            } }
        } },
        { "removeOldFiles", {
            { "short", "r" },
            { "long", "remove" },
            { "type", "boolean" },
            { "optional", true },
            { "help", "remove code module for files not present in input directory" },
            { "value", {
                { "default", false }
            } }
        } }
    };

nlohmann::json gPushCommandDef = {
        { "name", "push" },
        { "type", "system" },
        { "processor", "Push" },
        { "help", "retrieve code modules locally and push them on the server" }
    };

nlohmann::json gConsoleOptions = {
        { "command", {
            { "short", "c" },
            { "long", "command" },
            { "type", "string" },
            { "optional", false },
            { "help", "command to run on the server" },
            { "value", {
                { "required", true }
            } }
        } }
    };
nlohmann::json gConsoleCommandDef = {
        { "name", "console" },
        { "type", "system" },
        { "processor", "Console" },
        { "help", "send javascript code to the server" }
    };

nlohmann::json gPlaceSpawnOptions = {
        { "room", {
            { "short", "r" },
            { "long", "room" },
            { "type", "string" },
            { "optional", false },
            { "help", "name of the room where to place the spawn" },
            {"value", {
                { "required", true }
            } }
        } },
        { "posX", {
            { "short", "x" },
            { "type", "int" },
            { "optional", false },
            { "help", "X coordinate of the new spawn" },
            { "value", {
                { "required", true }
            } }
        } },
        { "posY", {
            { "short", "y" },
            { "type", "int" },
            { "optional", false },
            { "help", "Y coordinate of the new spawn" },
            { "value", {
                { "required", true }
            } }
        } }
    };

nlohmann::json gPlaceSpawnCommandDef = {
        { "name", "spawn" },
        { "type", "system" },
        { "processor", "Spawn" },
        { "help", "send javascript code to place a spawn" }
    };
class ServerOptions : public ArgumentParser
{
public:
    ServerOptions () : ArgumentParser (gServerOptions)
    {
    }
};

class Pull : public Command
{
public:
    Pull () : Command (gPullCommandDef,gPullOptions)
    {
    }
    virtual bool process ( std::shared_ptr < ScreepsApi::Api > client, Arguments args )
    {
        fs::path directory = args["output"].get<std::string> ();
        if (fs::exists(directory))
        {
            if (! fs::is_directory(directory) ) error ( "specified output is not a directory ["+directory.filename().string()+"]" );
            fs::directory_iterator it(directory), end_it;
            if ( ( it != end_it )  && ! args["force"].get<bool> () ) error ( "specified output is not empty, use -f option to force code saving" );
        }
        else
        {
            if ( ! args["force"].get<bool> () ) error ( "specified output does not exists, use -f option to force creation" );
            boost::system::error_code ec;
            if ( fs::create_directories ( directory, ec ) ) error ( ec.message () );
        }
        nlohmann::json branch = client->PullCode ( args["branch"].get<std::string> () );
        if ( branch.is_null () )
        {
            error ( "code pull request thrown an error" );
        }
        for (nlohmann::json::iterator it = branch [ "modules" ].begin(); it != branch [ "modules" ].end(); ++it)
        {
            std::string name = it.key();
            std::string code = it.value();
            ReplaceStringInPlace ( code, "\\n", "\n" );
            std::string fname = args["output"].get < std::string > () + "/" + name + ".js";
            std::ofstream stream ( fname );
            stream << code;
            stream.close ();
        }
    }
};

class Push : public Command
{
public:
    Push () : Command (gPushCommandDef,gPushOptions)
    {
    }
    virtual bool process ( std::shared_ptr < ScreepsApi::Api > client, Arguments args )
    {
        fs::path directory = args["input"].get<std::string> ();
        if (!fs::exists(directory)) error ( "specified input does not exists" );
        if (! fs::is_directory(directory) ) error ( "specified input is not a directory ["+directory.filename().string()+"]" );
        nlohmann::json branch = client->PullCode ( args["branch"].get<std::string> () );
        if ( branch.is_null () )
        {
            error ( "code pull request thrown an error" );
        }
        std::map < std::string, std::string > dirContent;
        for (auto&& x : fs::directory_iterator(directory))
        {
            if ( x.path ().extension ().string () == ".js" )
            {
                std::string stem = x.path().stem ().string();
                std::ifstream stream ( x.path ().string () );
                std::string content(std::istreambuf_iterator<char>(stream), {});
                stream.close ();
                dirContent[stem] = content;
            }
        }
        std::vector < std::string > newModules;
        std::vector < std::string > delModules;
        std::vector < std::string > updModules;
        for ( auto it : dirContent ) {
            if ( branch [ "modules" ].find ( it.first ) == branch [ "modules" ].end () ) newModules.push_back ( it.first );
            else updModules.push_back ( it.first );
        }
        for (nlohmann::json::iterator it = branch [ "modules" ].begin(); it != branch [ "modules" ].end(); ++it)
        {
            std::string name = it.key();
            if ( dirContent.find ( name ) == dirContent.end () ) delModules.push_back ( name );
        }
        for ( auto it : updModules ) std::cout << "* [" << (it) << "]" << std::endl;
        for ( auto it : newModules ) std::cout << "+ [" << (it) << "]" << std::endl;
        for ( auto it : delModules ) std::cout << "- [" << (it) << "]" << std::endl;
        std::map < std::string, std::string > modules;
        for ( auto it : updModules ) {
            modules[it]=dirContent[it];
        }
        if ( args["addNewFiles"].get<bool>() )
        {
            for ( auto it : newModules ) {
                modules[it]=dirContent[it];
            }
        }
        if ( ! args["removeOldFiles"].get<bool>() )
        {
            for ( auto it : delModules ) {
                std::string content = branch [ "modules" ][it].get < std::string > ();
                ReplaceStringInPlace ( content, "\\n", "\n" );
                modules[it]=content;
            }
        }
        bool ok = client->PushCode ( args["branch"].get<std::string> (), modules );
        if ( ! ok )
        {
            error ( "code push request thrown an error" );
        }
    }
};

class Console : public Command
{
public:
    Console () : Command (gConsoleCommandDef,gConsoleOptions)
    {
    }
    virtual bool process ( std::shared_ptr < ScreepsApi::Api > client, Arguments args )
    {
        bool ok = client->Console ( args["command"].get<std::string> () );
        if ( ! ok )
        {
            error ( "command execution thrown an error" );
        }
    }
};

class AddSpawn : public Command
{
public:
    AddSpawn () : Command (gPlaceSpawnCommandDef,gPlaceSpawnOptions)
    {
    }
    virtual bool process ( std::shared_ptr < ScreepsApi::Api > client, Arguments args )
    {
        bool ok = client->AddSpawn ( args["room"].get<std::string> (),args["posX"].get<std::string> (),args["posY"].get<std::string> () );
        if ( ! ok )
        {
            error ( "command execution thrown an error" );
        }
    }
};

Push gPushCommand;
Pull gPullCommand;
Console gConsoleCommand;
AddSpawn gAddSpawnCommand;

int main ( int argc, char** argv )
{
    int index = 1;
    ServerOptions server;
    Command::Arguments serverOptions = server.parseArgs ( index, argc, argv );

    std::shared_ptr < ScreepsApi::Web::Client > web (
        new WebClient ( serverOptions["serverIP"].get<std::string>()+":"+serverOptions["serverPort"].get<std::string>() )
    );
    ScreepsApi::ApiManager::Instance ().initialize ( web, NULL );
    std::shared_ptr < ScreepsApi::Api > client = ScreepsApi::ApiManager::Instance ().getApi ();
    bool ok = client->Signin ( serverOptions["username"], serverOptions["password"] );
    if ( ! ok ) {
        std::cerr << "Error: cannot connect/signin to the server" << std::endl;
        exit ( -1 );
    }
    while ( ! client->initialized () )
        std::this_thread::sleep_for ( std::chrono::milliseconds ( 5 ) );
    std::cout << "signed in " << ok << std::endl;
    std::string command = argv [ index ]; index ++;
    if ( command == "pull" )
    {
        Command::Arguments args = gPullCommand.parseArgs ( index, argc, argv );
        gPullCommand.process (client, args);
    }
    else if ( command == "push" )
    {
        Command::Arguments args = gPushCommand.parseArgs ( index, argc, argv );
        gPushCommand.process (client, args);
    }
    else if ( command == "console" )
    {
        Command::Arguments args = gConsoleCommand.parseArgs ( index, argc, argv );
        gConsoleCommand.process (client, args);
    }
    else if ( command == "spawn" )
    {
        Command::Arguments args = gAddSpawnCommand.parseArgs ( index, argc, argv );
        gAddSpawnCommand.process (client, args);
    }
    return 0;
}
