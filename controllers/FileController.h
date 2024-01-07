#pragma once

#include <drogon/HttpController.h>
#include <drogon/drogon.h>
using namespace drogon;
struct File{
  std::string fileName;
  std::string fileType;
  std::string MD5;
  std::string path;
};
class FileController : public drogon::HttpController<FileController>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(FileController::get, "/{2}/{1}", Get); // path is /FileController/{arg2}/{arg1}
    ADD_METHOD_TO(FileController::addFile, "/file/add", Post, "LoginFilter"); // path is /FileController/{arg1}/{arg2}/list
    ADD_METHOD_TO(FileController::deleteFile, "/file/delete", Post, "LoginFilter"); // path is /FileController/{arg1}/{arg2}/list
    ADD_METHOD_TO(FileController::listFile, "/file/list", Post, "LoginFilter"); // path is /FileController/{arg1}/{arg2}/list
    ADD_METHOD_TO(FileController::listAllFile, "/file/alllist", Post, "AdminFilter"); // path is /FileController/{arg1}/{arg2}/list
    ADD_METHOD_TO(FileController::findFileName, "/file/findName", Post, "LoginFilter"); // path is /FileController/{arg1}/{arg2}/list
    ADD_METHOD_TO(FileController::findFileMD5, "/file/findMD5", Post, "LoginFilter"); // path is /FileController/{arg1}/{arg2}/list
    ADD_METHOD_TO(FileController::downLoadFile, "/file/download", Post, "LoginFilter"); // path is /FileController/{arg1}/{arg2}/list
    ADD_METHOD_TO(FileController::downLoadFileGet, "/file/download?userid={1}&fileid={2}", Get, "LoginFilter"); // path is /FileController/{arg1}/{arg2}/list
    ADD_METHOD_TO(FileController::downLoadFileAdmin, "/file/download/admin?fileid={1}", Get, "AdminFilter"); // path is /FileController/{arg1}/{arg2}/list

    // ADD_METHOD_TO(FileController::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    void addFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const;
    void deleteFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;
    void listFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;
    void listAllFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const;
    void findFileName(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;
    void findFileMD5(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;
    void downLoadFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;
    void downLoadFileGet(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, std::string userId, std::string fileId) const;
    void downLoadFileAdmin(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, std::string fileId) const;

};
namespace drogon{
template<>
  inline Json::Value fromRequest(const HttpRequest &req){
    return *req.getJsonObject();
  }
}
