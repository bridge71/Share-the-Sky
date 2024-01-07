#include <drogon/drogon.h>
int main() {
    
    drogon::app().loadConfigFile("../config.json");
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().enableSession();
    drogon::app().run();
    return 0;
}
