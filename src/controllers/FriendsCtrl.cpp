#include "FriendsCtrl.h"
#include "common/Common.h"

namespace api::v1
{
  Task<void> Friends::create(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
  {
    auto jsonPtr = *(req->getJsonObject());
    if (jsonPtr.empty() || !jsonPtr.isMember("friendId"))
    {
      auto resp = Utils::Common::makeHttpJsonResponse(k400BadRequest, "Invalid parameter.");
      callback(resp);
      co_return;
    }

    auto userid = req->getAttributes()->get<int64_t>("jwt_userid");
    auto dbClientPtr = getDbClient();

    try
    {
      // auto transPtr = co_await dbClientPtr->newTransactionCoro();
    }
    catch (const DrogonDbException &err)
    {
    }

    co_return;
  }

} // namespace api::v1