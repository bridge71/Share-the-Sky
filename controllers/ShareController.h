#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class ShareController : public drogon::HttpController<ShareController>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(ShareController::get, "/{2}/{1}", Get); // path is /ShareController/{arg2}/{arg1}
    // METHOD_ADD(ShareController::your_method_name, "/{1}/{2}/list", Get); // path is /ShareController/{arg1}/{arg2}/list
    ADD_METHOD_TO(ShareController::shareFile, "/share", Post); // path is /absolute/path/{arg1}/{arg2}/list
    ADD_METHOD_TO(ShareController::getShareFile, "/share/acquire", Post); // path is /absolute/path/{arg1}/{arg2}/list


    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    void shareFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback ) const;
    void getShareFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const;

};
