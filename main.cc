#include <drogon/drogon.h>
int main() {
    //Set HTTP listener address and port
   // drogon::app().addListener("0.0.0.0", 5555);
    
    drogon::app().loadConfigFile("../config.json");
    drogon::app().setExceptionHandler([](const std::exception &e,
                const drogon::HttpRequestPtr &req,
                std::function<void (const drogon::HttpResponsePtr &)> &&callback){
                    Json::Value json;
                    json["error"] = e.what();
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
                    callback(resp);
                });
    //Load config file
    //drogon::app().loadConfigFile("../config.yaml");
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
