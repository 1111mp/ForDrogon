#include <drogon/drogon.h>

int main()
{
  drogon::app().addListener("127.0.0.1", 8848);
  // Load config file
  // drogon::app().loadConfigFile("../config.json");
  drogon::app().createDbClient("mysql", "127.0.0.1", 3306, "database_test", "root", "zhangyifan818").run();

  return 0;
}
