#include "ShareController.h"
#include <typeinfo>
#define INFINITY 1000000
using namespace drogon;
void ShareController::shareFile(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback)const{
		int fileId, userId, time, shareId;
    auto resJson = req->getJsonObject();
    userId = (*resJson)["userId"].as<int>();
		fileId = (*resJson)["fileId"].as<int>();
		time = (*resJson)["time"].as<int>() + 8;
		std::string code = "";
		std::string number;
	  const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";  
	//	int myRandom = ((time * fileId % userId + time % fileId) * time + userId) * time + fileId + time;
	//	for(int i = 0; i < 4; i++){
	//		myRandom = (myRandom + i) * time % INFINITY;
	//		code += characters[myRandom % 62];
	//	}
	//	LOG_ERROR << "code is " << code;
		code = "ssss";
		auto dbclient = drogon::app().getDbClient();
		Json::Value message;
    try{
        dbclient->execSqlSync("insert into share(file_id, user_id, time, code) values(?, ?, now() + interval ? hour, ?)", 
          userId, fileId, time, code);
				LOG_ERROR << "insert finished";
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
        auto result = dbclient->execSqlSync("select * from share where share_id = ? and code = ?", shareIdInt, shareCode);

				if(result.size() == 0){
					message["code"] = 1;
					message["error"] = "no such share";
					auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
   			  callback(resp);
					return ;
				}
				
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
			  std::string MD5, suffix;
				if(timeLimit >= timeNow){
					LOG_ERROR << "getting file";
					std::string sql = 
        " \
            SELECT f.MD5, f.fileExtension \
            FROM fileOfUser fu \
            JOIN file f ON fu.file_id = f.id \
            WHERE fu.user_id = ? AND fu.file_id = ?; \
        ";
				  auto future = dbclient->execSqlAsyncFuture(sql, userId, fileId);
          auto result = future.get();
					MD5 = result.at(0)["MD5"].as<std::string>();
        	suffix = result.at(0)["fileExtension"].as<std::string>();

  				auto resp = drogon::HttpResponse::newFileResponse("../uploads/"+MD5+suffix);
			    callback(resp); 
				}else{
					message["code"] = 1;
					message["warning"] = "invalid share";
					auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
   			  callback(resp);
					return ;
				}
    }catch(drogon::orm::DrogonDbException &e){
        message["error"] = "get share failed";
    }
    
}
