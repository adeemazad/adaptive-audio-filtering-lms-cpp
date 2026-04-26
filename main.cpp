#include <iostream>
#include <cmath>
#include <vector>
#include <stdio.h>
#include "sox.h"
#include <algorithm>
#include "Fir1.h"

int main() {
    sox_init();

    const char* in_file_wet = "Vibraphone.wav";
    const char* in_file_dry = "Trumpet.wav";
    const char* out_file = "output.wav";

    sox_signalinfo_t in_signal_wet, in_signal_dry;
    sox_format_t *in_file_wet_format = sox_open_read(in_file_wet, &in_signal_wet, NULL, NULL);
    sox_format_t *in_file_dry_format = sox_open_read(in_file_dry, &in_signal_dry, NULL, NULL);

    if (in_file_wet_format == NULL || in_file_dry_format == NULL) {
        std::cerr << "Error opening input file(s).\n";
        return 1;
    }

    int sample_rate = in_signal_wet.rate;
    int num_channels = in_signal_wet.channels;

    std::cout << "Sample rate: " << sample_rate << ", channels: " << num_channels << std::endl;

    size_t num_samples_wet = in_signal_wet.length / num_channels;
    size_t num_samples_dry = in_signal_dry.length / num_channels;

    std::vector<float> wet(num_samples_wet), dry(num_samples_dry);

    std::vector<sox_sample_t> wet_sox(num_samples_wet);
    std::vector<sox_sample_t> dry_sox(num_samples_dry);

    sox_read(in_file_wet_format, wet_sox.data(), num_samples_wet);
    sox_read(in_file_dry_format, dry_sox.data(), num_samples_dry);

    std::cout << "Wet file. Samples: " << num_samples_wet << ", max/min normalised samples: " << *std::max_element(wet.begin(), wet.end()) << ", " << *std::min_element(wet.begin(), wet.end()) << std::endl;
    std::cout << "Dry file. Samples: " << num_samples_dry << ", max/min normalised samples: " << *std::max_element(dry.begin(), dry.end()) << ", " << *std::min_element(dry.begin(), dry.end()) << std::endl;

    for (size_t i = 0; i < num_samples_wet; i++) {
        wet[i] = SOX_SAMPLE_TO_FLOAT_32BIT(wet_sox[i], sox_globals.clip);
        }

    for (size_t i = 0; i < num_samples_dry; i++) {
        dry[i] = SOX_SAMPLE_TO_FLOAT_32BIT(dry_sox[i], sox_globals.clip);
        }

    size_t filter_length = 0.3 * sample_rate;
    float learning_rate = 0.002;
    float skip_seconds = 4.9;

    Fir1 fir(filter_length);
    fir.setLearningRate(learning_rate);

    size_t num_samples_skip = skip_seconds * sample_rate;
    std::vector<float> training(dry.begin() + num_samples_skip, dry.end());
    std::vector<float> src(wet.begin() + num_samples_skip, wet.end());

    float msi = 0;
    for (auto s : src) {
        msi += s * s;
    }
    msi /= src.size();

    if (training.size() != src.size()) {
        std::cerr << "Interference and source samples are different lengths.\n";
        return 1;
    }

    // Run a couple of seconds through the filter so it's started to learn.
    size_t trial = 2 * sample_rate;

    std::cout << "Pre-training...\n";
    for (size_t i = 0; i < std::min(trial, src.size()); i++) {
        fir.lms_update(src[i] - fir.filter(training[i]));
    }

    std::cout << "Processing...\n";
    std::vector<float> error(training.size());
    for (size_t i = 0; i < src.size(); i++) {
        float filtered = fir.filter(training[i]);
        error[i] = src[i] - filtered;
        fir.lms_update(error[i]);
}

float power_gain = 0;
for (auto e : error) {
    power_gain += e * e;
}
power_gain /= msi;
std::cout << "Power gain: " << power_gain << std::endl;

sox_signalinfo_t out_signal;
out_signal.rate = sample_rate;
out_signal.channels = num_channels;
out_signal.precision = SOX_SAMPLE_PRECISION;
out_signal.length = error.size() * num_channels;
out_signal.mult = NULL;

sox_format_t *out_file_format = sox_open_write(out_file, &out_signal, NULL, "wav", NULL, NULL);

if (out_file_format == NULL) {
    std::cerr << "Error opening output file.\n";
    return 1;
}


sox_close(out_file_format);
sox_close(in_file_wet_format);
sox_close(in_file_dry_format);

sox_quit();

return 0;
}

