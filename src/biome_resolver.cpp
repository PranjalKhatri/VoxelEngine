#include <cstdlib>
#include <iostream>
#include <limits>
#include <string_view>
#include <fstream>
#include "biome/biome_resolver.hpp"

namespace pop::biome {
void BiomeResolver::AddPoint(const BiomePoint& point) {
    if (point.tempC >= kMaxTemp || point.tempC < kMinTemp ||
        point.precipCm >= kMaxPrecip || point.precipCm < 0) {
        std::cerr << "Biome Point Data out of range\n";
        std::exit(1);
    }
    points_.push_back(point);
}
void BiomeResolver::Bake(std::string_view outputPath) {
    if (points_.empty()) {
        std::cerr << "No biomes found\n";
        std::exit(1);
    }
    if (!lookup_table_) {
        lookup_table_ = std::make_unique<uint8_t[]>(kTableSize * kTableSize);
    }

    for (int x = 0; x < kTableSize; x++) {
        for (int y = 0; y < kTableSize; y++) {
            float currentT =
                kMinTemp + ((float)x / (kTableSize - 1)) * kTempRange;
            float currentP = ((float)y / (kTableSize - 1)) * kMaxPrecip;

            BiomeType closestType = points_[0].type;
            float     minDistSq   = std::numeric_limits<float>::max();

            for (const auto& point : points_) {
                float normCurT = (currentT - kMinTemp) / kTempRange;
                float normCurP = currentP / kMaxPrecip;

                float normPtT = (point.tempC - kMinTemp) / kTempRange;
                float normPtP = point.precipCm / kMaxPrecip;

                float dt     = normCurT - normPtT;
                float dp     = normCurP - normPtP;
                float distSq = (dt * dt) + (dp * dp);

                if (distSq < minDistSq) {
                    minDistSq   = distSq;
                    closestType = point.type;
                }
            }
            lookup_table_[Index(x, y)] = static_cast<uint8_t>(closestType);
        }
    }

    WriteToFile(outputPath);
}
void BiomeResolver::WriteToFile(std::string_view filePath) {
    std::ofstream ofs(std::string(filePath), std::ios::binary);
    // PPM Header: P6 (Binary), Width, Height, Max Color Value
    ofs << "P6\n" << kTableSize << " " << kTableSize << "\n255\n";

    for (int x = 0; x < kTableSize; x++) {
        for (int y = 0; y < kTableSize; y++) {
            BiomeType type = static_cast<BiomeType>(lookup_table_[Index(x, y)]);
            // TODO: change this to make fast instead of lookup everytime
            int color = 0x000000;
            for (const auto& p : points_) {
                if (p.type == type) {
                    color = p.biomeColor;
                    break;
                }
            }

            uint8_t r = (color >> 16) & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = color & 0xFF;

            ofs.write(reinterpret_cast<char*>(&r), 1);
            ofs.write(reinterpret_cast<char*>(&g), 1);
            ofs.write(reinterpret_cast<char*>(&b), 1);
        }
    }
    ofs.close();
    std::cout << "Biome Diagram exported to: " << filePath << "\n";
}
};  // namespace pop::biome
