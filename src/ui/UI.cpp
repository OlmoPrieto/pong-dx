#include "ui/ui.h"

namespace
{
  SUIButtonConfig s_oDefaultButtonConfig;
  SUICheckboxConfig s_oDefaultCheckboxConfig;

  const SUIStateColors& GetStateColors(const SUIStateColors& _oNormal, const SUIStateColors& _oHovered,
    const SUIStateColors& _oPressed, const SUIStateColors& _oDisabled, EUIElementState _eState)
  {
    switch (_eState)
    {
      case EUIElementState::HOVERED: return _oHovered;
      case EUIElementState::PRESSED: return _oPressed;
      case EUIElementState::DISABLED: return _oDisabled;
      default: return _oNormal;
    }
  }

  Rectangle ToRectangle(const CVector2D& _v2Position, const CVector2D& _v2Size)
  {
    return { _v2Position.x, _v2Position.y, _v2Size.x, _v2Size.y };
  }

  EUIElementState GetElementState(const Rectangle& _oBounds, bool _bEnabled)
  {
    if (!_bEnabled)
      return EUIElementState::DISABLED;

    if (!CheckCollisionPointRec(GetMousePosition(), _oBounds))
      return EUIElementState::NORMAL;

    return IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? EUIElementState::PRESSED : EUIElementState::HOVERED;
  }

  void DrawFrame(const Rectangle& _oBounds, const SUIStateColors& _oColors, float _fBorderWidth, float _fCornerRadius)
  {
    const float fRadius = Clamp(_fCornerRadius, 0.0f, 1.0f);
    DrawRectangleRounded(_oBounds, fRadius, 8, _oColors.m_cFill);
    if (_fBorderWidth > 0.0f)
      DrawRectangleRoundedLinesEx(_oBounds, fRadius, 8, _fBorderWidth, _oColors.m_cBorder);
  }
}

// ------------------------------------------------------------------------------------------

CButton::CButton(const char* _pText, const CVector2D& _v2Position)
{
  Draw(_pText, _v2Position, s_oDefaultButtonConfig);
}

CButton::CButton(const char* _pText, const CVector2D& _v2Position, const SUIButtonConfig& _oConfig)
{
  Draw(_pText, _v2Position, _oConfig);
}

void CButton::SetDefaultConfig(const SUIButtonConfig& _oConfig)
{
  s_oDefaultButtonConfig = _oConfig;
}

const SUIButtonConfig& CButton::GetDefaultConfig()
{
  return s_oDefaultButtonConfig;
}

void CButton::Draw(const char* _pText, const CVector2D& _v2Position, const SUIButtonConfig& _oConfig)
{
  const char* pText = _pText ? _pText : "";
  const Rectangle oBounds = ToRectangle(_v2Position, _oConfig.m_v2Size);
  m_eState = GetElementState(oBounds, _oConfig.m_bEnabled);
  m_bPressed = _oConfig.m_bEnabled && CheckCollisionPointRec(GetMousePosition(), oBounds) &&
    IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

  const SUIStateColors& oColors = GetStateColors(_oConfig.m_oNormal, _oConfig.m_oHovered,
    _oConfig.m_oPressed, _oConfig.m_oDisabled, m_eState);
  DrawFrame(oBounds, oColors, _oConfig.m_fBorderWidth, _oConfig.m_fCornerRadius);

  const int32 iTextWidth = MeasureText(pText, _oConfig.m_iFontSize);
  const int32 iTextX = (int32)(oBounds.x + (oBounds.width - iTextWidth) * 0.5f);
  const int32 iTextY = (int32)(oBounds.y + (oBounds.height - _oConfig.m_iFontSize) * 0.5f);
  DrawText(pText, iTextX, iTextY, _oConfig.m_iFontSize, oColors.m_cText);
}

// ------------------------------------------------------------------------------------------

CCheckbox::CCheckbox(const char* _pText, bool* _pbValue, const CVector2D& _v2Position, bool _bOnlyBox)
{
  Draw(_pText, _pbValue, _v2Position, s_oDefaultCheckboxConfig, _bOnlyBox);
}

CCheckbox::CCheckbox(const char* _pText, bool* _pbValue, const CVector2D& _v2Position, const SUICheckboxConfig& _oConfig,
  bool _bOnlyBox)
{
  Draw(_pText, _pbValue, _v2Position, _oConfig, _bOnlyBox);
}

void CCheckbox::SetDefaultConfig(const SUICheckboxConfig& _oConfig)
{
  s_oDefaultCheckboxConfig = _oConfig;
}

const SUICheckboxConfig& CCheckbox::GetDefaultConfig()
{
  return s_oDefaultCheckboxConfig;
}

void CCheckbox::Draw(const char* _pText, bool* _pbValue, const CVector2D& _v2Position, const SUICheckboxConfig& _oConfig,
  bool _bOnlyBox)
{
  const char* pText = _pText ? _pText : "";
  const int32 iTextWidth = MeasureText(pText, _oConfig.m_iFontSize);
  const float fTextHeight = (float)_oConfig.m_iFontSize;
  const CVector2D v2ControlSize(_oConfig.m_v2BoxSize.x + (pText[0] ? _oConfig.m_fTextSpacing + iTextWidth : 0.0f),
    _oConfig.m_v2BoxSize.y > fTextHeight ? _oConfig.m_v2BoxSize.y : fTextHeight);
  const Rectangle oHitBounds = ToRectangle(_v2Position, v2ControlSize);
  const Rectangle oBoxBounds = ToRectangle(_v2Position, _oConfig.m_v2BoxSize);
  const Rectangle& oInteractionBounds = _bOnlyBox ? oBoxBounds : oHitBounds;
  const bool bEnabled = _oConfig.m_bEnabled && _pbValue;
  m_eState = GetElementState(oInteractionBounds, bEnabled);
  m_bChanged = bEnabled && CheckCollisionPointRec(GetMousePosition(), oInteractionBounds) &&
    IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
  if (m_bChanged)
    *_pbValue = !*_pbValue;

  const SUIStateColors& oColors = GetStateColors(_oConfig.m_oNormal, _oConfig.m_oHovered,
    _oConfig.m_oPressed, _oConfig.m_oDisabled, m_eState);
  DrawFrame(oBoxBounds, oColors, _oConfig.m_fBorderWidth, _oConfig.m_fCornerRadius);

  if (_pbValue && *_pbValue)
  {
    const float fPadding = _oConfig.m_v2BoxSize.x * 0.22f;
    const Vector2 av2Tick[3] =
    {
      { oBoxBounds.x + fPadding, oBoxBounds.y + oBoxBounds.height * 0.53f },
      { oBoxBounds.x + oBoxBounds.width * 0.43f, oBoxBounds.y + oBoxBounds.height - fPadding },
      { oBoxBounds.x + oBoxBounds.width - fPadding, oBoxBounds.y + fPadding }
    };
    DrawLineEx(av2Tick[0], av2Tick[1], 3.0f, _oConfig.m_cMark);
    DrawLineEx(av2Tick[1], av2Tick[2], 3.0f, _oConfig.m_cMark);
  }

  if (pText[0])
  {
    const int32 iTextX = (int32)(_v2Position.x + _oConfig.m_v2BoxSize.x + _oConfig.m_fTextSpacing);
    const int32 iTextY = (int32)(_v2Position.y + (v2ControlSize.y - _oConfig.m_iFontSize) * 0.5f +
      _oConfig.m_fTextVerticalOffset);
    DrawText(pText, iTextX, iTextY, _oConfig.m_iFontSize, oColors.m_cText);
  }
}
