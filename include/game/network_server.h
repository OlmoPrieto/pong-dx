#pragma once

#include <vector>

#include "game/gameserver.h"

struct _ENetHost;
typedef struct _ENetHost ENetHost;
class CClientConnection;

class CNetworkServer
{
public:
  CNetworkServer();
  ~CNetworkServer();

  void Update();

private:
  std::vector<CGameServer> m_vctGameServers;
  std::vector<CClientConnection*> m_vctClients;
  ENetHost* m_pHost = nullptr;
};