#include "ShareController.h"
#include <typeinfo>
using namespace drogon;
void ShareController::shareFile(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback)const{

    unsigned int fileId, userId, time, shareId;
    auto resJson = req->getJsonObject();
    userId = (*resJson)["userId"].as<unsigned int>();
    fileId = (*resJson)["fileId"].as<unsigned int>();
    // change time zone to UTF+8
    time = (*resJson)["time"].as<unsigned int>() + 8;

    Json::Value message;
    if(time > 60000008){
      message["error"] = "time is too large, share failed";
      auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
      callback(resp);
      return ;
   }

     // gain random code 
    std::string code = "";
    std::string number;
    const std::string characters = "opqr59014lmnEvwxyBF23GCDLMN678HIJKAstuOUVWXYZabcSTdefghPQRijkz";  
    unsigned int myRandom = ((time * fileId  + time ) * time + userId) * time + fileId * time;
    for(int i = 0; i < 6; i++){
      myRandom = (myRandom + i * time + i * fileId)  * time + fileId;
      code += characters[myRandom % 62];
    }
    LOG_ERROR << "code is " << code;

    auto dbclient = drogon::app().getDbClient();
    try{
     // write share information to share table
      dbclient->execSqlSync("insert into share(file_id, user_id, time, code) values(?, ?, now() + interval ? hour, ?)", 
          userId, fileId, time, code);
      // gain share_id
      auto result = dbclient->execSqlSync("select max(share_id) from share");	
      for(auto row : result){
        number = row["max(share_id)"].as<std::string>();
      }
      message["path"] = "/" + number + "/" + code;
      message["code"] = "0";
    }catch(drogon::orm::DrogonDbException &e){
      message["error"] = "share failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
 
}
void ShareController::getShareFile(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback,
	std::string shareId, std::string shareCode)const{

    auto dbclient = drogon::app().getDbClient();
    Json::Value message;
    int shareIdInt = 0;
    int len = shareId.size();
    for(int i = 0; i < len ; i++){
      shareIdInt = shareIdInt * 10 + shareId[i] - '0';
    }
    LOG_ERROR << "share Id is "<< shareIdInt <<"  shareCode is "<<shareCode;

    try{
     // check if it is a legal share
      auto result = dbclient->execSqlSync("select * from share where share_id = ? and code = ?", shareIdInt, shareCode);

      if(result.size() == 0){
        message["code"] = 1;
        message["error"] = "no such share";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
         callback(resp);
        return ;
      }
    // gain information from table share	
    std::string timeLimit, userId, fileId;
    for(auto row : result){
      timeLimit = row["time"].as<std::string>();
      userId = row["user_id"].as<std::string>();
      fileId = row["file_id"].as<std::string>();
    }

    auto nowTime = dbclient->execSqlSync("select now() + interval 8 hour");
    std::string timeNow;
    for(auto row : nowTime){
      timeNow = row["now() + interval 8 hour"].as<std::string>();
    }
    LOG_ERROR << "now is " << timeNow;
    // check timeLimit
    if(timeLimit >= timeNow){
      std::string sql = 
        " \
            SELECT f.MD5, f.fileExtension \
            FROM fileOfUser fu \
            JOIN file f ON fu.file_id = f.id \
            WHERE fu.user_id = ? AND fu.file_id = ?; \
        ";
      auto future = dbclient->execSqlAsyncFuture(sql, userId, fileId);
      auto result = future.get();

      std::string MD5, suffix;
      MD5 = result.at(0)["MD5"].as<std::string>();
      suffix = result.at(0)["fileExtension"].as<std::string>();
      
      // note the location
      auto resp = drogon::HttpResponse::newFileResponse("../uploads/"+MD5+suffix);
        callback(resp); 
      }else{
        message["code"] = 1;
        message["warning"] = "the share beyonds time limit";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
        callback(resp);
        return ;
      }
    }catch(drogon::orm::DrogonDbException &e){
      message["code"] = 1;
      message["error"] = "get share failed";
    }
    
}
