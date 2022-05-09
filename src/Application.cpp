#include <drogon/drogon.h>
#include <sodium.h>

int main()
{
  if (sodium_init() == -1)
    return 1;
  // Load config file
  drogon::app().loadConfigFile("../config.json").run();

  return 0;
}
