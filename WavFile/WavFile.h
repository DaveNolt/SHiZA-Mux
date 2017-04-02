#ifndef WAVFILE_H
#define WAVFILE_H


#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include "Int24/Int24.h"
#include "Decibel/decibel.h"


class WrongDataTypeException : public std::runtime_error {
    public:
        WrongDataTypeException(std::string errorMessage):
            std::runtime_error(errorMessage)
        {}
};


class FileNotExistException : public std::runtime_error {
    public:
        FileNotExistException(std::string errorMessage):
            std::runtime_error(errorMessage)
        {}
};


class NotMonoException : public std::runtime_error {
    public:
        NotMonoException(std::string errorMessage):
            std::runtime_error(errorMessage)
        {}
};


class DifferentNumChannelsException : public std::runtime_error {
    public:
        DifferentNumChannelsException(std::string errorMessage):
            std::runtime_error(errorMessage)
        {}
};


class DifferentBitsPerSampleException : public std::runtime_error {
    public:
        DifferentBitsPerSampleException(std::string errorMessage):
            std::runtime_error(errorMessage)
        {}
};


class WavFile {
    public:
        enum DataType {
            INT_8_DATA,
            INT_16_DATA,
            INT_24_DATA,
            FLT_32_DATA
        };

        typedef std::vector<std::vector<int8_t>>    Data_i8;
        typedef std::vector<std::vector<int16_t>>   Data_i16;
        typedef std::vector<std::vector<Int24>>     Data_i24;
        typedef std::vector<std::vector<float>>     Data_f32;

        struct Header {
                char        chunkId[4];
                uint32_t    chunkSize;
                char        format[4];
                char        subchunk1Id[4];
                uint32_t    subchunk1Size;
                uint16_t    audioFormat;
                uint16_t    numChannels;
                uint32_t    sampleRate;
                uint32_t    byteRate;
                uint16_t    blockAlign;
                uint16_t    bitsPerSample;
                char        subchunk2Id[4];
                uint32_t    subchunk2Size;
        };


        WavFile(const std::string& filePath) throw (FileNotExistException);
        WavFile(const Header& header);

        static WavFile& mix(const WavFile& out, const WavFile& in);

        void        loadData();

        Header      getHeader() const;

        Data_i8     getInt8Data() throw (WrongDataTypeException);
        Data_i16    getInt16Data() throw (WrongDataTypeException);
        Data_i24    getInt24Data() throw (WrongDataTypeException);
        Data_f32    getFlt32Data() throw (WrongDataTypeException);

        void        mixWith(WavFile& otherFile) throw (DifferentNumChannelsException,
                                                             DifferentBitsPerSampleException);
        void        addMonoFrom(WavFile& otherFile) throw (NotMonoException);

        void        save(const std::string& path = "");
        void        saveAs(const std::string& fileName);

        void overVoice(WavFile & otherFile, double attack, double release, double silence, double threshold, double ratio);


    private:
        std::string _filePath;
        Header      _header;
        DataType    _dataType;
        Data_i8     _int8_data;
        Data_i16    _int16_data;
        Data_i24    _int24_data;
        Data_f32    _flt32_data;

        void _loadInt8Data();
        void _loadInt16Data();
        void _loadInt24Data();
        void _loadFlt32Data();

        void _mixInt8Data(WavFile& otherFile);
        void _mixInt16Data(WavFile& otherFile);
        void _mixInt24Data(WavFile& otherFile);
        void _mixFlt32Data(WavFile& otherFile);

        void _addMonoInt8Data(WavFile& otherFile);
        void _addMonoInt16Data(WavFile& otherFile);
        void _addMonoInt24Data(WavFile& otherFile);
        void _addMonoFlt32Data(WavFile& otherFile);

        void _overVoiceInt16(WavFile& otherFile, double attack, double release, double silence, Decibel<int16_t> threshold, Decibel<int16_t> ratio);

        void _saveInt16(const std::string& path);

        bool _dataInt16End(std::vector < std::vector < int16_t >::iterator > iterators) {
            bool result = true;
            for (int i = 0; i < _header.numChannels; i++)
            result &= (iterators[i] != _int16_data[i].end() );
            return result;
        }
};


#endif // WAVFILE_H
