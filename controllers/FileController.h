#pragma once

#include <drogon/HttpController.h>

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
     METHOD_ADD(FileController::addFile, "/addFile", Post); // path is /FileController/{arg1}/{arg2}/list
     METHOD_ADD(FileController::deleteFile, "/deleteFile", Post); // path is /FileController/{arg1}/{arg2}/list
     METHOD_ADD(FileController::listFile, "/listFile", Post); // path is /FileController/{arg1}/{arg2}/list
     METHOD_ADD(FileController::findFileName, "/findFileName", Post); // path is /FileController/{arg1}/{arg2}/list
     METHOD_ADD(FileController::findFileMD5, "/findFileMD5", Post); // path is /FileController/{arg1}/{arg2}/list

    // ADD_METHOD_TO(FileController::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    void addFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, File file) const;
    void deleteFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;
    void listFile(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;
    void findFileName(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;
    void findFileMD5(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;

};
namespace drogon{
  template<>
  inline File fromRequest(const HttpRequest &req){
    auto json = req.getJsonObject();
    File file;
    if(json == nullptr){
      throw std::runtime_error("format error, please use json");
    }
    if(json->isMember("fileName")){
      const auto &temp = (*json)["fileName"].asString();
      if(temp.size() > 100){
        throw std::runtime_error("the name is too longer");
      }
      file.fileName = temp;
    }else{
      throw std::runtime_error("name lost");
    }
    if(json->isMember("fileType")){
      const auto &temp = (*json)["fileType"].asString();
      if(temp.size() > 100){
        throw std::runtime_error("the type is too longer");
      }
      file.fileType = temp;
    }else{
      throw std::runtime_error("type lost");
    }
    if(json->isMember("MD5")){
      const auto &temp = (*json)["MD5"].asString();
      if(temp.size() != 32){
        throw std::runtime_error("the MD5 is wrong");
      }
      file.MD5 = temp;
    }else{
      throw std::runtime_error("MD5 lost");
    }
    if(json->isMember("path")){
      const auto &temp = (*json)["path"].asString();
      if(temp.size() > 100){
        throw std::runtime_error("the path is too long");
      }
      file.path = temp;
    }else{
      throw std::runtime_error("path lost");
    }
    return file;
  }
template<>
  inline Json::Value fromRequest(const HttpRequest &req){
    return *req.getJsonObject();
  }
}