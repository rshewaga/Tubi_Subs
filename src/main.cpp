#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <lyra/lyra.hpp>
#include <nlohmann/json.hpp>

/**
 * Parse command line inputs.
 * Exit on any issues.
 * 
 * @param argc From main
 * @param argv From main
 * @return The input absolute file path
 */
std::filesystem::path ParseCLI(const int argc, char** argv)
{
    bool show_help = false;
    std::string inputPath = "";

    lyra::cli cli = lyra::cli()
                | lyra::help(show_help).description("Convert a TubiTV downloaded subtitle file into a regular formatted .srt file. The output is in the same folder as the input file.")
                | lyra::arg(inputPath, "input file path")("Absolute file path to the TubiTV downloaded subtitle file.").required();

    auto result = cli.parse({argc, argv});
    if(!result)
    {
        std::cerr << "Error in command line: " << result.message() << std::endl;
        exit(EXIT_FAILURE);
    }

    if(show_help)
    {
        std::cout << cli << std::endl;
        exit(EXIT_SUCCESS);
    }

    // Check if the input path is actually an absolute file path
    std::filesystem::path asPath(inputPath);
    if(!asPath.is_absolute())
    {
        std::cerr << "Input file path isn't absolute!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(!std::filesystem::exists(asPath))
    {
        std::cerr << "The input file path doesn't point to an existing file!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(!std::filesystem::is_regular_file(asPath))
    {
        std::cerr << "The input file path is pointing to a non-file!" << std::endl;
        exit(EXIT_FAILURE);
    }

    return asPath;
}

int main(int argc, char** argv)
{
    // ParseCLI exits the program if there's an issue.
    // Therefore it always returns a valid path.
    std::filesystem::path inputPath = ParseCLI(argc, argv);

    // Open the input TubiTV subtitle file, check for error
    std::ifstream inputFile(inputPath);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening the file!" << std::endl;
        return EXIT_FAILURE;
    }

    // Parse it as JSON, check for error
    nlohmann::json data = nlohmann::json::parse(inputFile, nullptr, false);
    if(data.is_discarded())
    {
        std::cerr << "An error occurred during JSON parsing of the input file!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Open output file, check for error
    std::filesystem::path outputPath = inputPath.parent_path() / (inputPath.stem().string() + "_out.srt");
    
    std::ofstream output(outputPath);
    if (!output.is_open()) {
        std::cerr << "Error opening the output file!" << std::endl;
        return EXIT_FAILURE;
    }

    // Each entry is a subtitle to display within a start and end time.
    // Format them for .srt and output to the result file.
    for(size_t i = 0; i < data.size(); ++i)
    {
        // Convert start timestamp in milliseconds to elapsed hrs/mins/secs/ms
        int _ms = data[i]["start"].get<int>();
        int _hours = static_cast<int>(std::floor(_ms / 3600000));
        int _minutes = static_cast<int>(std::floor(_ms / 60000)) % 60;
        int _seconds = static_cast<int>(std::floor(_ms / 1000)) % 60;
        _ms %= 1000;
        
        std::string _timeStart = fmt::format("{:0>2}:{:0>2}:{:0>2},{:0>3}", _hours, _minutes, _seconds, _ms);
        
        // Convert end timestamp in milliseconds to elapsed hrs/mins/secs/ms
        _ms = data[i]["end"].get<int>();
        _hours = static_cast<int>(std::floor(_ms / 3600000));
        _minutes = static_cast<int>(std::floor(_ms / 60000)) % 60;
        _seconds = static_cast<int>(std::floor(_ms / 1000)) % 60;
        _ms %= 1000;

        std::string _timeEnd = fmt::format("{:0>2}:{:0>2}:{:0>2},{:0>3}", _hours, _minutes, _seconds, _ms);

        // The dialogue is a JSON array of strings.
        // e.g.: ["hi", "ho", "neighbourino"]
        // Format that into a single string without quotes. Each entry on a new line.
        std::string _dialogue = "";
        for(size_t j = 0; j < data[i]["text"].size(); ++j)
        {
            if(j >= 1)
            {
                _dialogue += "\n";
            }
            std::string _line = data[i]["text"][j].dump();
            _dialogue += _line.substr(1, _line.size() - 2);
        }

        output << fmt::format("{}\n{} --> {}\n{}\n\n",
                            data[i]["index"].dump(),
                            _timeStart, _timeEnd,
                            _dialogue);
    }

    // Close the file handles
    inputFile.close();
    output.close();


    std::cout << "Output to " << outputPath.string() << std::endl;
    return EXIT_SUCCESS;
}