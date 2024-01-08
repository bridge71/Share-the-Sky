#include "UserController.h"
#include <regex>
// Add definition of your processing function here

#define KB (1024)
#define MB ((KB)*(KB))
#define GB ((MB)*(KB))

void UserController::addUser(const HttpRequestPtr& req,
    std::function<void (const HttpResponsePtr &)> &&callback
) const {
    auto dbClient = drogon::app().getDbClient();
    auto transaction = dbClient->newTransaction();
    std::string sql = "INSERT INTO user(userName, passWord, permissions, capacity, remaining, email) VALUE(?, ?, ?, ?, ?, ?)";
    std::string userName, passWord, email;
    auto resJson = req->getJsonObject();
    userName = (*resJson)["userName"].asString();
    passWord = (*resJson)["passWord"].asString();
    email = (*resJson)["email"].asString(); 
    LOG_DEBUG << "email is " << email;
    std::regex email_regex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+.[A-Za-z]{2,}$");  
    
    Json::Value json;
    if (std::regex_match(email, email_regex)) {  
        LOG_DEBUG << "Valid email address";  
    } else {  
        LOG_ERROR << "Invalid email address";  
        json["status"] = 1;
        json["warning"] = "Invalid email address";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
        callback(resp);
        return;

    }  
    LOG_DEBUG<<userName;
    LOG_DEBUG<<passWord;
    try {
        std::string query = "select * from user where userName = ? or email = ?";
        auto result6 = transaction->execSqlSync(query, userName, email);
        if(result6.size() != 0){
            json["status"] = 1;
            json["warning"] = "the userName is occupied or the email is occupied";
            LOG_ERROR << " the userName is occupied or the email is occupied";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
            callback(resp);
            return;
        }
        transaction->execSqlSync(sql, userName, passWord, 2, 1*GB, 1*GB, email);
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
        json["status"] = 0;
        // json['data']['userId'] = 
    } catch (const drogon::orm::DrogonDbException &e){
        json["status"] = 2;
        json["error"] = "add user error";
        LOG_ERROR<<e.base().what();
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

        json["status"] = 0;
        //json["message"] = "delete success";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
        callback(resp);
        return ;
    } catch (const drogon::orm::DrogonDbException &e) {
        LOG_ERROR<<e.base().what();
        json["status"] = 2;
        json["error"] = "delete failed";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
        callback(resp);
        return ;
    }

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
        message["status"] = 0;
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_ERROR<<e.base().what();
        message["status"] = 2;
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
        message["status"] = 0;
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_ERROR<<e.base().what();
        message["status"] = 2;
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
        message["status"] = 0;
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_ERROR<<e.base().what();
        message["status"] = 2;
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
    LOG_DEBUG << "userId " << userId;
    LOG_DEBUG << "permissions " << permission;
    try {
        std::string sql = "UPDATE user set permissions = ? WHERE id=?;";
        dbClient->execSqlSync(sql, permission, userId);
        message["status"] = 0;
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_ERROR<<e.base().what();
        message["status"] = 2;
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
            json["status"] = 1;
            json["warning"] = "there is no such user";
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
            json["status"] = 0;
        }
    } catch (const drogon::orm::DrogonDbException &e) {
        json["status"] = 2;
        json["error"] = "error when selecting";
        LOG_ERROR<<e.base().what();
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
            json["status"] = 1;
            json["warning"] = "there is no such user or password is wrong"; 
        }else{
            auto session = req->session();
            session->insert("login", "ok");
            json["data"]["userId"] = ret.at(0)["id"].as<int>();
            LOG_DEBUG<<"userId:"<<ret.at(0)["id"].as<int>();
            json["data"]["userName"] = ret.at(0)["userName"].as<std::string>();
            LOG_DEBUG<<"userName:"<<ret.at(0)["userName"].as<std::string>();
            json["data"]["permission"] = (ret.at(0)["permissions"].as<int>() == 1);
            LOG_DEBUG<<"权限："<<ret.at(0)["permissions"].as<int>();
            int userId = ret.at(0)["id"].as<int>();
            sql = "SELECT * FROM folderOfUser WHERE userId=?;";
            auto ret2 = dbClient->execSqlSync(sql, userId);
            json["data"]["rootFolder"] = ret2.at(0)["folderId"].as<std::string>();
            LOG_DEBUG<<"根目录:"<<ret2.at(0)["folderId"].as<std::string>();
            json["status"] = 0;
        }
    } catch (const drogon::orm::DrogonDbException &e) {
        json["status"] = 2;
        json["error"] = "error when login";
        LOG_ERROR<<e.base().what();
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
            item["userType"] = row["permissions"].as<int>();
            json.append(item);
        }
 //       json["status"] = 0;
    } catch (drogon::orm::DrogonDbException &e) {
        LOG_ERROR<<e.base().what();
        json["status"] = 2;
        json["error"] = "get user list failed";
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    callback(resp);
}
