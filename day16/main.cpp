#include <bits/stdc++.h>
#include <vector>
#include <array>
#include <iostream>


class Packet
{
public:
    uint16_t version;
    uint16_t type;
    size_t length;
};

class LiteralPacket : public Packet
{
public:
    size_t value;
};

class OperatorPacket : public Packet
{
public:
    char labelType;
    uint16_t label;
    std::vector<std::shared_ptr<Packet>> subs;
};


class PacketConverter
{
public:
    static std::string hexes2bin(const std::string& hexdecimals)
    {
        std::string bin;

        for (const auto& hexdecimal : hexdecimals)
        {
            bin += hex2bin(hexdecimal);
        }

        return bin;
    }

private:
    static std::string hex2bin(const char& hexdecimal)
    {
        std::string bin = "0000";
        uint16_t decimal = char2decimal(hexdecimal);

        for (int i = 0 ; i < 4 ; i++)
        {
            uint16_t nextBin = decimal % 2;
            char nextChar = bin2char(nextBin);
            bin[3 - i] = nextChar;

            decimal /= 2;
        }

        return bin;
    }

    static uint16_t char2decimal(const char& hexdecimal)
    {
        return hexdecimal <= 57 ?  hexdecimal - 48 : hexdecimal - 55;
    }

    static char bin2char(const uint16_t bin)
    {
        return (char)(bin + 48);
    }
};


class PacketParser {
public:
    virtual std::shared_ptr<Packet> parse(const char* bins) = 0;
    virtual bool isParsable(const char* bins) = 0;

    void addParser(PacketParser* parser)
    {
        parsers.push_back(parser);
    }

protected:
    uint16_t parseVersion(const char* bins)
    {
        return str2decimal(bins + VERSION_OFFSET, VERSION_LENGTH);
    }

    uint16_t parseType(const char* bins)
    {
        return str2decimal(bins + TYPE_OFFSET, TYPE_LENGTH);
    }

    size_t str2decimal(const char* str, size_t length)
    {
        size_t decimal = 0;
        size_t binValue = 1;

        for (int i = 0 ; i < length; i++)
        {
            if (str[(length - 1) - i] == '1')
            {
                decimal += binValue;
            }

            binValue *= 2;
        }

        return decimal;
    }

protected:
    const uint16_t VERSION_OFFSET = 0;
    const uint16_t VERSION_LENGTH = 3;
    const uint16_t TYPE_OFFSET = (VERSION_OFFSET + VERSION_LENGTH);
    const uint16_t TYPE_LENGTH = 3;


    std::vector<PacketParser *> parsers;
};

class LiteralPacketParser : public PacketParser
{
public:
    std::shared_ptr<Packet> parse(const char* bins) override
    {
        uint16_t offset = 0;
        std::shared_ptr<LiteralPacket> packet(new LiteralPacket());

        packet->version = parseVersion(bins);
        packet->type = parseType(bins);

        offset += VERSION_LENGTH + TYPE_LENGTH;

        std::string literalValue;
        while (!isLastValue(bins + offset))
        {
            auto nextValue = std::string(bins + offset + 1, bins + offset + 5);
            literalValue += nextValue;
            offset += 5;
        }

        auto nextValue = std::string(bins + offset + 1, bins + offset + 5);
        literalValue += nextValue;
        offset += 5;

        packet->value = str2decimal(literalValue.c_str(), literalValue.size());
        packet->length = offset;

        return packet;
    }

    bool isParsable(const char* bins) override
    {
        auto type = parseType(bins);
        return type == 4;
    }

private:
    bool isLastValue(const char* bins) const
    {
        return *bins == '0';
    }
};


class OperatorPacketParser : public PacketParser
{
public:
    bool isParsable(const char* bins) override
    {
        auto type = parseType(bins);
        return type != 4;
    }

protected:
    void parseHeader(const char* bins, std::shared_ptr<OperatorPacket>& packet)
    {
        packet->version = parseVersion(bins);
        packet->type = parseType(bins);
        packet->labelType = parseLabelType(bins);
    }

    char parseLabelType(const char* bins)
    {
        return *(bins + LABEL_TYPE_OFFSET);
    }

protected:
    const uint16_t LABEL_TYPE_OFFSET = TYPE_OFFSET + TYPE_LENGTH;
    const uint16_t LABEL_TYPE_LENGTH = 1;
};

class TotalLengthOperatorPacketParser : public OperatorPacketParser
{
public:
    std::shared_ptr<Packet> parse(const char* bins) override
    {
        uint16_t offset = 0;
        std::shared_ptr<OperatorPacket> packet(new OperatorPacket());

        parseHeader(bins, packet);
        offset += LABEL_TYPE_OFFSET + LABEL_TYPE_LENGTH;

        auto totalLength = parseTotalLength(bins + offset);
        offset += LABEL_BITS_LENGTH;

        while (totalLength != 0)
        {
            for (PacketParser* parser : parsers)
            {
                if (parser->isParsable(bins + offset))
                {
                    auto subPacket = parser->parse(bins + offset);
                    offset += subPacket->length;
                    totalLength -= subPacket->length;
                    packet->subs.push_back(subPacket);
                    break;
                }
            }
        }

        packet->length = offset;
        return packet;
    }

    bool isParsable(const char* bins) override
    {
        auto type = parseLabelType(bins);
        return OperatorPacketParser::isParsable(bins) && type == '0';
    }

private:
    uint16_t parseTotalLength(const char* bins)
    {
        uint16_t totalLength = str2decimal(bins, LABEL_BITS_LENGTH);
        return totalLength;
    }

private:
    const uint16_t LABEL_BITS_LENGTH = 15;
};

class SubPacketNumOperatorPacketParser : public OperatorPacketParser
{
public:
public:
    std::shared_ptr<Packet> parse(const char* bins) override
    {
        uint16_t offset = 0;
        std::shared_ptr<OperatorPacket> packet(new OperatorPacket());

        parseHeader(bins, packet);
        offset += LABEL_TYPE_OFFSET + LABEL_TYPE_LENGTH;

        auto numOfSubpackets = parseNumOfSubPackets(bins + offset);
        offset += LABEL_BITS_LENGTH;

        for (int i = 0 ; i < numOfSubpackets; i++)
        {
            for (PacketParser* parser : parsers)
            {
                if (parser->isParsable(bins + offset))
                {
                    auto subPacket = parser->parse(bins + offset);
                    offset += subPacket->length;
                    packet->subs.push_back(subPacket);
                    break;
                }
            }
        }

        packet->length = offset;
        return packet;
    }


    bool isParsable(const char* bins) override
    {
        auto type = parseLabelType(bins);
        return OperatorPacketParser::isParsable(bins) && type == '1';
    }
private:
    uint16_t parseNumOfSubPackets(const char* bins)
    {
        uint16_t numOfSubpackets = str2decimal(bins, LABEL_BITS_LENGTH);
        return numOfSubpackets;
    }

private:
    const uint16_t LABEL_BITS_LENGTH = 11;
};


class PacketMaker
{
public:
    virtual std::shared_ptr<Packet> make(const std::string& bins)
    {
        for (PacketParser* parser : parsers)
        {
            if (parser->isParsable(bins.c_str()))
            {
                return parser->parse(bins.c_str());
            }
        }

        return std::shared_ptr<Packet>();
    }

    void addParser(PacketParser* parser)
    {
        parsers.push_back(parser);
    }

private:
    std::vector<PacketParser *> parsers;
};

class VersionSummer
{
public:
    static size_t sum(Packet* packet)
    {
        if (isLiteralType(packet->type))
        {
            return packet->version;
        }
        else
        {
            size_t total = packet->version;

            OperatorPacket* operatorPacket = static_cast<OperatorPacket *>(packet);
            for (const auto& subPacket : operatorPacket->subs)
            {
                total += sum(subPacket.get());
            }

            return total;
        }
    }

private:
    static bool isLiteralType(const uint16_t type)
    {
        return type == 4;
    }
};

class PacketCalculator
{
public:
    static size_t calculate(Packet* packet)
    {
        OperatorPacket* operatorPacket = static_cast<OperatorPacket *>(packet);
        std::vector<size_t> values;

        for (const auto& subPacket : operatorPacket->subs)
        {
            if (isLiteralType(subPacket->type))
            {
                LiteralPacket* literalPacket = static_cast<LiteralPacket *>(subPacket.get());
                values.push_back(literalPacket->value);
            }
            else
            {
                auto value = calculate(subPacket.get());
                values.push_back(value);
            }
        }

        auto result = operate(packet->type, values);
        return result;
     }

private:
    static size_t operate(const uint16_t type, std::vector<size_t> values)
    {
        size_t result = 0;

        switch(type)
        {
            case 0:
                for (const auto& value : values)
                {
                    result += value;
                }
                break;
            case 1:
                result = 1;
                for (const auto& value : values)
                {
                    result *= value;
                }
                break;
            case 2:
                std::sort(values.begin(), values.end());
                result = values.front();
                break;
            case 3:
                std::sort(values.begin(), values.end());
                result = values.back();
                break;
            case 5:
                result = values[0] > values[1] ? 1 : 0;
                break;
            case 6:
                result = values[0] < values[1] ? 1 : 0;
                break;
            case 7:
                result = values[0] == values[1] ? 1 : 0;
                break;
            default:
            {}
        }

        return result;
    }

    static bool isLiteralType(const uint16_t type)
    {
        return type == 4;
    }
};

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    std::string input;
    getline(std::cin, input);

    std::string bins = PacketConverter::hexes2bin(input);

    PacketMaker maker;
    LiteralPacketParser literalPacketParser;
    SubPacketNumOperatorPacketParser subPacketNumOperatorPacketParser;
    TotalLengthOperatorPacketParser totalLengthOperatorPacketParser;

    maker.addParser(&literalPacketParser);
    maker.addParser(&subPacketNumOperatorPacketParser);
    maker.addParser(&totalLengthOperatorPacketParser);

    subPacketNumOperatorPacketParser.addParser(&literalPacketParser);
    subPacketNumOperatorPacketParser.addParser(&subPacketNumOperatorPacketParser);
    subPacketNumOperatorPacketParser.addParser(&totalLengthOperatorPacketParser);

    totalLengthOperatorPacketParser.addParser(&literalPacketParser);
    totalLengthOperatorPacketParser.addParser(&subPacketNumOperatorPacketParser);
    totalLengthOperatorPacketParser.addParser(&totalLengthOperatorPacketParser);

    auto rootPacket = maker.make(bins);

    std::cout << "Day 16-1: " << VersionSummer::sum(rootPacket.get()) << " " << std::endl;
    std::cout << "Day 16-2: " << PacketCalculator::calculate(rootPacket.get()) << " " << std::endl;

    return 0;
}


