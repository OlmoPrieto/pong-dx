#include "game/network_server.h"

#include "enet/enet.h"
#include "game/client_connection.h"

// ------------------------

CNetworkServer::CNetworkServer()
{

}

// ------------------------

CNetworkServer::~CNetworkServer()
{

}

// ------------------------

void CNetworkServer::Update()
{
  ENetEvent oEvent;
  while (enet_host_service(m_pHost, &oEvent, 0) > 0)
  {
    switch (oEvent.type)
    {
      case ENET_EVENT_TYPE_CONNECT: // ----------------------------------------------
      {
        printf("Server connected!\n");

        CClientConnection* pClientConnection = new CClientConnection();
        pClientConnection->m_pServer = oEvent.peer;
        //pClientConnection->m_uPlayerId = m_vctClients.size();
        oEvent.peer->data = pClientConnection;

        m_vctClients.push_back(pClientConnection);

        break;
      }
      case ENET_EVENT_TYPE_RECEIVE: // ----------------------------------------------
      {
        printf("Server received a msg...\n");

        SNetStream oStream;
        InitNetStream(&oStream, oEvent.packet->data, oEvent.packet->dataLength);
        SPacketHeader oHeader;
        ReadHeader(&oStream, &oHeader);
        switch (oHeader.m_uMsgType)
        {
        case EMsgType::CONNECT_REQUEST: // ------------------------
        {
          printf("  Server received a Connect Request\n");

          SNetStream oSendStream;
          byte pBuffer[NET_MAX_PACKET_SIZE];
          memset(pBuffer, 0, NET_MAX_PACKET_SIZE);
          InitNetStream(&oSendStream, pBuffer, NET_MAX_PACKET_SIZE);
          SPacketHeader oHeader;
          WriteHeader(&oSendStream, EMsgType::CONNECT_ACCEPT);
          ENetPacket* pPacket = enet_packet_create(oSendStream.m_pData, oSendStream.m_uOffset, ENET_PACKET_FLAG_RELIABLE);
          if (enet_peer_send(oEvent.peer, 0, pPacket) < 0)
          {
            // Only destroy packets manually if send fails
            enet_packet_destroy(pPacket);
          }

          enet_host_flush(m_pHost);

          if (m_vctClients.size() == 2)
          {
            // Start game
            m_vctGameServers.push_back(CGameServer());
            CGameServer& oGameServer = m_vctGameServers.back();
            m_vctClients[0]->m_pGameServer = &oGameServer;
            m_vctClients[0]->m_uPlayerId = 0u;
            m_vctClients[1]->m_pGameServer = &oGameServer;
            m_vctClients[1]->m_uPlayerId = 1u;
            
            oGameServer.Begin(m_vctClients[0]->m_pServer, m_vctClients[1]->m_pServer);
          }

          break;
        }
        default:
        {
          assert(false);
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