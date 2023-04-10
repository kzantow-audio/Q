/*=============================================================================
   Copyright (c) 2014-2023 Joel de Guzman. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#if !defined(CYCFI_Q_SOFT_KNEE_COMPRESSOR_DECEMBER_7_2018)
#define CYCFI_Q_SOFT_KNEE_COMPRESSOR_DECEMBER_7_2018

#include <q/support/base.hpp>

namespace cycfi::q
{
   ////////////////////////////////////////////////////////////////////////////
   // soft_knee_compressor dynamically modulates the gain when the signal
   // envelope rises above a specified threshold. Envelope tracking is done
   // using an external envelope follower to make it possible to use
   // different types of envelope tracking schemes, the output of which is
   // the supplied 'env' argument to the function call operator
   // operator()(decibel env) where env is the envelope of the signal in
   // decibels obtained (e.g) using the envelope_follower.
   //
   // The soft_knee_compressor compressor variant provides a more gradual
   // "soft knee" gain transition around the threshold, given a knee width.
   //
   // Note that these are envelope processors that work in the logarithmic
   // domain (decibels) and process envelopes instead of the actual signal.
   // The output is the compressed envelope, also in decibels. Simply
   // multiply the signal by the result converted to float (or double). For
   // example:
   //
   //    auto gain = as_float(comp(env));
   //    auto left_out = left_signal * gain;
   //    auto right_out = right_signal * gain;
   //
   // where left_signal and right_signal are stereo input signals and
   // envelope is the computed envelope (e.g) using an envelope follower.
   //
   // The ratio parameter specifies the amount of gain applied. With the
   // typical "n:1" notation for compressors, the ratio parameter is 1/n,
   // thereby the ratio for compressors is normally from 0.0...1.0. (e.g. 4:
   // 1 compression is 1/4 or 0.25). Signal rising above the threshold is
   // attenuated, compressing the signal. For every dB above the threshold,
   // the signal is attenuated by n dB. For example, with a ratio of 4:1
   // (0.25), 1dB above the threshold is attenuated by 4dB.
   //
   // Typically, you add some makeup gain after compression to compensate for
   // the gain reduction.
   ////////////////////////////////////////////////////////////////////////////
   struct soft_knee_compressor
   {
                  soft_knee_compressor(decibel threshold, decibel width, float ratio);

      decibel     operator()(decibel env) const;
      void        threshold(decibel val);
      void        width(decibel val);
      void        ratio(float ratio);
      decibel     threshold() const;
      decibel     width() const;
      float       ratio() const;

      decibel     _threshold, _width, _lower, _upper;
      float       _slope;
   };

   ////////////////////////////////////////////////////////////////////////////
   // Implementation
   ////////////////////////////////////////////////////////////////////////////
   inline soft_knee_compressor::soft_knee_compressor(
      decibel threshold, decibel width, float ratio)
    : _threshold(threshold)
    , _width(width)
    , _lower(threshold - (_width * 0.5))
    , _upper(threshold + (_width * 0.5))
    , _slope(1.0f - ratio)
   {}

   inline decibel soft_knee_compressor::operator()(decibel env) const
   {
      if (env <= _lower)
      {
         return 0_dB;
      }
      else if (env <= _upper)
      {
         auto soft_slope = _slope * ((env - _lower) / _width) * 0.5;
         return soft_slope * (_lower - env);
      }
      else
      {
         return _slope * (_threshold - env);
      }
   }

   void soft_knee_compressor::threshold(decibel val)
   {
      _threshold = val;
      _lower = _threshold - (_width * 0.5);
      _upper = _threshold + (_width * 0.5);
   }

   inline void soft_knee_compressor::width(decibel val)
   {
      _width = val;
      _lower = _threshold - (_width * 0.5);
      _upper = _threshold + (_width * 0.5);
   }

   inline void soft_knee_compressor::ratio(float ratio)
   {
      _slope = 1.0f - ratio;
   }

   inline decibel soft_knee_compressor::threshold() const
   {
      return _threshold;
   }

   inline decibel soft_knee_compressor::width() const
   {
      return _width;
   }

   inline float soft_knee_compressor::ratio() const
   {
      return 1.0f - _slope;
   }
}

#endif
