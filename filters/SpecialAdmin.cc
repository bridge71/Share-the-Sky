/**
 *
 *  SpecialAdmin.cc
 *
 */

#include "SpecialAdmin.h"

using namespace drogon;

void SpecialAdmin::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb, std::string userId)
{
    //Edit your logic here
    LOG_DEBUG<<"检查是否为管理员";
    LOG_DEBUG << "userId " << userId;
    auto dbclient = drogon::app().getDbClient();
    bool isAdmin=false;
    try {
        std::string sql = "SELECT * FROM user WHERE id=?;";
        auto ret = dbclient->execSqlSync(sql, userId);
	if(ret.size() == 0){
             Json::Value message;
             message["message"] = "permission denied";
             LOG_DEBUG<<"the user is not admin, userId:"<<userId;
             auto res = drogon::HttpResponse::newHttpJsonResponse(message);
             fcb(res);
             return;
	}
        isAdmin = (ret.at(0)["permissions"].as<int>() == 1);
        LOG_DEBUG << "permissions is " <<  ret.at(0)["permissions"].as<int>();
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_DEBUG<<e.base().what();
    }
    if (isAdmin)
    {
        //Passed
        fccb();
        return;
    }
    //Check failed
    Json::Value message;
    message["message"] = "permission denied";
    LOG_DEBUG<<"the user is not admin, userId:"<<userId;
    auto res = drogon::HttpResponse::newHttpJsonResponse(message);
    fcb(res);

 }
