// Daisy Pod sine voice
// Knob 1: pitch (C2–C6, chromatic)
// Knob 2: envelope release
// Button 1: gate note
// Button 2: gate note +1 octave
// Encoder: octave (-2..+3)
// Encoder click: sine / tri / saw / square
// LED 1: envelope, LED 2: waveform

#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod   hw;
Oscillator osc;
Adsr       env;
Parameter  p_pitch;
Parameter  p_release;

static const uint8_t kWaveforms[] = {
    Oscillator::WAVE_SIN,
    Oscillator::WAVE_TRI,
    Oscillator::WAVE_POLYBLEP_SAW,
    Oscillator::WAVE_POLYBLEP_SQUARE,
};

int   waveform = 0;
int   octave   = 0;
float env_out  = 0.f;

void AudioCallback(AudioHandle::InputBuffer  in,
                   AudioHandle::OutputBuffer out,
                   size_t                    size)
{
    hw.ProcessAllControls();

    if(hw.encoder.RisingEdge())
        waveform = (waveform + 1) % 4;
    osc.SetWaveform(kWaveforms[waveform]);

    octave += hw.encoder.Increment();
    octave = DSY_CLAMP(octave, -2, 3);

    float midi = floorf(p_pitch.Process() + 0.5f) + (octave * 12.f);
    if(hw.button2.Pressed())
        midi += 12.f;
    osc.SetFreq(mtof(midi));

    float rel = p_release.Process();
    env.SetAttackTime(0.008f + rel * 0.04f);
    env.SetDecayTime(0.08f);
    env.SetSustainLevel(0.72f);
    env.SetReleaseTime(rel);

    bool gate = hw.button1.Pressed() || hw.button2.Pressed();

    for(size_t i = 0; i < size; i++)
    {
        env_out   = env.Process(gate);
        float sig = osc.Process() * env_out * 0.45f;
        out[0][i] = sig;
        out[1][i] = sig;
    }

    hw.led1.Set(0.f, env_out * 0.35f, env_out);
    float wr = (waveform == 2 || waveform == 3) ? 0.4f : 0.f;
    float wg = (waveform == 1 || waveform == 3) ? 0.4f : 0.f;
    float wb = (waveform == 0 || waveform == 3) ? 0.4f : 0.f;
    hw.led2.Set(wr, wg, wb);
    hw.UpdateLeds();
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(48);
    float sr = hw.AudioSampleRate();

    osc.Init(sr);
    osc.SetWaveform(Oscillator::WAVE_SIN);
    osc.SetAmp(1.f);

    env.Init(sr);
    env.SetAttackTime(0.01f);
    env.SetDecayTime(0.08f);
    env.SetSustainLevel(0.72f);
    env.SetReleaseTime(0.35f);

    p_pitch.Init(hw.knob1, 36.f, 84.f, Parameter::LINEAR);
    p_release.Init(hw.knob2, 0.05f, 2.2f, Parameter::LOGARITHMIC);

    hw.StartAdc();
    hw.StartAudio(AudioCallback);

    while(1) {}
}
