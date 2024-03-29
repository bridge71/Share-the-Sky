#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class UserController : public drogon::HttpController<UserController>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(UserController::get, "/{2}/{1}", Get); // path is /UserController/{arg2}/{arg1}
    // METHOD_ADD(UserController::your_method_name, "/{1}/{2}/list", Get); // path is /UserController/{arg1}/{arg2}/list
    // ADD_METHOD_TO(UserController::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
    //增
    ADD_METHOD_TO(UserController::addUser, "/drogon/user/add", Post, "TimeFilter" );
    //删
    ADD_METHOD_TO(UserController::removeUser, "/drogon/user/remove", Post, "LoginFilter", "AdminFilter");
    //改
    ADD_METHOD_TO(UserController::modifyUser, "/drogon/user/modify", Post, "LoginFilter");
    ADD_METHOD_TO(UserController::modifyUserName, "/drogon/user/modify/name", Post, "LoginFilter");
    ADD_METHOD_TO(UserController::modifyUserPassword, "/drogon/user/modify/password", Post, "LoginFilter");
    ADD_METHOD_TO(UserController::modifyUserPermissions, "/drogon/user/modify/permissions", Post, "LoginFilter","AdminFilter");
    //查
    ADD_METHOD_TO(UserController::selectUser, "/drogon/user/select", Post, "LoginFilter");
    //登录
    ADD_METHOD_TO(UserController::loginUser, "/drogon/user/login", Post);
    //管理员列出全部用户
    ADD_METHOD_TO(UserController::listAllUser, "/drogon/user/listall", Post, "LoginFilter","AdminFilter");


    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
    void addUser(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback
    ) const;
    void removeUser(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback
    ) const;
    void modifyUser(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback
    ) const;
    void modifyUserName(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback
    ) const;
    void modifyUserPassword(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback
    ) const;
    void modifyUserPermissions(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback
    ) const;

    //By userId
    void selectUser(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback
    ) const;
    void loginUser(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback
    ) const;

    void listAllUser(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback
    ) const;
};
