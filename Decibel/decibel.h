#ifndef DECIBEL
#define DECIBEL
#include <cmath>
#include <string>
//#include "C:/Users/Dave/Desktop/WavFileClass/WavFile/WavFile.h"
#include "C:/Users/Dave/Desktop/WavFileClass/Int24/Int24.h"

template<typename T>
class Decibel;

template<typename T>
const T operator + (const T &left, const Decibel<T> &right);

template<typename T>
const T operator - (const T &left, const Decibel<T> &right);

template<typename T>
class Decibel{

public:
    Decibel() : value(0) {}
    Decibel(double val) : value(val) {threshold = calculateThreshold();}

    void calculateRatio(T srcVal);
    void calculateRatio(T refVal, T srcVal);

    friend const T operator+ <>(const T &left, const Decibel<T> &right);

    friend const T operator- <>(const T &left, const Decibel<T> &right);

    bool operator <(const Decibel<T> & obj);
    bool operator >(const Decibel<T> & obj);

    double getVal() { return value; }

    Decibel & operator =(const Decibel &obj);
//    Decibel & operator =(const  &val);

private:
    double value;
    double calculateThreshold();
    double threshold;
};

template<typename T>
void Decibel<T>::calculateRatio(T srcVal){
    double subThreshold  = threshold;
    if(srcVal < 0){
        subThreshold = -threshold - 1;
    }
    value = log10((double)srcVal / subThreshold);
}

template<typename T>
void Decibel<T>::calculateRatio(T refVal, T srcVal){
    value = log10((double)srcVal / (double)refVal);
}

template<int int_part, int fl_part, int dot_encountered = 0> double chpkToDouble (){
//    return (double(int_part) + [fl_part](){ while ((fl_part % 10) != 0 ) fl_part /= 10; return fl_part; } );
    int i = fl_part;
    double result = i;

    while (i % 10 != 0){
        i /= 10;
        result /= 10;
    }

    result += double(int_part);
    return result;
}

template<int int_part, int fl_part, int dot_encountered, char chr, char... rest> double chpkToDouble () {
    if(chr == '.') {
        return chpkToDouble<int_part, fl_part, 1, rest...>();
    }

    else if (dot_encountered != 0) {
        return chpkToDouble<int_part, (10 * fl_part + chr - '0'), dot_encountered, rest...>();
    }

    else {
        return chpkToDouble<(10 * int_part + chr - '0'), fl_part, dot_encountered, rest...>();
    }
}


template<char... STR>
Decibel<char> operator"" _db8(){
    return Decibel<char>( chpkToDouble<0, 0, 0, STR...>() );
}

template<char... STR>
Decibel<short> operator"" _db16(){
    return Decibel<short>( chpkToDouble<0, 0, 0, STR...>() );
}

template<char... STR>
Decibel<Int24> operator"" _db24(){
    return Decibel<Int24>( chpkToDouble<0, 0, 0, STR...>() );
}

template<char... STR>
Decibel<float> operator"" _db32(){
    return Decibel<float>( chpkToDouble<0, 0, 0, STR...>() );
}


template<typename T>
Decibel<T> & Decibel<T>::operator =(const Decibel<T> &obj){
    if (this != &obj)
        value = obj.value;
    return *this;
}

template<typename T>
const T operator +(const T &left, const Decibel<T> &right){
    return T((double)left * pow(10, 0.1 * right.value));
}

template<typename T>
const T operator -(const T &left, const Decibel<T> &right){
    return T((double)left * pow(10, -0.1 * right.value));
}

template<typename T>
bool Decibel<T>::operator <(const Decibel<T> & obj){
    return value < obj.value;
}

template<typename T>
bool Decibel<T>::operator >(const Decibel<T> & obj){
    return value > obj.value;
}

// Private calculations

template<typename T>
double Decibel<T>::calculateThreshold(){
    int bitDepth = sizeof(T) * 8;
    switch(bitDepth){
    case 8  : return 127.0;
    case 16 : return 32767.0;
    case 24 : return 8388607.0;
    case 32 : return 1.0;
    }
}

#endif // DECIBEL

