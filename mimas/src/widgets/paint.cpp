#include "mimas.h"
#include "m_slider.h"
#include "m_pad.h"

#include "m_object_proxy.h"

#define SLIDER_BORDER_WIDTH 2
#define SLIDER_BORDER_RADIUS 6

// =============================================
// ==             MSlider                     ==
// =============================================

/**
 * TODO: use a table lookup for the round_delta ?
 */

void MSlider::paint(Graphics &g) {
  g.setColour(Colours::grey);
  g.fillRoundedRectangle(
    SLIDER_BORDER_WIDTH / 2,
    SLIDER_BORDER_WIDTH / 2,
    getWidth()-SLIDER_BORDER_WIDTH,
    getHeight()-SLIDER_BORDER_WIDTH,
    SLIDER_BORDER_RADIUS
  );
  g.setColour(Colours::pink);
  if (slider_type_ == VerticalSliderType) {
    // vertical slider
    int remote_pos = scaled_remote_value(getHeight()-2);
    int h = getHeight() - remote_pos - SLIDER_BORDER_WIDTH / 2;
    int round_delta = 0;
    if (h < SLIDER_BORDER_RADIUS) {
      round_delta = SLIDER_BORDER_RADIUS - h;
    } else if (h > getHeight() - SLIDER_BORDER_RADIUS) {
      round_delta = h - getHeight() - SLIDER_BORDER_RADIUS;
    }
    g.fillRoundedRectangle(
      SLIDER_BORDER_WIDTH / 2,
      h,
      getWidth() - SLIDER_BORDER_WIDTH,
      remote_pos,
      SLIDER_BORDER_RADIUS
    );
    // remove rounded borders on top of slider
    g.fillRect(
      round_delta + SLIDER_BORDER_WIDTH / 2,
      h,
      getWidth() - SLIDER_BORDER_WIDTH - round_delta,
      SLIDER_BORDER_RADIUS - round_delta
    );
    // small line on top of value
    g.setColour(Colours::black);
    g.drawLine(
      round_delta + SLIDER_BORDER_WIDTH / 2,
      h - 0.5f,
      getWidth() - SLIDER_BORDER_WIDTH - round_delta,
      h - 0.5f,
      1.0f
    );
    //if (remote_pos - position > 3 || remote_pos - position < -3) {
      // TODO: should be if dragged_ || last_drag ...
      // draw ghost
    //}
  } else {
    // horizontal slider
    int remote_pos = scaled_remote_value(getWidth()-2);
    g.fillRect(0, 0, remote_pos, getHeight());
  }
  g.setColour(Colours::black);
  g.drawRoundedRectangle(
    SLIDER_BORDER_WIDTH / 2,
    SLIDER_BORDER_WIDTH / 2,
    getWidth() - SLIDER_BORDER_WIDTH,
    getHeight() - SLIDER_BORDER_WIDTH,
    SLIDER_BORDER_RADIUS,
    SLIDER_BORDER_WIDTH
  );

  //g.drawRect(0, 0, getWidth(), getHeight());
}

// =============================================
// ==             MPad                        ==
// =============================================

void MPad::paint(Graphics& g) {
  float radius = 8;
  float pos_x;
  float pos_y;

  g.fillAll(Colours::grey);

  if (abs(range_x_.value_ - range_x_.remote_value_) + abs(range_y_.value_ - range_y_.remote_value_) > 4 * radius) {
    // remote_value_
    pos_x = range_x_.scaled_remote_value(getWidth()) - radius;
    pos_y = getHeight() - range_y_.scaled_remote_value(getHeight()) - radius;
    g.setColour(Colours::lightgrey);
    g.fillEllipse(pos_x, pos_y, 2*radius, 2*radius);

    g.setColour(Colours::darkgrey);
    g.drawEllipse(pos_x, pos_y, 2*radius, 2*radius, 2.0f);
  }

  // value_
  pos_x = range_x_.scaled_value(getWidth())  - radius;
  pos_y = getHeight() - range_y_.scaled_value(getHeight()) - radius;
  g.setColour(Colours::white);
  g.fillEllipse(pos_x, pos_y, 2*radius, 2*radius);

  g.setColour(Colours::black);
  g.drawEllipse(pos_x, pos_y, 2*radius, 2*radius, 2.0f);

  g.setColour(Colours::black);
  g.drawRect(
    0,
    0,
    getWidth(),
    getHeight(),
    SLIDER_BORDER_WIDTH
  );
}