#include "Audio.h"
#include <cassert>
#include <fstream>

Audio* Audio::instance = nullptr;

void Audio::Initialize(const std::string& directoryPath)
{
	HRESULT hr;

	directoryPath_ = directoryPath;

	hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	hr = xAudio2->CreateMasteringVoice(&masterVoice);
}

Audio* Audio::GetInstance()
{
	if (instance == nullptr) {
		instance = new Audio;
	}
	return instance;
}

void Audio::LoadWave(const std::string& filename) {
	// 既に読み込み済みの場合は何もしない
	if (filenameToIndex.find(filename) != filenameToIndex.end()) {
		return;
	}

	std::string fullPath = directoryPath_ + "/" + filename;

	std::ifstream file;
	file.open(fullPath, std::ios_base::binary);
	assert(file.is_open());

	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}

	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	ChunkHeader chunkHeader;
	FormatChunk format = {};

	while (file.read((char*)&chunkHeader, sizeof(chunkHeader))) {
		if (strncmp(chunkHeader.id, "fmt ", 4) == 0) {
			assert(chunkHeader.size <= sizeof(format.fmt));

			format.chunk = chunkHeader;
			file.read((char*)&format.fmt, chunkHeader.size);

			break;
		}
		else {
			file.seekg(chunkHeader.size, std::ios_base::cur);
		}
	}

	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	// --- チャンク読み込みとスキップ処理 ---
	ChunkHeader data;
	while (file.read((char*)&data, sizeof(data))) {
		if (strncmp(data.id, "data", 4) == 0) {
			break;
		}
		else {
			file.seekg(data.size, std::ios_base::cur);
		}
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	std::vector<uint8_t> buffer(data.size);
	file.read(reinterpret_cast<char*>(buffer.data()), data.size);

	file.close();

	SoundData& soundData = soundDatas_[soundDataIndex];
	soundData.wfex = format.fmt;
	soundData.buffer = std::move(buffer);
	soundData.name_ = filename;

	// ファイル名とインデックスの対応を保存
	filenameToIndex[filename] = static_cast<uint32_t>(soundDataIndex);

	// ベース名でも登録（階層パス省略での検索に対応）
	std::string baseName = GetBaseName(filename);
	if (baseName != filename) {
		filenameToIndex[baseName] = static_cast<uint32_t>(soundDataIndex);
	}

	soundDataIndex = (soundDataIndex + 1) % kMaxSoundData;
}

void Audio::Unload(const std::string& filename) {
	auto it = filenameToIndex.find(filename);
	if (it == filenameToIndex.end()) {
		return; // ファイルが見つからない場合は何もしない
	}

	uint32_t soundIndex = it->second;
	SoundData& soundData = soundDatas_[soundIndex];

	std::string originalName = soundData.name_;
	std::string baseName = GetBaseName(originalName);

	soundData.buffer.clear();  // バッファを空にする
	soundData.wfex = {};
	soundData.name_.clear();

	// マッピングからも削除
	filenameToIndex.erase(filename);

	// ベース名のマッピングも削除
	if (baseName != originalName) {
		filenameToIndex.erase(baseName);
	}
}

uint32_t Audio::GetSoundIndex(const std::string& filename) const {
	auto it = filenameToIndex.find(filename);
	assert(it != filenameToIndex.end()); // ファイルが読み込まれていない場合はアサート
	return it->second;
}

std::string Audio::GetBaseName(const std::string& filename) const
{
	size_t pos = filename.find_last_of("/\\");
	if (pos != std::string::npos) {
		return filename.substr(pos + 1);
	}
	return filename;
}

void Audio::PlayWave(const std::string& filename, float volume, bool loop) {
	// ファイルが読み込まれていない場合は読み込み
	if (filenameToIndex.find(filename) == filenameToIndex.end()) {
		LoadWave(filename);
	}

	uint32_t soundIndex = GetSoundIndex(filename);
	const SoundData& soundData = soundDatas_[soundIndex];

	HRESULT result;

	Voice* voice = new Voice();
	voice->filename = filename;
	voice->volume = volume;

	VoiceCallback* voiceCallback = new VoiceCallback();

	result = xAudio2->CreateSourceVoice(&voice->sourceVoice, &soundData.wfex, 0, XAUDIO2_DEFAULT_FREQ_RATIO, voiceCallback);
	assert(SUCCEEDED(result));

	// --- バッファを設定 ---
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.buffer.data();
	buf.AudioBytes = static_cast<uint32_t>(soundData.buffer.size());
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.pContext = voice;

	// --- ループ再生の設定 ---
	if (loop) {
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;  // 無限ループで再生
	}
	else {
		buf.LoopCount = 0;  // ループしない
	}

	result = voice->sourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));

	result = voice->sourceVoice->Start();
	assert(SUCCEEDED(result));

	// --- 音量を設定 ---
	voice->sourceVoice->SetVolume(voice->volume);

	// 再生中のボイスをセットに追加
	voices_.insert(voice);
}

void Audio::StopWave(const std::string& filename)
{
	// --- 音を停止 ---
	for (auto it = voices_.begin(); it != voices_.end(); ) {
		if ((*it)->filename == filename) {
			if ((*it)->sourceVoice != nullptr) {
				(*it)->sourceVoice->Stop(0);
				(*it)->sourceVoice->DestroyVoice();
			}
			delete* it;
			it = voices_.erase(it);
		}
		else {
			++it;
		}
	}
}

void Audio::SetVolume(const std::string& filename, float volume)
{
	for (auto& voice : voices_) {
		if (voice->filename == filename) {
			voice->volume = volume;
			voice->sourceVoice->SetVolume(volume);
		}
	}
}

void Audio::Finalize()
{
	if (masterVoice) {
		masterVoice->DestroyVoice();
		masterVoice = nullptr;
	}

	for (auto voice : voices_) {
		if (voice->sourceVoice) {
			voice->sourceVoice->DestroyVoice();
		}
		delete voice;
	}
	if (xAudio2) {
		xAudio2.Reset();
	}

	voices_.clear();
	filenameToIndex.clear();
	delete instance;
	instance = nullptr;
}