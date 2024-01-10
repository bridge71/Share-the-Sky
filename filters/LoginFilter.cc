/**
 *
 *  LoginFilter.cc
 *
 */

#include "LoginFilter.h"

using namespace drogon;

void LoginFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{

    auto session = req->session();
    if(session->find("login"))
    {
        fccb();
        return ;
    }
    auto res=HttpResponse::newHttpResponse();
    res->setStatusCode(drogon::k502BadGateway);
    fcb(res);
    return;
}

