/*
 * cgi.cxx
 *
 */

/*
 * Copyright (C) 2002-2004 Isaac W. Foraker (isaac at noscience dot net)
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Author nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <cgixx/cgi.h>
#include "cgi_impl.h"
#include <cstdio>
#include <cctype>

namespace cgixx {

static const std::string cgixx_version("1.07");

/**
 * Construct an instance of cgi.
 */
cgi::cgi() : imp(new cgi_impl)
{
}


/**
 * Destruct *this instance of cgi.
 */
cgi::~cgi()
{
    delete imp;
}


/**
 * Get the cgixx library version string.
 *
 * @return  The cgixx version string.
 */
const std::string& cgi::libver()
{
    return cgixx_version;
}


/**
 * Get the count of values for the CGI variable with the specified id.
 * This count is decremented with each call to get.
 *
 * @param   id      Identifier of variable.
 * @return  Count of values for vairable.
 */
unsigned cgi::count(const std::string& id) const
{
    ParameterList::const_iterator it(imp->vars.find(id));
    if (it == imp->vars.end())
        return 0;
    return it->second.size();
}


/**
 * Check whether the specified variable exists.  This result is
 * affected by calls to get.
 *
 * @param   id      Identifier of variable.
 * @return  true if variable exists;
 * @return  false if variable does not exist.
 */
bool cgi::exists(const std::string& id)
{
    ParameterList::const_iterator it(imp->vars.find(id));
    return it != imp->vars.end();
}


/**
 * Get the next value of the CGI variable with the specified id.  Once a
 * value has been retrieved, that value is removed, and the next value,
 * if any, is will be available on the next call to get.
 *
 * @param   id      Identifier of CGI variable.
 * @param   value   Reference to string to receive value of variable.
 * @return  false on success; true when no more values are available.
 */
bool cgi::get(const std::string& id, std::string& value)
{
    ParameterList::iterator it(imp->vars.find(id));
    if (it == imp->vars.end())
        return true;
    strqueue& sq = it->second;
    value = sq.front();
    sq.pop();
    if (it->second.empty())
        imp->vars.erase(it);
    return false;
}


/**
 * Get the list of variable identifiers.  If all values for a variable
 * are retrieved using get, the associated variable identifier will not
 * be returned by getvariablelist.
 *
 * @param   idlist      Reference to list to receive variable IDs.
 * @return  nothing
 */
void cgi::getvariablelist(identifierlist& idlist) const
{
    ParameterList::const_iterator it(imp->vars.begin()),
        end(imp->vars.end());
    idlist.clear();
    for (; it != end; ++it)
        idlist.push_back(it->first);
}


/**
 * Get the count of values for the cookie with the specified id.  This
 * count is decremented with each call to getcookie.
 *
 * @param   id      Identifier of cookie.
 * @return  Count of values for specified cookie.
 */
unsigned cgi::countcookie(const std::string& id) const
{
    ParameterList::const_iterator it(imp->cookies.find(id));
    if (it == imp->cookies.end())
        return 0;
    return it->second.size();
}


/**
 * Check whether the specified cookie exists.  This result is
 * affected by calls to getcookie.
 *
 * @param   id      Identifier of cookie.
 * @return  true if cookie exists;
 * @return  false if cookie does not exist.
 */
bool cgi::cookieexists(const std::string& id)
{
    ParameterList::const_iterator it(imp->cookies.find(id));
    return it != imp->cookies.end();
}


/**
 * Get the next value of the cookie with the specified id.  Once a
 * cookie has been retrieved, that value is removed, and the next value,
 * if any, is made available.
 *
 * @param   id      Identifier of cookie.
 * @param   value   Reference to string to receive value of cookie.
 * @return  false on success; true when no more values are available.
 */
bool cgi::getcookie(const std::string& id, std::string& value)
{
    ParameterList::iterator it(imp->cookies.find(id));
    if (it == imp->cookies.end())
        return true;
    strqueue& sq = it->second;
    value = sq.front();
    sq.pop();
    if (it->second.empty())
        imp->cookies.erase(it);
    return false;
}


/**
 * Get the list of cookie identifiers.  If all values for a cookie
 * are retrieved using get, the associated cookie identifier will not
 * be returned by getcookielist.
 *
 * @param   idlist      Reference to list to receive cookie IDs.
 * @return  nothing
 */
void cgi::getcookielist(identifierlist& idlist) const
{
    ParameterList::const_iterator it(imp->cookies.begin()),
        end(imp->cookies.end());
    idlist.clear();
    for (; it != end; ++it)
        idlist.push_back(it->first);
}


/**
 * Copy the value of the specified variable into the specified string.
 * If an invalud header is specified, getheader returns true.
 *
 * @param   hid     The header identifier from the headers enumeration.
 * @param   copy    Reference to string to receive value of header.
 * @return  false on success;
 * @return  true if an invalid header is specified.
 */
bool cgi::getheader(headers hid, std::string& copy) const
{
    switch (hid) {
    case header_request_method:
        imp->getenvvar(copy, "REQUEST_METHOD");
        break;
    case header_query_string:
        imp->getenvvar(copy, "QUERY_STRING");
        break;
    case header_server_software:
        imp->getenvvar(copy, "SERVER_SOFTWARE");
        break;
    case header_server_name:
        imp->getenvvar(copy, "SERVER_NAME");
        break;
    case header_gateway_interface:
        imp->getenvvar(copy, "GATEWAY_INTERFACE");
        break;
    case header_server_protocol:
        imp->getenvvar(copy, "SERVER_PROTOCOL");
        break;
    case header_server_port:
        imp->getenvvar(copy, "SERVER_PORT");
        break;
    case header_path_info:
        imp->getenvvar(copy, "PATH_INFO");
        break;
    case header_path_translated:
        imp->getenvvar(copy, "PATH_TRANSLATED");
        break;
    case header_script_name:
        imp->getenvvar(copy, "SCRIPT_NAME");
        break;
    case header_remote_addr:
        imp->getenvvar(copy, "REMOTE_ADDR");
        break;
    case header_remote_host:
        imp->getenvvar(copy, "REMOTE_HOST");
        break;
    case header_auth_type:
        imp->getenvvar(copy, "AUTH_TYPE");
        break;
    case header_remote_user:
        imp->getenvvar(copy, "REMOTE_USER");
        break;
    case header_remote_ident:
        imp->getenvvar(copy, "REMOTE_IDENT");
        break;
    case header_content_type:
        imp->getenvvar(copy, "CONTENT_TYPE");
        break;
    case header_content_length:
        imp->getenvvar(copy, "CONTENT_LENGTH");
        break;
    case header_http_accept:
        imp->getenvvar(copy, "HTTP_ACCEPT");
        break;
    case header_http_user_agent:
        imp->getenvvar(copy, "HTTP_USER_AGENT");
        break;
    case header_http_cookie:
        imp->getenvvar(copy, "HTTP_COOKIE");
        break;
    default:
        copy.erase();
        return true;
    }
    return false;
}


/**
 * Get the method of the request in the form of an enumerated id.
 *
 * @return  The method id, which must be a member of the methods enum.
 */
methods cgi::getmethod() const
{
    return imp->method;
}

/**
 * Convert a string for use in a URL.  All non-alphanumeric characters in the
 * string will be converted to %hex notation.
 *
 * @param   instr       Input string
 * @param   outstr      Output string
 */
std::string& makesafestring(const std::string& instr, std::string& outstr)
{
    outstr.erase();
    std::size_t i, end = instr.length();
    char buf[17];
    for (i = 0; i != end; ++i)
    {
        unsigned char c = instr[i];
        if (std::isalnum(c) || c == '_')
            outstr+= c;
        else if (c == ' ')
            outstr+= '+';
        else
        {
            std::sprintf(buf, "%02X", c);
            outstr+= '%';
            outstr+= buf;
        }
    }
    return outstr;
}

} // end namespace cgixx
