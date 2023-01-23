#include "WebSocketServerCtrl.h"
#include "json/JsonUtil.h"

namespace api::v1
{
	void WebSocketServer::handleNewMessage(const WebSocketConnectionPtr &wsConnPtr, std::string &&message, const WebSocketMessageType &type)
	{
		// wsConnPtr->setPingMessage
		// write your application logic here
		auto &sub = wsConnPtr->getContextRef<Subscriber>();
		LOG_INFO << "new websocket message:" << message;
		LOG_INFO << sub.userId;
		if (type == WebSocketMessageType::Ping)
		{
			LOG_DEBUG << "recv a ping";
		}
		else if (type == WebSocketMessageType::Text)
		{
			// custom protocol
			if (message == "ping")
			{
				LOG_INFO << "recv a ping";
				// ping pong for WS
				auto &sub = wsConnPtr->getContextRef<Subscriber>();
				WebSocketServer::m_Clients.publish(sub.userId, "pong");
				return;
			}

			auto jsonMsg = Utils::JsonUtil::parse(message);
			auto type = jsonMsg["type"].asString();
			auto msgId = jsonMsg["msgId"].asString();

			if (type == "ack")
			{
				// ack
				WebSocketServer::m_acks.publish(msgId, message);
				return;
			}

			auto receiver = jsonMsg["receiver"].asString();
			LOG_INFO << msgId;
			LOG_INFO << receiver;

			// tell sender that send message successed
			auto &sub = wsConnPtr->getContextRef<Subscriber>();
			WebSocketServer::m_Clients.publish(sub.userId, makeResponseForACK());

			// exe handler when receiver sure to received message
			makeSubscriberForACK(msgId, [](const std::string &msgId,
																		 const std::string &message)
													 {
														 // write your application logic here
													 });
			// send message to receiver
			WebSocketServer::m_Clients.publish(receiver, message);
		}
	}

	void WebSocketServer::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr &wsConnPtr)
	{
		// write your application logic here
		LOG_INFO << "new websocket connection!";
		auto jwt_userid = req->getAttributes()->get<int64_t>("jwt_userid");
		auto userid = std::to_string(jwt_userid);

		LOG_INFO << userid;

		// drogon websocket server support default ping/pong
		// wsConnPtr->disablePing();

		Subscriber sub;
		sub.userId = userid;
		sub.socketId = WebSocketServer::m_Clients.subscribe(sub.userId,
																												[wsConnPtr](const std::string &topic,
																																		const std::string &message)
																												{
																													LOG_INFO << "topic" << topic;
																													// Supress unused variable warning
																													(void)topic;
																													wsConnPtr->send(message);
																												});
		wsConnPtr->setContext(std::make_shared<Subscriber>(std::move(sub)));
	}

	void WebSocketServer::handleConnectionClosed(const WebSocketConnectionPtr &wsConnPtr)
	{
		// write your application logic here
		LOG_INFO << "websocket closed!";
		auto &sub = wsConnPtr->getContextRef<Subscriber>();
		WebSocketServer::m_Clients.unsubscribe(sub.userId, sub.socketId);
	}

	const std::string WebSocketServer::makeResponseForACK()
	{
		Json::Value resp;
		resp["code"] = k200OK;
		resp["msg"] = "Sunccessed.";
		auto jsonStr = Utils::JsonUtil::stringify(resp);

		return jsonStr;
	}

	void WebSocketServer::makeSubscriberForACK(const std::string &msgId, const ACKHandler &handler)
	{
		SubscriberForACK sub;
		sub.msgId = msgId;
		sub.subId = WebSocketServer::m_acks.subscribe(sub.msgId, handler);
	}
}
