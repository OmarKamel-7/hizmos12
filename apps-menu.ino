



////////////////questions appp
// apps.ino
#include "db.h"

#include "icons.h"

#include <Preferences.h>

// ==================== FORWARD DECLARATION ====================
// Make sure academyApp is declared before handleappsmenu uses it
void academyApp();  // ADD THIS LINE AT THE TOP

// ==================== FLASHLIGHT APP ====================
// ... (keep your existing flashlight code here)
// ... (I'm keeping it short, but keep your full flashlight implementation)

// ==================== ACADEMY APP ====================

// Academy global variables
Preferences academyPrefs;

struct AcademyStats {
    uint32_t totalXP;
    uint16_t rfLevel;
    uint16_t embeddedLevel;
    uint16_t hardwareLevel;
};

AcademyStats academyStats = {0, 1, 1, 1};

QuizQuestion* currentExam[10];
uint8_t userAnswers[10];
bool examInProgress = false;
uint8_t currentQuestionIndex = 0;
uint8_t selectedAnswer = 0;
uint8_t examSize = 10;
const char* currentCategory = "";
const char* currentDifficulty = "";
QuizQuestion* currentQuestionSet = nullptr;
uint8_t questionSetSize = 0;

// Learning mode variables
bool learningMode = false;
uint8_t learningCardIndex = 0;
uint8_t learningTotalCards = 0;

// Academy state machine
enum AcademyState {
    STATE_MAIN_MENU,
    STATE_CATEGORY_MENU,
    STATE_DIFFICULTY_MENU,
    STATE_MODE_SELECT,
    STATE_EXAM,
    STATE_SHOW_RESULTS,
    STATE_REVIEW_MISTAKES,
    STATE_LEARNING_MODE,
    STATE_STATISTICS,
    STATE_EXIT      // ADD THIS LINE
};
AcademyState currentState = STATE_MAIN_MENU;

// Menu selections
uint8_t mainMenuSelected = 0;
uint8_t categorySelected = 0;
uint8_t difficultySelected = 0;
uint8_t modeSelected = 0;
uint8_t resultsMenuSelected = 0;

// Results data
uint8_t lastCorrectCount = 0;
uint8_t lastPercentage = 0;
char lastGrade = 'F';

// Button debounce for academy
unsigned long lastAcademyButtonTime = 0;


// Function declarations
void academyApp();
void academyUpdate();
void academyDrawMainMenu();
void academyDrawCategoryMenu();
void academyDrawDifficultyMenu();
void academyDrawModeSelect();
void academyDrawStatistics();
void academyGenerateExam();
void academyDrawQuestion();
void academyHandleExamInput();
void academyShowResultsScreen();
void academyDrawResults();
void academyReviewMistakesScreen();
void academyDrawLearningCard();
void academyHandleLearningInput();
void academyAwardXP(uint8_t correctCount);
void academySaveStats();
void academyLoadStats();
void academySetQuestionSet();

// ==================== IMPLEMENTATIONS ====================

void academyLoadStats() {
    academyPrefs.begin("academy", false);
    academyStats.totalXP = academyPrefs.getUInt("totalXP", 0);
    academyStats.rfLevel = academyPrefs.getUShort("rfLevel", 1);
    academyStats.embeddedLevel = academyPrefs.getUShort("embeddedLevel", 1);
    academyStats.hardwareLevel = academyPrefs.getUShort("hardwareLevel", 1);
    academyPrefs.end();
}

void academySaveStats() {
    academyPrefs.begin("academy", false);
    academyPrefs.putUInt("totalXP", academyStats.totalXP);
    academyPrefs.putUShort("rfLevel", academyStats.rfLevel);
    academyPrefs.putUShort("embeddedLevel", academyStats.embeddedLevel);
    academyPrefs.putUShort("hardwareLevel", academyStats.hardwareLevel);
    academyPrefs.end();
}

void academyAwardXP(uint8_t correctCount) {
    uint16_t baseXP = 10;
    uint16_t difficultyMultiplier = 1;
    
    if (strcmp(currentDifficulty, "Medium") == 0) difficultyMultiplier = 2;
    else if (strcmp(currentDifficulty, "Hard") == 0) difficultyMultiplier = 3;
    
    uint16_t xpEarned = baseXP * difficultyMultiplier * correctCount;
    academyStats.totalXP += xpEarned;
    
    if (strcmp(currentCategory, "RF") == 0) {
        academyStats.rfLevel = 1 + (academyStats.totalXP / 1000);
    } else if (strcmp(currentCategory, "Embedded Systems") == 0) {
        academyStats.embeddedLevel = 1 + (academyStats.totalXP / 1000);
    } else if (strcmp(currentCategory, "Hardware Pentesting") == 0) {
        academyStats.hardwareLevel = 1 + (academyStats.totalXP / 1000);
    }
    
    academySaveStats();
}

void academySetQuestionSet() {
    if (strcmp(currentCategory, "RF") == 0) {
        if (strcmp(currentDifficulty, "Easy") == 0) {
            currentQuestionSet = (QuizQuestion*)easyRFQuestions;
            questionSetSize = 20;
        } else if (strcmp(currentDifficulty, "Medium") == 0) {
            currentQuestionSet = (QuizQuestion*)mediumRFQuestions;
            questionSetSize = 20;
        } else {
            currentQuestionSet = (QuizQuestion*)hardRFQuestions;
            questionSetSize = 20;
        }
    } else if (strcmp(currentCategory, "Embedded Systems") == 0) {
        if (strcmp(currentDifficulty, "Easy") == 0) {
            currentQuestionSet = (QuizQuestion*)easyEmbeddedQuestions;
            questionSetSize = 20;
        } else if (strcmp(currentDifficulty, "Medium") == 0) {
            currentQuestionSet = (QuizQuestion*)mediumEmbeddedQuestions;
            questionSetSize = 20;
        } else {
            currentQuestionSet = (QuizQuestion*)hardEmbeddedQuestions;
            questionSetSize = 20;
        }
    } else {
        if (strcmp(currentDifficulty, "Easy") == 0) {
            currentQuestionSet = (QuizQuestion*)easyHardwareQuestions;
            questionSetSize = 20;
        } else if (strcmp(currentDifficulty, "Medium") == 0) {
            currentQuestionSet = (QuizQuestion*)mediumHardwareQuestions;
            questionSetSize = 20;
        } else {
            currentQuestionSet = (QuizQuestion*)hardHardwareQuestions;
            questionSetSize = 20;
        }
    }
}

void academyGenerateExam() {
    bool used[20] = {false};
    uint8_t selected = 0;
    
    while (selected < examSize) {
        uint8_t randomIndex = random(questionSetSize);
        if (!used[randomIndex]) {
            used[randomIndex] = true;
            currentExam[selected] = &currentQuestionSet[randomIndex];
            selected++;
        }
    }
    
    currentQuestionIndex = 0;
    selectedAnswer = 0;
    memset(userAnswers, 0xFF, sizeof(userAnswers));
}


 
 
  ////ممكن تعدل لو عايز الخط يبقي مختلف او حجمه مختلف


void academyDrawQuestion() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_5x7_tf);
    
    char header[32];
    snprintf(header, sizeof(header), "Q%d/%d", currentQuestionIndex + 1, examSize);
    u8g2.drawStr(0, 8, header);
    
    const char* question = currentExam[currentQuestionIndex]->question;
    char line1[25], line2[25];
    
    strncpy(line1, question, 24);
    line1[24] = '\0';
    
    if (strlen(question) > 24) {
        strncpy(line2, question + 24, 24);
        line2[24] = '\0';
        u8g2.drawStr(0, 16, line1);
        u8g2.drawStr(0, 24, line2);
    } else {
        u8g2.drawStr(0, 16, line1);
    }
    
    // Show all 4 answers with very tight spacing
    for (int i = 0; i < 4; i++) {
        u8g2.setCursor(8, 36 + (i * 7));  // 7 pixels between answers
        
        if (i == selectedAnswer) {
            u8g2.print(">");
        } else {
            u8g2.print(" ");
        }
        
        char answerText[25];
        strncpy(answerText, currentExam[currentQuestionIndex]->answers[i], 24);
        answerText[24] = '\0';
        u8g2.print(answerText);
    }
    
    u8g2.sendBuffer();
}
void academyHandleExamInput() {
    if (millis() - lastAcademyButtonTime < 150) return;
    
    if (digitalRead(BTN_UP) == LOW) {
        lastAcademyButtonTime = millis();
        if (selectedAnswer > 0) selectedAnswer--;
        academyDrawQuestion();
    }
    
    if (digitalRead(BTN_DOWN) == LOW) {
        lastAcademyButtonTime = millis();
        if (selectedAnswer < 3) selectedAnswer++;
        academyDrawQuestion();
    }
    
    if (digitalRead(BTN_SELECT) == LOW) {
        lastAcademyButtonTime = millis();
        userAnswers[currentQuestionIndex] = selectedAnswer;
        
        if (currentQuestionIndex < examSize - 1) {
            currentQuestionIndex++;
            selectedAnswer = 0;
            academyDrawQuestion();
        } else {
            // Calculate results
            lastCorrectCount = 0;
            for (int i = 0; i < examSize; i++) {
                if (userAnswers[i] == currentExam[i]->correct) {
                    lastCorrectCount++;
                }
            }
            lastPercentage = (lastCorrectCount * 100) / examSize;
            if (lastPercentage >= 90) lastGrade = 'A';
            else if (lastPercentage >= 80) lastGrade = 'B';
            else if (lastPercentage >= 70) lastGrade = 'C';
            else if (lastPercentage >= 60) lastGrade = 'D';
            else lastGrade = 'F';
            
            academyAwardXP(lastCorrectCount);
            currentState = STATE_SHOW_RESULTS;
            resultsMenuSelected = 0;
        }
    }
    
    if (digitalRead(BTN_BACK) == LOW) {
        lastAcademyButtonTime = millis();
        currentState = STATE_MAIN_MENU;
        mainMenuSelected = 0;
    }
}

void academyDrawResults() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    
    char line1[32];
    snprintf(line1, sizeof(line1), "Score: %d/%d", lastCorrectCount, examSize);
    u8g2.drawStr(0, 10, line1);
    
    char line2[32];
    snprintf(line2, sizeof(line2), "Percent: %d%%", lastPercentage);
    u8g2.drawStr(0, 22, line2);
    
    char line3[32];
    snprintf(line3, sizeof(line3), "Grade: %c", lastGrade);
    u8g2.drawStr(0, 34, line3);
    
    const char* options[] = {"Review Mistakes", "Exit"};
    for (int i = 0; i < 2; i++) {
        u8g2.setCursor(10, 50 + (i * 8));
        if (i == resultsMenuSelected) u8g2.print(">");
        else u8g2.print(" ");
        u8g2.print(options[i]);
    }
    
    u8g2.sendBuffer();
}

void academyShowResultsScreen() {
    if (millis() - lastAcademyButtonTime < 150) return;
    
    if (digitalRead(BTN_UP) == LOW) {
        lastAcademyButtonTime = millis();
        if (resultsMenuSelected > 0) resultsMenuSelected--;
        academyDrawResults();
    }
    
    if (digitalRead(BTN_DOWN) == LOW) {
        lastAcademyButtonTime = millis();
        if (resultsMenuSelected < 1) resultsMenuSelected++;
        academyDrawResults();
    }
    
    if (digitalRead(BTN_SELECT) == LOW) {
        lastAcademyButtonTime = millis();
        if (resultsMenuSelected == 0) {
            currentState = STATE_REVIEW_MISTAKES;
        } else {
            currentState = STATE_MAIN_MENU;
            mainMenuSelected = 0;
        }
    }
    
    if (digitalRead(BTN_BACK) == LOW) {
        lastAcademyButtonTime = millis();
        currentState = STATE_MAIN_MENU;
        mainMenuSelected = 0;
    }
}

void academyReviewMistakesScreen() {
    static uint8_t mistakeIndices[10];
    static uint8_t mistakeCount = 0;
    static uint8_t currentMistake = 0;
    static bool showingExplanation = false;
    static unsigned long screenStartTime = 0;
    
    if (mistakeCount == 0) {
        // Calculate mistakes once
        mistakeCount = 0;
        for (int i = 0; i < examSize; i++) {
            if (userAnswers[i] != currentExam[i]->correct) {
                mistakeIndices[mistakeCount++] = i;
            }
        }
        currentMistake = 0;
        showingExplanation = false;
    }
    
    if (mistakeCount == 0) {
        // No mistakes
        if (millis() - lastAcademyButtonTime > 2000) {
            currentState = STATE_SHOW_RESULTS;
            mistakeCount = 0;
        }
        return;
    }
    
    if (!showingExplanation) {
        // Show mistake
        uint8_t qIndex = mistakeIndices[currentMistake];
        QuizQuestion* q = currentExam[qIndex];
        
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        
        char header[32];
        snprintf(header, sizeof(header), "Mistake %d/%d", currentMistake + 1, mistakeCount);
        u8g2.drawStr(0, 10, header);
        
        char qline1[21], qline2[21];
        strncpy(qline1, q->question, 20);
        qline1[20] = '\0';
        u8g2.drawStr(0, 22, qline1);
        
        if (strlen(q->question) > 20) {
            strncpy(qline2, q->question + 20, 20);
            qline2[20] = '\0';
            u8g2.drawStr(0, 34, qline2);
        }
        
        u8g2.setCursor(0, 46);
        u8g2.print("You: ");
        u8g2.print(q->answers[userAnswers[qIndex]]);
        
        u8g2.setCursor(0, 54);
        u8g2.print("Correct: ");
        u8g2.print(q->answers[q->correct]);
        
        u8g2.drawStr(0, 62, "SELECT for explanation");
        u8g2.sendBuffer();
        
        if (digitalRead(BTN_SELECT) == LOW && millis() - lastAcademyButtonTime > 150) {
            lastAcademyButtonTime = millis();
            showingExplanation = true;
        }
        if (digitalRead(BTN_BACK) == LOW && millis() - lastAcademyButtonTime > 150) {
            lastAcademyButtonTime = millis();
            currentState = STATE_SHOW_RESULTS;
            mistakeCount = 0;
        }
    } else {
        // Show explanation
        uint8_t qIndex = mistakeIndices[currentMistake];
        QuizQuestion* q = currentExam[qIndex];
        
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 10, "Explanation:");
        
        char expline1[21], expline2[21];
        strncpy(expline1, q->explanation, 20);
        expline1[20] = '\0';
        u8g2.drawStr(0, 22, expline1);
        
        if (strlen(q->explanation) > 20) {
            strncpy(expline2, q->explanation + 20, 20);
            expline2[20] = '\0';
            u8g2.drawStr(0, 34, expline2);
        }
        
        u8g2.drawStr(0, 58, "SELECT for next");
        u8g2.sendBuffer();
        
        if (digitalRead(BTN_SELECT) == LOW && millis() - lastAcademyButtonTime > 150) {
            lastAcademyButtonTime = millis();
            if (currentMistake < mistakeCount - 1) {
                currentMistake++;
                showingExplanation = false;
            } else {
                currentState = STATE_SHOW_RESULTS;
                mistakeCount = 0;
                showingExplanation = false;
            }
        }
        if (digitalRead(BTN_BACK) == LOW && millis() - lastAcademyButtonTime > 150) {
            lastAcademyButtonTime = millis();
            currentState = STATE_SHOW_RESULTS;
            mistakeCount = 0;
            showingExplanation = false;
        }
    }
}

void academyDrawMainMenu() {
    const char* menuItems[] = {"Start Quiz", "Statistics"};
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "HIZMOS Academy");
    
    for (int i = 0; i < 2; i++) {
        u8g2.setCursor(10, 25 + (i * 12));
        if (i == mainMenuSelected) u8g2.print(">");
        else u8g2.print(" ");
        u8g2.print(menuItems[i]);
    }
    
    char xpLine[32];
    snprintf(xpLine, sizeof(xpLine), "Total XP: %lu", academyStats.totalXP);
    u8g2.drawStr(0, 55, xpLine);
    
    u8g2.sendBuffer();
}

void academyDrawCategoryMenu() {
    const char* categories[] = {"RF", "Embedded Systems", "Hardware Pentesting"};
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Select Category");
    
    for (int i = 0; i < 3; i++) {
        u8g2.setCursor(10, 25 + (i * 12));
        if (i == categorySelected) u8g2.print(">");
        else u8g2.print(" ");
        u8g2.print(categories[i]);
    }
    
    u8g2.drawStr(0, 60, "SELECT: Choose | BACK");
    u8g2.sendBuffer();
}

void academyDrawDifficultyMenu() {
    const char* difficulties[] = {"Easy", "Medium", "Hard"};
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Select Difficulty");
    
    for (int i = 0; i < 3; i++) {
        u8g2.setCursor(10, 25 + (i * 12));
        if (i == difficultySelected) u8g2.print(">");
        else u8g2.print(" ");
        u8g2.print(difficulties[i]);
    }
    
    u8g2.drawStr(0, 60, "SELECT: Next | BACK");
    u8g2.sendBuffer();
}

void academyDrawModeSelect() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 20, "1. Start Exam");
    u8g2.drawStr(0, 35, "2. Learning Mode");
    if (modeSelected == 0) u8g2.drawStr(100, 20, "<");
    if (modeSelected == 1) u8g2.drawStr(100, 35, "<");
    u8g2.sendBuffer();
}

void academyDrawStatistics() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    char line1[32];
    snprintf(line1, sizeof(line1), "Total XP: %lu", academyStats.totalXP);
    u8g2.drawStr(0, 10, line1);
    
    char line2[32];
    snprintf(line2, sizeof(line2), "RF Level: %d", academyStats.rfLevel);
    u8g2.drawStr(0, 22, line2);
    
    char line3[32];
    snprintf(line3, sizeof(line3), "Embedded Level: %d", academyStats.embeddedLevel);
    u8g2.drawStr(0, 34, line3);
    
    char line4[32];
    snprintf(line4, sizeof(line4), "HW Level: %d", academyStats.hardwareLevel);
    u8g2.drawStr(0, 46, line4);
    
    u8g2.drawStr(0, 58, "Press BACK");
    u8g2.sendBuffer();
}

void academyDrawLearningCard() {
    if (learningTotalCards == 0) return;
    
    QuizQuestion* card = &currentQuestionSet[learningCardIndex];
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    
    char header[32];
    snprintf(header, sizeof(header), "Card %d/%d", learningCardIndex + 1, learningTotalCards);
    u8g2.drawStr(0, 10, header);
    
    char categoryLine[32];
    snprintf(categoryLine, sizeof(categoryLine), "%s - %s", currentCategory, currentDifficulty);
    u8g2.drawStr(0, 20, categoryLine);
    
    char qline1[21], qline2[21];
    strncpy(qline1, card->question, 20);
    qline1[20] = '\0';
    u8g2.drawStr(0, 32, qline1);
    
    if (strlen(card->question) > 20) {
        strncpy(qline2, card->question + 20, 20);
        qline2[20] = '\0';
        u8g2.drawStr(0, 44, qline2);
    }
    
    u8g2.drawStr(0, 54, "Answer: ");
    u8g2.drawStr(48, 54, card->answers[card->correct]);
    
    u8g2.drawStr(0, 63, "UP/DOWN: Nav | BACK");
    
    u8g2.sendBuffer();
}

void academyHandleLearningInput() {
    if (millis() - lastAcademyButtonTime < 150) return;
    
    if (digitalRead(BTN_UP) == LOW) {
        lastAcademyButtonTime = millis();
        if (learningCardIndex > 0) {
            learningCardIndex--;
            academyDrawLearningCard();
        }
    }
    
    if (digitalRead(BTN_DOWN) == LOW) {
        lastAcademyButtonTime = millis();
        if (learningCardIndex < learningTotalCards - 1) {
            learningCardIndex++;
            academyDrawLearningCard();
        }
    }
    
    if (digitalRead(BTN_SELECT) == LOW) {
        lastAcademyButtonTime = millis();
        // Show explanation popup
        QuizQuestion* card = &currentQuestionSet[learningCardIndex];
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.drawStr(0, 10, "Explanation:");
        
        char expline1[21], expline2[21];
        strncpy(expline1, card->explanation, 20);
        expline1[20] = '\0';
        u8g2.drawStr(0, 22, expline1);
        
        if (strlen(card->explanation) > 20) {
            strncpy(expline2, card->explanation + 20, 20);
            expline2[20] = '\0';
            u8g2.drawStr(0, 34, expline2);
        }
        
        u8g2.drawStr(0, 58, "Press any key");
        u8g2.sendBuffer();
        
        // Wait for any button press
        while (digitalRead(BTN_SELECT) == HIGH && 
               digitalRead(BTN_UP) == HIGH && 
               digitalRead(BTN_DOWN) == HIGH && 
               digitalRead(BTN_BACK) == HIGH) {
            delay(10);
        }
        delay(200);
        academyDrawLearningCard();
    }
    
    if (digitalRead(BTN_BACK) == LOW) {
        lastAcademyButtonTime = millis();
        currentState = STATE_DIFFICULTY_MENU;
        difficultySelected = 0;
    }
}

void academyUpdate() {
    switch (currentState) {
        case STATE_MAIN_MENU:
            academyDrawMainMenu();
            if (millis() - lastAcademyButtonTime >= 150) {
                if (digitalRead(BTN_UP) == LOW) {
                    lastAcademyButtonTime = millis();
                    if (mainMenuSelected > 0) mainMenuSelected--;
                    academyDrawMainMenu();
                }
                if (digitalRead(BTN_DOWN) == LOW) {
                    lastAcademyButtonTime = millis();
                    if (mainMenuSelected < 1) mainMenuSelected++;
                    academyDrawMainMenu();
                }
                if (digitalRead(BTN_SELECT) == LOW) {
                    lastAcademyButtonTime = millis();
                    if (mainMenuSelected == 0) {
                        currentState = STATE_CATEGORY_MENU;
                        categorySelected = 0;
                    } else {
                        currentState = STATE_STATISTICS;
                    }
                }
                if (digitalRead(BTN_BACK) == LOW) {
                    lastAcademyButtonTime = millis();
                    // Exit academy - will be handled by the calling function
                }
            }
            break;
            
        case STATE_CATEGORY_MENU:
            academyDrawCategoryMenu();
            if (millis() - lastAcademyButtonTime >= 150) {
                if (digitalRead(BTN_UP) == LOW) {
                    lastAcademyButtonTime = millis();
                    if (categorySelected > 0) categorySelected--;
                    academyDrawCategoryMenu();
                }
                if (digitalRead(BTN_DOWN) == LOW) {
                    lastAcademyButtonTime = millis();
                    if (categorySelected < 2) categorySelected++;
                    academyDrawCategoryMenu();
                }
                if (digitalRead(BTN_SELECT) == LOW) {
                    lastAcademyButtonTime = millis();
                    const char* categories[] = {"RF", "Embedded Systems", "Hardware Pentesting"};
                    currentCategory = categories[categorySelected];
                    currentState = STATE_DIFFICULTY_MENU;
                    difficultySelected = 0;
                }
                if (digitalRead(BTN_BACK) == LOW) {
                    lastAcademyButtonTime = millis();
                    currentState = STATE_MAIN_MENU;
                    mainMenuSelected = 0;
                }
            }
            break;
            
        case STATE_DIFFICULTY_MENU:
            academyDrawDifficultyMenu();
            if (millis() - lastAcademyButtonTime >= 150) {
                if (digitalRead(BTN_UP) == LOW) {
                    lastAcademyButtonTime = millis();
                    if (difficultySelected > 0) difficultySelected--;
                    academyDrawDifficultyMenu();
                }
                if (digitalRead(BTN_DOWN) == LOW) {
                    lastAcademyButtonTime = millis();
                    if (difficultySelected < 2) difficultySelected++;
                    academyDrawDifficultyMenu();
                }
                if (digitalRead(BTN_SELECT) == LOW) {
                    lastAcademyButtonTime = millis();
                    const char* difficulties[] = {"Easy", "Medium", "Hard"};
                    currentDifficulty = difficulties[difficultySelected];
                    academySetQuestionSet();
                    currentState = STATE_MODE_SELECT;
                    modeSelected = 0;
                }
                if (digitalRead(BTN_BACK) == LOW) {
                    lastAcademyButtonTime = millis();
                    currentState = STATE_CATEGORY_MENU;
                    categorySelected = 0;
                }
            }
            break;
            
        case STATE_MODE_SELECT:
            academyDrawModeSelect();
            if (millis() - lastAcademyButtonTime >= 150) {
                if (digitalRead(BTN_UP) == LOW) {
                    lastAcademyButtonTime = millis();
                    if (modeSelected > 0) modeSelected--;
                    academyDrawModeSelect();
                }
                if (digitalRead(BTN_DOWN) == LOW) {
                    lastAcademyButtonTime = millis();
                    if (modeSelected < 1) modeSelected++;
                    academyDrawModeSelect();
                }
                if (digitalRead(BTN_SELECT) == LOW) {
                    lastAcademyButtonTime = millis();
                    if (modeSelected == 0) {
                        academyGenerateExam();
                        currentState = STATE_EXAM;
                        academyDrawQuestion();
                    } else {
                        learningTotalCards = questionSetSize;
                        learningCardIndex = 0;
                        currentState = STATE_LEARNING_MODE;
                        academyDrawLearningCard();
                    }
                }
                if (digitalRead(BTN_BACK) == LOW) {
                    lastAcademyButtonTime = millis();
                    currentState = STATE_DIFFICULTY_MENU;
                    difficultySelected = 0;
                }
            }
            break;
            
        case STATE_EXAM:
            academyHandleExamInput();
            break;
            
        case STATE_SHOW_RESULTS:
            academyDrawResults();
            academyShowResultsScreen();
            break;
            
        case STATE_REVIEW_MISTAKES:
            academyReviewMistakesScreen();
            break;
            
        case STATE_LEARNING_MODE:
            academyHandleLearningInput();
            break;
            
        case STATE_STATISTICS:
            academyDrawStatistics();
            if (digitalRead(BTN_BACK) == LOW && millis() - lastAcademyButtonTime >= 150) {
                lastAcademyButtonTime = millis();
                currentState = STATE_MAIN_MENU;
                mainMenuSelected = 0;
            }
            break;
    }
}

// Main Academy App - This runs continuously and returns when user exits
void academyApp() {
    academyLoadStats();
    randomSeed(esp_random());
    
    // Reset state
    currentState = STATE_MAIN_MENU;
    mainMenuSelected = 0;
    examInProgress = false;
    lastAcademyButtonTime = 0;
    
    // Run the academy until user exits (presses BACK in main menu)
    while (currentState != STATE_EXIT) {
        academyUpdate();
        delay(10);
        
        // Check for exit from main menu
        if (currentState == STATE_MAIN_MENU) {
            if (digitalRead(BTN_BACK) == LOW && millis() - lastAcademyButtonTime >= 150) {
                lastAcademyButtonTime = millis();
                currentState = STATE_EXIT;
            }
        }
    }
}






















// ─────────────────────────────
// App structure
// ─────────────────────────────
struct AppEntry
{
    const char* name;
    const uint8_t* icon;      // 32x32 XBM
    void (*appFunc)();
};

// ─────────────────────────────
// APPS (MATCH icons.h EXACT NAMES)
// ─────────────────────────────
AppEntry apps[] =
{
    {"Calculator",  nullptr,      claculaterloop},
    {"Timer",       timer,         pomdorotimerloop},
    {"Wave",        waveicon,      wavecreator},
    {"Snake Game",        nullptr,      snakeLoop},
    {"SD Flasher",  sdflash,       flasherloop},
    {"Password",    passwordicon,  loading},
    {"QR Code",     qrcodeicon,    handleQRCodeApp},
    {"Flashlight",  flashlight,    flashlightApp},
    {"Academy",     nullptr,   academyApp},
    {"freq finder", freqsicon, frequencyFinderApp},
    {"Safe Mode",   epd_bitmap_Screenshot_2026_06_13_124244__2_, nullptr},
    {"Space fighters",flightgame, spacegame}
};

const int appCount = sizeof(apps) / sizeof(apps[0]);


//////// MENU

void handleappsmenu()
{
    static int selectedItem = 0;
    static unsigned long lastInputTime = 0;
    static bool lastLeftState = HIGH;
    static bool lastRightState = HIGH;
    static bool lastSelectState = HIGH;

    unsigned long now = millis();

    // ─────────────────────────────
    // INPUT (EDGE DETECTION)
    // ─────────────────────────────
    bool leftState = digitalRead(BTN_LEFT);
    bool rightState = digitalRead(BTN_RIGHT);
    bool selectState = digitalRead(BTN_SELECT);

    if (now - lastInputTime > 120) // faster + smoother response
    {
        // LEFT (only on press edge)
        if (leftState == LOW && lastLeftState == HIGH)
        {
            selectedItem = (selectedItem - 1 + appCount) % appCount;
            lastInputTime = now;
        }

        // RIGHT (only on press edge)
        if (rightState == LOW && lastRightState == HIGH)
        {
            selectedItem = (selectedItem + 1) % appCount;
            lastInputTime = now;
        }

        // SELECT
        if (selectState == LOW && lastSelectState == HIGH)
        {
            if (apps[selectedItem].appFunc != nullptr)
            {
                // optional small debounce BEFORE switching context
                delay(80);

                runLoop(apps[selectedItem].appFunc);
            }

            lastInputTime = now;
        }
    }

    lastLeftState = leftState;
    lastRightState = rightState;
    lastSelectState = selectState;

    // ─────────────────────────────
    // PREV / NEXT CALC
    // ─────────────────────────────
    int prev = (selectedItem - 1 + appCount) % appCount;
    int next = (selectedItem + 1) % appCount;

    // ─────────────────────────────
    // DRAW UI
    // ─────────────────────────────
    u8g2.clearBuffer();

    // Header
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.drawStr(34, 8, "APPS");   //////the up text

    // Frame
    u8g2.drawRFrame(24, 10, 80, 52, 4);

    // ─────────────────────────────
    // MAIN ICON
    // ─────────────────────────────
    if (apps[selectedItem].icon != nullptr)
    {
        u8g2.drawXBMP(48, 14, 32, 32, apps[selectedItem].icon);
    }

    // ─────────────────────────────
    // PREV ICON
    // ─────────────────────────────
    if (apps[prev].icon != nullptr)
    {
        u8g2.drawXBMP(2, 20, 16, 16, apps[prev].icon);
    }

    // ─────────────────────────────
    // NEXT ICON
    // ─────────────────────────────
    if (apps[next].icon != nullptr)
    {
        u8g2.drawXBMP(110, 20, 16, 16, apps[next].icon);
    }

    // ─────────────────────────────
    // NAME
    // ─────────────────────────────
    u8g2.setFont(u8g2_font_6x12_tf);

    int textWidth = strlen(apps[selectedItem].name) * 6;
    int textX = (128 - textWidth) / 2;

    u8g2.drawStr(textX, 57, apps[selectedItem].name);  /////app names

    // ─────────────────────────────
    // ARROWS
    // ─────────────────────────────
    u8g2.drawTriangle(2, 32, 10, 28, 10, 36); ///////اسهم شمال
    u8g2.drawTriangle(126, 32, 118, 28, 118, 36); ////سهم يمين

    // ─────────────────────────────
    // PAGE INDICATOR بتعرف مكان المينيو 
    // ─────────────────────────────
    char page[12];
    sprintf(page, "%d/%d", selectedItem + 1, appCount);

    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.drawStr(60, 8, page);   /////مكان البتاع دا مش فاكر

    u8g2.sendBuffer();
}









// flashlight app

// ================= FLASHLIGHT APP =================
// ================= FLASHLIGHT MENU =================

enum FlashlightMode {
  FLASH_WHITE,
  FLASH_WARM,
  FLASH_COOL,
  FLASH_CUSTOM,
  FLASH_RAINBOW,
  FLASH_BREATH,
  FLASH_STROBE,
  FLASH_POLICE,
  FLASH_SOS
};

FlashlightMode flashMode = FLASH_WHITE;

const char* flashlightMenu[] = {
  "White",
  "Warm White",
  "Cool White",
  "Custom RGB",
  "Rainbow",
  "Breathing",
  "Strobe",
  "Police",
  "SOS",
  "Brightness",
  "Exit"
};

const int flashlightMenuSize =
sizeof(flashlightMenu)/sizeof(flashlightMenu[0]);

int flashlightIndex = 0;

uint8_t flashBrightness = 150;

uint8_t customR = 255;
uint8_t customG = 0;
uint8_t customB = 0;

// ================= HELPERS =================

void setNeo(uint8_t r,uint8_t g,uint8_t b,uint8_t bright)
{
  pixel.setBrightness(bright);
  pixel.setPixelColor(0,pixel.Color(r,g,b));
  pixel.show();
}

uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;

  if(WheelPos < 85)
    return pixel.Color(255 - WheelPos * 3,0,WheelPos * 3);

  if(WheelPos < 170)
  {
    WheelPos -= 85;
    return pixel.Color(0,WheelPos * 3,255 - WheelPos * 3);
  }

  WheelPos -= 170;
  return pixel.Color(WheelPos * 3,255 - WheelPos * 3,0);
}

// ================= RGB EDITOR =================

void rgbEditor()
{
  int channel = 0;

  while(true)
  {
    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_6x12_tf);

    u8g2.drawStr(0,10,"CUSTOM RGB");

    char buf[32];

    sprintf(buf,"R:%3d",customR);
    u8g2.drawStr(15,25,buf);

    sprintf(buf,"G:%3d",customG);
    u8g2.drawStr(15,40,buf);

    sprintf(buf,"B:%3d",customB);
    u8g2.drawStr(15,55,buf);

    if(channel==0) u8g2.drawStr(0,25,">");
    if(channel==1) u8g2.drawStr(0,40,">");
    if(channel==2) u8g2.drawStr(0,55,">");

    u8g2.sendBuffer();

    setNeo(customR,customG,customB,flashBrightness);

    if(digitalRead(BTN_UP)==LOW)
    {
      channel--;
      if(channel<0) channel=2;
      delay(150);
    }

    if(digitalRead(BTN_DOWN)==LOW)
    {
      channel++;
      if(channel>2) channel=0;
      delay(150);
    }

    if(digitalRead(BTN_LEFT)==LOW)
    {
      if(channel==0) customR=max(0,(int)customR-5);
      if(channel==1) customG=max(0,(int)customG-5);
      if(channel==2) customB=max(0,(int)customB-5);

      delay(50);
    }

    if(digitalRead(BTN_RIGHT)==LOW)
    {
      if(channel==0) customR=min(255,(int)customR+5);
      if(channel==1) customG=min(255,(int)customG+5);
      if(channel==2) customB=min(255,(int)customB+5);

      delay(50);
    }

    if(digitalRead(BTN_BACK)==LOW)
    {
      while(digitalRead(BTN_BACK)==LOW);
      return;
    }
  }
}

// ================= BRIGHTNESS MENU =================

void brightnessMenu()
{
  while(true)
  {
    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_6x12_tf);

    u8g2.drawStr(0,15,"BRIGHTNESS");

    char buf[20];
    sprintf(buf,"%d",flashBrightness);

    u8g2.drawStr(45,40,buf);

    u8g2.drawFrame(10,50,108,8);
    u8g2.drawBox(10,50,map(flashBrightness,0,255,0,108),8);

    u8g2.sendBuffer();

    if(digitalRead(BTN_LEFT)==LOW)
    {
      flashBrightness=max(0,(int)flashBrightness-5);
      delay(50);
    }

    if(digitalRead(BTN_RIGHT)==LOW)
    {
      flashBrightness=min(255,(int)flashBrightness+5);
      delay(50);
    }

    if(digitalRead(BTN_BACK)==LOW)
    {
      while(digitalRead(BTN_BACK)==LOW);
      return;
    }
  }
}

// ================= MAIN APP =================

void flashlightApp()
{
  uint16_t rainbowPos = 0;

  while(true)
  {
    switch(flashMode)
    {
      case FLASH_WHITE:
        setNeo(255,255,255,flashBrightness);
        break;

      case FLASH_WARM:
        setNeo(255,180,80,flashBrightness);
        break;

      case FLASH_COOL:
        setNeo(180,220,255,flashBrightness);
        break;

      case FLASH_CUSTOM:
        setNeo(customR,customG,customB,flashBrightness);
        break;

      case FLASH_RAINBOW:
        pixel.setBrightness(flashBrightness);
        pixel.setPixelColor(0,Wheel(rainbowPos++));
        pixel.show();
        break;

      case FLASH_BREATH:
      {
        static int b = 0;
        static int dir = 5;

        b += dir;

        if(b > flashBrightness)
        {
          b = flashBrightness;
          dir = -5;
        }

        if(b < 10)
        {
          b = 10;
          dir = 5;
        }

        setNeo(255,255,255,b);
      }
      break;

      case FLASH_STROBE:
        setNeo(255,255,255,flashBrightness);
        delay(50);
        setNeo(0,0,0,0);
        delay(50);
        break;

      case FLASH_POLICE:
        setNeo(255,0,0,flashBrightness);
        delay(100);
        setNeo(0,0,255,flashBrightness);
        delay(100);
        break;

      case FLASH_SOS:
        setNeo(255,255,255,flashBrightness);
        delay(100);
        setNeo(0,0,0,0);
        delay(100);
        break;
    }

    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_6x12_tf);

    u8g2.drawStr(0,10,"FLASHLIGHT");

    for(int i=0;i<4;i++)
    {
      int item=i;

      if(item>=flashlightMenuSize)
        break;

      if(item==flashlightIndex)
        u8g2.drawStr(0,25+i*10,">");

      u8g2.drawStr(10,25+i*10,flashlightMenu[item]);
    }

    u8g2.sendBuffer();

    if(digitalRead(BTN_UP)==LOW)
    {
      flashlightIndex--;
      if(flashlightIndex<0)
        flashlightIndex=flashlightMenuSize-1;

      delay(150);
    }

    if(digitalRead(BTN_DOWN)==LOW)
    {
      flashlightIndex++;
      if(flashlightIndex>=flashlightMenuSize)
        flashlightIndex=0;

      delay(150);
    }

    if(digitalRead(BTN_SELECT)==LOW)
    {
      switch(flashlightIndex)
      {
        case 0: flashMode=FLASH_WHITE; break;
        case 1: flashMode=FLASH_WARM; break;
        case 2: flashMode=FLASH_COOL; break;
        case 3:
          flashMode=FLASH_CUSTOM;
          rgbEditor();
          break;
        case 4: flashMode=FLASH_RAINBOW; break;
        case 5: flashMode=FLASH_BREATH; break;
        case 6: flashMode=FLASH_STROBE; break;
        case 7: flashMode=FLASH_POLICE; break;
        case 8: flashMode=FLASH_SOS; break;
        case 9: brightnessMenu(); break;
        case 10:
          setNeo(0,0,0,0);
          return;
      }

      while(digitalRead(BTN_SELECT)==LOW);
    }

    if(digitalRead(BTN_BACK)==LOW)
    {
      setNeo(0,0,0,0);

      while(digitalRead(BTN_BACK)==LOW);

      return;
    }

    delay(10);
  }
}



// ===== STATE =====
static char inputBuffer[12] = "";
static int cursorRow = 0;
static int cursorCol = 0;
static int selected = -1;
static bool showResult = false;

// ===== KEYBOARD =====
const char kb[3][6] = {
  {'1','2','3','4','5','6'},
  {'7','8','9','0','<','->'},  // '>' = ENTER (we map ->)  // empty row (optional future tools)
};

const int kbRows = 2;
const int kbCols = 6;
// ===== HELPERS =====
void addChar(char c) {
  int len = strlen(inputBuffer);
  if (len < 11) {
    inputBuffer[len] = c;
    inputBuffer[len + 1] = '\0';
  }
}

void delChar() {
  int len = strlen(inputBuffer);
  if (len > 0) inputBuffer[len - 1] = '\0';
}

void frequencyFinderApp()
{
  static char inputBuffer[10] = "";
  static int index = 0;

  static int cursorRow = 0;
  static int cursorCol = 0;

  static int selected = -1;
  static bool showResult = false;

  // ================= RESULT SCREEN =================
  if (showResult)
  {
    u8g2.clearBuffer();

    if (selected != -1)
    {
      // BIG RANGE TITLE
      char range[32];
      sprintf(range, "%.1f - %.1f MHz",
              freqDB[selected].startMHz,
              freqDB[selected].endMHz);

      u8g2.setFont(u8g2_font_7x14B_tf);
      u8g2.drawStr(0, 14, range);

      // DETAILS
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawStr(0, 32, freqDB[selected].name);
      u8g2.drawStr(0, 48, freqDB[selected].usage);
      u8g2.drawStr(0, 64, freqDB[selected].note);
    }
    else
    {
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawStr(0, 35, "NO MATCH FOUND");
    }

    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.drawStr(0, 78, "BACK -> SEARCH");

    u8g2.sendBuffer();

    // BACK from RESULT → SEARCH ONLY
    if (digitalRead(BTN_BACK) == LOW)
    {
      showResult = false;
      selected = -1;
      delay(200);
    }

    return;
  }

  // ================= SEARCH SCREEN =================
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);

  u8g2.drawStr(0, 10, "FREQUENCY FINDER");

  u8g2.drawStr(0, 25, "MHz:");
  u8g2.drawStr(35, 25, inputBuffer);

  // ================= NAVIGATION =================
  if (digitalRead(BTN_UP) == LOW)
  {
    cursorRow = (cursorRow - 1 + kbRows) % kbRows;
    delay(150);
  }

  if (digitalRead(BTN_DOWN) == LOW)
  {
    cursorRow = (cursorRow + 1) % kbRows;
    delay(150);
  }

  if (digitalRead(BTN_LEFT) == LOW)
  {
    cursorCol = (cursorCol - 1 + kbCols) % kbCols;
    delay(150);
  }

  if (digitalRead(BTN_RIGHT) == LOW)
  {
    cursorCol = (cursorCol + 1) % kbCols;
    delay(150);
  }

  // ================= SELECT KEY =================
  if (digitalRead(BTN_SELECT) == LOW)
  {
    char key = kb[cursorRow][cursorCol];

    // BACKSPACE
    if (key == '<')
    {
      if (index > 0)
        inputBuffer[--index] = '\0';
    }

    // ENTER → SEARCH
    else if (key == '>')
    {
      float freq = atof(inputBuffer);

      selected = -1;

      for (int i = 0; i < freqDBSize; i++)
      {
        if (freq >= freqDB[i].startMHz && freq <= freqDB[i].endMHz)
        {
          selected = i;
          break;
        }
      }

      showResult = true;
    }

    // NUMBER INPUT
    else
    {
      if (index < 9)
      {
        inputBuffer[index++] = key;
        inputBuffer[index] = '\0';
      }
    }

    delay(200);
  }

  // ================= BACK (CLEAR INPUT) =================
  if (digitalRead(BTN_SELECT) == LOW)
  {
    frequencyFinderApp();
  }

  // ================= KEYBOARD =================
  int boxW = 18;
  int boxH = 12;
  int startX = 0;
  int startY = 35;

  for (int r = 0; r < kbRows; r++)
  {
    for (int c = 0; c < kbCols; c++)
    {
      int x = startX + c * (boxW + 1);
      int y = startY + r * (boxH + 1);

      char key = kb[r][c];

      if (r == cursorRow && c == cursorCol)
      {
        u8g2.drawBox(x, y, boxW, boxH);
        u8g2.setDrawColor(0);
        u8g2.setCursor(x + 4, y + 8);
        u8g2.print(key);
        u8g2.setDrawColor(1);
      }
      else
      {
        u8g2.drawFrame(x, y, boxW, boxH);
        u8g2.setCursor(x + 4, y + 8);
        u8g2.print(key);
      }
    }
  }

  u8g2.sendBuffer();
}



///qr code app






void handleQRCodeApp() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 12, "QR MODE ACTIVE");
  u8g2.drawStr(0, 28, "Send text via USB");
  u8g2.drawStr(0, 44, "Waiting...");
  u8g2.sendBuffer();

  String inputText = "";
  bool received = false;

  while (true) {

    // ===== EXIT BUTTON =====
    if (digitalRead(BTN_BACK) == LOW) {
      while (digitalRead(BTN_BACK) == LOW);
      break;
    }

    // ===== READ USB SERIAL =====
    while (Serial.available()) {
      char c = Serial.read();

      if (c == '\n') {
        received = true;
        break;
      } else {
        inputText += c;
      }
    }

    if (received && inputText.length() > 0) {
      drawQR_HIZMOS(inputText);
      inputText = "";
      received = false;
    }
  }
}



void drawQR_HIZMOS(String text) {

  u8g2.clearBuffer();

  // very simple fake QR fallback screen first
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 12, "QR GENERATED");

  u8g2.drawStr(0, 28, "DATA:");
  u8g2.drawStr(0, 42, text.c_str());

  u8g2.drawStr(0, 60, "NO QR LIB INSTALLED");

  u8g2.sendBuffer();
}