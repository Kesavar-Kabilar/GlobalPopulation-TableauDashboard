#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <limits>

// A struct to hold information about a country's population change
struct PopulationData {
    std::string country;
    double populationChange;
};

int main() {
    std::ifstream inputFile("modified_data_one.csv");
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open modified_data_one.csv" << std::endl;
        return 1;
    }

    std::ofstream outputFile("modified_data_two.csv");
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not create modified_data_two.csv" << std::endl;
        return 1;
    }

    // Write the header to the new CSV file
    outputFile << "Year,Continent,Country,PopulationChange" << std::endl;

    std::string line;
    std::getline(inputFile, line); // Read and discard the header row

    // Maps to store the highest and lowest population change for each year-continent pair
    std::map<std::pair<int, std::string>, PopulationData> highestChanges;
    std::map<std::pair<int, std::string>, PopulationData> lowestChanges;

    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;

        // Manually handle quoted commas within a cell
        bool inQuote = false;
        std::string currentCell;
        for (char c : line) {
            if (c == '"') {
                inQuote = !inQuote;
            } else if (c == ',' && !inQuote) {
                row.push_back(currentCell);
                currentCell = "";
            } else {
                currentCell += c;
            }
        }
        row.push_back(currentCell);

        if (row.size() < 4) continue; // Skip malformed rows

        int year;
        std::string continent = row[1];
        std::string country = row[2];
        double populationChange;

        try {
            year = std::stoi(row[0]);
            populationChange = std::stod(row[3]);
        } catch (const std::invalid_argument& ia) {
            continue; // Skip if year or population change are not valid numbers
        }

        std::pair<int, std::string> key = {year, continent};

        // Check for the highest population change
        if (highestChanges.find(key) == highestChanges.end() || populationChange > highestChanges[key].populationChange) {
            highestChanges[key] = {country, populationChange};
        }

        // Check for the lowest population change
        if (lowestChanges.find(key) == lowestChanges.end() || populationChange < lowestChanges[key].populationChange) {
            lowestChanges[key] = {country, populationChange};
        }
    }

    // Iterate through the maps and write the results to the output file
    for (const auto& pair : highestChanges) {
        int year = pair.first.first;
        std::string continent = pair.first.second;
        const PopulationData& data = pair.second;

        // Output the highest change
        outputFile << year << "," << continent << "," << data.country << "," << data.populationChange << std::endl;
        
        // Output the lowest change from the corresponding lowestChanges map
        if (lowestChanges.count(pair.first)) {
            const PopulationData& lowestData = lowestChanges[pair.first];
            outputFile << year << "," << continent << "," << lowestData.country << "," << lowestData.populationChange << std::endl;
        }
    }

    inputFile.close();
    outputFile.close();

    std::cout << "Successfully created modified_data_two.csv" << std::endl;

    return 0;
}