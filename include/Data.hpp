#ifndef Screeps_Data_Hpp
#define Screeps_Data_Hpp

#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"

namespace V12 {

class CodeModule
{
public:
    CodeModule ();
    CodeModule ( std::string name, std::string code );
    std::string m_name;
    std::string m_content;
    bool m_initialized;
};

class CodeBranch
{
public:
    CodeBranch ();
    void FromJSON ( nlohmann::json data );
    void ToJSON ( nlohmann::json& data );
    std::string m_branch;
    std::map < std::string, CodeModule > m_modules;
    bool m_initialized;
};

class Code
{
public:
    Code ();
    void FromJSON ( nlohmann::json data );
    void ToJSON ( nlohmann::json& data );
    std::map < std::string, CodeBranch > m_branches;
    bool m_initialized;
};

class Badge
{
public:
    Badge ();
    void FromJSON ( nlohmann::json data );
    void ToJSON ( nlohmann::json& data );
    std::string m_color1;
    std::string m_color2;
    std::string m_color3;
    bool m_flip;
    int m_param;
    int m_type;
    bool m_initialized;
};

class Steam
{
public:
    Steam ();
    void FromJSON ( nlohmann::json data );
    void ToJSON ( nlohmann::json& data );
    std::string m_id;
    bool m_initialized;
};

class User
{
public:
    User ();
    void FromJSON ( nlohmann::json data );
    void ToJSON ( nlohmann::json& data );
    std::string m_username;
    std::string m_email;
    std::string m_id;
    Badge m_badge;
    Steam m_steam;
    bool m_initialized;
};

class Room
{
public:
    Room ();
    void FromJSON ( nlohmann::json data );
    void ToJSON ( nlohmann::json& data );
    std::string m_room;
    std::string m_terrain;
    std::string m_type;
    std::string m_id;
    bool m_initialized;
};

class Data
{
public:
    static Data& Get ();
    bool isUserModified ();
    void setUser ( nlohmann::json data );
    const User& getUser ();
    bool isRoomModified ();
    void setRoom ( nlohmann::json data );
    const Room& getRoom ();
protected:
    static Data* m_instance;
    /* USER */
    User m_user;
    bool m_userModified;
    /* ROOM */
    Room m_room;
    bool m_roomModified;
public:
    /* CODE */
    Code m_code;
    bool m_codeModified;
    /**/
    bool m_connected;
    /**/
    std::string m_workingDirectory;
    std::string m_server;
    int m_port;
    std::string m_socketPath;
};

}

#endif
