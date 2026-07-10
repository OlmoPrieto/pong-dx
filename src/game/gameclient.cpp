#include "game/gameclient.h"

#include <random>
#include <chrono>

#include "raylib/raylib.h"
#include "game/network_protocol.h"
#include "game/network_client.h"
#include "game/gameserver.h"

// ------------------------------------------------
// ------------------------------------------------

CClientBall::CClientBall()
{

}

// ------------------------

CClientBall::~CClientBall()
{
  
}

// ------------------------

void CClientBall::Init()
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

void CClientBall::UnloadResources()
{
  if (m_spTexture.get())
  {
    UnloadTexture(*m_spTexture.get());
  }
}

// ------------------------

void CClientBall::Draw()
{
  DrawTextureV(*m_spTexture, Vector2{ m_v2Pos.x - m_spTexture->width * 0.5f,
    m_v2Pos.y - m_spTexture->height * 0.5f },
    WHITE);
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

CGameClient::CGameClient(void)
  : m_oPlayerPaddle(this, { sm_uWindowWidth - 8.0f * 4.0f, sm_uWindowHeight - 32.0f * 4.0f }, true)
  , m_oEnemyPaddle(this,  { 8.0f * 4.0f, 32.0f * 4.0f }, false)
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

  if (m_bWantClose)
  {
    CloseWindow();
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
  //CloseWindow();
}

// ------------------------

void CGameClient::Begin(CNetworkClient* _pNetworkClient, uint8 _uPlayerId)
{
  m_pNetworkClient = _pNetworkClient;
  m_uClientId = _uPlayerId;
  assert(m_uClientId != UINT8_MAX);

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
    SendInput();

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

void CGameClient::OnGameStateReceived(SNetStream* _pStream)
{
  m_oGameState.Read(_pStream);

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

  m_oPlayerPaddle.m_v2Pos = m_oGameState.m_v2Player0Pos;
  m_oEnemyPaddle.m_v2Pos  = m_oGameState.m_v2Player1Pos;

  //printf("Ball pos: %.3f,%.3f\n", m_vctBalls[0].m_v2Pos.x, m_vctBalls[0].m_v2Pos.y);

  //printf("GameStateFrame: %llu | PacketFrame: %llu\n", m_uLogicTick, m_oGameState.m_uFrame);
}

// ------------------------

void CGameClient::ProcessInput()
{
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

void CGameClient::SendInput()
{
  SNetStream oStream;
  byte pBuffer[NET_MAX_PACKET_SIZE];
  memset(pBuffer, 0, NET_MAX_PACKET_SIZE);
  InitNetStream(&oStream, pBuffer, NET_MAX_PACKET_SIZE);

  SPacketHeader oHeader;
  WriteHeader(&oStream, EMsgType::PLAYER_INPUT);

  WriteUint8(&oStream, m_uClientId);
  WriteFloat32(&oStream, m_oPlayerPaddle.m_v2Pos.x);
  WriteFloat32(&oStream, m_oPlayerPaddle.m_v2Pos.y);

  m_pNetworkClient->Send(&oStream, EMsgPriority::HIGH);

  m_oPlayerPaddle.m_v2Pos.y = 0.0f; // HACK: if you ever see the paddle on height 0, a bug happened
  m_oEnemyPaddle.m_v2Pos.y = 0.0f; // HACK: if you ever see the paddle on height 0, a bug happened
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