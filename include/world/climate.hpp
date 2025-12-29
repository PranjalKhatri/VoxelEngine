#pragma once

#include "fast_noise_lite.h"

namespace pop::world::climate {
class ClimateModule {
   public:
    struct ClimateSettings {
        struct TemperatureSettings {
            float noiseScale = 0.01;
            float noiseOffset;
            float tempDropRateWithAltitute = 0.1f;
            int   tempBaseAltitude;
            float roomTemperature;
            float minTemperature;
            float maxTemperature;
        } temperatureSettings;
        struct PrecipitationSettings {
            float noiseScale = 0.01;
            float noiseOffset;
            float maxPrecipitation;
            float meanPrecipitation;
            float minPrecipitation;
        } precipitationSettings;
        int seed = 1337;
    };
    ClimateModule(ClimateSettings climateSettings);
    // return temperature in the range [minTemp,maxTemp] passed in settings
    float GetTemperatureAt(int x, int y, int z) const;
    // return precipitation in range [0.0,1.0]
    float GetPrecipitationAt(int x, int y, int z) const;

   private:
    ClimateSettings climate_settings_;
    FastNoiseLite   temperature_noise_;
    FastNoiseLite   precipitation_noise_;
};
};  // namespace pop::world::climate
