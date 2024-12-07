#include <iostream>
#include <fstream>
#include <vector>

class CacheEntry
{
public:
    bool valid;
    unsigned int tag;

    CacheEntry() : valid(false), tag(0) {}
};

class Cache
{
private:
    std::vector<CacheEntry> cache;
    unsigned int cacheSize;

    unsigned int getIndex(unsigned int address) const
    {
        return address % cacheSize;
    }

    unsigned int getTag(unsigned int address) const
    {
        return address / cacheSize;
    }

public:
    Cache(unsigned int size) : cacheSize(size), cache(size) {}

    bool isHit(unsigned int address) const
    {
        unsigned int index = getIndex(address);
        unsigned int tag = getTag(address);

        std::cout << "Checking Address: " << address
                  << ", Index: " << index
                  << ", Tag: " << tag
                  << ", Cache Valid: " << cache[index].valid
                  << ", Cache Tag: " << cache[index].tag
                  << std::endl;

        return (cache[index].valid && cache[index].tag == tag);
    }

    void processMemoryReference(unsigned int address)
    {
        unsigned int index = getIndex(address);
        unsigned int tag = getTag(address);

        if (cache[index].valid && cache[index].tag != tag)
        {
            std::cout << "Evicting Index: " << index
                      << ", Old Tag: " << cache[index].tag
                      << ", New Tag: " << tag
                      << std::endl;
        }

        cache[index].valid = true;
        cache[index].tag = tag;

        std::cout << "Updating Cache - Index: " << index
                  << ", Tag: " << tag << std::endl;
    }

    float getHitRate(int totalAccesses) const
    {
        int hits = 0;
        for (const auto &entry : cache)
        {
            if (entry.valid)
            {
                ++hits;
            }
        }
        return static_cast<float>(hits) / totalAccesses * 100;
    }
};

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " mem_size cache_size access_filename\n";
        return 1;
    }

    unsigned int memSize = std::stoi(argv[1]);
    unsigned int cacheSize = std::stoi(argv[2]);
    std::string accessFileName = argv[3];

    if (memSize % cacheSize != 0)
    {
        std::cerr << "Invalid memory or cache size configuration.\n";
        return 1;
    }

    Cache cache(cacheSize);

    std::ifstream accessFile(accessFileName);
    if (!accessFile.is_open())
    {
        std::cerr << "Error opening access file.\n";
        return 1;
    }

    unsigned int address;
    int totalAccesses = 0;
    std::ofstream outputFile("sim_output.txt");

    while (accessFile >> address)
    {
        ++totalAccesses;
        if (cache.isHit(address))
        {
            outputFile << address << ": HIT\n";
        }
        else
        {
            outputFile << address << ": MISS\n";
            cache.processMemoryReference(address);
        }
    }

    std::cout << "Out of " << totalAccesses << " memory accesses, there was a "
              << static_cast<int>(cache.getHitRate(totalAccesses)) << "% cache hit rate and a "
              << 100 - static_cast<int>(cache.getHitRate(totalAccesses)) << "% miss rate.\n";

    accessFile.close();
    outputFile.close();

    return 0;
}
