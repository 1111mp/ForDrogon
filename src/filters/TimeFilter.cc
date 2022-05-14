/**
 *
 *  TimeFilter.cc
 *
 */

#include "TimeFilter.h"

using namespace drogon;
namespace api::v1::filters
{

	void TimeFilter::doFilter(const HttpRequestPtr &req,
														FilterCallback &&fcb,
														FilterChainCallback &&fccb)
	{
		auto userID = req->getHeader("userid");

		if (userID.empty())
		{
			return fcb(HttpResponse::newNotFoundResponse());
		}

		auto key = app().getCustomConfig()["redis"]["limit_key"].asString() + "_" + userID;
		const trantor::Date now = trantor::Date::date();
		auto redisClientPtr = getRedisClient();

		try
		{
			auto lastDate = redisClientPtr->execCommandSync<trantor::Date>(
					[](const nosql::RedisResult &r)
					{
						return trantor::Date(std::atoll(r.asString().data()));
					},
					"get %s", key.data());
			LOG_TRACE << "last:" << lastDate.toFormattedString(false);

			updateCache(key, now);

			if (now > lastDate.after(10))
			{
				// 10 sec later can visit again.
				return fccb();
			}

			Json::Value resultJson;
			resultJson["code"] = k429TooManyRequests;
			resultJson["msg"] = "Access interval should be at least 10 seconds.";
			auto resp = HttpResponse::newHttpJsonResponse(resultJson);
			resp->setStatusCode(k429TooManyRequests);
			return fcb(resp);
		}
		catch (const std::exception &e)
		{
			// First visit, insert visitDate.
			try
			{
				updateCache(key, now);
			}
			catch (const std::exception &err)
			{
				LOG_ERROR << err.what();
				auto resp = HttpResponse::newHttpJsonResponse(err.what());
				resp->setStatusCode(k500InternalServerError);
				return fcb(resp);
			}

			return fccb();
		}
	}

	void TimeFilter::updateCache(const std::string &key, const trantor::Date &date)
	{
		auto value = std::to_string(date.microSecondsSinceEpoch());

		getRedisClient()->execCommandSync<std::string>(
				[](const nosql::RedisResult &r)
				{
					return r.asString();
				},
				"set %s %s", key.data(), value.data());
	}

}