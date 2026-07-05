#include <iostream>
#include <cassert>

//#include "game/game.h"
#include "game/gameclient.h"
#include "game/gameserver.h"
#include "game/ball.h"
#include "enet/enet.h"
#include "game/serverhandle.h"
#include "game/clienthandle.h"
#include "base/network_serialization.h"
#include "game/network_protocol.h"

int main(int argc, char** argv)
{
  enet_initialize();

  ENetAddress oAddress;
  oAddress.host = ENET_HOST_ANY;
  oAddress.port = 1234;
  
  ENetHost* pServer = nullptr;
  ENetHost* pClient = nullptr;
  ENetPeer* pPeer   = nullptr;

  uint32 uConnectedPlayers = 0u;

  if (argc > 1)
  {
    if (strcmp(argv[1], "-server") == 0)
    {
      // Init Server
      pServer = enet_host_create(&oAddress, 1, 2, 0, 0);
      assert(pServer != nullptr);
      // Hack for making the server owner a player as well
      ++uConnectedPlayers;
    }
    else if (strcmp(argv[1], "-client") == 0)
    {
      // Init client
      char sIp[3*4] = "127.0.0.1";
      if (argc > 2)
      {
        // Read ip
        memcpy(sIp, argv[2], strlen(argv[2]));
      }

      pClient = enet_host_create(nullptr, 1, 2, 0, 0);
      assert(pClient != nullptr);
      enet_address_set_host(&oAddress, sIp);
      oAddress.port = 1234;

      pPeer = enet_host_connect(pClient, &oAddress, 2, 0);
      assert(pPeer);
    }
  }

  // Create game
  CGameServer oGameServer;
  oGameServer.Init();

  CGameClient oGameClient;
  oGameClient.Init();

  while (oGameClient.WantClose() == false)
  {
    if (pServer)
    {
      oGameServer.Loop();
    }
    oGameClient.Loop();

    // -------- NETWORK --------
    ENetEvent oEvent;
    if (pServer)
    {
      while (enet_host_service(pServer, &oEvent, 0) > 0)
      {
        switch (oEvent.type)
        {
          case ENET_EVENT_TYPE_CONNECT:
          {
            printf("Server connected!\n");
            pPeer = oEvent.peer;

            break;
          }
          case ENET_EVENT_TYPE_RECEIVE:
          {
            printf("Server received a msg...\n");

            SNetStream oStream;
            InitNetStream(&oStream, oEvent.packet->data, oEvent.packet->dataLength);
            SPacketHeader oHeader;
            ReadHeader(&oStream, &oHeader);
            switch (oHeader.m_uMsgType)
            {
              case EMsgType::CONNECT_REQUEST:
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

                enet_host_flush(pServer);

                ++uConnectedPlayers;
                assert(uConnectedPlayers == 2u);
                oGameClient.Begin(nullptr);

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
          case ENET_EVENT_TYPE_DISCONNECT:
          {
            if (oEvent.peer == pPeer)
            {
              pPeer = nullptr;
            }
            else
            {
              assert(false);
            }

            break;
          }
          case ENET_EVENT_TYPE_NONE:
          {
            assert(false);
            break;
          }
        }

        enet_packet_destroy(oEvent.packet);
      }
    }
    else if (pClient && !oGameClient.GetGameStarted())
    {
      while (enet_host_service(pClient, &oEvent, 0) > 0)
      {
        switch (oEvent.type)
        {
          case ENET_EVENT_TYPE_CONNECT:
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

            enet_host_flush(pClient);

            break;
          }
          case ENET_EVENT_TYPE_RECEIVE:
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

                oGameClient.Begin(oEvent.peer);

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
          case ENET_EVENT_TYPE_DISCONNECT:
          {
            break;
          }
          case ENET_EVENT_TYPE_NONE:
          {
            assert(false);
            break;
          }
        }

        enet_packet_destroy(oEvent.packet);
      }
    }

    if (pServer && uConnectedPlayers == 2u && !oGameServer.GetGameStarted())
    {
      // - Prepare handles
      // - Begin game
      /*uint32 uClient0Ip = 
        (127  << 24) | 
        (0    << 16) | 
        (0    << 8)  | 
        (0    << 0);
      uint32 uClient1Ip = 
        (127  << 24) |
        (0    << 16) |
        (0    << 8)  |
        (0    << 0);*/
      assert(pPeer);
      oGameServer.Begin(nullptr, pPeer);
    }

    //printf("Server tick: %llu\n", oGameServer.m_uLogicTick);
  }

  if (pClient)
  {
    enet_peer_disconnect(pPeer, 0);
  }
  enet_deinitialize();

  return 0;
}