/*
 * SPDX-License-Identifier: (Apache-2.0 OR LGPL-2.1-or-later)
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation
 */

#ifndef DUMMY_WSTRING_H_
#define DUMMY_WSTRING_H_

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Arduino.h"

class StringHelper;

class String {
public:
    String(const char *val = "");
    String(const String &val);
    explicit String(char val);
    explicit String(unsigned char val, unsigned char base = 10);
    explicit String(int val, unsigned char base = 10);
    explicit String(unsigned int val, unsigned char base = 10);
    explicit String(long val, unsigned char base = 10);
    explicit String(unsigned long val, unsigned char base = 10);
    explicit String(float val, unsigned char decimalPlaces = 2);
    explicit String(double val, unsigned char decimalPlaces = 2);
    String &operator=(const String &rhs);
    String &operator=(const char *rhs);
    ~String(void);

    char charAt(unsigned int n) const;
    int compareTo(const String &str) const;
    unsigned char concat(const String &val);
    unsigned char concat(const char *val);
    unsigned char concat(char val);
    unsigned char concat(unsigned char val);
    unsigned char concat(int val);
    unsigned char concat(unsigned int val);
    unsigned char concat(long val);
    unsigned char concat(unsigned long val);
    unsigned char concat(float val);
    unsigned char concat(double val);
    const char *c_str(void) const;
    unsigned char endsWith(const String &str) const;
    unsigned char equals(const String &str) const;
    unsigned char equals(const char *str) const;
    unsigned char equalsIgnoreCase(const String &str) const;
    void getBytes(unsigned char *buf, unsigned int len) const;
    int indexOf(char val) const;
    int indexOf(char val, unsigned int from) const;
    int indexOf(const String &val) const;
    int indexOf(const String &val, unsigned int from) const;
    int lastIndexOf(char val) const;
    int lastIndexOf(char val, unsigned int from) const;
    int lastIndexOf(const String &val) const;
    int lastIndexOf(const String &val, unsigned int from) const;
    unsigned int length(void) const;
    void remove(unsigned int index);
    void remove(unsigned int index, unsigned int count);
    void replace(char ch1, char ch2);
    void replace(const String &str1, const String &str2);
    void reserve(unsigned int size);
    void setCharAt(unsigned int index, char c);
    unsigned char startsWith(const String &str) const;
    String substring(unsigned int from) const;
    String substring(unsigned int from, unsigned int to) const;
    void toCharArray(char *buf, unsigned int len) const;
    double toDouble(void) const;
    long toInt(void) const;
    float toFloat(void) const;
    void toLowerCase(void);
    void toUpperCase(void);
    void trim(void);

    char operator[](unsigned int index) const;
    char &operator[](unsigned int index);
    friend StringHelper &operator+(const StringHelper &lhs, const String &rhs);
    friend StringHelper &operator+(const StringHelper &lhs, const char *rhs);
    friend StringHelper &operator+(const StringHelper &lhs, char rhs);
    friend StringHelper &operator+(const StringHelper &lhs, unsigned char rhs);
    friend StringHelper &operator+(const StringHelper &lhs, int rhs);
    friend StringHelper &operator+(const StringHelper &lhs, unsigned int rhs);
    friend StringHelper &operator+(const StringHelper &lhs, long rhs);
    friend StringHelper &operator+(const StringHelper &lhs, unsigned long rhs);
    friend StringHelper &operator+(const StringHelper &lhs, float rhs);
    friend StringHelper &operator+(const StringHelper &lhs, double rhs);
    String &operator+=(const String &rhs);
    String &operator+=(const char *rhs);
    String &operator+=(char rhs);
    String &operator+=(unsigned char rhs);
    String &operator+=(int rhs);
    String &operator+=(unsigned int rhs);
    String &operator+=(long rhs);
    String &operator+=(unsigned long rhs);
    String &operator+=(float rhs);
    String &operator+=(double rhs);
    unsigned char operator==(const String &rhs) const;
    unsigned char operator==(const char *rhs) const;
    unsigned char operator>(const String &rhs) const;
    unsigned char operator>=(const String &rhs) const;
    unsigned char operator<(const String &rhs) const;
    unsigned char operator<=(const String &rhs) const;
    unsigned char operator!=(const String &rhs) const;
    unsigned char operator!=(const char *rhs) const;

    char *begin(void);
    char *end(void);
    const char *begin(void) const;
    const char *end(void) const;

protected:
    char *buffer_;
    unsigned int capacity_;
    unsigned int len_;

protected:
    void invalidate(void);
    unsigned char concatenate(const char *buf, unsigned int len);
};

class StringHelper : public String {
public:
    StringHelper(const String &val);
    StringHelper(const char *val);
    StringHelper(char val);
    StringHelper(unsigned char val);
    StringHelper(int val);
    StringHelper(unsigned int val);
    StringHelper(long val);
    StringHelper(unsigned long val);
    StringHelper(float val);
    StringHelper(double val);
};

StringHelper &operator+(const StringHelper &lhs, const String &rhs);
StringHelper &operator+(const StringHelper &lhs, const char *rhs);
StringHelper &operator+(const StringHelper &lhs, char rhs);
StringHelper &operator+(const StringHelper &lhs, unsigned char rhs);
StringHelper &operator+(const StringHelper &lhs, int rhs);
StringHelper &operator+(const StringHelper &lhs, unsigned int rhs);
StringHelper &operator+(const StringHelper &lhs, long rhs);
StringHelper &operator+(const StringHelper &lhs, unsigned long rhs);
StringHelper &operator+(const StringHelper &lhs, float rhs);
StringHelper &operator+(const StringHelper &lhs, double rhs);

#endif  // DUMMY_WSTRING_H_
