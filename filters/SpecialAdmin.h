/**
 *
 *  SpecialAdmin.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;


class SpecialAdmin : public HttpFilter<SpecialAdmin>
{
  public:
    SpecialAdmin() {}
    void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&fcb,
                  FilterChainCallback &&fccb) override;
};

