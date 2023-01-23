#pragma once

#include <drogon/WebSocketController.h>
#include <drogon/PubSubService.h>

using namespace drogon;

namespace api::v1
{
  using ACKHandler =
      std::function<void(const std::string &, const std::string &)>;

  struct Subscriber
  {
    std::string userId;
    drogon::SubscriberID socketId;
  };

  struct SubscriberForACK
  {
    std::string msgId;
    drogon::SubscriberID subId;
  };

  class WebSocketServer : public drogon::WebSocketController<WebSocketServer>
  {
  public:
    void handleNewMessage(const WebSocketConnectionPtr &,
                          std::string &&,
                          const WebSocketMessageType &) override;
    void handleNewConnection(const HttpRequestPtr &,
                             const WebSocketConnectionPtr &) override;
    void handleConnectionClosed(const WebSocketConnectionPtr &) override;

    WS_PATH_LIST_BEGIN
    // list path definitions here;
    WS_PATH_ADD("/im");
    WS_PATH_LIST_END

  public:
    static inline PubSubService<std::string> m_Clients;
    static inline PubSubService<std::string> m_acks;

  private:
    const std::string makeResponseForACK();
    void makeSubscriberForACK(const std::string &msgId, const ACKHandler &);
  };
}
