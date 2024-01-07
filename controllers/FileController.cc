#include "FileController.h"
#include "drogon/orm/Exception.h"
#include "drogon/orm/Result.h"
#include <vector>

using namespace drogon;
void FileController::addFile(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback)const{
    Json::Value message;
    MultiPartParser fileUpload;

    fileUpload.parse(req);
    auto para = fileUpload.getParameters();
    auto allFile = fileUpload.getFiles();    
    if(para.size()!= 2 || allFile.size() != 1){
        LOG_ERROR<<"文件/参数错误"<<"参数："<<para.size()<<"文件："<<allFile.size();
        message["status"] = 2;
        message["error"] = "parameters error, upload failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
        callback(resp);
        return;
    }

    auto &file = allFile[0];
    auto MD5 = file.getMd5();
    message["md5"] = MD5;

    std::string key2 = "userId";
    std::string temp = para[key2];
    int userId = 0;
    int lenUserId = temp.size();
    for(int i = 0; i < lenUserId; i++){
        userId = userId * 10 + temp[i] - '0';
    }
    LOG_DEBUG << "userId is " << userId;

    auto dbclient = drogon::app().getDbClient();

    int fatherFolderId;
    try{
        auto result1 = dbclient->execSqlSync("select * from folderOfUser where userId = ?", userId);
        for(auto row : result1){
        fatherFolderId = row["folderId"].as<int>();
    }
    }catch(drogon::orm::DrogonDbException &e) {
        LOG_ERROR<<e.base().what();
    }
    LOG_DEBUG << "rootFolderId is "<< fatherFolderId;
     
    std::string key = "path";
    std::string path = para[key];
    LOG_DEBUG<<"path:"<<path;
    std::string folderName = "";
    int pathLength = path.size();
    if(path[pathLength - 1] != '/'){
        LOG_ERROR<<"path error"<<"  path: "<<path;
        message["status"] = 2;
        message["error"] = "path error, upload failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
        callback(resp);
        return;
    }

    for(int i = 0; i < pathLength; i++){
        if(path[i] == '/'){
            if(folderName.size() != 0){
                try{
                    auto result = dbclient->execSqlSync("select * from folder where folderName = ? and fatherFolderId = ?", folderName, fatherFolderId);
                    if(result.size() == 0){
                        dbclient->execSqlSync("insert into folder (folderName, fatherFolderId) values(?, ?)", folderName, fatherFolderId);
                    }
                    auto result2 = dbclient->execSqlSync("select * from folder where folderName = ? and fatherFolderId = ?", folderName, fatherFolderId);
                    for(auto row : result2){
                        fatherFolderId = row["folderId"].as<int>();
                    }
//                    LOG_DEBUG << "fatherFolderId is "<< fatherFolderId;

                    folderName = "";
                }catch(drogon::orm::DrogonDbException &e) {
                    LOG_DEBUG<<e.base().what();
                }
           }

        }else{
            folderName += path[i];
        }
    }
    int folderId = fatherFolderId;
    LOG_DEBUG << "folderId is "<< folderId;

    std::string fileName = file.getFileName();
		std::string suffix = "";
		for(int i = fileName.size() - 1; i > 0; i--){
			suffix += fileName[i];
			if(fileName[i] == '.')
				break;
		}
    std::reverse(suffix.begin(), suffix.end());
    std::string rename = MD5 + suffix;

        //用户是否存在
    try{
        
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_ERROR<<e.base().what();
    }

    //查询容量，剩余不够添加文件，不填加
    LOG_DEBUG<<"file size"<<file.fileLength();
    try{
        std::string sql = "SELECT * FROM user where id=?;";
        auto result = dbclient->execSqlSync(sql, userId);
        auto remaining = result.at(0)["remaining"].as<int>();
        auto fileSize = file.fileLength();
        if(fileSize > remaining) {
            LOG_ERROR<<"容量不够";
            message["status"] = 2;
            message["error"] = "volume lack, upload failed";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
            callback(resp);
            return ;
        }

    } catch (drogon::orm::DrogonDbException &e) {
        LOG_ERROR<<e.base().what();
        message["status"] = 1;
        message["error"] = "error at point1, upload failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
        callback(resp);
        return ;
    }
    auto transaction = dbclient->newTransaction();
    try{
        auto result = transaction->execSqlSync("select * from file where MD5 = ?", MD5);
        int sum = result.size();
        if(sum == 0){
            file.saveAs(rename);
        //  dbclient->execSqlSync("insert into file(fileType, MD5) values(?, ?)", fileType, MD5);

            transaction->execSqlSync("insert into file(MD5, fileExtension) values(?, ?)", MD5, suffix);
            result = transaction->execSqlSync("select * from file where MD5 = ?", MD5);
        }
        
        int fileId;
        for(const auto &row : result){
            fileId = row["id"].as<int>();
        }
       
        auto fileEnum = file.getFileType();
        std::string fileType;
        switch (fileEnum){
            case 2 : fileType = "document"; break;
            case 3 : fileType = "archive"; break;
            case 4 : fileType = "audio"; break;
            case 5 : fileType = "media"; break;
            case 6 : fileType = "image"; break;
            default : fileType = "unknown";
        } 

        auto query = transaction->execSqlSync("select * from fileOfUser where userId = ? and fileId = ? and folderId = ?;",
          userId, fileId, folderId);

        if(query.size() == 0){
            transaction->execSqlSync("insert into fileOfUser (userId, fileId, path, fileName, time, fileSize, folderId) values(?, ?, ?, ?, now() + interval 8 hour, ?, ?);", 
            userId, fileId, path, fileName, file.fileLength(), folderId);
            std::string sql = "UPDATE user SET remaining=remaining-? WHERE id=?";
            transaction->execSqlSync(sql, file.fileLength(), userId);
        }else{
            message["status"] = 1;
            message["warning"] = "file has been uploaded, don't upload repeately";
        }

    }catch(drogon::orm::DrogonDbException &e){
        message["status"] = 2;
        message["error"] = "error at point 2, upload failed";
        transaction->rollback();
    }
    message["status"] = 0;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}



void FileController::deleteFile(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback, Json::Value json)const{
    auto dbclient = drogon::app().getDbClient();
    Json::Value message;
    std::string fileName = json["fileName"].as<std::string>();
    std::string fileId = json["fileId"].as<std::string>();
    std::string userId = json["userId"].as<std::string>();
    LOG_DEBUG<<"文件ID:"<<fileId;
    LOG_DEBUG<<"用户ID:"<<userId;
    try{
        auto ret = dbclient->execSqlSync("select * from fileOfUser where userId = ? AND fileId = ?", userId, fileId);
        int fileSize = ret.at(0)["fileSize"].as<int>();
        LOG_DEBUG<<"delete file size:"<<fileSize;
        std::string sql = "UPDATE user SET remaining=remaining+? WHERE id=?";
        dbclient->execSqlSync(sql, fileSize, userId);
        dbclient->execSqlSync("delete from fileOfUser where userId = ? AND fileId = ?", userId, fileId);
        message["code"] = 0;
    }catch (drogon::orm::DrogonDbException &e){
        message["status"] = 2;
        message["error"] = "Delete failed";
        LOG_DEBUG<<e.base().what();
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}
void FileController::findFileName(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback, Json::Value json)const{
    auto dbclient = drogon::app().getDbClient();
    Json::Value message;
    try{
        std::string fileName = json["fileName"].as<std::string>();
        std::string sql = "select * from file where fileName = ?;";
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
        message["status"] = 2;
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
        std::string sql = "select * from file where MD5 = ?;";
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
        message["status"] = 2;
        message["error"] = "find file by MD5 failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}

void FileController::listAllFile(const HttpRequestPtr& req, 
    std::function<void (const HttpResponsePtr &)> &&callback
) const {
    Json::Value json;
    auto dbClient = drogon::app().getDbClient();
    try {
        std::string sql = "SELECT * FROM file;";
        auto ret = dbClient->execSqlSync(sql);
        for (const auto &row : ret){
            Json::Value item;
            item["fileId"] = row["id"].as<std::string>();
            item["fileName"] = row["MD5"].as<std::string>()+row["fileExtension"].as<std::string>();
            json.append(item);
        }
        // json["status"] = "ok";
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_ERROR<<e.base().what();
        json["status"] = 2;
        json["error"] = "get file list failed";
    }
    LOG_DEBUG << "have gained";
    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    callback(resp);
}

void FileController::listFile(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> && callback, Json::Value json)const{
    auto dbclient = drogon::app().getDbClient();
    Json::Value message;
    try{
        int folderId = json["folderId"].as<int>();
        LOG_DEBUG<<"folderId:"<<folderId;
        
        std::string sql = "select * from folder where fatherFolderId = ?;";
        auto result1 = dbclient->execSqlSync(sql, folderId);
        for (const auto &row : result1){
            Json::Value item;
            item["type"] = "0";
            item["folderId"] = row["folderId"].as<std::string>();
            item["folderName"] = row["folderName"].as<std::string>();
            //item["time"] = row["time"].as<std::string>();
            //item["path"] = row["path"].as<std::string>();
            message.append(item);
        }
        std::string sql2 = "select * from fileOfUser where folderId = ?;";
        auto result2 = dbclient->execSqlSync(sql2, folderId);
        for (const auto &row : result2){
            Json::Value item;
            item["type"] = "1";
            item["fileId"] = row["fileId"].as<std::string>();
            item["fileName"] = row["fileName"].as<std::string>();
            //item["time"] = row["time"].as<std::string>();
            //item["path"] = row["path"].as<std::string>();
            message.append(item);
        }
    }catch (drogon::orm::DrogonDbException &e){
        LOG_ERROR<<e.base().what();
        message["status"] = 2;
        message["error"] = "list failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}

void FileController::downLoadFile(const HttpRequestPtr& req, 
    std::function<void (const HttpResponsePtr &)> &&callback, 
    Json::Value json
) const {
    LOG_DEBUG << "there is downLoadFile";
    auto dbclient = drogon::app().getDbClient();
    Json::Value message;
    try{
        std::string MD5 = json["MD5"].as<std::string>();
        std::string sql = "select * from file where MD5 = ?";
        auto future = dbclient->execSqlAsyncFuture(sql, MD5);
        auto result = future.get();
        if(result.empty()) {
            message["error"] = "download failed";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
            callback(resp);
            return ;
        }
    }catch (drogon::orm::DrogonDbException &e){
        message["status"] = 2;
        message["error"] = "download failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
        callback(resp);
        return ;
    }

    // auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    // callback(resp);
    auto resp = drogon::HttpResponse::newFileResponse("./uploads/Screenshot_2023-11-10_10-20-01.png");
    callback(resp);
}

void FileController::downLoadFileGet(const HttpRequestPtr& req, 
    std::function<void (const HttpResponsePtr &)> &&callback,
    std::string userId,
    std::string fileId
) const{
    LOG_DEBUG << "there is downLoadFileGet";
    auto dbclient = drogon::app().getDbClient();
    Json::Value message;
    std::string MD5, suffix;
    try{
        std::string sql = 
        " \
            SELECT f.MD5, f.fileExtension \
            FROM fileOfUser fu \
            JOIN file f ON fu.fileId = f.Id \
            WHERE fu.userId = ? AND fu.fileId = ?; \
        ";
        auto future = dbclient->execSqlAsyncFuture(sql, userId, fileId);
        auto result = future.get();
        if(result.empty()) {
            LOG_ERROR<<"没有查询到文件";
            message["status"] = 2;
            message["error"] = "there is no such file, download failed";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
            callback(resp);
            return ;
        }
        MD5 = result.at(0)["MD5"].as<std::string>();
        suffix = result.at(0)["fileExtension"].as<std::string>();
        suffix = result.at(0)["fileExtension"].as<std::string>();
    }catch (drogon::orm::DrogonDbException &e){
        LOG_ERROR<<e.base().what();
        message["status"] = 2;
        message["error"] = "download failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
        callback(resp);
        return ;
    }

    // auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    // callback(resp);
    auto resp = drogon::HttpResponse::newFileResponse("./uploads/"+MD5+suffix);
    callback(resp);

}

void FileController::downLoadFileAdmin(const HttpRequestPtr& req, 
    std::function<void (const HttpResponsePtr &)> &&callback,
    std::string fileId
) const{
    LOG_DEBUG << "there is downLoadFileAdmin";
    auto dbclient = drogon::app().getDbClient();
    Json::Value message;
    std::string MD5, suffix;
    try{
        std::string sql = 
        " \
            SELECT MD5, fileExtension \
            FROM file \
            WHERE id = ?; \
        ";
        auto future = dbclient->execSqlAsyncFuture(sql, fileId);
        auto result = future.get();
        if(result.empty()) {
            LOG_ERROR<<"没有查询到文件";
            message["status"] = 2;
            message["error"] = "there is no such file, download failed";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
            callback(resp);
            return ;
        }
        MD5 = result.at(0)["MD5"].as<std::string>();
        suffix = result.at(0)["fileExtension"].as<std::string>();
    }catch (drogon::orm::DrogonDbException &e){
        LOG_DEBUG<<e.base().what();
        message["status"] = 2;
        message["error"] = "download failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
        callback(resp);
        return ;
    }
    auto resp = drogon::HttpResponse::newFileResponse("./uploads/"+MD5+suffix);
    callback(resp);
}
