#include "ButtonWidget.h"

/***************************************************************************************
** Code for the GFX button UI element
** Grabbed from Adafruit_tft library and enhanced to handle any label font
****************************************************************************************
*** Modified label datum calculation of _yd to allow button heights as small as 12 px
*** Added Enabled, Visible and Smooth states,
*** with enable(), disable(), show(), hide() and added redraw() methods,
*** and modification to contains() method to process only enabled and visible buttons.
*** Modified Sep 23, 2026 - Larry Coffey
***************************************************************************************/
ButtonWidget::ButtonWidget(TFT_eSPI *tft) {
  _tft       = tft;
  _xd        = 0;
  _yd        = 0;
  _textdatum = MC_DATUM;
  _label[9]  = '\0';
  _currstate = false;
  _laststate = false;
  _enblstate = false;
  _vsblstate = false;
  _smthstate = true;
  _inverted  = false;
}

void ButtonWidget::setPressAction(actionCallback action)
{
  pressAction = action;
}

void ButtonWidget::setReleaseAction(actionCallback action)
{
  releaseAction = action;
}

// Classic initButton() function: pass center & size
void ButtonWidget::initButton(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize)
{
  // Tweak arguments and pass to the newer initButtonUL() function...
  initButtonUL(x - (w / 2), y - (h / 2), w, h, outline, fill, textcolor, label, textsize);
}

// Newer function instead accepts upper-left corner & size
void ButtonWidget::initButtonUL(int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize)
{
  _x1           = x1;
  _y1           = y1;
  _w            = w;
  _h            = h;
  _outlinecolor = outline;
  _outlinewidth = 2;
  _fillcolor    = fill;
  _textcolor    = textcolor;
  _textsize     = textsize;
  strncpy(_label, label, 9);
  _pressTime    = 0xFFFFFFFF;
  _releaseTime  = 0xFFFFFFFF;
}

// Adjust text datum and x, y deltas
void ButtonWidget::setLabelDatum(int16_t x_delta, int16_t y_delta, uint8_t datum) {
  int16_t _yd_min = 5; // added int16_t vartiable _yd_min = 5, for minimum of 12 px height buttons
  _xd        = x_delta;
  _yd        = max(y_delta, _yd_min); // changed from y_delta to max(y_delta, _yd_min) Sept 2026 LC
  _textdatum = datum;
}

void ButtonWidget::drawButton(bool inverted, String long_name) {
  uint16_t fill, outline, text;

  _inverted  = inverted;

  if(!inverted) {
    fill    = _fillcolor;
    outline = _outlinecolor;
    text    = _textcolor;
  } else {
    fill    = _textcolor;
    outline = _outlinecolor;
    text    = _fillcolor;
  }

  uint8_t r = min(_w, _h) / 4; // Corner radius
  _tft->fillRoundRect(_x1, _y1, _w, _h, r, fill);
  _tft->drawRoundRect(_x1, _y1, _w, _h, r, outline);

  if (_tft->textfont == 255) {
    _tft->setCursor(_x1 + (_w / 8),
                    _y1 + (_h / 4));
    _tft->setTextColor(text);
    _tft->setTextSize(_textsize);
    _tft->print(_label);
  }
  else {
    _tft->setTextColor(text, fill);
    _tft->setTextSize(_textsize);

    uint8_t tempdatum = _tft->getTextDatum();
    _tft->setTextDatum(_textdatum);
    uint16_t tempPadding = _tft->getTextPadding();
    _tft->setTextPadding(0);

    if (long_name == "")
      _tft->drawString(_label, _x1 + (_w/2) + _xd, _y1 + (_h/2) + _yd); // changed from (_h/2) - 4 + _yd to (_h/2) + _yd
    else
      _tft->drawString(long_name, _x1 + (_w/2) + _xd, _y1 + (_h/2) + _yd); // after the change above to _yd = min(..., 5)

    _tft->setTextDatum(tempdatum);
    _tft->setTextPadding(tempPadding);
  }
  _smthstate = false; // added Sept 26 LC
}

void ButtonWidget::drawSmoothButton(bool inverted, int16_t outlinewidth, uint32_t bgcolor, String long_name) {
  uint16_t fill, outline, text;
  if (bgcolor != 0x00FFFFFF) _bgcolor = bgcolor;
  if (outlinewidth >=0) _outlinewidth = outlinewidth;
  _inverted  = inverted;

  if(!inverted) {
    fill    = _fillcolor;
    outline = _outlinecolor;
    text    = _textcolor;
  } else {
    fill    = _textcolor;
    outline = _outlinecolor;
    text    = _fillcolor;
  }

  uint8_t r = min(_w, _h) / 4; // Corner radius
  if (outlinewidth > 0) _tft->fillSmoothRoundRect(_x1, _y1, _w, _h, r, outline, _bgcolor);
  _tft->fillSmoothRoundRect(_x1+_outlinewidth, _y1+_outlinewidth, _w-(2*_outlinewidth), _h-(2*_outlinewidth), r-_outlinewidth, fill, outline);

  if (_tft->textfont == 255) {
    _tft->setCursor(_x1 + (_w / 8),
                    _y1 + (_h / 4));
    _tft->setTextColor(text);
    _tft->setTextSize(_textsize);
    _tft->print(_label);
  }
  else {
    _tft->setTextColor(text, fill);
    _tft->setTextSize(_textsize);

    uint8_t tempdatum = _tft->getTextDatum();
    _tft->setTextDatum(_textdatum);
    uint16_t tempPadding = _tft->getTextPadding();
    _tft->setTextPadding(0);

    if (long_name == "")
      _tft->drawString(_label, _x1 + (_w/2) + _xd, _y1 + (_h/2) + _yd); // changed from (_h/2) - 4 + _yd to (_h/2) + _yd
    else
      _tft->drawString(long_name, _x1 + (_w/2) + _xd, _y1 + (_h/2) + _yd); // after the change above to _yd = min(..., 5)

    _tft->setTextDatum(tempdatum);
    _tft->setTextPadding(tempPadding);
  }
  _smthstate = true; // added Sept 26 LC
}
// added Sept 2026 LC
void ButtonWidget::redraw() {
  if(_smthstate) drawSmoothButton(_inverted, _outlinewidth, _bgcolor, _label);
  else drawButton(_inverted, _label);
}

void ButtonWidget::disable() { // Disable button (prevent interaction)
  _enblstate = false;
  redrawDisabled();
}

void ButtonWidget::enable() { // Enable button (allow interaction)
  _enblstate = true;
  redraw();
}

void ButtonWidget::show() { // Show button (make visible)
  _vsblstate = true;
  redraw();
}

void ButtonWidget::hide() { // Hide button (make invisible)
  _vsblstate = false;
  erase();
}

void ButtonWidget::erase() {  // Erase button
  _tft->fillRect(_x1, _y1, _w, _h, _bgcolor);
}

void ButtonWidget::toggleEnabled() {  // Toggle enable/disable
  _enblstate = !_enblstate;
  redraw();
}

void ButtonWidget::toggleVisible() {  // Toggle visible/hidden
  _vsblstate = !_vsblstate;
  if (_vsblstate) redraw();
  else erase();
}

void ButtonWidget::redrawDisabled() {  // Draw disabled appearance
    if (_vsblstate) redrawDisabledInternal();
}

bool ButtonWidget::contains(int16_t x, int16_t y) {
  if (!_vsblstate || !_enblstate) return false; // added Sept 2026
  return ((x >= _x1) && (x < (_x1 + _w)) &&
          (y >= _y1) && (y < (_y1 + _h)));
}

void ButtonWidget::press(bool p) { // modified Sept 2026 LC
  if (_vsblstate && _enblstate) { // ignore press(T/F) if not visible or enabled
    _laststate = _currstate;
    _currstate = p;
  }
}

bool ButtonWidget::isPressed()    { return _currstate; }
bool ButtonWidget::justPressed()  { return (_currstate && !_laststate); }
bool ButtonWidget::justReleased() { return (!_currstate && _laststate); }
