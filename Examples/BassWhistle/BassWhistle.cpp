#include "BassWhistle.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

#include <math.h>

// x >= 0
// 0 <= wave_shape <= 1
double shape(double x, double wave_shape) {
  x -= ((int)x);
  double anchor = x < 0.5 ? 0.25 : 0.75;
  return wave_shape * anchor + (1-wave_shape)*x;
}

double sine_decimal(double x) {
  return sin(x*M_PI*2);
}

BassWhistle::BassWhistle(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms))
{
  GetParam(kF1Fade)->InitDouble("F1 fade", 14.0, 0., 30.0, 0.01, "");
  GetParam(kF2Fade)->InitDouble("F2 fade", 22.0, 0., 30.0, 0.01, "");
  GetParam(kF3Fade)->InitDouble("F3 fade", 7.5, 0., 30.0, 0.01, "");
  GetParam(kF4Fade)->InitDouble("F4 fade", 8.5, 0., 30.0, 0.01, "");
  GetParam(kNFade)->InitDouble("Noise fade", 8.5, 0., 30.0, 0.01, "");

  GetParam(kF1)->InitDouble("F1", 25.0, 0., 100.0, 0.01, "%");
  GetParam(kF2)->InitDouble("F2", 25.0, 0., 100.0, 0.01, "%");
  GetParam(kF3)->InitDouble("F3", 25.0, 0., 100.0, 0.01, "%");
  GetParam(kF4)->InitDouble("F4", 25.0, 0., 100.0, 0.01, "%");
  GetParam(kN)->InitDouble("Noise", 25.0, 0., 100.0, 0.01, "%");

  GetParam(kGate)->InitDouble("Gate", 11.5, 1., 20.0, 0.1, "");
  GetParam(kAttackSpeed)->InitDouble("Attack Speed", 3.5, 1., 10.0, 0.1, "");
  GetParam(kReleaseSpeed)->InitDouble("Release Speed", 5.5, 1., 10.0, 0.1, "");
  GetParam(kOctave)->InitDouble("Octave", 4, 1.0, 7.0, 1, "");
  GetParam(kWaveShape)->InitDouble("Wave Shape", 0.0, 0., 100.0, 0.01, "%");

  GetParam(kSlide)->InitDouble("Slide", 4, 1., 20.0, 0.1, "");
  GetParam(kRangeLow)->InitDouble("RangeLow", 83, 20, 200, 1, "");
  GetParam(kRangeHigh)->InitDouble("RangeHigh", 14, 5, 50, 1, "");
  GetParam(kResponse)->InitDouble("Response", 0, 0., 100., 1, "%");

  GetParam(kAttackPeak)->InitDouble("Attack Peak", 10, 0., 30.0, 0.1, "");
  GetParam(kAttackLength)->InitDouble("Attack Length", 2, 1., 20.0, 0.1, "");
  GetParam(kDecayLength)->InitDouble("Decay Length", 8, 1., 20.0, 0.1, "");
  GetParam(kAccuracy)->InitDouble("Accuracy", 1, 1.0, 10.0, 1, "");



#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();

    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-200), kF1));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100), kF2));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(0), kF3));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(100), kF4));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(200), kN));

    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(-100).GetVShifted(-200), kF1Fade));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(-100).GetVShifted(-100), kF2Fade));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(-100).GetVShifted(0), kF3Fade));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(-100).GetVShifted(100), kF4Fade));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(-100).GetVShifted(200), kNFade));

    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(-200).GetVShifted(-200), kGate));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(-200).GetVShifted(-100), kAttackSpeed));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(-200).GetVShifted(0), kReleaseSpeed));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(-200).GetVShifted(100), kOctave));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(-200).GetVShifted(200), kWaveShape));

    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(100).GetVShifted(-200), kSlide));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(100).GetVShifted(-100), kRangeLow));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(100).GetVShifted(0), kRangeHigh));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(100).GetVShifted(100), kResponse));

    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(200).GetVShifted(-200), kAttackPeak));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(200).GetVShifted(-100), kAttackLength));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(200).GetVShifted(0), kDecayLength));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetHShifted(200).GetVShifted(100), kAccuracy));

  };
#endif

  for (int i = 0; i < history_size; i++) {
    history[i] = 0;
  }
  history_pos = 0;

  samples_since_last_crossing = 0;
  samples_since_attack_began = 0;

  f1_val = 0;
  f2_val = 0;
  f3_val = 0;
  f4_val = 0;

  positive = TRUE;
  previous_sample = 0;
  rough_input_period = 40;
  
  note_period = rough_input_period*32; // in samples
  note_loc = 0;  // 0-1, where we are in note_period
  
  rms_energy = 0;
  
  val = 0;

  // -1: ramp down
  //  1: ramp up
  ramp_direction = -1;    
  
  ramp = 0;

  current_volume = 0;
  target_volume = 0;
}

#if IPLUG_DSP
void BassWhistle::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double f1 = GetParam(kF1)->Value() / 100.;
  const double f2 = GetParam(kF2)->Value() / 100.;
  const double f3 = GetParam(kF3)->Value() / 100.;
  const double f4 = GetParam(kF4)->Value() / 100.;
  const double noise = GetParam(kN)->Value() / 100.;

  const double gate = 1/exp(GetParam(kGate)->Value());
  const double attack_speed = 1/exp(GetParam(kAttackSpeed)->Value());
  const double release_speed = 1/exp(GetParam(kReleaseSpeed)->Value());
  const double octave = 2 << (int)(GetParam(kOctave)->Value() + 0.5);

  const double slide = 2 << (int)(GetParam(kSlide)->Value() + 0.5);
  const int max_input_period = GetParam(kRangeLow)->Value();
  const int min_input_period = GetParam(kRangeHigh)->Value();
  const double volume_response = GetParam(kResponse)->Value() / 100.;

  const double attack_peak = exp(GetParam(kAttackPeak)->Value() / 10.);
  const double attack_length = exp(GetParam(kAttackLength)->Value());
  const double decay_length = exp(GetParam(kDecayLength)->Value());

  const double f1_fade = 1-1/exp(GetParam(kF1Fade)->Value());
  const double f2_fade = 1-1/exp(GetParam(kF2Fade)->Value());
  const double f3_fade = 1-1/exp(GetParam(kF3Fade)->Value());
  const double f4_fade = 1-1/exp(GetParam(kF4Fade)->Value());
  const double noise_fade = 1-1/exp(GetParam(kNFade)->Value());

  const int accuracy = (int)(GetParam(kAccuracy)->Value() + 0.5);
  const double wave_shape = GetParam(kWaveShape)->Value() / 100.;

  for (int i = 0; i < nFrames; i++) {
    sample s = inputs[0][i];

    rms_energy += s*s;
    history[history_pos] = s;
    history_pos = (history_pos + 1) % max_input_period;
    
    samples_since_last_crossing++;
    samples_since_attack_began++;
    
    val = 0;
    if (positive) {
      if (s < 0) {
        /**
         * Let's say we take samples at p and n:
         *
         *  p
         *   \
         *    \
         *  -------
         *      \
         *       n
         *
         * we could say the zero crossing is at n, but better would be to say
         * it's between p and n in proportion to how far each is from zero.  So,
         * if n is the current sample, that's:
         *
         *        |n|
         *   - ---------
         *     |n| + |p|
         *
         * But p is always positive and n is always negative, so really:
         *
         *        |n|            -n         n
         *   - ---------  =  - ------  =  -----
         *     |n| + |p|       -n + p     p - n
         */
        sample first_negative = s;
        sample last_positive = previous_sample;
        sample adjustment = first_negative / (last_positive - first_negative);
        samples_since_last_crossing -= adjustment;
        rough_input_period = samples_since_last_crossing;

        bool ok = true;
        sample sample_max = 0;
        sample sample_min = 0;

        if (rough_input_period > min_input_period &&
            rough_input_period < max_input_period) {
          float sample_max_loc = -1000;
          float sample_min_loc = -1000;
          for (int j = 0; j < rough_input_period; j++) {
            sample histval = history[(max_input_period + history_pos - j) %
                                     max_input_period];
            if (histval < sample_min) {
              sample_min = histval;
              sample_min_loc = j;
            } else if (histval > sample_max) {
              sample_max = histval;
              sample_max_loc = j;
            }
          }
          /**
           * With a perfect sine wave centered on 0 and lined up with our
           * sampling we'd expect:
           *
           * history[now] == 0                         (verified)
           * history[now - input_period] == 0          (verified)
           * history[now - input_period/2] == 0        (very likely)
           * history[now - input_period/4] == max
           * history[now - 3*input_period/4] == min
           *
           * Let's check if that's right, to within a sample or so.
           */
          sample error = 0;
          // You could make these better by finding the second
          // highest/lowest values to figure out which direction the peak
          // is off in, and adjusting.  Or construct a whole sine wave and
          // see how well it fits history.
          error += (sample_max_loc - rough_input_period/4)*(sample_max_loc - rough_input_period/4);
          error += (sample_min_loc - 3*rough_input_period/4)*(sample_min_loc - 3*rough_input_period/4);
          
          //printf("%.5f\t\t%.3f\n", sample_max, sample_energy);
          
          sample rough_period_rms_energy = rms_energy/rough_input_period;
          if (rough_period_rms_energy < gate) {
            //printf("not ok: rough_period_rms_energy=%.8f\n",
            //       rough_period_rms_energy);
            ok = false;
          } else if (error > 5 && rough_period_rms_energy < (10*gate)) {
            //printf("not ok: error=%.2f with rough_period_rms_energy=%.5f\n",
            //       error, rough_period_rms_energy);
            ok = false;
          }
        } else {
          ok = false;
        }

        // If we're off, require `accuracy` periods before turning on.  If we're fully on require
        // `accuracy` periods before turning off.  If we're in between, ignore accuracy and just
        // go with our current best guess.

        bool currently_on = ramp_direction > 0 && ramp > .5;
        bool currently_off = ramp_direction < 0 && ramp < 0.0001;

        if (currently_on) {
          on_count = 0;
          if (ok) {
            off_count = 0;
          } else {
            off_count++;
          }
        } else if (currently_off) {
          off_count = 0;
          if (!ok) {
            on_count = 0;
          } else {
            on_count++;
          }
        } else {
          off_count = 0;
          on_count = 0;
        }

        float goal_period = rough_input_period*octave;

        if (on_count >= accuracy) {
          ramp_direction = 1;
          note_period = goal_period;
          samples_since_attack_began = 0;
          note_loc = 0.5;
          f1_val = 1;
          f2_val = 1;
          f3_val = 1;
          f4_val = 1;
        } else if (currently_on) {
          // already playing, change slowly
          note_period = ((slide-1)*note_period + goal_period)/slide;
        }

        if (ok) {
          target_volume = fmax(sample_max, -sample_min);
          if (target_volume > 1) {
            target_volume = 1;
          }
        }

        if (off_count >= accuracy) {
          ramp_direction = -1;
        }
        
        positive = false;
        
        samples_since_last_crossing = -adjustment;
        rms_energy = 0;
      }
    } else {
      if (s > 0) {
        positive = true;
      }
    }
    previous_sample = s;
    
    // start by synthesizing the lowest tone
    sample h1 = sine_decimal(shape(note_loc, wave_shape));
    sample h2 = sine_decimal(shape(note_loc * 2, wave_shape));
    sample h3 = sine_decimal(shape(note_loc * 3, wave_shape));
    sample h4 = sine_decimal(shape(note_loc * 4, wave_shape));
    
    if (ramp_direction > 0) {
      if (ramp < 0.00001) {
        ramp = 0.00001;
      }
      ramp *= (1 + attack_speed);
    } else if (ramp_direction < 0) {
      ramp *= (1 - release_speed);
    }
    
    if (ramp > 1) {
      ramp = 1;
    } else if (ramp < 0.000001) {
      ramp = 0;
    }

    current_volume = (127*current_volume + target_volume) / 128;
    
    note_loc += 1/note_period;
    if (note_loc > 1) {
      note_loc -= 1;
    }

    double attack_volume_amount = 1;
    if (samples_since_attack_began >= 0) {
      if (samples_since_attack_began < attack_length) {
        attack_volume_amount = attack_peak;
      } else if (samples_since_attack_began < (attack_length + decay_length)) {
        double decay_so_far = samples_since_attack_began - attack_length;
        attack_volume_amount = 1 + ((attack_peak-1) * (1 - decay_so_far / decay_length));
      }
    }

    f1_val *= f1_fade;
    f2_val *= f2_fade;
    f3_val *= f3_fade;
    f4_val *= f4_fade;

    val = ((volume_response*current_volume) + (1-volume_response)) *
      0.4 *
      ramp *
      attack_volume_amount *
      (f1*h1*f1_val +
       f2*h2*f2_val +
       f3*h3*f3_val +
       f4*h4*f4_val);

    outputs[0][i] = val;
  }
}
#endif
