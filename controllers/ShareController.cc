#include "ShareController.h"
#include <typeinfo>
#include <random>
using namespace drogon;
void ShareController::shareFile(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback)const{

    unsigned int fileId, userId, time;
    std::string fileName;
    auto resJson = req->getJsonObject();
    userId = (*resJson)["userId"].as<unsigned int>();
    fileId = (*resJson)["fileId"].as<unsigned int>();
    fileName = (*resJson)["fileName"].as<std::string>();

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
    std::random_device rd;  
    std::mt19937 gen(rd());  
    int minValue = 1;  
    int maxValue = 10000;  
  
    std::uniform_int_distribution<> dis(minValue, maxValue);  
    unsigned int offset = dis(gen);  
    LOG_DEBUG << "offset is " << offset;
    std::string code = "";
    const std::string characters = "opqr59014lmnEvwxyBF23GCDLMN678HIJKAstuOUVWXYZabcSTdefghPQRijkz";  
    unsigned int myRandom = ((time * fileId  + time ) * time + userId) * time + fileId * time + offset;
    for(int i = 0; i < 6; i++){
        myRandom = (myRandom + i * time + i * fileId)  * time + fileId;
        code += characters[myRandom % 62];
    }
    LOG_ERROR << "code is " << code;

    auto dbClient = drogon::app().getDbClient();
    try{
       // write share information to share table
        dbClient->execSqlSync("insert into share(fileId, userId, time, code, fileName) values(?, ?, now() + interval ? hour, ?, ?)", 
            fileId, userId, time, code, fileName);
        // gain share_id
        auto result = dbClient->execSqlSync("select * from share where fileId = ? and userId = ? and code = ?",
            fileId, userId, code);	
        int shareSize = result.size();
        int shareId = result.at(shareSize - 1)["shareId"].as<int>();
        
        message["shareId"] = shareId;
        message["code"] = code;
    }catch(drogon::orm::DrogonDbException &e){
        message["error"] = "share failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
 
}
void ShareController::getShareFile(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback)const{

    auto resJson = req->getJsonObject();
    int shareId;
    std::string code;
    shareId = (*resJson)["shareId"].as<int>();
    code = (*resJson)["code"].as<std::string>();

    auto dbClient = drogon::app().getDbClient();
    Json::Value message;
       
    LOG_ERROR << "shareId is "<< shareId <<"  code is "<<code;

    try{
     // check if it is a legal share
        auto result = dbClient->execSqlSync("select * from share where shareId = ? and code = ?", shareId, code);

        if(result.size() == 0){
            message["status"] = 1;
            message["error"] = "no such share";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
            callback(resp);
            return ;
         }
      // gain information from table share
         std::string timeLimit, fileName;
         int userId, fileId;
         for(auto row : result){
             timeLimit = row["time"].as<std::string>();
             userId = row["userId"].as<int>();
             fileId = row["fileId"].as<int>();
             fileName = row["fileName"].as<std::string>();
         }

         auto nowTime = dbClient->execSqlSync("select now() + interval 8 hour");
         std::string timeNow;
         for(auto row : nowTime){
             timeNow = row["now() + interval 8 hour"].as<std::string>();
         }
         LOG_ERROR << "now is " << timeNow;
         // check timeLimit
         if(timeLimit >= timeNow){
             auto result = dbClient->execSqlSync("select * from fileOfUser where userId = ? and fileId = ?",
                 userId, fileId); 
             int fileSize = result.at(0)["fileSize"].as<int>(); 
             message["fileSize"] = fileSize;
             auto result2 = dbClient->execSqlSync("select * from file where id = ?", fileId);
             std::string fileExtension = result2.at(0)["fileExtension"].as<std::string>();
             message["fileExtension"] = fileExtension;
             message["fileId"] = fileId;
             message["fileName"] = fileName;
         }else{
            message["status"] = 1;
            message["warning"] = "the share beyonds time limit";
            
         }
    }catch(drogon::orm::DrogonDbException &e){
        message["status"] = 1;
        message["error"] = "get share failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
 
}
