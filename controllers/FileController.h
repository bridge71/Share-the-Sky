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
     METHOD_ADD(FileController::Add_File, "/Add_File", Post); // path is /FileController/{arg1}/{arg2}/list
     METHOD_ADD(FileController::Delete_File, "/Delete_File", Post); // path is /FileController/{arg1}/{arg2}/list
     METHOD_ADD(FileController::List_File, "/List_File", Post); // path is /FileController/{arg1}/{arg2}/list
     METHOD_ADD(FileController::Find_File_Name, "/Find_File_Name", Post); // path is /FileController/{arg1}/{arg2}/list
     METHOD_ADD(FileController::Find_File_MD5, "/Find_File_MD5", Post); // path is /FileController/{arg1}/{arg2}/list

    // ADD_METHOD_TO(FileController::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    void Add_File(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, File file) const;
    void Delete_File(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;
    void List_File(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;
    void Find_File_Name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;
    void Find_File_MD5(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, Json::Value json) const;

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