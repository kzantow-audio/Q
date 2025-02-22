/*=============================================================================
   Copyright (c) 2014-2021 Joel de Guzman. All rights reserved.

   Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
=============================================================================*/
#include <q/support/literals.hpp>
#include <q/support/note_names.hpp>
#include <q/synth/saw.hpp>
#include <q_io/audio_file.hpp>
#include <array>

namespace q = cycfi::q;
using namespace q::literals;
using namespace q::notes;

constexpr auto sps = 48000;

int main()
{
   ////////////////////////////////////////////////////////////////////////////
   // Synthesize a 10-second band-limited saw wave

   constexpr auto size = sps * 10;
   constexpr auto n_channels = 1;
   constexpr auto buffer_size = size * n_channels;

   auto buff = std::array<float, buffer_size>{};   // The output buffer
   const auto f = q::phase(C[3], sps);             // The synth frequency
   auto ph = q::phase();                           // Our phase accumulator

   for (auto i = 0; i != size; ++i)
   {
      buff[i] = q::saw(ph, f) * 0.9;
      ph += f;
   }

   ////////////////////////////////////////////////////////////////////////////
   // Write to a wav file

   q::wav_writer wav(
      "results/gen_saw.wav", n_channels, sps // mono, 48000 sps
   );
   wav.write(buff);

   return 0;
}
