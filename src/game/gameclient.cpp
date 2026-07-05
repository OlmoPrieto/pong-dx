#include "game/gameclient.h"

#include <random>
#include <chrono>

#include "raylib/raylib.h"
#include "game/network_protocol.h"
#include "game/gameserver.h"

// ------------------------------------------------
// ------------------------------------------------

CGameClient::CClientBall::CClientBall()
{

}

// ------------------------

CGameClient::CClientBall::~CClientBall()
{
  
}

// ------------------------

void CGameClient::CClientBall::Init()
{
  assert(!m_spTexture);

  // Image img = LoadImage("assets/ball_basic.png");
  // ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  // Texture2D ballTex = LoadTextureFromImage(img);
  // m_spTexture = std::make_shared<Texture2D>(ballTex);
  m_spTexture = std::make_shared<Texture2D>(LoadTexture("assets/ball_basic.png"));
  printf("Client ball creation w:%u h:%d\n", m_spTexture->width, m_spTexture->height);
}

// ------------------------

void CGameClient::CClientBall::UnloadResources()
{
  UnloadTexture(*m_spTexture.get());
}

// ------------------------

void CGameClient::CClientBall::Draw()
{
  DrawTextureV(*m_spTexture, Vector2{ m_v2Pos.x - m_spTexture->width * 0.5f,
    m_v2Pos.y - m_spTexture->height * 0.5f },
    WHITE);
}

// ------------------------------------------------
// ------------------------------------------------

CGameClient::CGameClient(void)
  : m_oPlayerPaddle(this, { sm_uWindowWidth - 8.0f * 4.0f, sm_uWindowHeight - 32.0f * 4.0f }, false)
  , m_oEnemyPaddle(this, { 8.0f * 4.0f, 32.0f * 4.0f }, false)
{
  for (uint32 i = 0; i < 1; ++i)
  {
    m_vctBalls.push_back(CClientBall());
  }

  m_oGameState.m_uNumBalls = 1;
  m_oGameState.m_vctBallsPos.push_back(CVector2D::v2ZERO);
  m_oGameState.m_vctBallsVel.push_back(CVector2D::v2ZERO);
}

// ------------------------

CGameClient::~CGameClient()
{
  for (uint32 i = 0; i < 1; ++i)
  {
    m_vctBalls[i].UnloadResources();
  }
}

// ------------------------

void CGameClient::Init()
{
  // Window creation
  //SetConfigFlags(FLAG_VSYNC_HINT);
  InitWindow(CGame::sm_uWindowWidth, CGame::sm_uWindowHeight, "pong-dx");
  SetTargetFPS(60);

  // For texture loading, raylib must be initialized
  m_oPlayerPaddle.Init();
  m_oEnemyPaddle.Init();

  for (uint32 i = 0; i < 1; ++i)
  {
    m_vctBalls[i].Init();
  }
}

// ------------------------

void CGameClient::End()
{
  // Raylib
  CloseWindow();
}

// ------------------------

void CGameClient::Begin(ENetPeer* _pServerPeer)
{
  m_oGameServer.Setup(_pServerPeer);

  m_bGameStarted = true;
}

// ------------------------

void CGameClient::Loop()
{
  m_bWantClose = WindowShouldClose();

  // TODO: to avoid checking against booleans every time (like m_bGameStarted),
  //  use different scenes, like WaitingRoom, Game, etc.

  if (m_bWantClose == false && m_bGameStarted == true)
  {
    //printf("Looping on client\n");
    
    // -------- INPUT --------
    ProcessInput();

    // -------- UPDATE --------
    Update(GetFrameTime());

    // -------- DRAW --------
    BeginDrawing();
    ClearBackground(BLACK);

    //DrawText("It works!", 20, 20, 20, WHITE);

    Draw();

    EndDrawing();
  }
}

// ------------------------

void CGameClient::ReceiveGameState()
{
  byte pBuffer[NET_MAX_PACKET_SIZE];
  SNetStream oStream;
  InitNetStream(&oStream, pBuffer, NET_MAX_PACKET_SIZE);
  if (m_oGameServer.Receive(&oStream))
  {
    //printf("Received something on client\n");
    SPacketHeader oHeader;
    ReadHeader(&oStream, &oHeader);

    if (oHeader.m_uMsgType == EMsgType::GAME_STATE)
    {
      //printf("  Received GAME_STATE on client\n");

      m_oGameState.Read(&oStream);

      if (m_oGameState.m_uNumBalls != m_vctBalls.size())
      {
        assert(false);
        // TODO: need to create a new ball (??)
      }

      for (uint32 i = 0; i < m_vctBalls.size(); ++i)
      {
        m_vctBalls[i].m_v2Pos = m_oGameState.m_vctBallsPos[i];
      }

      for (uint32 i = 0; i < m_vctBalls.size(); ++i)
      {
        m_vctBalls[i].m_v2Velocity = m_oGameState.m_vctBallsVel[i];
      }

      m_oGameState.m_oTimeReceived = std::chrono::high_resolution_clock::now();

      //printf("Ball pos: %.3f,%.3f\n", m_vctBalls[0].m_v2Pos.x, m_vctBalls[0].m_v2Pos.y);

      //printf("GameStateFrame: %llu | PacketFrame: %llu\n", m_uLogicTick, m_oGameState.m_uFrame);
    }
  }
}

// ------------------------

void CGameClient::ProcessInput()
{
  // Receive state from server
  ReceiveGameState();

  // Read input for current player
  m_oPlayerPaddle.Update(0.016f);

  // Do predictions based on received info and read info

  // Update enemy paddle
  // m_oEnemyPaddle.m_v2Pos = ServerEnemyPaddlePos();
  //  NOTE: right now is moved by AI, so just update here.
  //    Next step is to receive fake info from the server
  //    which will do de AI update of the paddle.
  m_oEnemyPaddle.Update(0.016f);
}

// ------------------------

void CGameClient::Update(float _fDt)
{
  // TODO: send info to server?

  UpdateBalls(0.016f);

  UpdateGameState();

  ++m_uLogicTick;
}

// ------------------------

void CGameClient::Draw()
{
  m_oPlayerPaddle.Draw();
  m_oEnemyPaddle.Draw();
  for (uint32 i = 0; i < m_vctBalls.size(); ++i)
  {
    m_vctBalls[i].Draw();
  }
}

// ------------------------

bool CGameClient::WantClose()
{
  return m_bWantClose;
}

// ------------------------

void CGameClient::UpdateBalls(float _fDt)
{
  if (!m_oPrevGameState.IsValid() || !m_oGameState.IsValid())
  {
    return;
  }

  std::chrono::time_point<std::chrono::high_resolution_clock> m_oNow = std::chrono::high_resolution_clock::now();

  const std::chrono::duration<float> oTimeDiff = m_oNow - m_oGameState.m_oTimeReceived;

  for (uint32 i = 0; i < m_vctBalls.size(); ++i)
  {
#define USE_PREDICTION 0
#if USE_PREDICTION
    CVector2D v2PredictedPos = m_vctBalls[i].m_v2Pos + m_oGameState.m_vctBallsVel[i] * (_fDt);
    CVector2D v2PredictedServerPos = m_oGameState.m_vctBallsPos[i] + m_oGameState.m_vctBallsVel[i] * (oTimeDiff.count());
    CVector2D v2Error = v2PredictedServerPos - v2PredictedPos;
    m_vctBalls[i].m_v2Pos = Lerp(m_vctBalls[i].m_v2Pos, v2PredictedPos, 0.2f) + v2Error * 0.1f;
#endif // USE_PREDICTION

#define PREDICT_VELOCITY 1
#if PREDICT_VELOCITY
    /*CVector2D v2PredictedPos = m_oGameState.m_vctBallsPos[i] + m_oGameState.m_vctBallsVel[i] * _fDt;
    m_vctBalls[i].m_v2Pos += v2PredictedPos - m_vctBalls[i].m_v2Pos;*/

    //m_vctBalls[i].m_v2Pos = m_oGameState.m_vctBallsVel[i] * _fDt;

#endif // PREDICT_VELOCITY

#if 0
    m_vctBalls[i].m_v2Pos = m_oGameState.m_vctBallsPos[i];
#endif
  }
}

// ------------------------

void CGameClient::UpdateGameState()
{
  m_oPrevGameState = m_oGameState;
}

// ------------------------
// ------------------------
// ------------------------