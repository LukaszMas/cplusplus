// This is an example source file for generating C++ code with re2c lexer
//
// As example a fast endpoints route generation for HttpServer is used:

#define ROUTE(get, post, del) \
     static const Route r(get, post, del); return &r;

// Quick path lookup, URLs must be NULL terminated
const HttpServer::Route* HttpServer::find_route(const char* YYCURSOR)
{
    const char *YYMARKER;
     /*!re2c
    re2c:define:YYCTYPE = char;
    re2c:yyfill:enable = 0;
    "/endpoint_one\x00"                         { ROUTE(&HttpServer::endpoint_one_get, 0, &HttpServer::endpoint_one_del); }
    "/endpoint_two\x00"                         { ROUTE(&HttpServer::endpoint_two_get, 0, 0); }
    "/endpoint_three\x00"                       { ROUTE(0, &HttpServer::endpoint_three_post, 0); }
    *                                           { return 0; }
    */
     return 0;
}
