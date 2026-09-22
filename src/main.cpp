#include <iostream>
#include <cassert>

#include "enet/enet.h"
#include "game/network_server.h"
#include "game/network_client.h"

int main(int argc, char** argv)
{
  enet_initialize();

  bool bIsServer = false;
  bool bLocalGame = false;
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
      bIsServer = true;

      oClientLocal.Init();
      oClientLocal.Connect();

      if (bLocalGame)
      {
        oClient.SetIp("127.0.0.1");
        oClient.Init(bLocalGame);
        oClient.Connect();
      }
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
    else if (strcmp(argv[1], "-localgame") == 0)
    {
      bLocalGame = true;

      oServer.Init();
      oServer.SetLocalServer(true);
      bIsServer = true;

      oClientLocal.Init(false);
      oClientLocal.Connect();

      oClient.SetIp("127.0.0.1");
      oClient.Init(true);
      oClient.Connect();
    }
  }
  // TODO: if no argument is provided, assume it's client

  if (bIsServer)
  {
    while (oClientLocal.WantClose() == false)
    {
      oServer.Update();
      oClientLocal.Update();

      if (bLocalGame)
      {
        oClient.Update();
      }
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