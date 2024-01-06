#include "FolderController.h"

void FolderController:: makeFolder(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const{


    auto dbClient = drogon::app().getDbClient();

    auto resJson = req->getJsonObject();
    std::string folderName;
    int fatherFolderId;
    folderName = (*resJson)["folderName"].asString();
    fatherFolderId = (*resJson)["fatherFolderId"].as<int>();
    LOG_DEBUG << "fatherFolderId is " << fatherFolderId;
    LOG_DEBUG << "folderName is " << folderName;
    Json::Value message;
    try{
        auto result = dbClient->execSqlSync("select * from folder where folderName = ? and fatherFolderId = ?",
           folderName, fatherFolderId);
        if(result.size() != 0){
            message["warning"] = "folder has been made";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
            callback(resp);
            return ;
        }
    }catch(drogon::orm::DrogonDbException &e){
        LOG_ERROR << e.base().what();
        message["error"] = "make failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
        callback(resp);
        return ;
    }
       
    try{
        dbClient->execSqlSync("insert into folder(folderName, fatherFolderId) values(?, ?)",
          folderName, fatherFolderId);
        auto result = dbClient->execSqlSync("select * from folder where folderName = ? and fatherFolderId = ?",
          folderName, fatherFolderId);
        int folderId = result.at(0)["folderId"].as<int>();
        message["folderId"] = folderId;
        message["folderName"] = folderName;
    }catch(drogon::orm::DrogonDbException &e) {
        LOG_DEBUG<<e.base().what();
        message["error"] = "make failed";
    }

    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}