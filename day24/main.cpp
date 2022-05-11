#include <vector>
#include <iostream>
#include <string>
#include <stack>
#include <array>

struct Operation {
    std::string instruction;
    std::string leftOperand;
    std::string rightOperand;
};

using Operations = std::vector<Operation>;

struct OperationSet {
    Operations operations;
};

using OperationSets = std::vector<OperationSet>;

class OperationReader
{
public:
    static OperationSets read()
    {
        std::ios_base::sync_with_stdio(false);
        freopen(INPUT_FILE_PATH, "rt", stdin);

        OperationSets operationSets;
        std::string input;

        while (!std::cin.eof())
        {
            getline(std::cin, input);
            if (input.empty())
            {
                continue;
            }


            std::vector<std::string> words;
            size_t pos = 0;
            while ((pos = input.find(' ')) != std::string::npos) {
                auto word = input.substr(0, pos);
                input.erase(0, pos + 1);
                words.push_back(word);
            }
            words.emplace_back(input);

            if (words[0] == "inp") {
                operationSets.push_back({});
            } else {
                auto& latestOperationSet = operationSets.back();
                Operation operation;
                operation.instruction = words[0];
                operation.leftOperand = words[1];
                operation.rightOperand = words[2];
                latestOperationSet.operations.push_back(operation);
            }
        }

        return operationSets;
    }

};

// Key Rule
//inp w
//mul x 0
//add x z
//mod x 26
//div z keyOperands[0]
//add x keyOperands[1]
//eql x w
//eql x 0
//mul y 0
//add y 25
//mul y x
//add y 1
//mul z y
//mul y 0
//add y w
//add y keyOperands[2]
//mul y x
//add z y
// keyOperands are variables
class KeyRightOperandExtractor {
public:
    explicit KeyRightOperandExtractor(const OperationSet &operationSet) : operationSet(operationSet) {}

    std::vector<int> extract() {
        std::vector<int> keyOperands;

        for (const auto& keyOperandIndex : keyOperandIndexes) {
            auto& operation = operationSet.operations[keyOperandIndex];
            auto rightOperand = std::stoi(operation.rightOperand);
            keyOperands.push_back(rightOperand);
        }

        return keyOperands;
    }

private:
    const OperationSet& operationSet;

    const std::vector<int> keyOperandIndexes = {
            3, 4, 14
    };
};

// Assumption
// 1. "keyOperands[2]" is unconditionally positive.
//  - So "keyOperands[2] + w" must also be positive.
//
// 2. "keyOperands[0]" is 1 or 26
//
// 3. When calculating the number, x must be 1 when z is divisible by 1, and x must be 0 when z is divisible by 26.
//  - If this assumption is not satisfied, z cannot end in 0.
//  - Because we can reduce the value only when z is divided by 26, for this to happen, x must be 0.
//
// 4. Formula
//  1) x = (((z % 26) + keyOperands[1]) != w)
//  2) if x == 1 then z = z * 26 + w + keyOperands[2]
//  3) if x == 0 then z = z / 26
class ModelNumberFinder {
public:
    explicit ModelNumberFinder(const OperationSets &operationSets) : operationSets(operationSets) {}


    std::string findLargestNumber() {
        return find(9, 1);
    }

    std::string findSmallestNumber() {
        return find(1, -1);
    }

private:
    std::string find(int desireDigitNumber, int compareSign) {
        modelNumber = std::array<int, 14>();
        modelNumber.fill(desireDigitNumber);

        std::stack<std::pair<int, int>> indexAndKeyOperand2Stack;

        for (int i = 0 ; i < operationSets.size(); i++) {
            const auto& operationSet = operationSets[i];
            KeyRightOperandExtractor extractor(operationSet);
            auto keyOperands = extractor.extract();

            const int& divider = keyOperands[0];

            if (divider == 1) {
                indexAndKeyOperand2Stack.emplace(i, keyOperands[2]);
            } else {
                auto indexAndKeyOperand2 = indexAndKeyOperand2Stack.top();
                indexAndKeyOperand2Stack.pop();

                const int& index = indexAndKeyOperand2.first;
                const int& latestOperand2WhenDividerIs1 = indexAndKeyOperand2.second;
                int& latestDigitWhenDividerIs1 = modelNumber[index];

                // compare is "((z % 26) + keyOperands[1])"
                int compare = latestDigitWhenDividerIs1 + latestOperand2WhenDividerIs1 + keyOperands[1];
                if (compare * compareSign > desireDigitNumber * compareSign) {
                    latestDigitWhenDividerIs1 += desireDigitNumber - compare;
                } else {
                    modelNumber[i] = compare;
                }
            }

        }

        return array2str();
    }

private:
    std::string array2str() {
        std::string str;
        for (const auto& num : modelNumber) {
            str += std::to_string(num);
        }
        return str;
    }

private:
    const OperationSets& operationSets;
    std::array<int, 14> modelNumber{};

};


int main()
{
    auto operationSets = OperationReader::read();

    ModelNumberFinder finder(operationSets);

    std::cout << "24-1: " << finder.findLargestNumber() << std::endl;
    std::cout << "24-2: " << finder.findSmallestNumber() << std::endl;
}