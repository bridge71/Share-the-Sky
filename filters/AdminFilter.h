/**
 *
 *  AdminFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <drogon/drogon.h>
using namespace drogon;


class AdminFilter : public HttpFilter<AdminFilter>
{
  public:
    AdminFilter() {}
    void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&fcb,
                  FilterChainCallback &&fccb) override;
};

