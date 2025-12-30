#pragma once

#include "fast_noise_lite.h"

namespace pop::world::climate {
class ClimateModule {
   public:
    struct ClimateSettings {
        struct TemperatureSettings {
            float noiseScale               = 0.0008f;
            float noiseOffset              = 67.0f;
            // in Celcius per block
            float tempDropRateWithAltitute = 0.1f;
            int   tempBaseAltitude         = 64.0f;
            float roomTemperature          = 20.0f;   // in Celcius
            float minTemperature           = -20.0f;  // in Celcius
            float maxTemperature           = 50.0f;   // in Celcius
        } temperatureSettings;
        struct PrecipitationSettings {
            float noiseScale        = 0.0011;
            float noiseOffset       = 420.0f;
            float maxPrecipitation  = 350.0f;  // in cm
            float meanPrecipitation = 100.0f;  // in cm
            float minPrecipitation  = 0.0f;    // in cm
        } precipitationSettings;
        int seed = 1337;
    };
    ClimateModule(ClimateSettings climateSettings);
    ClimateModule() : ClimateModule(ClimateSettings{}) {}
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
