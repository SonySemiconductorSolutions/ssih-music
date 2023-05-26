/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <vector>

#include <strings.h>

#include "Arduino.h"
#include "HardwareSerial.h"
#include "Print.h"
#include "Stream.h"
#include "WString.h"

#include "arch/board/cxd56_audio.h"
#include "File.h"
#include "FrontEnd.h"
#include "MemoryUtil.h"
#include "MP.h"
#include "OutputMixer.h"
#include "SDHCI.h"
#include "Storage.h"

//// Arduino.h

int digitalRead(uint8_t pin) {
    return 0;
}

void digitalWrite(uint8_t pin, uint8_t value) {
    return;
}

int analogRead(uint8_t pin) {
    return 0;
}

void analogWrite(uint8_t pin, uint8_t value) {
    return;
}

int isAlpha(int ch) {
    return isalpha(ch);
}

int isAlphaNumeric(int ch) {
    return isalnum(ch);
}

int isAscii(int ch) {
    return isascii(ch);
}

int isControl(int ch) {
    return iscntrl(ch);
}

int isDigit(int ch) {
    return isdigit(ch);
}

int isGraph(int ch) {
    return isgraph(ch);
}

int isHexadecimalDigit(int ch) {
    return isxdigit(ch);
}

int isLowerCase(int ch) {
    return islower(ch);
}

int isPrintable(int ch) {
    return isprint(ch);
}

int isPunct(int ch) {
    return ispunct(ch);
}

int isSpace(int ch) {
    return isspace(ch);
}

int isUpperCase(int ch) {
    return isupper(ch);
}

int isWhitespace(int ch) {
    return isblank(ch);
}

static uint64_t g_ms = 0;

uint64_t millis(void) {
    g_ms += 5;
    return g_ms;
}

uint64_t getTime(void) {
    return g_ms;
}

void setTime(uint64_t time) {
    g_ms = time;
}

//// HardwareSerial.h

HardwareSerial::HardwareSerial(uint8_t) {
}

HardwareSerial::operator bool(void) const {
    return false;
}

int HardwareSerial::available(void) {
    return 0;
}

int HardwareSerial::availableForWrite(void) {
    return 0;
}

void HardwareSerial::begin(unsigned long baud) {
}

void HardwareSerial::begin(unsigned long baud, uint16_t config) {
}

void HardwareSerial::end(void) {
}

void HardwareSerial::flush(void) {
}

int HardwareSerial::peek(void) {
    return 0;
}

int HardwareSerial::read(void) {
    return 0;
}

size_t HardwareSerial::write(uint8_t val) {
    return 0;
}

size_t HardwareSerial::write(const char *str) {
    return 0;
}

size_t HardwareSerial::write(const uint8_t *buf, size_t len) {
    return 0;
}

HardwareSerial Serial = HardwareSerial();
HardwareSerial Serial2 = HardwareSerial();

//// Print.h

size_t Print::print(const char *val) {
    return write(val);
}

size_t Print::print(const String &val) {
    return write(val.c_str(), val.length());
}

size_t Print::print(char val) {
    return write(val);
}

size_t Print::print(unsigned char val, int base) {
    return print(String(val, base));
}

size_t Print::print(int val, int base) {
    return print(String(val, base));
}

size_t Print::print(unsigned int val, int base) {
    return print(String(val, base));
}

size_t Print::print(long val, int base) {
    return print(String(val, base));
}

size_t Print::print(unsigned long val, int base) {
    return print(String(val, base));
}

size_t Print::print(double val, int decimalPlaces) {
    return print(String(val, decimalPlaces));
}

size_t Print::println(const char *val) {
    return print(val) + println();
}

size_t Print::println(const String &val) {
    return print(val) + println();
}

size_t Print::println(char val) {
    return print(val) + println();
}

size_t Print::println(unsigned char val, int base) {
    return print(val, base) + println();
}

size_t Print::println(int val, int base) {
    return print(val, base) + println();
}

size_t Print::println(unsigned int val, int base) {
    return print(val, base) + println();
}

size_t Print::println(long val, int base) {
    return print(val, base) + println();
}

size_t Print::println(unsigned long val, int base) {
    return print(val, base) + println();
}

size_t Print::println(double val, int decimalPlaces) {
    return print(val, decimalPlaces) + println();
}

size_t Print::println(void) {
    return write("\r\n");
}

size_t Print::write(const char *str) {
    if (str == nullptr) {
        return 0;
    }
    return write((const uint8_t *)str, strlen(str));
}

size_t Print::write(const uint8_t *buf, size_t len) {
    size_t n = 0;
    for (n = 0; n < len; n++) {
        if (write(buf[n]) == 0) {
            break;
        }
    }
    return n;
}

size_t Print::write(const char *buf, size_t len) {
    return write((const uint8_t *)buf, len);
}

//// Stream.h

Stream::Stream(void) {
}

String Stream::readStringUntil(char terminator) {
    String ret;
    int c = read();
    while (c >= 0 && c != terminator) {
        ret += (char)c;
        c = read();
    }
    return ret;
}

void Stream::setTimeout(unsigned long timeout) {
    timeout_ = timeout;
}

//// WString.h

static char *itos(int val, int base, char *buf) {
    switch (base) {
        case OCT:
            sprintf(buf, "%o", val);
            break;
        case DEC:
            sprintf(buf, "%u", val);
            break;
        case HEX:
            sprintf(buf, "%x", val);
            break;
        default:
            return 0;
    }
    return buf;
}

static char *ftos(double val, unsigned int width, unsigned int decimalPlaces, char *buf) {
    char format[32];
    snprintf(format, sizeof(format), "%%%d.%df", width, decimalPlaces);
    sprintf(buf, format, val);
    return buf;
}

void String::invalidate(void) {
    if (buffer_) {
        free(buffer_);
    }
    buffer_ = nullptr;
    capacity_ = len_ = 0;
}

unsigned char String::concatenate(const char *buf, unsigned int len) {
    if (buf == nullptr || len == 0) {
        return 0;
    }
    reserve(len_ + len);
    strcpy(buffer_ + len_, buf);
    len_ += len;
    return 1;
}

String::String(const char *val) : buffer_(nullptr), capacity_(0), len_(0) {
    *this = val;
}

String::String(const String &val) : buffer_(nullptr), capacity_(0), len_(0) {
    *this = val;
}

String::String(char val) : buffer_(nullptr), capacity_(0), len_(0) {
    char buf[] = {val, '\0'};
    *this = buf;
}

String::String(unsigned char val, unsigned char base) : buffer_(nullptr), capacity_(0), len_(0) {
    char buf[32];
    memset(buf, 0x00, sizeof(buf));
    itos((unsigned int)val, base, buf);
    *this = buf;
}

String::String(int val, unsigned char base) : buffer_(nullptr), capacity_(0), len_(0) {
    char buf[32];
    memset(buf, 0x00, sizeof(buf));
    itos((unsigned int)val, base, buf);
    *this = buf;
}

String::String(unsigned int val, unsigned char base) : buffer_(nullptr), capacity_(0), len_(0) {
    char buf[32];
    memset(buf, 0x00, sizeof(buf));
    itos((unsigned int)val, base, buf);
    *this = buf;
}

String::String(long val, unsigned char base) : buffer_(nullptr), capacity_(0), len_(0) {
    char buf[32];
    memset(buf, 0x00, sizeof(buf));
    itos((unsigned int)val, base, buf);
    *this = buf;
}

String::String(unsigned long val, unsigned char base) : buffer_(nullptr), capacity_(0), len_(0) {
    char buf[32];
    memset(buf, 0x00, sizeof(buf));
    itos((unsigned int)val, base, buf);
    *this = buf;
}

String::String(float val, unsigned char decimalPlaces) : buffer_(nullptr), capacity_(0), len_(0) {
    char buf[32];
    memset(buf, 0x00, sizeof(buf));
    ftos(val, 2 + decimalPlaces, decimalPlaces, buf);
    *this = buf;
}

String::String(double val, unsigned char decimalPlaces) : buffer_(nullptr), capacity_(0), len_(0) {
    char buf[32];
    memset(buf, 0x00, sizeof(buf));
    ftos(val, 2 + decimalPlaces, decimalPlaces, buf);
    *this = buf;
}

String &String::operator=(const String &rhs) {
    if (this != &rhs) {
        reserve(rhs.len_);
        strcpy(buffer_, rhs.buffer_);
        len_ = rhs.len_;
    }
    return *this;
}

String &String::operator=(const char *rhs) {
    unsigned int len = 0;
    if (rhs == nullptr) {
        len_ = 0;
    } else {
        len = strlen(rhs);
        reserve(len);
        strcpy(buffer_, rhs);
        len_ = len;
    }
    return *this;
}

String::~String(void) {
    invalidate();
}

char String::charAt(unsigned int n) const {
    return operator[](n);
}

int String::compareTo(const String &str) const {
    return strcmp(buffer_, str.buffer_);
}

unsigned char String::concat(const String &val) {
    return concatenate(val.buffer_, val.len_);
}

unsigned char String::concat(const char *val) {
    return concat(String(val));
}

unsigned char String::concat(char val) {
    return concat(String(val));
}

unsigned char String::concat(unsigned char val) {
    return concat(String(val));
}

unsigned char String::concat(int val) {
    return concat(String(val));
}

unsigned char String::concat(unsigned int val) {
    return concat(String(val));
}

unsigned char String::concat(long val) {
    return concat(String(val));
}

unsigned char String::concat(unsigned long val) {
    return concat(String(val));
}

unsigned char String::concat(float val) {
    return concat(String(val));
}

unsigned char String::concat(double val) {
    return concat(String(val));
}

const char *String::c_str(void) const {
    return buffer_;
};

unsigned char String::endsWith(const String &str) const {
    if (len_ < str.len_) {
        return 0;
    }
    return strcmp(&buffer_[len_ - str.len_], str.buffer_) == 0;
}

unsigned char String::equals(const String &str) const {
    return compareTo(str) == 0;
}

unsigned char String::equals(const char *str) const {
    return equals(String(str));
}

unsigned char String::equalsIgnoreCase(const String &str) const {
    return strcasecmp(buffer_, str.buffer_);
}

void String::getBytes(unsigned char *buf, unsigned int len) const {
    unsigned int n = len - 1;
    if (n > len_) {
        n = len_;
    }
    strncpy((char *)buf, buffer_, n);
    buf[n] = 0;
}

int String::indexOf(char val) const {
    return indexOf(val, 0);
}

int String::indexOf(char val, unsigned int from) const {
    if (from >= len_) {
        return -1;
    }
    const char *pos = strchr(buffer_ + from, val);
    if (pos == nullptr) {
        return -1;
    }
    return pos - buffer_;
}

int String::indexOf(const String &val) const {
    return indexOf(val, 0);
}

int String::indexOf(const String &val, unsigned int from) const {
    if (from + val.len_ > len_) {
        return -1;
    }
    const char *pos = strstr(buffer_ + from, val.buffer_);
    if (pos == nullptr) {
        return -1;
    }
    return pos - buffer_;
}

int String::lastIndexOf(char val) const {
    return lastIndexOf(val, len_ - 1);
}

int String::lastIndexOf(char val, unsigned int from) const {
    if (from >= len_) {
        return -1;
    }
    for (int pos = from; pos >= 0; pos--) {
        if (buffer_[pos] == val) {
            return pos;
        }
    }
    return -1;
}

int String::lastIndexOf(const String &val) const {
    return lastIndexOf(val, len_ - val.len_);
}

int String::lastIndexOf(const String &val, unsigned int from) const {
    if (val.len_ > len_) {
        return -1;
    }
    if (from >= len_) {
        from = len_ - 1;
    }
    char *pos = nullptr;
    for (char *p = buffer_; p <= buffer_ + from; p++) {
        p = strstr(p, val.buffer_);
        if (p == nullptr) {
            break;
        }
        if (p >= buffer_ + from) {
            break;
        }
        pos = p;
    }
    if (pos == nullptr) {
        return -1;
    }
    return pos - buffer_;
}

unsigned int String::length(void) const {
    return len_;
}

void String::remove(unsigned int index) {
    remove(index, ~0);
}

void String::remove(unsigned int index, unsigned int count) {
    if (index >= len_) {
        return;
    }
    if (count > len_ - index) {
        count = len_ - index;
    }
    String s1 = substring(0, index);
    String s2 = substring(index + count);
    *this = s1 + s2;
}

void String::replace(char ch1, char ch2) {
    for (unsigned int i = 0; i < len_; i++) {
        if (buffer_[i] == ch1) {
            buffer_[i] = ch2;
        }
    }
}

void String::replace(const String &str1, const String &str2) {
    int pos = 0;
    while ((pos = indexOf(str1, pos)) >= 0) {
        *this = substring(0, pos) + str2 + substring(pos + str1.len_);
        pos += str2.len_;
    }
}

void String::reserve(unsigned int size) {
    if (buffer_ && capacity_ >= size) {
        return;
    }
    char *buf = (char *)realloc(buffer_, size + 1);
    if (buf) {
        buffer_ = buf;
        capacity_ = size;
    }
}

void String::setCharAt(unsigned int index, char c) {
    if (index < len_) {
        buffer_[index] = c;
    }
}

unsigned char String::startsWith(const String &str) const {
    if (len_ < str.len_) {
        return 0;
    }
    return strncmp(buffer_, str.buffer_, str.len_) == 0;
}

String String::substring(unsigned int from) const {
    return substring(from, len_);
}

String String::substring(unsigned int from, unsigned int to) const {
    if (from > to) {
        unsigned int tmp = to;
        to = from;
        from = tmp;
    }
    String ret;
    if (from >= len_) {
        return ret;
    }
    if (to > len_) {
        to = len_;
    }
    char stash = buffer_[to];
    buffer_[to] = '\0';
    ret = buffer_ + from;
    buffer_[to] = stash;
    return ret;
}

void String::toCharArray(char *buf, unsigned int len) const {
    getBytes((unsigned char *)buf, len);
}

double String::toDouble(void) const {
    if (buffer_) {
        return atof(buffer_);
    }
    return 0;
}

long String::toInt(void) const {
    if (buffer_) {
        return atol(buffer_);
    }
    return 0;
}

float String::toFloat(void) const {
    return float(toDouble());
}

void String::toLowerCase(void) {
    for (unsigned int i = 0; i < len_; i++) {
        buffer_[i] = tolower(buffer_[i]);
    }
}

void String::toUpperCase(void) {
    for (unsigned int i = 0; i < len_; i++) {
        buffer_[i] = toupper(buffer_[i]);
    }
}

void String::trim(void) {
    if (len_ == 0) {
        return;
    }
    unsigned int from = 0;
    for (from = 0; from < len_; from++) {
        if (!isspace(buffer_[from])) {
            break;
        }
    }
    unsigned int to = 0;
    for (to = len_; to > 0; to--) {
        if (!isspace(buffer_[to - 1])) {
            break;
        }
    }
    if (from < to) {
        *this = substring(from, to);
    } else {
        *this = String();
    }
}

char String::operator[](unsigned int index) const {
    if (index >= len_ || !buffer_) {
        return 0;
    }
    return buffer_[index];
}

char &String::operator[](unsigned int index) {
    static char dummy_writable_char;
    if (index >= len_ || !buffer_) {
        dummy_writable_char = 0;
        return dummy_writable_char;
    }
    return buffer_[index];
}

String &String::operator+=(const String &rhs) {
    concat(rhs);
    return (*this);
}

String &String::operator+=(const char *rhs) {
    concat(rhs);
    return (*this);
}

String &String::operator+=(char rhs) {
    concat(rhs);
    return (*this);
}

String &String::operator+=(unsigned char rhs) {
    concat(rhs);
    return (*this);
}

String &String::operator+=(int rhs) {
    concat(rhs);
    return (*this);
}

String &String::operator+=(unsigned int rhs) {
    concat(rhs);
    return (*this);
}

String &String::operator+=(long rhs) {
    concat(rhs);
    return (*this);
}

String &String::operator+=(unsigned long rhs) {
    concat(rhs);
    return (*this);
}

String &String::operator+=(float rhs) {
    concat(rhs);
    return (*this);
}

String &String::operator+=(double rhs) {
    concat(rhs);
    return (*this);
}

unsigned char String::operator==(const String &rhs) const {
    return equals(rhs);
}

unsigned char String::operator==(const char *cstr) const {
    return equals(cstr);
}

unsigned char String::operator>(const String &rhs) const {
    return compareTo(rhs) > 0;
}

unsigned char String::operator>=(const String &rhs) const {
    return compareTo(rhs) >= 0;
}

unsigned char String::operator<(const String &rhs) const {
    return compareTo(rhs) < 0;
}

unsigned char String::operator<=(const String &rhs) const {
    return compareTo(rhs) <= 0;
}

unsigned char String::operator!=(const String &rhs) const {
    return !equals(rhs);
}

unsigned char String::operator!=(const char *rhs) const {
    return !equals(rhs);
}

char *String::begin(void) {
    return buffer_;
}

char *String::end(void) {
    return buffer_ + length();
}

const char *String::begin(void) const {
    return c_str();
}

const char *String::end(void) const {
    return c_str() + length();
}

StringHelper::StringHelper(const String &val) : String(val) {
}

StringHelper::StringHelper(const char *val) : String(val) {
}

StringHelper::StringHelper(char val) : String(val) {
}

StringHelper::StringHelper(unsigned char val) : String(val) {
}

StringHelper::StringHelper(int val) : String(val) {
}

StringHelper::StringHelper(unsigned int val) : String(val) {
}

StringHelper::StringHelper(long val) : String(val) {
}

StringHelper::StringHelper(unsigned long val) : String(val) {
}

StringHelper::StringHelper(float val) : String(val) {
}

StringHelper::StringHelper(double val) : String(val) {
}

StringHelper &operator+(const StringHelper &lhs, const String &rhs) {
    StringHelper &a = const_cast<StringHelper &>(lhs);
    a.concatenate(rhs.buffer_, rhs.len_);
    return a;
}

StringHelper &operator+(const StringHelper &lhs, const char *rhs) {
    StringHelper &a = const_cast<StringHelper &>(lhs);
    a.concat(rhs);
    return a;
}

StringHelper &operator+(const StringHelper &lhs, char rhs) {
    StringHelper &a = const_cast<StringHelper &>(lhs);
    a.concat(rhs);
    return a;
}

StringHelper &operator+(const StringHelper &lhs, unsigned char rhs) {
    StringHelper &a = const_cast<StringHelper &>(lhs);
    a.concat(rhs);
    return a;
}

StringHelper &operator+(const StringHelper &lhs, int rhs) {
    StringHelper &a = const_cast<StringHelper &>(lhs);
    a.concat(rhs);
    return a;
}

StringHelper &operator+(const StringHelper &lhs, unsigned int rhs) {
    StringHelper &a = const_cast<StringHelper &>(lhs);
    a.concat(rhs);
    return a;
}

StringHelper &operator+(const StringHelper &lhs, long rhs) {
    StringHelper &a = const_cast<StringHelper &>(lhs);
    a.concat(rhs);
    return a;
}

StringHelper &operator+(const StringHelper &lhs, unsigned long rhs) {
    StringHelper &a = const_cast<StringHelper &>(lhs);
    a.concat(rhs);
    return a;
}

StringHelper &operator+(const StringHelper &lhs, float rhs) {
    StringHelper &a = const_cast<StringHelper &>(lhs);
    a.concat(rhs);
    return a;
}

StringHelper &operator+(const StringHelper &lhs, double rhs) {
    StringHelper &a = const_cast<StringHelper &>(lhs);
    a.concat(rhs);
    return a;
}

//// arch/board/cxd56_audio.h

int board_external_amp_mute_control(bool en) {
    return 0;
}

//// File.h

struct DummyFile {
    String path;
    uint8_t *content;
    int size;
};

std::vector<DummyFile> g_dummy_files;

void registerDummyFile(const String &path, const uint8_t *content, int size) {
    for (auto &e : g_dummy_files) {
        if (e.path == path) {
            delete[] e.content;
            e.content = new uint8_t[size];
            memcpy(e.content, content, size);
            return;
        }
    }
    DummyFile item;
    item.path = path;
    item.content = new uint8_t[size];
    memcpy(item.content, content, size);
    item.size = size;
    g_dummy_files.push_back(item);
}

File::File(const char *name, uint8_t mode)
    : name_(nullptr), fp_(nullptr), size_(0), curpos_(0), binary_data_(nullptr), is_directory_(false), dummy_index_(0), dummy_smf_index_(0) {
    // printf("%s:%d:%s(%s,%u)\n", __FILE__, __LINE__, __func__, name, mode);
    if (!name) {
        return;
    }
    int index = 0;
    for (const auto &e : g_dummy_files) {
        if (e.path == name) {
            binary_data_ = new uint8_t[e.size];
            memcpy(binary_data_, e.content, e.size);
            name_ = strdup(name);
            size_ = e.size;
            curpos_ = 0;
            dummy_smf_index_ = index;
            return;
        } else if (e.path.startsWith(name)) {
            is_directory_ = true;
            dummy_smf_index_ = index;
            return;
        }
        index++;
    }

    if (mode == FILE_READ) {
        fp_ = fopen(name, "rb");
        if (fp_) {
            name_ = strdup(name);
            fseek(fp_, 0, SEEK_END);
            size_ = ftell(fp_);
            fseek(fp_, 0, SEEK_SET);
            curpos_ = ftell(fp_);
        }
    } else if (mode == FILE_WRITE) {
        fp_ = fopen(name, "wb");
        if (fp_) {
            name_ = strdup(name);
            fseek(fp_, 0, SEEK_SET);
            size_ = 0;
            curpos_ = ftell(fp_);
        }
    }
}

File::File(void) : name_(nullptr), fp_(nullptr), size_(0), curpos_(0) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
}

File::~File(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
    if (fp_) {
        // fclose(m_fp);
        fp_ = nullptr;
    }
    if (name_) {
        // free(name_);
        name_ = nullptr;
    }
}

size_t File::write(uint8_t val) {
    // printf("%s:%d:%s(%u)\n", __FILE__, __LINE__, __func__, val);
    size_t ret = 0;
    if (fp_) {
        ret = fwrite(&val, sizeof(val), 1, fp_);
        if (ret >= 0) {
            curpos_ += ret;
            size_ = curpos_;
        }
    }
    return ret;
}

size_t File::write(const uint8_t *buf, size_t len) {
    // printf("%s:%d:%s(%p,%u)\n", __FILE__, __LINE__, __func__, buf, len);
    size_t ret = 0;
    if (fp_) {
        ret = fwrite(&buf, sizeof(buf[0]), len, fp_);
        if (ret >= 0) {
            curpos_ += ret;
            size_ = curpos_;
        }
    }
    return ret;
}

int File::read(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
    size_t ret = -1;
    if (content_.length() > 0) {
        if (curpos_ >= size_) {
            return -1;
        }
        return (int)((unsigned char)content_[curpos_++]);
    }

    if (binary_data_ != nullptr && 0 < size()) {
        if (curpos_ >= size_) {
            return -1;
        }
        return (int)((unsigned char)binary_data_[curpos_++]);
    }
    uint8_t data = 0;
    if (fp_) {
        ret = fread(&data, sizeof(data), 1, fp_);
        if (ret >= 1) {
            curpos_ += ret;
            return data;
        }
    }
    return -1;
}

int File::peek(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
    size_t ret = -1;
    if (content_.length() > 0) {
        if (curpos_ >= size_) {
            return -1;
        }
        return (int)((unsigned char)content_[curpos_]);
    }
    if (binary_data_ != nullptr && 0 < size()) {
        if (curpos_ >= size_) {
            return -1;
        }
        return (int)((unsigned char)binary_data_[curpos_]);
    }

    uint8_t data = 0;
    if (fp_) {
        long pos = ftell(fp_);
        ret = fread(&data, sizeof(data), 1, fp_);
        fseek(fp_, pos, SEEK_SET);
        if (ret == 1) {
            return data;
        }
    }
    return -1;
}

int File::available(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
    if (content_.length() > 0) {
        return size() - position();
    }
    if (binary_data_ != nullptr && 0 < size()) {
        return size() - position();
    }
    if (fp_) {
        return size() - position();
    }
    return 0;
}

void File::flush(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
    if (fp_) {
        fflush(fp_);
    }
};

int File::read(void *buf, size_t len) {
    // printf("%s:%d:%s(%p,%u)\n", __FILE__, __LINE__, __func__, buf, len);
    size_t ret = -1;
    if (content_.length() > 0) {
        if (buf == nullptr) {
            return ret;
        }
        ret = (len < (size_t)available()) ? len : available();
        memcpy(buf, content_.c_str() + curpos_, ret);
        curpos_ += ret;
        return ret;
    }
    if (binary_data_ != nullptr && 0 < size()) {
        if (buf == nullptr) {
            return ret;
        }
        ret = (len < (size_t)available()) ? len : available();
        memcpy(buf, binary_data_ + curpos_, ret);
        curpos_ += ret;
        return ret;
    }
    if (fp_) {
        ret = fread(buf, 1, len, fp_);
        if (ret >= 0) {
            curpos_ += ret;
        }
    }
    return ret;
}

boolean File::seek(uint32_t pos) {
    // printf("%s:%d:%s(%u)\n", __FILE__, __LINE__, __func__, pos);
    if (content_.length() > 0) {
        curpos_ = (pos < size_) ? pos : size();
        return true;
    }

    if (binary_data_ != nullptr && 0 < size()) {
        curpos_ = (pos < size_) ? pos : size();
        return true;
    }

    if (fp_) {
        int ret = fseek(fp_, pos, SEEK_SET);
        if (ret == 0) {
            curpos_ = pos;
            return true;
        }
    }
    return false;
}

uint32_t File::position(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
    return curpos_;
}

uint32_t File::size(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
    return size_;
}

void File::close(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
    if (fp_) {
        fclose(fp_);
        fp_ = nullptr;
    }
}

File::operator bool(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
    if (isDirectory()) {
        return true;
    }
    if (content_.length() > 0) {
        return true;
    }
    if (binary_data_ != nullptr && 0 < size()) {
        return true;
    }
    if (fp_) {
        return true;
    }
    return false;
}

char *File::name(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
    return name_;
}

boolean File::isDirectory(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
    return is_directory_;
}

File File::openNextFile(uint8_t mode) {
    // printf("%s:%d:%s(%u)\n", __FILE__, __LINE__, __func__, mode);
    if (isDirectory()) {
        for (long long unsigned int i = dummy_index_ + 1; i < g_dummy_files.size(); i++) {
            if (g_dummy_files[i].path.startsWith(name_)) {
                dummy_index_ = i;
                return File(g_dummy_files[i].path.c_str(), mode);
            }
        }
        for (long long unsigned int i = dummy_smf_index_ + 1; i < g_dummy_files.size(); i++) {
            if (g_dummy_files[i].path.startsWith(name_)) {
                dummy_smf_index_ = i;
                return File(g_dummy_files[i].path.c_str(), mode);
            }
        }
    }

    return File();
}

void File::rewindDirectory(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
}

//// FrontEnd.h

bool AS_ReceiveObjectReply(MsgQueId msgq_id, AudioObjReply *reply) {
    return true;
}

bool AS_InitMicFrontend(AsInitMicFrontendParam *initparam) {
    return true;
}

err_t FrontEnd::begin(void) {
    return FRONTEND_ECODE_OK;
}

err_t FrontEnd::activate(void) {
    return FRONTEND_ECODE_OK;
}

err_t FrontEnd::init(uint8_t channel_number, uint8_t bit_length, uint32_t samples_per_frame, uint8_t data_path, AsDataDest dest) {
    return FRONTEND_ECODE_OK;
}

err_t FrontEnd::init(uint8_t channel_number, uint8_t bit_length, uint32_t samples_per_frame, uint8_t data_path, AsDataDest dest, uint8_t preproc_type,
                     const char *dsp_path) {
    return FRONTEND_ECODE_OK;
}

err_t FrontEnd::start(void) {
    return FRONTEND_ECODE_OK;
}

err_t FrontEnd::setMicGain(int16_t mic_gain) {
    return FRONTEND_ECODE_OK;
}

FrontEnd *FrontEnd::getInstance(void) {
    static FrontEnd instance;
    return &instance;
}

FrontEnd::FrontEnd(void) {
}

FrontEnd::FrontEnd(const FrontEnd &) {
}

FrontEnd &FrontEnd::operator=(const FrontEnd &) {
    return *this;
}

FrontEnd::~FrontEnd(void) {
}

//// MemoryUtil.h

int initMemoryPools(void) {
    return 0;
}

int createStaticPools(uint8_t layout_no) {
    return 0;
}

MemHandle::MemHandle(void) : chunk_(nullptr) {
    chunk_ = new MemHandle::Chunk();
    if (chunk_) {
        chunk_->addr_ = nullptr;
        chunk_->size_ = 0;
        chunk_->ref_ = 1;
    }
}

MemHandle::MemHandle(const MemHandle &rhs) {
    if (rhs.chunk_) {
        rhs.chunk_->ref_++;
        chunk_ = rhs.chunk_;
    }
}

MemHandle::~MemHandle(void) {
    freeSeg();
}

MemHandle &MemHandle::operator=(const MemHandle &rhs) {
    if (rhs.chunk_) {
        rhs.chunk_->ref_++;
        freeSeg();
        chunk_ = rhs.chunk_;
    }
    return *this;
}

// static const int kRendPcmBufSize = 90000;
static const int kRendPcmBufSize = 5 * 960;
static size_t g_rend_pcm_buf_remain = kRendPcmBufSize;

err_t MemHandle::allocSeg(PoolId id, size_t size_for_check) {
    freeSeg();
    if (size_for_check > g_rend_pcm_buf_remain) {
        // printf("%s:%d:%s: alloc: %d, size=%d => NG\n", __FILE__, __LINE__, __func__, (int)g_rend_pcm_buf_remain, (int)size_for_check);
        return ERR_NG;
    }

    // printf("%s:%d:%s: alloc: %d, size=%d => OK\n", __FILE__, __LINE__, __func__, (int)g_rend_pcm_buf_remain, (int)size_for_check);
    void *buf = malloc(size_for_check);
    if (buf == nullptr) {
        return ERR_NG;
    }
    g_rend_pcm_buf_remain -= size_for_check;
    chunk_ = new MemHandle::Chunk();
    if (chunk_) {
        chunk_->size_ = size_for_check;
        chunk_->addr_ = buf;
        chunk_->ref_ = 1;
    }
    return ERR_OK;
}

err_t MemHandle::freeSeg(void) {
    if (chunk_) {
        chunk_->ref_--;
        if (chunk_->ref_ <= 0) {
            if (chunk_->addr_) {
                g_rend_pcm_buf_remain += chunk_->size_;
                // printf("%s:%d:%s: free: %d\n", __FILE__, __LINE__, __func__, (int)g_rend_pcm_buf_remain);
                free(chunk_->addr_);
            }
            chunk_->addr_ = nullptr;
            chunk_->size_ = 0;
            delete chunk_;
        }
        chunk_ = nullptr;
    }
    return ERR_OK;
}

void *MemHandle::getVa(void) const {
    return (chunk_) ? chunk_->addr_ : nullptr;
}

void *MemHandle::getPa(void) const {
    return (chunk_) ? chunk_->addr_ : nullptr;
}

//// MP.h

MPClass MP;

MPClass::MPClass(void) {
}

MPClass::~MPClass(void) {
}

int MPClass::begin(int subid) {
    return 0;
}

int end(int subid) {
    return 0;
}

int MPClass::Send(int8_t msgid, void *msgaddr, int subid) {
    return 0;
}

int MPClass::Recv(int8_t *msgid, void *msgaddr, int subid) {
    return 0;
}

void MPClass::RecvTimeout(uint32_t timeout) {
    return;
}

void MPClass::GetMemoryInfo(int &usedMem, int &freeMem, int &largestFreeMem) {
    usedMem = 0;
    freeMem = 0;
    largestFreeMem = 0;
}

//// OutputMixer.h

err_t OutputMixer::create(void) {
    return ERR_OK;
}

err_t OutputMixer::activate(AsOutputMixerHandle handle, OutputMixerCallback omcb) {
    return activate(handle, HPOutputDevice, omcb);
}

err_t OutputMixer::activate(AsOutputMixerHandle handle, uint8_t output_device, OutputMixerCallback omcb) {
    return OUTPUTMIXER_ECODE_OK;
}

err_t OutputMixer::sendData(AsOutputMixerHandle handle, PcmProcDoneCallback pcmdone_cb, AsPcmDataParam pcm) {
    // printf("%s:%d:%s: entry: %d, cb=%p, pa=%p\n", __FILE__, __LINE__, __func__, handle, pcmdone_cb, pcm.mh.getPa());
    AsSendDataOutputMixer data;
    data.handle = handle;
    data.callback = pcmdone_cb;
    data.pcm = pcm;
    queue_.push_back(data);
    return OUTPUTMIXER_ECODE_OK;
}

err_t OutputMixer::activateBaseband(void) {
    clear();
    return OUTPUTMIXER_ECODE_OK;
}

bool OutputMixer::setRenderingClkMode(uint8_t clk_mode) {
    return true;
}

err_t OutputMixer::setVolume(int master, int player0, int player1) {
    return OUTPUTMIXER_ECODE_OK;
}

OutputMixer *OutputMixer::getInstance(void) {
    static OutputMixer instance;
    return &instance;
}

void OutputMixer::setOutputHandler(void *(*handler)(void *arg, AsSendDataOutputMixer *data), void *arg) {
    // printf("%s:%d:%s: entry handler=%p arg=%p\n", __FILE__, __LINE__, __func__, handler, arg);
    handler_ = handler;
    handler_arg_ = arg;
}

void OutputMixer::flush(int n) {
    if (n < 0) {
        n = queue_.size();
    }
    if ((int)queue_.size() < n) {
        n = queue_.size();
    }
    for (int i = 0; i < n; i++) {
        auto &e = queue_[i];
        if (handler_) {
            // printf("handler_\n");
            handler_(handler_arg_, &e);
        }
        if (e.callback) {
            // printf("callback_\n");
            e.callback(e.handle, e.pcm.is_end);
        }
        e.pcm.mh.freeSeg();
    }
    for (int i = 0; i < n; i++) {
        queue_.erase(queue_.begin());
    }
}

void OutputMixer::clear() {
    for (auto &e : queue_) {
        e.pcm.mh.freeSeg();
    }
    queue_.clear();
}

OutputMixer::OutputMixer(void) {
}

OutputMixer::OutputMixer(const OutputMixer &) {
}

OutputMixer &OutputMixer::operator=(const OutputMixer &) {
    return *this;
}

OutputMixer::~OutputMixer(void) {
    clear();
}

//// SDHCI.h

SDClass::SDClass(void) {
    // printf("%s:%d:%s()\n", __FILE__, __LINE__, __func__);
}

boolean SDClass::begin(uint8_t dummy) {
    return true;
}

//// Storage.h

#define MAXPATHLEN (256)

StorageClass::StorageClass(void) {
}

StorageClass::StorageClass(const char *str) {
}

File StorageClass::open(const char *filepath, uint8_t mode) {
    return File(filepath, mode);
}

File StorageClass::open(const String &filepath, uint8_t mode) {
    return open(filepath.c_str(), mode);
}

boolean StorageClass::exists(const char *filepath) {
    if (filepath) {
        for (const auto &e : g_dummy_files) {
            if (e.path == filepath) {
                return true;
            }
        }
        struct stat st;
        return (::stat(filepath, &st) == 0);
    } else {
        return false;
    }
}

boolean StorageClass::exists(const String &filepath) {
    return exists(filepath.c_str());
}

boolean StorageClass::mkdir(const char *filepath) {
    char fullpath[MAXPATHLEN];
    struct stat st;
    char *p;
    char tmp;
    if (!filepath) {
        return false;
    };
    strcpy(fullpath, filepath);
    for (p = &fullpath[1]; *p; ++p) {
        tmp = 0;
        if (*p == '/' || *(p + 1) == 0) {
            if (*p == '/') {
                tmp = *p;
                *p = 0;
            }
            if (::stat(filepath, &st) != 0 || !S_ISDIR(st.st_mode)) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
                if (::mkdir(fullpath) != 0) {
                    return false;
                }
#else
                if (::mkdir(fullpath, 0755) != 0) {
                    return false;
                }
#endif
            }
            if (tmp == '/') {
                *p = tmp;
            }
        }
    }
    return true;
}

boolean StorageClass::mkdir(const String &filepath) {
    return mkdir(filepath.c_str());
}

boolean StorageClass::remove(const char *filepath) {
    if (!filepath) {
        return false;
    };

    for (auto it = g_dummy_files.begin(); it != g_dummy_files.end();) {
        if (it->path == filepath) {
            it = g_dummy_files.erase(it);
            return true;
        } else {
            it++;
        }
    }

    for (auto it = g_dummy_files.begin(); it != g_dummy_files.end();) {
        if (it->path == filepath) {
            it = g_dummy_files.erase(it);
            return true;
        } else {
            it++;
        }
    }

    char path[MAXPATHLEN];
    strcpy(path, filepath);
    return (unlink(path) == 0);
}

boolean StorageClass::remove(const String &filepath) {
    return remove(filepath.c_str());
}

boolean StorageClass::rmdir(const char *filepath) {
    if (!filepath) {
        return false;
    };
    char path[MAXPATHLEN];
    strcpy(path, filepath);
    size_t n = strlen(path);
    if (path[n - 1] == '/') {
        path[n - 1] = '\0';
    }
    return (::rmdir(path) == 0);
}

boolean StorageClass::rmdir(const String &filepath) {
    return rmdir(filepath.c_str());
}

StorageClass Storage;
