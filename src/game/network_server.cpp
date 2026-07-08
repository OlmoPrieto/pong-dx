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
  for (uint32 i = 0; i < m_vctClients.size(); ++i)
  {
    if (m_vctClients[i])
    {
      delete m_vctClients[i];
    }
  }
}

// ------------------------

void CNetworkServer::Init()
{
  ENetAddress oAddress;
  oAddress.host = ENET_HOST_ANY;
  oAddress.port = 1234;

  m_pHost = enet_host_create(&oAddress, 4, 2, 0, 0);
  assert(m_pHost != nullptr);
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
        printf("Server received a connect request!\n");

        CClientConnection* pClientConnection = new CClientConnection();
        pClientConnection->m_pClient = oEvent.peer;
        //pClientConnection->m_uPlayerId = m_vctClients.size();
        oEvent.peer->data = pClientConnection;

        m_vctClients.push_back(pClientConnection);

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
          case EMsgType::CONNECT_REQUEST: // ------------------------
          {
            printf("  Server accepted a connect request\n");

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
              printf("2 players connected, starting game\n");

              // Start game
              m_vctGameServers.emplace_back();
              CGameServer& oGameServer = m_vctGameServers.back();
              m_vctClients[0]->m_pGameServer = &oGameServer;
              m_vctClients[0]->m_uPlayerId = 0u;
              m_vctClients[1]->m_pGameServer = &oGameServer;
              m_vctClients[1]->m_uPlayerId = 1u;
            
              oGameServer.Begin(this, m_vctClients[0], m_vctClients[1]);
            }

            break;
          }
          case EMsgType::PLAYER_INPUT: // ------------------------
          {
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

  for (uint32 i = 0; i < m_vctGameServers.size(); ++i)
  {
    m_vctGameServers[i].Loop();
  }
}

// ------------------------

bool CNetworkServer::SendToClient(SNetStream* _pStream, CClientConnection* _pClient, EMsgPriority _eMsgPriority)
{
  if (_pClient->m_pClient)
  {
    ENetPacket* pPacket = enet_packet_create(_pStream->m_pData, _pStream->m_uOffset, ENET_PACKET_FLAG_RELIABLE);
    int32 iReturnValue = enet_peer_send(_pClient->m_pClient, (int32)(_eMsgPriority), pPacket);
    if (iReturnValue < 0)
    {
      // Only destroy packets manually if send fails
      enet_packet_destroy(pPacket);
      return false;
    }

    enet_host_flush(m_pHost);
    //printf("Server sent data\n");

    return true;
  }

  return false;
}

// ------------------------

bool CNetworkServer::Receive(SNetStream* _pStream)
{
  if (m_pHost)
  {
    ENetEvent oEvent;
    bool bReceived = false;
    while (enet_host_service(m_pHost, &oEvent, 0) > 0)
    {
      if (oEvent.type == ENET_EVENT_TYPE_RECEIVE)
      {
        memcpy(_pStream->m_pData, oEvent.packet->data, oEvent.packet->dataLength);
        _pStream->m_uSize = oEvent.packet->dataLength;
        _pStream->m_uOffset = 0u;
        _pStream->m_bError = false;
        bReceived = true;

        enet_packet_destroy(oEvent.packet);

        // TODO: there could be more than one packet
      }
    }
    return bReceived;
  }

  return false;
}

// ------------------------