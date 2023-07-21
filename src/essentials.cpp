#include "capbot/essentials.h"

void replace_all(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty()) return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();  // In case 'to' contains 'from', like
								   // replacing 'x' with 'yx'
	}
}

void join(const std::vector<std::string>& v, char c, std::string& s) {
	s.clear();

	for (std::vector<std::string>::const_iterator p = v.begin(); p != v.end();
		 ++p) {
		s += *p;
		if (p != v.end() - 1) s += c;
	}
}

std::string join_vec(const std::vector<std::string>& v, std::string c) {
    std::string s = "";
    for (std::vector<std::string>::const_iterator p = v.begin(); p != v.end(); ++p) {
		s += *p;
		if (p != v.end() - 1) s += c;
	}
    return s;
}

// void pcm_to_wav(std::string input, std::string output) {
// 	int16_t numChannels = 2; // Change this to 1 for mono or 2 for stereo
//     int32_t sampleRate = 44100; // Sample rate (e.g., 44100 samples per second)
//     int16_t bitsPerSample = 16; // Change this to 8 or 16
// 	std::ifstream pcmFile(input, std::ios::binary);
//     if (!pcmFile) {
//         std::cerr << "Error opening PCM input file.\n";
//         return;
//     }

//     pcmFile.seekg(0, std::ios::end);
//     int32_t dataSize = static_cast<int32_t>(pcmFile.tellg());
//     pcmFile.seekg(0, std::ios::beg);

//     std::ofstream wavFile(output, std::ios::binary);
//     if (!wavFile) {
//         std::cerr << "Error creating WAV output file.\n";
//         return;
//     }

// 	writeWavHeader(wavFile, numChannels, sampleRate, bitsPerSample, dataSize);

//     // Convert PCM data to WAV format by copying it directly
//     wavFile << pcmFile.rdbuf();

//     std::cout << "Conversion successful: " << output << " created.\n";

//     pcmFile.close();
//     wavFile.close();
// }

// void writeWavHeader(std::ofstream& file, int16_t numChannels, int32_t sampleRate, int16_t bitsPerSample, int32_t dataSize) {
//     char header[44] = {};

//     // RIFF chunk descriptor
//     header[0] = 'R'; header[1] = 'I'; header[2] = 'F'; header[3] = 'F';
//     *(int32_t*)(header + 4) = dataSize + 36;

//     // Format chunk
//     header[8] = 'W'; header[9] = 'A'; header[10] = 'V'; header[11] = 'E';
//     header[12] = 'f'; header[13] = 'm'; header[14] = 't'; header[15] = ' ';
//     *(int32_t*)(header + 16) = 16;                // Subchunk1Size
//     *(int16_t*)(header + 20) = 1;                 // AudioFormat: PCM
//     *(int16_t*)(header + 22) = numChannels;       // NumChannels
//     *(int32_t*)(header + 24) = sampleRate;        // SampleRate
//     *(int32_t*)(header + 28) = sampleRate * numChannels * bitsPerSample / 8; // ByteRate
//     *(int16_t*)(header + 32) = numChannels * bitsPerSample / 8; // BlockAlign
//     *(int16_t*)(header + 34) = bitsPerSample;     // BitsPerSample

//     // Data chunk
//     header[36] = 'd'; header[37] = 'a'; header[38] = 't'; header[39] = 'a';
//     *(int32_t*)(header + 40) = dataSize;

//     file.write(header, 44);
// }