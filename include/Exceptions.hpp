#pragma once
#include <stdexcept>
#include <string>

// Aksi yang melanggar aturan simulasi. Simulator menangkapnya dengan try-catch.
class InvalidActionException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
class SpeedLimitException : public InvalidActionException {   // melebihi 0.5 m/tick
public:
    using InvalidActionException::InvalidActionException;
};
class TurnLimitException : public InvalidActionException {    // putar > 90 derajat/tick
public:
    using InvalidActionException::InvalidActionException;
};
class OutOfBoundsException : public InvalidActionException {  // keluar lapangan
public:
    using InvalidActionException::InvalidActionException;
};
class BlockedException : public InvalidActionException {      // menabrak bola
public:
    using InvalidActionException::InvalidActionException;
};
class KickNotPossibleException : public InvalidActionException {  // bola tidak di petak depan
public:
    using InvalidActionException::InvalidActionException;
};

// Kesalahan file konfigurasi / skenario.
class ConfigException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
