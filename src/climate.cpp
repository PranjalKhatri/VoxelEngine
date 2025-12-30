#include "world/climate.hpp"
#include <algorithm>
#include "fast_noise_lite.h"

namespace pop::world::climate {
ClimateModule::ClimateModule(ClimateModule::ClimateSettings climateSettnigs)
    : climate_settings_{climateSettnigs} {
    const auto &temperatureSettings   = climate_settings_.temperatureSettings;
    const auto &precipitationSettings = climate_settings_.precipitationSettings;

    auto temperatureSeed =
        climate_settings_.seed + temperatureSettings.noiseOffset;
    auto precipitationSeed =
        climate_settings_.seed + precipitationSettings.noiseOffset;

    temperature_noise_.SetSeed(temperatureSeed);
    temperature_noise_.SetFrequency(temperatureSettings.noiseScale);
    temperature_noise_.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    precipitation_noise_.SetSeed(precipitationSeed);
    precipitation_noise_.SetFrequency(precipitationSettings.noiseScale);
    temperature_noise_.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
}

float ClimateModule::GetTemperatureAt(int x, int y, int z) const {
    const auto &s           = climate_settings_.temperatureSettings;
    float       noiseSample = temperature_noise_.GetNoise((float)x, (float)z);

    float baseTemp =
        s.roomTemperature + noiseSample * (s.maxTemperature - s.minTemperature);

    int   altitudeDiff = y - s.tempBaseAltitude;
    float finalTemp    = baseTemp - altitudeDiff * s.tempDropRateWithAltitute;
    return std::clamp(finalTemp, s.minTemperature, s.maxTemperature);
}

float ClimateModule::GetPrecipitationAt(int x, int /*y*/, int z) const {
    const auto &s = climate_settings_.precipitationSettings;

    float noiseSample =
        (precipitation_noise_.GetNoise((float)x, (float)z) + 1.0) * 0.5;

    float baseHumidity =
        s.meanPrecipitation +
        noiseSample * (s.maxPrecipitation - s.minPrecipitation);
    return std::clamp(baseHumidity, s.minPrecipitation, s.maxPrecipitation);
}
};  // namespace pop::world::climate
