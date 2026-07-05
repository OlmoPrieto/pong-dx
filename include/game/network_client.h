#pragma once

#include "game/gameclient.h"

struct _ENetHost;
typedef struct _ENetHost ENetHost;
class CClientConnection;

class CNetworkSClient
{
public:
  CNetworkSClient();
  ~CNetworkSClient();

  void Connect();
  void Update();

private:
  CGameClient m_oGameClient;
  ENetHost* m_pHost = nullptr;
  ENetPeer* m_pServerPeer = nullptr;
};