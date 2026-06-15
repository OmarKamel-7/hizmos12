// db.h
#ifndef DB_H
#define DB_H

#include <Arduino.h>
#pragma once

struct FreqEntry {
  float startMHz;
  float endMHz;
  const char* name;
  const char* usage;
  const char* note;
};

extern const FreqEntry freqDB[];
extern const int freqDBSize;

const FreqEntry* findFrequency(float f);

struct QuizQuestion {
    const char* question;
    const char* answers[4];
    uint8_t correct;
    const char* explanation;
};

// RF Questions - Easy
extern const QuizQuestion easyRFQuestions[20];

// RF Questions - Medium  
extern const QuizQuestion mediumRFQuestions[20];

// RF Questions - Hard
extern const QuizQuestion hardRFQuestions[20];

// Embedded Questions - Easy
extern const QuizQuestion easyEmbeddedQuestions[20];

// Embedded Questions - Medium
extern const QuizQuestion mediumEmbeddedQuestions[20];

// Embedded Questions - Hard
extern const QuizQuestion hardEmbeddedQuestions[20];

// Hardware Pentesting Questions - Easy
extern const QuizQuestion easyHardwareQuestions[20];

// Hardware Pentesting Questions - Medium
extern const QuizQuestion mediumHardwareQuestions[20];

// Hardware Pentesting Questions - Hard
extern const QuizQuestion hardHardwareQuestions[20];




// page selector
enum PortalPageType {
  PAGE_HOTEL = 0,
  PAGE_SIMPLE,
  PAGE_3,
  PAGE_4
};

// external variables (optional)
extern const char* renderPortalPage(int page, const char* ssid);

// HTML templates (optional declarations if needed)
extern const char SIMPLE_insta0[];
extern const char SIMPLE_whats1[];
extern const char SIMPLE_hot2[];
extern const char SIMPLE_masr3[];


#endif