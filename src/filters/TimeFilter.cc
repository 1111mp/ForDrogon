/**
 *
 *  TimeFilter.cc
 *
 */

#include "TimeFilter.h"
#include "common/Common.h"

using namespace drogon;
namespace api::v1::filters
{

	Task<HttpResponsePtr> TimeFilter::doFilter(const HttpRequestPtr &req)
	{
		auto userID = req->getHeader("userid");

		if (userID.empty())
		{
			co_return HttpResponse::newNotFoundResponse();
		}

		auto key = app().getCustomConfig()["redis"]["limit_key"].asString() + "_" + userID;
		const trantor::Date now = trantor::Date::date();
		auto redisClientPtr = getRedisClient();

		try
		{
			auto result = co_await redisClientPtr->execCommandCoro("get %s", key.data());
			if (result.isNil())
			{
				// First visit, insert visitDate.
				co_await updateCache(key, now);
				co_return {};
			}

			auto lastDate = trantor::Date(std::atoll(result.asString().data()));
			LOG_TRACE << "last:" << lastDate.toFormattedString(false);

			co_await updateCache(key, now);

			if (now > lastDate.after(10))
			{
				// 10 sec later can visit again.
				co_return {};
			}

			auto resp = Utils::Common::makeHttpJsonResponse(k429TooManyRequests, "Access interval should be at least 10 seconds.");
			co_return resp;
		}
		catch (const std::exception &err)
		{
			LOG_ERROR << err.what();
			auto resp = Utils::Common::makeHttpJsonResponse(k500InternalServerError, err.what());
			co_return resp;
		}
	}

	Task<void> TimeFilter::updateCache(const std::string &key, const trantor::Date &date)
	{
		auto value = std::to_string(date.microSecondsSinceEpoch());

		co_await getRedisClient()->execCommandCoro("set %s %s", key.data(), value.data());
	}

}