#include "WavFile.h"
#include <iostream>
#include <algorithm>
#include <fstream>


WavFile::WavFile(const std::string& filePath) throw (FileNotExistException):
    _filePath(filePath)
{
    std::ifstream ifs(_filePath);

    if (!ifs) {
        throw FileNotExistException(std::string("File '") + _filePath +
                                    std::string("' doesn't exist!"));
    }

    ifs.read((char*) &_header, sizeof (_header));
    ifs.close();

    switch (_header.bitsPerSample) {
        case 8:
            _dataType = INT_8_DATA;
            break;

        case 16:
            _dataType = INT_16_DATA;
            break;

        case 24:
            _dataType = INT_24_DATA;
            break;

        case 32:
            _dataType = FLT_32_DATA;
            break;

        default:
            // what about some exception?
            break;
    }
}


WavFile::WavFile(const Header& header):
    _header(header)
{
    switch (_header.bitsPerSample) {
        case 8:
            _dataType = INT_8_DATA;
            break;

        case 16:
            _dataType = INT_16_DATA;
            break;

        case 24:
            _dataType = INT_24_DATA;
            break;

        case 32:
            _dataType = FLT_32_DATA;
            break;

        default:
            // what about some exception?
            break;
        }
}


void WavFile::loadData() {
    switch (_dataType) {
        case INT_8_DATA:
            _loadInt8Data();
            break;

        case INT_16_DATA:
            _loadInt16Data();
            break;

        case INT_24_DATA:
            _loadInt24Data();
            break;

        case FLT_32_DATA:
//            _loadFlt32Data();
            break;
    }
}


WavFile::Header WavFile::getHeader() const {
    return _header;
}


WavFile::Data_i8 WavFile::getInt8Data() throw (WrongDataTypeException) {
    if (_dataType != INT_8_DATA) {
        throw WrongDataTypeException(std::string("File '") + _filePath +
                                     std::string("' doesn't contain INT_8_DATA!"));
    }

    return _int8_data;
}


WavFile::Data_i16 WavFile::getInt16Data() throw (WrongDataTypeException) {
    if (_dataType != INT_8_DATA) {
        throw WrongDataTypeException(std::string("File '") + _filePath +
                                     std::string("' doesn't contain INT_16_DATA!"));
    }

    return _int16_data;
}


WavFile::Data_i24 WavFile::getInt24Data() throw (WrongDataTypeException) {
    if (_dataType != INT_24_DATA) {
        throw WrongDataTypeException(std::string("File '") + _filePath +
                                     std::string("' doesn't contain INT_24_DATA!"));
    }

    return _int24_data;
}


WavFile::Data_f32 WavFile::getFlt32Data() throw (WrongDataTypeException) {
    if (_dataType != FLT_32_DATA) {
        throw WrongDataTypeException(std::string("File '") + _filePath +
                                     std::string("' doesn't contain FLT_32_DATA!"));
    }

    return _flt32_data;
}


void WavFile::mixWith(WavFile& otherFile) throw (DifferentNumChannelsException,
                                                       DifferentBitsPerSampleException) {
    if (_header.numChannels != otherFile._header.numChannels) {
        throw DifferentNumChannelsException(std::string("Files '") + _filePath +
                                            std::string("' and '") + otherFile._filePath +
                                            std::string("' have different numChannels!"));
    }

    if (_header.bitsPerSample != otherFile._header.bitsPerSample) {
        throw DifferentBitsPerSampleException(std::string("Files '") + _filePath +
                                              std::string("' and '") + otherFile._filePath +
                                              std::string("' have different bitsPerSample!"));
    }

    switch (_dataType) {
        case INT_8_DATA:
            _mixInt8Data(otherFile);
            break;

        case INT_16_DATA:
            _mixInt16Data(otherFile);
            break;

        case INT_24_DATA:
            _mixInt24Data(otherFile);
            break;

        case FLT_32_DATA:
            _mixFlt32Data(otherFile);
            break;
    }
}


void WavFile::addMonoFrom(WavFile& otherFile) throw (NotMonoException) {
    if (otherFile._header.numChannels != 1) {
        throw NotMonoException(std::string("File '") + otherFile._filePath +
                               std::string("' haven't mono track!"));
    }

    switch (_dataType) {
        case INT_8_DATA:
            _addMonoInt8Data(otherFile);
            break;

        case INT_16_DATA:
            _addMonoInt16Data(otherFile);
            break;

        case INT_24_DATA:
            _addMonoInt24Data(otherFile);
            break;

        case FLT_32_DATA:
            _addMonoFlt32Data(otherFile);
            break;
    }
}


void WavFile::_loadInt8Data() {
    std::ifstream ifs(_filePath, std::ios::in | std::ios::binary);

    if (!ifs) {
        throw FileNotExistException(std::string("File '") + _filePath +
                                    std::string("' doesn't exist!"));
    }

    char* headBuf = new char[44];

    ifs.read((char*)headBuf, sizeof(Header));

    delete[] headBuf;

    for (int i = 0; i < _header.numChannels; i++) {
        _int8_data.push_back(std::vector<int8_t>());
    }

    int8_t* buf;

    try {
        buf = new int8_t[_header.numChannels];
    } catch (std::bad_alloc e) {
        std::cerr << e.what() << std::endl;
        ifs.close();
        return;
    }

    do {
        ifs.read((char*) buf, sizeof (buf));

        for (int i = 0; i < _header.numChannels; i++) {
            _int8_data.at(i).push_back(buf[i]);
        }
    } while (!ifs.eof());

    delete[] buf;

    ifs.close();
}


void WavFile::_loadInt16Data() {
    std::ifstream ifs(_filePath, std::ios::binary);

    if (!ifs) {
        throw FileNotExistException(std::string("File '") + _filePath +
                                    std::string("' doesn't exist!"));
    }

    ifs.seekg(std::streampos(44));

    for (int i = 0; i < _header.numChannels; i++) {
        _int16_data.push_back(std::vector<int16_t>());
    }

    int16_t* buf;

    try {
        buf = new int16_t[_header.numChannels];
    } catch (std::bad_alloc e) {
        std::cerr << e.what() << std::endl;
        ifs.close();
        return;
    }
    for (int i = 0; i < _header.numChannels; i++){
        _int16_data.at(i).reserve(_header.subchunk2Size/(_header.numChannels*(_header.bitsPerSample/8)));
    }
    try{
    while (ifs.read((char*)buf, sizeof (buf[0])*_header.numChannels)) {
        for (int i = 0; i < _header.numChannels; i++) {
            _int16_data.at(i).push_back(buf[i]);
        }
    }
    }
    catch (std::bad_alloc err){
        std::cerr << err.what() << std::endl;
        ifs.close();
        return;
    }

    delete[] buf;

//    for (int i = 0; i < _header.numChannels; i++)
//        _int16_data.at(i).pop_back();

    ifs.close();
}


void WavFile::_loadInt24Data() {
    std::ifstream ifs(_filePath);

    if (!ifs) {
        throw FileNotExistException(std::string("File '") + _filePath +
                                    std::string("' doesn't exist!"));
    }

    char* headBuf = new char[44];

    ifs.read((char*)headBuf, sizeof(Header));

    delete[] headBuf;

    for (int i = 0; i < _header.numChannels; i++) {
        _int24_data.push_back(std::vector<Int24>());
    }

    Int24* buf;

    try {
        buf = new Int24[_header.numChannels];
    } catch (std::bad_alloc e) {
        std::cerr << e.what() << std::endl;
        ifs.close();
        return;
    }

    do {
        ifs.read((char*) buf, sizeof (buf[0])*_header.numChannels);

        for (int i = 0; i < _header.numChannels; i++) {
            _int24_data.at(i).push_back(buf[i]);
        }
    } while (!ifs.eof());

    delete[] buf;

    ifs.close();
}


void WavFile::_mixInt8Data(WavFile& otherFile) {
    std::vector<std::vector<int8_t>::iterator> iters_1;
    decltype (iters_1) iters_2;

    for (int i = 0; i < _header.numChannels; ++i) {
        iters_1.push_back(_int8_data.at(i).begin());
        iters_2.push_back(otherFile._int8_data.at(i).begin());
    }

    while (iters_1.at(0) != _int8_data.at(0).end()) {
        for (int i = 0; i < _header.numChannels; ++i) {
            *iters_1.at(i) += *iters_2.at(i);
            (iters_1.at(i))++;
            (iters_2.at(i))++;
        }
    }
}


void WavFile::_mixInt16Data(WavFile& otherFile) {
    std::vector<std::vector<int16_t>::iterator> iters_1;
    decltype (iters_1) iters_2;

    for (int i = 0; i < _header.numChannels; ++i) {
        iters_1.push_back(_int16_data.at(i).begin());
        iters_2.push_back(otherFile._int16_data.at(i).begin());
    }

    while (iters_1.at(0) != _int16_data.at(0).end() && iters_2.at(0) != otherFile._int16_data.at(0).end()) {
        for (int i = 0; i < _header.numChannels; ++i) {
            *iters_1.at(i) += *iters_2.at(i);
            (iters_1.at(i))++;
            (iters_2.at(i))++;
        }
    }
}


void WavFile::_mixInt24Data(WavFile& otherFile) {
    std::vector<std::vector<Int24>::iterator> iters_1;
    decltype (iters_1) iters_2;

    for (int i = 0; i < _header.numChannels; ++i) {
        iters_1.push_back(_int24_data.at(i).begin());
        iters_2.push_back(otherFile._int24_data.at(i).begin());
    }

    while (iters_1.at(0) != _int24_data.at(0).end()) {
        for (int i = 0; i < _header.numChannels; ++i) {
            *iters_1.at(i) += *iters_2.at(i);
            (iters_1.at(i))++;
            (iters_2.at(i))++;
        }
    }
}


void WavFile::_mixFlt32Data(WavFile& otherFile) {
    std::vector<std::vector<float>::iterator> iters_1;
    decltype (iters_1) iters_2;

    for (int i = 0; i < _header.numChannels; ++i) {
        iters_1.push_back(_flt32_data.at(i).begin());
        iters_2.push_back(otherFile._flt32_data.at(i).begin());
    }

    while (iters_1.at(0) != _flt32_data.at(0).end()) {
        for (int i = 0; i < _header.numChannels; ++i) {
            *iters_1.at(i) += *iters_2.at(i);
            (iters_1.at(i))++;
            (iters_2.at(i))++;
        }
    }
}


void WavFile::_addMonoInt8Data(WavFile& otherFile) {
    std::vector<std::vector<int8_t>::iterator> iters_1;
    std::vector<int8_t>::iterator iters_2 = otherFile._int8_data.at(0).begin();

    for (int i = 0; i < _header.numChannels; ++i) {
        iters_1.push_back(_int8_data.at(i).begin());
    }

    while (iters_2 != otherFile._int8_data.at(0).end()) {
        for (int i = 0; i < _header.numChannels; ++i) {
            *iters_1.at(i) += *iters_2 / _header.numChannels;
            (iters_1.at(i))++;
            iters_2++;
        }
    }
}


void WavFile::_addMonoInt16Data(WavFile& otherFile) {
    std::vector<std::vector<int16_t>::iterator> iters_1;
    std::vector<int16_t>::iterator iters_2 = otherFile._int16_data.at(0).begin();

    for (int i = 0; i < _header.numChannels; ++i) {
        iters_1.push_back(_int16_data.at(i).begin());
    }

    while (iters_2 != otherFile._int16_data.at(0).end()) {
        for (int i = 0; i < _header.numChannels; ++i) {
            *iters_1.at(i) += *iters_2 / _header.numChannels;
            (iters_1.at(i))++;
            iters_2++;
        }
    }
}


void WavFile::_addMonoInt24Data(WavFile& otherFile) {
    std::vector<std::vector<Int24>::iterator> iters_1;
    std::vector<Int24>::iterator iters_2 = otherFile._int24_data.at(0).begin();

    for (int i = 0; i < _header.numChannels; ++i) {
        iters_1.push_back(_int24_data.at(i).begin());
    }

    while (iters_2 != otherFile._int24_data.at(0).end()) {
        for (int i = 0; i < _header.numChannels; ++i) {
            *iters_1.at(i) += *iters_2 / _header.numChannels;
            (iters_1.at(i))++;
            iters_2++;
        }
    }
}


void WavFile::_addMonoFlt32Data(WavFile& otherFile) {
    std::vector<std::vector<float>::iterator> iters_1;
    std::vector<float>::iterator iters_2 = otherFile._flt32_data.at(0).begin();

    for (int i = 0; i < _header.numChannels; ++i) {
        iters_1.push_back(_flt32_data.at(i).begin());
    }

    while (iters_2 != otherFile._flt32_data.at(0).end()) {
        for (int i = 0; i < _header.numChannels; ++i) {
            *iters_1.at(i) += *iters_2 / _header.numChannels;
            (iters_1.at(i))++;
            iters_2++;
        }
    }
}

void WavFile::save(const std::string& path) {
    _saveInt16(path);           //TODO: make this shit work with all types
}

void WavFile::_saveInt16(const std::string& path){
    std::ofstream ofs;

    ofs.open(path.empty() ? _filePath : path, std::ios::out|std::ios::binary|std::ios::trunc);

//    if (!ofs){
//        throw FileNotExistException(std::string("File '") + _filePath +
//                                    std::string("' doesn't exist!"));
//    }

    ofs.write((char*)&_header, sizeof(_header));

    std::vector < std::vector < int16_t >::iterator > iterators;

    for(int i = 0; i < _header.numChannels; i++){
        iterators.push_back(_int16_data.at(i).begin());
    }

    unsigned long long count = 0;

    while (_dataInt16End(iterators)){
        int16_t buf[_header.numChannels];
        for (int i = 0; i < _header.numChannels; i++){
            if (iterators[i] != _int16_data.at(i).end()){
                buf[i] = *(iterators[i]);
                iterators[i]++;
            }
            else{
                buf[i] = 0;
            }
        }
        ofs.write((char*)buf, sizeof(buf[0])*_header.numChannels);

        count++;
    }

    count *= _header.numChannels * _header.bitsPerSample / 8;

    ofs.close();
}

void WavFile::overVoice(WavFile & otherFile, double attack, double release, double silence, double threshold, double ratio){
    _overVoiceInt16(otherFile, attack, release, silence, Decibel<int16_t>(threshold), Decibel<int16_t>(ratio));
}

void WavFile::_overVoiceInt16(WavFile& otherFile, double attack, double release, double silence, Decibel<int16_t> threshold, Decibel<int16_t> ratio){
    typedef unsigned long long ultInt;
    typedef std::vector<int16_t>::iterator iter_type;

    Decibel< int16_t > tmp;

    std::vector< iter_type > iteratorsOrig;
    std::vector< iter_type > iteratorsVoice;

    for (int i = 0; i < _header.numChannels; i++)
        iteratorsOrig.push_back(_int16_data.at(i).begin());

    ultInt offset = (ultInt)(attack*_header.sampleRate);

    for (int i = 0; i < otherFile._header.numChannels; i++){
        otherFile._int16_data.at(i).insert(otherFile._int16_data.at(i).begin(), 9600, 0);
    }

    for (int i = 0; i < otherFile._header.numChannels; i++){
        iteratorsVoice.push_back(otherFile._int16_data.at(i).begin() + offset);
    }

    int16_t mux;

    ultInt sl = 0;
    double envelope = 0.;

    ultInt COUNT = 0;

    while (iteratorsOrig.at(0) != _int16_data.at(0).end() && iteratorsVoice.at(0) != otherFile._int16_data.at(0).end()){
        mux = 0;
        COUNT++;
        for (unsigned i = 0; i < iteratorsVoice.size(); i++){
            mux = std::max(mux, *( iteratorsVoice.at(i) ) );
        }

        tmp.calculateRatio(mux / iteratorsVoice.size());
        if (tmp > threshold){
            sl=0;
            if (envelope < ratio.getVal()){
                envelope += ratio.getVal()/(attack*_header.sampleRate);
            }
            else
            {
                envelope = ratio.getVal();
            }
        }
        else if (sl < (ultInt)(silence*_header.sampleRate) && envelope != 0){
            if (envelope < ratio.getVal()){
                envelope += ratio.getVal()/(attack*_header.sampleRate);
            }
            else
            {
                envelope = ratio.getVal();
            }
            sl++;
        }
        else{
            if (sl == (ultInt)(silence*_header.sampleRate)){
                for (unsigned i = 0; i < iteratorsOrig.size(); i++){
                    std::reverse_iterator< iter_type > rit(iteratorsOrig.at(i));
                    for (ultInt j = 0; j < silence*_header.sampleRate; j++){
                        rit++;
                        iteratorsOrig.at(i) = rit.base();
                        *(iteratorsOrig.at(i)) = *(iteratorsOrig.at(i)) - Decibel<short>(-15);
                    }
                }

                for (unsigned i = 0; i < iteratorsVoice.size(); i++){
                    std::reverse_iterator < iter_type > rit(iteratorsVoice.at(i));
                    for (ultInt j = 0; j < silence*_header.sampleRate; j++)
                    rit++;
                    iteratorsVoice.at(i) = rit.base();
                }
                sl++;
            }

            if (envelope > 0) envelope -= ratio.getVal()/(release*_header.sampleRate);
            else envelope = 0;
        }

        for (unsigned i = 0; i < iteratorsOrig.size(); i++){
            *(iteratorsOrig.at(i)) = *(iteratorsOrig.at(i)) - Decibel<short>(envelope);
        }

        for (unsigned i = 0; i < iteratorsOrig.size(); i++){
            iteratorsOrig.at(i)++;
        }

        for (unsigned i = 0; i < iteratorsVoice.size(); i++){
            iteratorsVoice.at(i)++;
        }
    }
}

