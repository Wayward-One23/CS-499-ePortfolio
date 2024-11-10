#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>   // For std::stringstream
#include <algorithm> // For std::transform
#include <cstdlib>   // For std::rand and std::srand
#include <ctime>     // For std::time
#include <vector>    // For std::vector

// Helper function to trim whitespace from a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Function to load acronyms from a file
std::unordered_map<std::string, std::string> loadAcronyms(const std::string& filename) {
    std::unordered_map<std::string, std::string> acronyms;
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Error: Unable to open file: " << filename << std::endl;
        return acronyms;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string acronym, fullName;

        // Split the line by the comma
        if (std::getline(ss, acronym, ',') && std::getline(ss, fullName)) {
            acronym = trim(acronym); // Trim any whitespace around the acronym
            fullName = trim(fullName); // Trim any whitespace around the full name

            // Convert acronym to uppercase for consistency
            std::transform(acronym.begin(), acronym.end(), acronym.begin(), ::toupper);

            acronyms[acronym] = fullName; // Store in map
        }
    }

    return acronyms;
}

// Function to display the title screen
int showTitleScreen(sf::RenderWindow& window) {
    sf::Font font;
    if (!font.loadFromFile("Fonts/gyparody hv.ttf")) {
        std::cerr << "Error loading font\n";
        return -1;
    }

    // Load background texture
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("background.jpg")) {
        std::cerr << "Error loading background texture\n";
        return -1;
    }

    sf::Sprite backgroundSprite(backgroundTexture);

    // Scale the background sprite to fit the window
    backgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
        static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
    );

    // Title text
    sf::Text title("Acronym Invaders!", font, 50);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition(200, 100);

    // Game mode options
    sf::Text mode1("1. Acronym Mode", font, 30);
    mode1.setFillColor(sf::Color::White);
    mode1.setPosition(300, 250);

    sf::Text mode2("2. Term Mode", font, 30);
    mode2.setFillColor(sf::Color::White);
    mode2.setPosition(300, 300);

    sf::Text exit("Press 'Q' to Quit", font, 24);
    exit.setFillColor(sf::Color::White);
    exit.setPosition(300, 350);

    // Display the title screen
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '1') {
                    return 1; // Classic Mode selected
                }
                else if (event.text.unicode == '2') {
                    return 2; // Timed Mode selected
                }
                else if (event.text.unicode == 'Q' || event.text.unicode == 'q') {
                    window.close(); // Quit the game
                }
            }
        }

        // Clear and draw title screen
        window.clear(); // Clear the window
        window.draw(backgroundSprite); // Draw the background
        window.draw(title); // Draw title text
        window.draw(mode1); // Draw mode 1 text
        window.draw(mode2); // Draw mode 2 text
        window.draw(exit);  // Draw ext text
        window.display();   // Display everything
    }
    return 0; // In case of window close without selection
}

// Entry point for Windows applications
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Acronym Invaders!");

    // Game loop control variable
    bool playAgain = true;

    while (playAgain) {

        // Show the title Screen and get the selected game momde
        int gameMode = showTitleScreen(window);
        if (gameMode == 0) {
            break; // Exit the game if the window was closed
        }

        // Load the acronyms from the acronyms.txt file
        std::unordered_map<std::string, std::string> acronyms = loadAcronyms("acronyms.txt");

        // Load background texture
        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("background.jpg")) {
            std::cerr << "Error loading background texture\n";
            return -1;
        }
        sf::Sprite backgroundSprite(backgroundTexture);

        // Scale the background sprite to fit the window
        backgroundSprite.setScale(
            static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
            static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
        );

        // Load multiple spaceship textures
        std::vector<sf::Texture> spaceshipTextures;
        for (int i = 1; i <= 4; ++i) { // Assuming you have 4 spaceship images named 1.png, 2.png, 3.png, and 4.png
            sf::Texture texture;
            if (!texture.loadFromFile(std::to_string(i) + ".png")) {
                std::cerr << "Error loading spaceship texture " << i << "\n";
                return -1;
            }
            spaceshipTextures.push_back(texture);
        }

        // Create spaceship sprite with a random texture
        sf::Sprite spaceshipSprite(spaceshipTextures[rand() % spaceshipTextures.size()]);

        // Scale the spaceship down
        spaceshipSprite.setScale(0.5f, 0.5f); // Scale down to 50% of its original size

        // Set initial spaceship position with random horizontal position at the top
        spaceshipSprite.setPosition(rand() % (window.getSize().x - static_cast<int>(spaceshipSprite.getGlobalBounds().width * 0.5f)), 0); // Adjust for scale

        sf::Font font;
        if (!font.loadFromFile("Fonts/arial.ttf")) {
            std::cerr << "Error loading font\n";
            return -1;
        }

        // Variables for user input
        std::string userInput;
        sf::Text userInputText("", font, 24);
        userInputText.setFillColor(sf::Color::White);
        userInputText.setPosition(50, 550);

        // Text for displaying full name of the acronym
        sf::Text fullNameText("", font, 24);
        fullNameText.setFillColor(sf::Color::White);
        fullNameText.setPosition(50, 520); // Position above the user input area

        // Create a rectangle for the combined text background for the full name and user input
        sf::RectangleShape combinedBox(sf::Vector2f(700, 120)); // Set size for the combined box
        combinedBox.setFillColor(sf::Color(0, 0, 0, 150)); // Semi-transparent black
        combinedBox.setPosition(40, 480); // Position above the user's input area

        // Text for feedback messages
        sf::Text feedbackText("", font, 24);
        feedbackText.setFillColor(sf::Color::Green);
        feedbackText.setPosition(50, 485); // Position inside feedback box

        // Initial variables for game logic
        int score = 0;
        std::string currentAcronym = "";
        bool acronymHit = false;

        // Timer variables
        int timer = 30; // Start the timer at 30 seconds
        sf::Text timerText("Time: 30", font, 24);
        timerText.setFillColor(sf::Color::White);
        timerText.setPosition(50, 10); // Position at the top left

        // Score display
        sf::Text scoreText("Score: 0", font, 24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(650, 10); // Position at the top right

        // Seed the random number generator
        std::srand(static_cast<unsigned int>(std::time(0)));

        // Select a random acronym at the start
        int randomIndex = rand() % acronyms.size();
        auto it = acronyms.begin();
        std::advance(it, randomIndex);
        currentAcronym = it->first;

        if (gameMode == 1) {
            fullNameText.setString(it->second); // Update full name for the classic mode
        }
        else if (gameMode == 2) {
            fullNameText.setString(it->first);  // Display the acronym in the new mode
        }

        // Main game loop
        sf::Clock clock; // Create a clock to track time
        sf::Clock spaceshipClock; // Create a clock for spaceship movement
        bool gameOver = false; // Flag for game over state

        while (window.isOpen() && !gameOver) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                // Handle typing input
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == 8) { // Handle backspace
                        if (!userInput.empty())
                            userInput.pop_back();
                    }
                    else if (event.text.unicode == 13) { // Handle enter (check input)
                        std::transform(userInput.begin(), userInput.end(), userInput.begin(), ::toupper);

                        if (gameMode == 1) {
                            // Check if the input matches any acronym
                            if (acronyms.find(userInput) != acronyms.end()) {
                                // If a hit, display full name and increment score
                                feedbackText.setString("Hit!");
                                score += 100; // Increment score by 100
                                timer += 5; // Add 5 seconds to the timer

                                // Set random spaceship position after a hit
                                spaceshipSprite.setPosition(rand() % (window.getSize().x - static_cast<int>(spaceshipSprite.getGlobalBounds().width)), 0); // Random position

                                // Select a random spaceship texture
                                int randomTextureIndex = rand() % spaceshipTextures.size();
                                spaceshipSprite.setTexture(spaceshipTextures[randomTextureIndex]); // Set the new texture for the spaceship

                                // Select a new random acronym for next round
                                randomIndex = rand() % acronyms.size();
                                it = acronyms.begin();
                                std::advance(it, randomIndex);
                                currentAcronym = it->first;
                                fullNameText.setString(it->second); // Update full name for the new acronym
                            }

                            else if (userInput == "Q") {
                                std::cout << "Quitting the game.\n";
                                window.close();
                            }

                            else {
                                // Missed attempt; display a missed message
                                feedbackText.setString("Missed!");
                                score -= 100; // Deduct 100 points
                                // Move the spaceship down slightly
                                spaceshipSprite.move(0, 45); // Move down 45 pixels
                            }
                        }

                        else if (gameMode == 2) {
                            // New Mode: Check if input matches the full name of the current acronym
                            std::string fullName = it->second;
                            std::transform(fullName.begin(), fullName.end(), fullName.begin(), ::toupper);

                            if (userInput == fullName) {
                                // If a hit, display full name and increment score
                                feedbackText.setString("Hit!");
                                score += 100; // Increment score by 100
                                timer += 5; // Add 5 seconds to the timer

                                // Set random spaceship position after a hit
                                spaceshipSprite.setPosition(rand() % (window.getSize().x - static_cast<int>(spaceshipSprite.getGlobalBounds().width)), 0); // Random position

                                // Select a random spaceship texture
                                int randomTextureIndex = rand() % spaceshipTextures.size();
                                spaceshipSprite.setTexture(spaceshipTextures[randomTextureIndex]); // Set the new texture for the spaceship

                                // Select a new random acronym for next round
                                randomIndex = rand() % acronyms.size();
                                it = acronyms.begin();
                                std::advance(it, randomIndex);
                                currentAcronym = it->first;
                                fullNameText.setString(it->first); // Update full name for the new acronym
                            }
                            else if (userInput == "Q") {
                                std::cout << "Quitting the game.\n";
                                window.close();
                            }
                            else {
                                // Missed attempt; display a missed message
                                feedbackText.setString("Incorrect! Try again.");
                                score -= 100; // Deduct 100 points
                                spaceshipSprite.move(0, 45); // Move down 45 pixels
                            }
                        }


                            // Clear input for next round
                            userInput.clear();
                        }

                        else if (event.text.unicode >= 32 && event.text.unicode <= 126) {
                            // Only handle printable characters
                            userInput += static_cast<char>(event.text.unicode);
                        }
                }
            }

            // Update the timer
            if (clock.getElapsedTime().asSeconds() >= 1) {
                timer--;
                clock.restart(); // Reset the clock
            }

            // Update spaceship position to fall at 1 pixel per second for gameMode 1 only
            if (gameMode == 1) {
                if (spaceshipClock.getElapsedTime().asSeconds() >= 1.0f / 60.0f) { // Check if enough time has passed for movement
                    spaceshipSprite.move(0, 0.75); // Move down by 1 pixels
                    spaceshipClock.restart(); // Reset the spaceship clock
                }
            }

            // If the timer reaches 0 or the spaceship touches the combined box, end the game
            if (timer <= 0 || (spaceshipSprite.getPosition().y + spaceshipSprite.getGlobalBounds().height >= window.getSize().y)) {
                feedbackText.setString("Game Over! Final Score: " + std::to_string(score));
                userInputText.setString(""); // Clear input
                gameOver = true; // Set game over flag
            }

            // Update the timer and score display
            timerText.setString("Time: " + std::to_string(timer));
            scoreText.setString("Score: " + std::to_string(score));

            // Rendering logic
            window.clear();

            // Draw the background
            window.draw(backgroundSprite);

            // Draw the spaceship
            window.draw(spaceshipSprite);

            // Draw the feedback text
            window.draw(feedbackText);

            // Draw the combined text background
            window.draw(combinedBox);

            // Draw the full name of the acronym (if hit or missed)
            window.draw(fullNameText);

            // Draw the user's input
            userInputText.setString("Input: (Press 'q' to Quit) " + userInput);
            window.draw(userInputText);

            // Draw the timer and score
            window.draw(timerText);
            window.draw(scoreText);

            window.display();
        }

        // Replay prompt display
        sf::Text replayText("Play again? (Y/N)", font, 24);
        replayText.setFillColor(sf::Color::White);
        replayText.setPosition(300, 340); // Position in the middle of the window, below the final score

        // Text for displaying final score
        sf::Text finalScoreText("", font, 24);
        finalScoreText.setFillColor(sf::Color::White);
        finalScoreText.setPosition(300, 300); // Position above the replay prompt

        // Show the replay prompt
        while (gameOver && window.isOpen()) {
            // Set the final score message
            finalScoreText.setString("Final Score: " + std::to_string(score));

            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == 'Y' || event.text.unicode == 'y') {
                        playAgain = true; // Restart the game
                        gameOver = false; // Reset game over state
                        score = 0; // Reset score
                        timer = 30; // Reset timer
                        feedbackText.setString(""); // Clear feedback text
                        userInput.clear(); // Clear user input
                    }
                    else if (event.text.unicode == 'N' || event.text.unicode == 'n') {
                        playAgain = false; // Exit the game loop
                        window.close(); // Close the window
                    }
                }
            }

            // Render the replay prompt
            window.clear();
            window.draw(backgroundSprite);
            window.draw(finalScoreText); // Draw the final score text
            window.draw(replayText); // Draw replay text
            window.display();
        }
    }

    return 0;
}