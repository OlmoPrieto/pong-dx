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
#include "game/network_server.h"
#include "game/network_client.h"

int main(int argc, char** argv)
{
  enet_initialize();

  bool bIsServer = false;
  CNetworkServer oServer;
  CNetworkClient oClientLocal("127.0.0.1", 1234);
  CNetworkClient oClient("127.0.0.1", 1234);

  if (argc > 1)
  {
    if (strcmp(argv[1], "-server") == 0)
    {
      // Init Server
      // -- 
      oServer.Init();
      oClientLocal.Init();
      oClientLocal.Connect();
      bIsServer = true;
    }
    else if (strcmp(argv[1], "-client") == 0)
    {
      // Init clientA
      if (argc > 2)
      {
        // Read ip
        oClient.SetIp(argv[2]);
      }

      oClient.Init();
      oClient.Connect();
    }
  }

  if (bIsServer)
  {
    while (oClientLocal.WantClose() == false)
    {
      oServer.Update();
      oClientLocal.Update();
    }
  }
  else
  {
    while (oClient.WantClose() == false)
    {
      oClient.Update();
    }
  }

  enet_deinitialize();

  return 0;
}