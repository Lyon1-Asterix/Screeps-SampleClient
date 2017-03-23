#include "Api-V12/Data.hpp"

namespace V12 {

void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}


CodeModule::CodeModule ()
{
    m_initialized = false;
}
CodeModule::CodeModule ( std::string name, std::string code )
{
    m_name = name;
    m_content = code; 
    m_initialized = true;
}

CodeBranch::CodeBranch ()
{
    m_initialized = false;
}
void CodeBranch::FromJSON ( nlohmann::json data )
{
    m_branch = data["branch"].get<std::string> ();
    for (nlohmann::json::iterator it = data [ "modules" ].begin(); it != data [ "modules" ].end(); ++it) {
        std::string name = it.key();
        std::string code = it.value();
        ReplaceStringInPlace ( code, "\\n", "\n" );
        m_modules [ name ] = CodeModule ( name, code );
    }
    m_initialized = true;
}
void CodeBranch::ToJSON ( nlohmann::json& data )
{
    data["branch"] = m_branch;
    for (auto it : m_modules ) {
        std::string name = it.first;
        std::string code = it.second.m_content;
        data [ "modules" ] [ name ] = code;
    }
}

/*
void CodeBranch::Save ( std::string directory )
{
    std::string dir = directory + "/" + m_branch;
    for ( auto it : m_modules )
    {
        std::string fname = dir + "/" + it.first + ".js";
        std::ofstream t( fname.c_str () );
        t << it.second.m_content.c_str ();
        std::clog << it.first << " saved in " << fname << std::endl;
    }
}

void CodeBranch::Load ( std::string directory )
{
    std::string dir = directory + "/" + m_branch;
    for ( auto it : m_modules )
    {
        std::string fname = dir + "/" + it.first + ".js";
        std::ifstream t( fname.c_str () );
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::string code = buffer.str ();
        m_modules[ it.first ].m_content = code;
    }
}
*/
Code::Code ()
{
    m_initialized = false;
}
void Code::FromJSON ( nlohmann::json data )
{
    m_initialized = true;
}
void Code::ToJSON ( nlohmann::json& data )
{
}

Badge::Badge ()
{
    m_initialized = false;
}
void Badge::FromJSON ( nlohmann::json data )
{
}
void Badge::ToJSON ( nlohmann::json& data )
{
}
/*
Badge::Badge ( nlohmann::json data )
{
    m_color1 = data["color1"].get<std::string>();
    m_color2 = data["color2"].get<std::string>();
    m_color3 = data["color3"].get<std::string>();
    m_flip = data["flip"].get<bool>();
    m_param = data["param"].get<int>();
    m_type = data["type"].get<int>();
    m_initialized = true;
}
*/

Steam::Steam ()
{
    m_initialized = false;
}
void Steam::FromJSON ( nlohmann::json data )
{
}
void Steam::ToJSON ( nlohmann::json& data )
{
}
/*
Steam::Steam ( nlohmann::json data )
{
    m_id = data["id"].get<std::string>();
    m_initialized = true;
}
*/

User::User ()
{
    m_initialized = false;
}
void User::FromJSON ( nlohmann::json data )
{
}
void User::ToJSON ( nlohmann::json& data )
{
}
/*
User::User ( nlohmann::json data )
{
    m_username = data["username"].get<std::string>();
    m_email = data["email"].get<std::string>();
    m_id = data["_id"].get<std::string>();
    m_badge = Badge ( data["badge"] );
    m_steam = Steam ( data["steam"] );
    m_initialized = true;
}
*/

Room::Room ()
{
    m_initialized = false;
}
void Room::FromJSON ( nlohmann::json data )
{
}
void Room::ToJSON ( nlohmann::json& data )
{
}
/*
Room::Room ( nlohmann::json data )
{
    m_room = data["room"].get<std::string>();
    m_terrain = data["terrain"].get<std::string>();
    m_type = data["type"].get<std::string>();
    m_id = data["_id"].get<std::string>();
    m_initialized = true;
}
*/

Data* Data::m_instance = NULL;

Data& Data::Get () {
    if ( m_instance == NULL ) m_instance = new Data;
    return *m_instance;
}
bool Data::isUserModified () { return m_userModified; }
void Data::setUser (nlohmann::json data) { m_user.FromJSON ( data ); m_userModified = true; }
const User& Data::getUser () { m_userModified = false; return m_user; }
bool Data::isRoomModified () { return m_roomModified; }
void Data::setRoom (nlohmann::json data) { /*m_inspectedRoom = data;*/ m_room.FromJSON ( data ); m_roomModified = true; }
const Room& Data::getRoom () { m_roomModified = false; return m_room; }

}
