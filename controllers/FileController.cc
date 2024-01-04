#include "FileController.h"
#include "drogon/orm/Exception.h"
#include "drogon/orm/Result.h"

using namespace drogon;
void FileController::addFile(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback)const{
    Json::Value message;
    MultiPartParser fileUpload;

    fileUpload.parse(req);
    auto para = fileUpload.getParameters();
    auto allFile = fileUpload.getFiles();    
    if(para.size()!= 1 || allFile.size() != 1){
        message["code"] = 1;
        message["error"] = "too much parameters, add failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
        callback(resp);
        return ;
    }

    auto &file = allFile[0];
    auto MD5 = file.getMd5();
    message["md5"] = MD5;

    std::string key = "path";
    std::string path = para[key];

    auto dbclient = drogon::app().getDbClient();
    try{
        auto result = dbclient->execSqlSync("select * from file where MD5 = ?", MD5);
        int sum = result.size();
        if(sum == 0){
            file.save();
        }
       
        std::string fileName = file.getFileName();
        auto fileEnum = file.getFileType();
        std::string fileType;
        switch (fileEnum){
            case 2 : fileType = "document"; break;
            case 3 : fileType = "archieve"; break;
            case 4 : fileType = "audio"; break;
            case 5 : fileType = "media"; break;
            case 6 : fileType = "image"; break;
            default : fileType = "unknown";
        } 

        dbclient->execSqlSync("insert into file(fileName, fileType, MD5, path) values(?, ?, ?, ?)",
         fileName, fileType, MD5, path);
        message["code"] = "0";
    }catch(drogon::orm::DrogonDbException &e){
        message["error"] = "Add failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}

void FileController::deleteFile(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback, Json::Value json)const{
    auto dbclient = drogon::app().getDbClient();
    Json::Value message;
    int id = json["id"].as<int>();
    try{
        dbclient->execSqlSync("delete from file where id = ?", id);
        message["code"] = 0;
    }catch (drogon::orm::DrogonDbException &e){
        message["error"] = "Delete failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}
void FileController::findFileName(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback, Json::Value json)const{
    auto dbclient = drogon::app().getDbClient();
    Json::Value message;
    try{
        std::string fileName = json["fileName"].as<std::string>();
        std::string sql = "select * from file where fileName = ?";
        std::cout<<sql;
        auto future = dbclient->execSqlAsyncFuture(sql, fileName);
        auto result = future.get();
        for (const auto &row : result){
            Json::Value item;
            item["id"] = row["id"].as<int>();
            item["fileName"] = row["fileName"].as<std::string>();
            item["fileType"] = row["fileType"].as<std::string>();
            item["MD5"] = row["MD5"].as<std::string>();
            item["path"] = row["path"].as<std::string>();
            message.append(item);
        }
    }catch (drogon::orm::DrogonDbException &e){
        message["error"] = "find file by Name failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}
void FileController::findFileMD5(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback, Json::Value json)const{
    auto dbclient = drogon::app().getDbClient();
    Json::Value message;
    try{
        std::string MD5 = json["MD5"].as<std::string>();
        std::string sql = "select * from file where MD5 = ?";
        auto future = dbclient->execSqlAsyncFuture(sql, MD5);
        auto result = future.get();
        for (const auto &row : result){
            Json::Value item;
            item["id"] = row["id"].as<int>();
            item["fileName"] = row["fileName"].as<std::string>();
            item["fileType"] = row["fileType"].as<std::string>();
            item["MD5"] = row["MD5"].as<std::string>();
            item["path"] = row["path"].as<std::string>();
            message.append(item);
        }
    }catch (drogon::orm::DrogonDbException &e){
        message["error"] = "find file by MD5 failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}
void FileController::listFile(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback, Json::Value json)const{
    auto dbclient = drogon::app().getDbClient();
    Json::Value message;
    try{
        std::string path = json["path"].as<std::string>();
        std::string sql = "select * from file where path = ?";
        auto future = dbclient->execSqlAsyncFuture(sql, path);
        auto result = future.get();
        for (const auto &row : result){
            Json::Value item;
            item["id"] = row["id"].as<int>();
            item["fileName"] = row["fileName"].as<std::string>();
            item["fileType"] = row["fileType"].as<std::string>();
            item["MD5"] = row["MD5"].as<std::string>();
            item["path"] = row["path"].as<std::string>();
            message.append(item);
        }
    }catch (drogon::orm::DrogonDbException &e){
        message["error"] = "list failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}
