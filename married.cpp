#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <string>
#include <iomanip>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <locale>
#include <algorithm>
#include <functional> // Для std::min и std::max

#ifdef _WIN32
#include <windows.h>
#include "married.h"
#endif

// Human class models a person with a name and a desire
class Human {
public:
    explicit Human(std::string name)
        : name(std::move(name)), desire(generateDesire()) {}

    const std::string& getName() const { return name; }
    std::string getDesire() const {
        std::lock_guard<std::mutex> lock(desireMutex);
        return desire;
    }
    void setDesire(const std::string& newDesire) {
        std::lock_guard<std::mutex> lock(desireMutex);
        desire = newDesire;
    }

    // Generate a random desire from the list
    static std::string generateDesire() {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int> dis(0, static_cast<int>(desires.size()) - 1);
        return desires[dis(gen)];
    }

private:
    std::string name;
    std::string desire;
    mutable std::mutex desireMutex;
    static const std::vector<std::string> desires;
};

const std::vector<std::string> Human::desires = {
    "to become a programmer",
    "to help people",
    "to travel the world",
    "to make a difference in the world",
    "to find love",
    "to learn a new language",
    "to start a business",
    "to write a book",
    "to solve a world problem",
    "to become a doctor",
    "to become a lawyer",
    "to become a teacher",
    "to become a scientist",
    "to become an artist", 
    "to become an athlete",
    "to become a musician",
    "to become a writer", 
    "to become an entrepreneur",
    "to become a philanthropist",
    "to become a politician",
    "to become a world leader",
    "to become a unicorn",
    "to become a superhero",
    "to become a billionaire",
    "to become immortal", 
    "to become a god"
};

// Clear screen function for different OS
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif
}

// Set text color for status cell
void setColor(bool isMatch) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (isMatch)
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    else
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
    if (isMatch)
        std::cout << "\033[32m"; // green
    else
        std::cout << "\033[31m"; // red
#endif
}

void resetColor() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
    std::cout << "\033[0m";
#endif
}

struct DesireSync {
    std::mutex mtx;
    std::condition_variable cv;
    bool updated1 = false;
    bool updated2 = false;
    bool stop = false;
};

void desireThread(Human& human, bool& updated, DesireSync& sync) {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(sync.mtx);
            if (sync.stop) break;
            human.setDesire(Human::generateDesire());
            updated = true;
            sync.cv.notify_one();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// Main function for modeling desire matches
void start() {
    Human human1("Marina");
    Human human2("Roman");

    int matchCount = 0;
    int attempts = 0;
    std::vector<std::string> matchedDesires; // Vector to store matched desires

    DesireSync sync;
    bool updated1 = false, updated2 = false;

    std::thread t1(desireThread, std::ref(human1), std::ref(updated1), std::ref(sync));
    std::thread t2(desireThread, std::ref(human2), std::ref(updated2), std::ref(sync));

    // Sleep duration logic
    const int defaultSleepMs = 700;
    const int minSleepMs = 100;
    int sleepMs = defaultSleepMs;

    // For recent attempts history
    std::vector<std::tuple<int, std::string, std::string, bool>> recentAttempts;

    // For statistics
    std::vector<int> matchAttempts; // attempts between matches
    int lastMatchAttempt = 0;
    int fastestMatch = 0, slowestMatch = 0;

    // Main thread checks matches and prints the table
    while (matchCount < 5) {
        {
            std::unique_lock<std::mutex> lock(sync.mtx);
            sync.cv.wait(lock, [&] { return updated1 && updated2; });
        }

        std::string desire1 = human1.getDesire();
        std::string desire2 = human2.getDesire();
        ++attempts;

        bool isMatch = (desire1 == desire2);
        if (isMatch) {
            ++matchCount;
            matchedDesires.push_back(desire1); // Store matched desire
            sleepMs = defaultSleepMs; // Restore to default after match
            int attemptsSinceLastMatch = attempts - lastMatchAttempt;
            matchAttempts.push_back(attemptsSinceLastMatch);
            lastMatchAttempt = attempts;
            if (matchAttempts.size() == 1) {
                fastestMatch = slowestMatch = attemptsSinceLastMatch;
            } else {
                fastestMatch = (std::min)(fastestMatch, attemptsSinceLastMatch);
                slowestMatch = (std::max)(slowestMatch, attemptsSinceLastMatch);
            }
        } else {
            // Decrease sleep duration by 100ms, but not below minSleepMs
            sleepMs = (sleepMs - 100 > minSleepMs) ? (sleepMs - 100) : minSleepMs;
        }

        clearScreen();
        // Highlight current match count
        std::cout << std::string(87, '=') << "\n";
        setColor(true);
        std::cout << "MATCHES FOUND: " << matchCount << " of 5\n";
        resetColor();
        std::cout << std::string(87, '=') << "\n";

        // Responsive table width (calculate max desire length)
        size_t maxDesireLen = (std::max)({ desire1.length(), desire2.length(), size_t(20) });
        size_t colWidth = (std::max)(maxDesireLen + 2, size_t(30));

        // Main status table
        std::cout << std::left
                  << std::setw(12) << "Attempt"
                  << std::setw(colWidth) << human1.getName()
                  << std::setw(colWidth) << human2.getName()
                  << std::setw(15) << "Status" << "\n";
        std::cout << std::string(12 + colWidth * 2 + 15, '-') << "\n";
        std::cout << std::left
                  << std::setw(12) << attempts
                  << std::setw(colWidth) << desire1
                  << std::setw(colWidth) << desire2;

        setColor(isMatch);
        std::cout << std::setw(15) << (isMatch ? "MATCH! Congratulations!" : "No match");
        resetColor();
        std::cout << "\n";
        std::cout << std::string(12 + colWidth * 2 + 15, '-') << "\n";

        // Progress bar
        int barWidth = 40;
        int progress = (matchCount * barWidth) / 5;
        std::cout << "Progress: [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < progress) std::cout << "=";
            else std::cout << " ";
        }
        std::cout << "] " << matchCount << "/5 matches\n";

        // Store recent attempts (max 5)
        recentAttempts.push_back({attempts, desire1, desire2, isMatch});
        if (recentAttempts.size() > 5) recentAttempts.erase(recentAttempts.begin());

        // Print recent attempts
        std::cout << "\nRecent attempts:\n";
        std::cout << std::left << std::setw(8) << "No." << std::setw(colWidth) << "Desire1"
                  << std::setw(colWidth) << "Desire2" << std::setw(15) << "Status" << "\n";
        for (const auto& att : recentAttempts) {
            std::cout << std::setw(8) << std::get<0>(att)
                      << std::setw(colWidth) << std::get<1>(att)
                      << std::setw(colWidth) << std::get<2>(att);
            setColor(std::get<3>(att));
            std::cout << std::setw(15) << (std::get<3>(att) ? "MATCH" : "No match");
            resetColor();
            std::cout << "\n";
        }

        std::cout << "\nCurrent sleep duration: " << sleepMs << " ms\n";
        if (sleepMs < defaultSleepMs)
            std::cout << "Speeding up selection due to no matches!\n";

        updated1 = false;
        updated2 = false;

        // Animated spinner effect
        static const char spinner[] = "|/-\\";
        std::cout << " Waiting ";
        for (int i = 0; i < 10; ++i) {
            std::cout << spinner[i % 4] << "\r" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs / 10));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
    }

    {
        std::lock_guard<std::mutex> lock(sync.mtx);
        sync.stop = true;
        sync.cv.notify_all();
    }
    t1.join();
    t2.join();

    clearScreen();
    std::cout << "Matched desires:\n";
    std::cout << std::left << std::setw(10) << "No." << std::setw(50) << "Desire" << "\n";
    std::cout << std::string(60, '-') << "\n";
    for (size_t i = 0; i < matchedDesires.size(); ++i) {
        std::cout << std::left << std::setw(10) << (i + 1) << std::setw(50) << matchedDesires[i] << "\n";
    }
    std::cout << std::string(60, '-') << "\n";
    std::cout << "\nFinished after " << attempts << " attempts.\n";
}

// Program entry point
int main() {
	#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::ios_base::sync_with_stdio(false);
#else
    setlocale(LC_ALL, "");
#endif

    start();
    return 0;
}
