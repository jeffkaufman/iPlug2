#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPrograms = 1;

const int history_size = 512;

enum EParams
{
  kF1 = 0,
  kF2 = 1,
  kF3 = 2,
  kF4 = 3,
  kGate = 4,
  kAttackSpeed = 5,
  kReleaseSpeed = 6,
  kSlide = 7,
  kRangeLow = 8,
  kRangeHigh = 9,
  kOctave = 10,
  kResponse = 11,
  kAttackLength = 12,
  kDecayLength = 13,
  kAttackPeak = 14,
  kF1Fade = 15,
  kF2Fade = 16,
  kF3Fade = 17,
  kF4Fade = 18,
  kAccuracy = 19,
  kN = 20,
  kNFade = 21,
  kWaveShape = 22,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class BassWhistle : public Plugin
{
public:
  BassWhistle(const InstanceInfo& info);

#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif

 private:
  sample history[history_size];
  int history_pos;

  float samples_since_last_crossing;
  int samples_since_attack_began;

  float f1_val;
  float f2_val;
  float f3_val;
  float f4_val;

  bool positive;
  sample previous_sample;
  float rough_input_period;

  float note_period;
  long double note_loc;

  float rms_energy;

  float current_volume;
  float target_volume;

  sample val;

  char ramp_direction;

  float ramp;

  int on_count = 0;
  int off_count = 0;

};
