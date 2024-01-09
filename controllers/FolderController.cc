#include "FolderController.h"
#include <queue>
std::queue<int>q;
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
            message["status"] = 1;
            auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
            callback(resp);
            return ;
        }
    }catch(drogon::orm::DrogonDbException &e){
        LOG_ERROR << e.base().what();
        message["status"] = 2;
        message["error"] = "error when selecting";
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
        message["status"] = 0;
    }catch(drogon::orm::DrogonDbException &e) {
        LOG_ERROR<<e.base().what();
        message["status"] = 2;
        message["error"] = "make failed";
    }

    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}
void FolderController:: deleteFolder(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const{

  auto dbClient = drogon::app().getDbClient();
  auto transaction = dbClient->newTransaction();
  auto resJson = req->getJsonObject();
  int folderId;
  folderId = (*resJson)["folderId"].as<int>();
  LOG_DEBUG<<"folderId:"<<folderId;
  Json::Value message;
  q.push(folderId); 
  try{
      while(q.size()){
          int folderId = q.front();
          q.pop();
          LOG_DEBUG << "folderId " << folderId; 
          std::string sql = "select * from folder where fatherFolderId = ?;";
          auto result1 = transaction->execSqlSync(sql, folderId);
          for (const auto &row : result1){
              int id = row["folderId"].as<int>();
              q.push(id);
          }
          std::string sql2 = "select * from fileOfUser where folderId = ?;";
          auto result2 = transaction->execSqlSync(sql2, folderId);
          for (const auto &row : result2){
              int fileId = row["fileId"].as<int>();
              LOG_DEBUG << "fileId " << fileId;
              int userId = row["userId"].as<int>();
              LOG_DEBUG << "userId " << userId;
              int fileSize = row["fileSize"].as<int>();
              transaction->execSqlSync("update user set capacity = capacity + ? where id = ?", fileSize, userId); 
              transaction->execSqlSync("delete from fileOfUser where fileId = ? and userId = ?", fileId, userId);
          }
          transaction->execSqlSync("delete from folder where folderId = ?", folderId);
       }
       message["status"] = 0;
  }catch (drogon::orm::DrogonDbException &e){
      LOG_ERROR<<e.base().what();
      transaction->rollback();
      message["status"] = 2;
      message["error"] = "delete failed";
  }
  auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
  callback(resp);
}
