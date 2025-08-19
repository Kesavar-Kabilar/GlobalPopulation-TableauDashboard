#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

int main() {
    std::ifstream inputFile("original_data.csv");
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open the original data file." << std::endl;
        return 1;
    }

    std::ofstream outputFile("modified_data_one.csv");
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not create the output file." << std::endl;
        return 1;
    }

    // Write the header to the new CSV file
    outputFile << "Year,Continent,Country,PopulationChange" << std::endl;

    std::string line;
    std::getline(inputFile, line); // Read and discard the header row

    // A map to store the parent code (subregion code) and its corresponding name (continent)
    std::map<int, std::string> subregionMap;

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

        if (row.size() < 22) continue; // Skip malformed rows

        std::string type = row[8];
        int locationCode;
        try {
            locationCode = std::stoi(row[4]);
        } catch (const std::invalid_argument& ia) {
            continue; // Skip if location code is not a valid number
        }

        if (type == "Subregion") {
            // Store the subregion (continent) name and its code
            subregionMap[locationCode] = row[2];
        } else if (type == "Country/Area") {
            int year;
            double populationChange;
            int parentCode;

            try {
                year = std::stoi(row[10]);
                populationChange = std::stod(row[21]);
                parentCode = std::stoi(row[9]);
            } catch (const std::invalid_argument& ia) {
                continue; // Skip if year, pop change, or parent code are not valid numbers
            }

            // Find the continent from the map
            std::string continent = "N/A";
            if (subregionMap.count(parentCode)) {
                continent = subregionMap[parentCode];
            }

            std::string country = row[2];
            // Remove leading/trailing quotes if they exist
            if (country.size() > 1 && country.front() == '"' && country.back() == '"') {
                country = country.substr(1, country.size() - 2);
            }
            // Remove the note about asterisk if present
            size_t pos = country.find(" *");
            if (pos != std::string::npos) {
                country.erase(pos, 2);
            }

            // Write the new row to the output file
            outputFile << year << "," << continent << "," << country << "," << populationChange << std::endl;
        }
    }

    inputFile.close();
    outputFile.close();

    std::cout << "Successfully created modified_data_one.csv" << std::endl;

    return 0;
}