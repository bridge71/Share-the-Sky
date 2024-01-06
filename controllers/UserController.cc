#include "UserController.h"

// Add definition of your processing function here

#define KB (1024)
#define MB ((KB)*(KB))
#define GB ((MB)*(KB))

void UserController::addUser(const HttpRequestPtr& req,
    std::function<void (const HttpResponsePtr &)> &&callback
) const {
    auto dbClient = drogon::app().getDbClient();
    auto transaction = dbClient->newTransaction();
    std::string sql = "INSERT INTO user(userName, passWord, permissions, capacity, remaining) VALUE(?, ?, ?, ?, ?)";
    std::string userName, passWord;
    auto resJson = req->getJsonObject();
    userName = (*resJson)["userName"].asString();
    passWord = (*resJson)["passWord"].asString();
    LOG_DEBUG<<userName;
    LOG_DEBUG<<passWord;
    Json::Value json;
    try {
        transaction->execSqlSync(sql, userName, passWord, 2, 1*GB, 1*GB);
        auto result = transaction->execSqlSync("select max(id) from user");
        std::string  id;
        for(auto row : result){
            id = row["max(id)"].as<std::string>();
        }
        transaction->execSqlSync("insert into folder (folderName, fatherFolderId) values(?, 0)", id);
        auto result2 = transaction->execSqlSync("select * from folder where folderName = ? and fatherFolderId = 0", id);
        int folderId;
        for(auto row : result2){
            folderId = row["folderId"].as<int>();
        }
        transaction->execSqlSync("insert into folderOfUser (folderId, userId) values(?, ?)", folderId, id); 
        json["status"] = true;
        // json['data']['userId'] = 
    } catch (const drogon::orm::DrogonDbException &e){
        json["status"] = false;
        LOG_DEBUG<<e.base().what();
        transaction->rollback();
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    callback(resp);
}

void UserController::removeUser(const HttpRequestPtr& req,
    std::function<void (const HttpResponsePtr &)> &&callback
) const {
    auto resJson = req->getJsonObject();
    std::string removeUserId = (*resJson)["removeUserId"].asString();
    LOG_DEBUG<<"remove:"<<removeUserId;
    Json::Value json;
    auto dbClient = drogon::app().getDbClient();

    try {
        std::string sql;
        sql = "DELETE FROM user WHERE id=?";
        dbClient->execSqlSync(sql, removeUserId);

        json["status"] = true;
        json["message"] = "delete success";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
        callback(resp);
        return ;
    } catch (const drogon::orm::DrogonDbException &e) {
        LOG_DEBUG<<e.base().what();
        json["status"] = false;
        json["message"] = "delete failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
        callback(resp);
        return ;
    }

    json["status"] = false;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    callback(resp);
}

void UserController::modifyUser(const HttpRequestPtr& req,
    std::function<void (const HttpResponsePtr &)> &&callback
) const {
    Json::Value message;
    auto json = req->getJsonObject();
    std::string userId = (*json)["userId"].as<std::string>();
    std::string userName = (*json)["userName"].as<std::string>();
    auto dbClient = drogon::app().getDbClient();
    try {
        std::string sql = "UPDATE user set userName = ? WHERE id=?;";
        dbClient->execSqlSync(sql, userName, userId);
        message["status"] = "modify user name success";
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_DEBUG<<e.base().what();
        message["error"] = "modify user name failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}

void UserController::modifyUserName(const HttpRequestPtr& req,
    std::function<void (const HttpResponsePtr &)> &&callback
) const {
    Json::Value message;
    auto json = req->getJsonObject();
    std::string userId = (*json)["userId"].as<std::string>();
    std::string userName = (*json)["userName"].as<std::string>();
    auto dbClient = drogon::app().getDbClient();
    try {
        std::string sql = "UPDATE user set userName = ? WHERE id=?;";
        dbClient->execSqlSync(sql, userName, userId);
        message["status"] = "modify user name success";
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_DEBUG<<e.base().what();
        message["error"] = "modify user name failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}

void UserController::modifyUserPassword(const HttpRequestPtr& req,
    std::function<void (const HttpResponsePtr &)> &&callback
) const {
    Json::Value message;
    auto json = req->getJsonObject();
    std::string userId = (*json)["userId"].as<std::string>();
    std::string passWord = (*json)["passWord"].as<std::string>();
    auto dbClient = drogon::app().getDbClient();
    try {
        std::string sql = "UPDATE user set passWord = ? WHERE id=?;";
        dbClient->execSqlSync(sql, passWord, userId);
        message["status"] = "modify user password success";
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_DEBUG<<e.base().what();
        message["error"] = "modify user passWord failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}

void UserController::modifyUserPermissions(const HttpRequestPtr& req,
    std::function<void (const HttpResponsePtr &)> &&callback
) const {
    Json::Value message;
    auto json = req->getJsonObject();
    std::string userId = (*json)["userIdM"].as<std::string>();
    std::string permission = (*json)["permissions"].as<std::string>();
    auto dbClient = drogon::app().getDbClient();
    try {
        std::string sql = "UPDATE user set permissions = ? WHERE id=?;";
        dbClient->execSqlSync(sql, permission, userId);
        message["status"] = "modify user permissions success";
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_DEBUG<<e.base().what();
        message["error"] = "modify user permissions failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
    callback(resp);
}

//by ID
void UserController::selectUser(const HttpRequestPtr& req,
    std::function<void (const HttpResponsePtr &)> &&callback
) const {
    auto resJson = req->getJsonObject();
    int userId = (*resJson)["userId"].asInt();

    auto dbClient = drogon::app().getDbClient();
    std::string sql = "SELECT * FROM user WHERE id=?";
    Json::Value json;
    try {
        auto ret = dbClient->execSqlSync(sql, userId);
        if(ret.empty()) {
            json["status"] = false;
        }else{
            for (const auto &user:ret) {
                json["data"]["userId"] = user["id"].as<int>();
                LOG_DEBUG<<user["id"].as<int>();
                json["data"]["userName"] = user["userName"].as<std::string>();
                LOG_DEBUG<<user["userName"].as<std::string>();
                json["data"]["permissions"] = user["permissions"].as<int>();
                LOG_DEBUG<<user["permissions"].as<int>();
                json["data"]["capacity"] = user["capacity"].as<double>();
                LOG_DEBUG<<user["capacity"].as<double>();
            }
            json["status"] = true;
        }
    } catch (const drogon::orm::DrogonDbException &e) {
        json["status"] = false;
        LOG_DEBUG<<e.base().what();
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    callback(resp);
}

void UserController::loginUser(const HttpRequestPtr& req,
    std::function<void (const HttpResponsePtr &)> &&callback
) const {
    auto resJson = req->getJsonObject();
    std::string userName = (*resJson)["userName"].asString();
    std::string passWord = (*resJson)["passWord"].asString();
    LOG_DEBUG<<userName;
    LOG_DEBUG<<passWord;

    auto dbClient = drogon::app().getDbClient();
    std::string sql = "SELECT * FROM user WHERE (userName=? AND passWord=?)";
    Json::Value json;
    try {
        auto ret = dbClient->execSqlSync(sql, userName, passWord);
        if(ret.empty()) {
            json["status"] = false;
        }else{
            for (const auto &user:ret) {
                json["data"]["userId"] = user["id"].as<int>();
                LOG_DEBUG<<user["id"].as<int>();
                json["data"]["userName"] = user["userName"].as<std::string>();
                LOG_DEBUG<<user["userName"].as<std::string>();
                json["data"]["permission"] = (user["permissions"].as<int>() == 1);
                LOG_DEBUG<<"权限："<<user["permissions"].as<int>();
            }
            json["status"] = true;
        }
    } catch (const drogon::orm::DrogonDbException &e) {
        json["status"] = false;
        LOG_DEBUG<<e.base().what();
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    callback(resp);
}

void UserController::listAllUser(const HttpRequestPtr& req,
    std::function<void (const HttpResponsePtr &)> &&callback
) const {
    Json::Value json;
    auto dbClient = drogon::app().getDbClient();
    try {
        std::string sql = "SELECT * FROM user;";
        auto ret = dbClient->execSqlSync(sql);
        for (const auto &row : ret){
            Json::Value item;
            item["userId"] = row["id"].as<std::string>();
            item["userName"] = row["userName"].as<std::string>();
            item["permission"] = row["permissions"].as<int>();
            json.append(item);
        }
        // json["status"] = "ok";
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_DEBUG<<e.base().what();
        json["error"] = "get user list failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    callback(resp);
}
