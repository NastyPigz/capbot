#ifndef CAPBOT_ESSENTIALS_H_
#define CAPBOT_ESSENTIALS_H_
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>

void replace_all(std::string& str, const std::string& from, const std::string& to);

void join(const std::vector<std::string>& v, char c, std::string& s);

std::string join_vec(const std::vector<std::string>& v, std::string c);

// void pcm_to_wav(std::string input, std::string output);

// void writeWavHeader(std::ofstream& file, int16_t numChannels, int32_t sampleRate, int16_t bitsPerSample, int32_t dataSize);

#endif