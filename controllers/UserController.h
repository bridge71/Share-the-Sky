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
    ADD_METHOD_TO(UserController::addUser, "/drogon/user/add", Post);
    //删
    ADD_METHOD_TO(UserController::removeUser, "/drogon/user/remove", Post);
    //改
    ADD_METHOD_TO(UserController::modifyUser, "/drogon/user/modify", Post);
    //查
    ADD_METHOD_TO(UserController::selectUser, "/drogon/user/select", Post);
    //登录
    ADD_METHOD_TO(UserController::loginUser, "/drogon/user/login", Post);

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

    //By userId
    void selectUser(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback
    ) const;
    void loginUser(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback
    ) const;
};
