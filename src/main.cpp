#include <iostream>
#include <string>

#include "ScreepsApi/ApiManager.hpp"
#include "ScreepsApi/Web.hpp"

#include "client_http.hpp"

/*
 *
 * encapsulation of Web::Client inside a ScreepsApi::Web::Client
 *
 */

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

class WebClient : public ScreepsApi::Web::Client
{
public:
    WebClient ( std::string host_port ) : m_web ( host_port ) {}
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

int main ( int argc, char** argv )
{
    if ( argc != 4 )
    {
        std::cerr << "Usage: " << argv [ 0 ] << " apilibrary username password" << std::endl;
        exit ( -1 );
    }
    std::shared_ptr < ScreepsApi::Web::Client > web ( new WebClient ( "localhost:21025" ) );
    
    ScreepsApi::ApiManager::Instance ().initialize ( web );

    std::shared_ptr < ScreepsApi::Api > client = ScreepsApi::ApiManager::Instance ().getApi ();

    bool ok = client->Signin ( argv[ 2 ], argv [ 3 ] );
    std::cout << "signed in " << ok << std::endl;
    return 0;
}
