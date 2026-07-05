#include "game/network_client.h"

#include <cassert>

#include "enet/enet.h"
#include "game/network_protocol.h"

// ------------------------

CNetworkSClient::CNetworkSClient()
{

}

// ------------------------

CNetworkSClient::~CNetworkSClient()
{

}

// ------------------------

void CNetworkSClient::Connect()
{

}

// ------------------------

void CNetworkSClient::Update()
{
  ENetEvent oEvent;
  while (enet_host_service(m_pHost, &oEvent, 0) > 0)
  {
    switch (oEvent.type)
    {
      case ENET_EVENT_TYPE_CONNECT: // ----------------------------------------------
      {
        printf("Client connected!\n");

        SNetStream oStream;
        byte pBuffer[NET_MAX_PACKET_SIZE];
        memset(pBuffer, 0, NET_MAX_PACKET_SIZE);
        InitNetStream(&oStream, pBuffer, NET_MAX_PACKET_SIZE);
        SPacketHeader oHeader;
        WriteHeader(&oStream, EMsgType::CONNECT_REQUEST);
        ENetPacket* pPacket = enet_packet_create(oStream.m_pData, oStream.m_uOffset, ENET_PACKET_FLAG_RELIABLE);
        if (enet_peer_send(oEvent.peer, 0, pPacket) < 0)
        {
          // Only destroy packets manually if send fails
          enet_packet_destroy(pPacket);
        }

        enet_host_flush(m_pHost);

        break;
      }
      case ENET_EVENT_TYPE_RECEIVE: // ----------------------------------------------
      {
        SNetStream oStream;
        InitNetStream(&oStream, oEvent.packet->data, oEvent.packet->dataLength);
        SPacketHeader oHeader;
        ReadHeader(&oStream, &oHeader);
        switch (oHeader.m_uMsgType)
        {
          SNetStream oStream;
          InitNetStream(&oStream, oEvent.packet->data, oEvent.packet->dataLength);
          SPacketHeader oHeader;
          ReadHeader(&oStream, &oHeader);
          switch (oHeader.m_uMsgType)
          {
            case EMsgType::CONNECT_ACCEPT:
            {
              printf("Connection accepted by the server\n");

              m_oGameClient.Begin(oEvent.peer);

              break;
            }
            default:
            {
              //assert(false);
              break;
            }
          }

        break;
      }
      case ENET_EVENT_TYPE_DISCONNECT:  // ----------------------------------------------
      {
        break;
      }
      case ENET_EVENT_TYPE_NONE:        // ----------------------------------------------
      {
        assert(false);
        break;
      }
    }

    enet_packet_destroy(oEvent.packet);
  }
}

// ------------------------