#pragma once

#include "base/maths.h"
#include "base/types.h"
#include "raylib/raylib.h"

// Los controles se construyen cada frame, entre BeginDrawing() y EndDrawing().
// El resultado del constructor se puede usar directamente:
// if (CButton("Jugar", CVector2D(20.0f, 20.0f))) { ... }

enum class EUIElementState : uint8
{
  NORMAL,
  HOVERED,
  PRESSED,
  DISABLED
};

struct SUIStateColors
{
  Color m_cFill = { 54, 62, 78, 255 };
  Color m_cBorder = { 145, 158, 181, 255 };
  Color m_cText = WHITE;
};

struct SUIButtonConfig
{
  SUIStateColors m_oNormal;
  SUIStateColors m_oHovered = { { 72, 83, 104, 255 }, { 188, 205, 232, 255 }, WHITE };
  SUIStateColors m_oPressed = { { 35, 42, 55, 255 }, { 108, 120, 143, 255 }, { 220, 220, 220, 255 } };
  SUIStateColors m_oDisabled = { { 42, 45, 52, 255 }, { 74, 78, 88, 255 }, { 130, 133, 141, 255 } };
  // m_v2Size es el tamaño mínimo. El botón crece si el texto y este relleno lo requieren.
  CVector2D m_v2Size = CVector2D(140.0f, 36.0f);
  CVector2D m_v2TextPadding = CVector2D(10.0f, 8.0f);
  float m_fBorderWidth = 2.0f;
  float m_fCornerRadius = 0.18f;
  int32 m_iFontSize = 20;
  bool m_bEnabled = true;
};

struct SUICheckboxConfig
{
  SUIStateColors m_oNormal = { { 34, 39, 49, 255 }, { 145, 158, 181, 255 }, WHITE };
  SUIStateColors m_oHovered = { { 48, 57, 72, 255 }, { 188, 205, 232, 255 }, WHITE };
  SUIStateColors m_oPressed = { { 26, 31, 39, 255 }, { 108, 120, 143, 255 }, { 220, 220, 220, 255 } };
  SUIStateColors m_oDisabled = { { 42, 45, 52, 255 }, { 74, 78, 88, 255 }, { 130, 133, 141, 255 } };
  Color m_cMark = { 75, 179, 105, 255 };
  CVector2D m_v2BoxSize = CVector2D(22.0f, 22.0f);
  float m_fTextSpacing = 8.0f;
  float m_fBorderWidth = 2.0f;
  float m_fCornerRadius = 0.16f;
  float m_fTextVerticalOffset = 2.0f;
  int32 m_iFontSize = 20;
  bool m_bEnabled = true;
};

class CButton
{
public:
  CButton(const char* _pText = "", const CVector2D& _v2Position = CVector2D::v2ZERO);
  CButton(const char* _pText, const CVector2D& _v2Position, const SUIButtonConfig& _oConfig);

  explicit operator bool() const { return m_bPressed; }
  bool WasPressed() const { return m_bPressed; }
  EUIElementState GetState() const { return m_eState; }

  // Devuelve el tamaño final, incluido el relleno del texto y el tamaño mínimo configurado.
  static CVector2D CalculateSize(const char* _pText);
  static CVector2D CalculateSize(const char* _pText, const SUIButtonConfig& _oConfig);
  static void SetDefaultConfig(const SUIButtonConfig& _oConfig);
  static const SUIButtonConfig& GetDefaultConfig();

private:
  void Draw(const char* _pText, const CVector2D& _v2Position, const SUIButtonConfig& _oConfig);

  EUIElementState m_eState = EUIElementState::NORMAL;
  bool m_bPressed = false;
};

class CCheckbox
{
public:
  // Devuelve true solo en el frame en que cambia el valor apuntado por _pbValue.
  // Por defecto solo el cuadro responde al raton. Pasa false a _bOnlyBox para hacer clickable texto y cuadro.
  CCheckbox(const char* _pText, bool* _pbValue, const CVector2D& _v2Position = CVector2D::v2ZERO,
    bool _bOnlyBox = true);
  CCheckbox(const char* _pText, bool* _pbValue, const CVector2D& _v2Position, const SUICheckboxConfig& _oConfig,
    bool _bOnlyBox = true);

  explicit operator bool() const { return m_bChanged; }
  bool WasChanged() const { return m_bChanged; }
  EUIElementState GetState() const { return m_eState; }

  static void SetDefaultConfig(const SUICheckboxConfig& _oConfig);
  static const SUICheckboxConfig& GetDefaultConfig();

private:
  void Draw(const char* _pText, bool* _pbValue, const CVector2D& _v2Position, const SUICheckboxConfig& _oConfig,
    bool _bOnlyBox);

  EUIElementState m_eState = EUIElementState::NORMAL;
  bool m_bChanged = false;
};
