#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class FolderController : public drogon::HttpController<FolderController>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(FolderController::get, "/{2}/{1}", Get); // path is /FolderController/{arg2}/{arg1}
    // METHOD_ADD(FolderController::your_method_name, "/{1}/{2}/list", Get); // path is /FolderController/{arg1}/{arg2}/list
    ADD_METHOD_TO(FolderController::makeFolder, "/folder/make", Post); // path is /absolute/path/{arg1}/{arg2}/list
    ADD_METHOD_TO(FolderController::deleteFolder, "/folder/delete", Post); // path is /absolute/path/{arg1}/{arg2}/list
    ADD_METHOD_TO(FolderController::renameFolder, "/folder/rename", Post); // path is /absolute/path/{arg1}/{arg2}/list




    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    void makeFolder(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const;
    void deleteFolder(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const;
    void renameFolder(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const;


};
