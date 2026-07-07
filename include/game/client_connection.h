#pragma once

#include <climits>

#include "base/types.h"

struct _ENetPeer;
typedef struct _ENetPeer ENetPeer;
class CGameServer;

class CClientConnection
{
public:
  ENetPeer* m_pClient = nullptr;
  CGameServer* m_pGameServer = nullptr;
  uint32 m_uPlayerId = UINT_MAX;

private:
};