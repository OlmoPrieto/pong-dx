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

  void Init();  // Creates net host
  void Update();
  bool SendToClient(SNetStream* _pStream, CClientConnection* _pClient, EMsgPriority _eMsgPriority);
  bool Receive(SNetStream* _pStream);

private:
  std::vector<CGameServer> m_vctGameServers;
  std::vector<CClientConnection*> m_vctClients;
  ENetHost* m_pHost = nullptr;
};