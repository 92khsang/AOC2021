#include <vector>
#include <iostream>
#include <string>
#include <stack>

using Node = std::string;

class InputReader
{
public:
    static std::vector<std::string> read()
    {
        std::ios_base::sync_with_stdio(false);
        freopen(INPUT_FILE_PATH, "rt", stdin);

        std::vector<std::string> inputs;
        std::string input;

        while (!std::cin.eof())
        {
            getline(std::cin, input);
            inputs.emplace_back(input);
        }

        return inputs;
    }
};

class Reducable
{

public:
    virtual bool isTarget(const std::vector<std::string>& nodes) = 0;
    virtual void action(std::vector<std::string>& nodes) = 0;

};

class Exploder : public Reducable
{

public:
    bool isTarget(const std::vector<Node>& nodes) override
    {
        return findNestedIndex(nodes) != -1;
    }

    void action(std::vector<Node>& nodes) override
    {
        int32_t nestedIndex = findNestedIndex(nodes);
        auto numberPair = converNodes2Pair(nodes, nestedIndex);

        int32_t leftNumberNodeIndex = findLeftNumberNodeIndex(nodes, nestedIndex);
        if (-1 != leftNumberNodeIndex)
        {
            addNumberToNode(numberPair.first, nodes[leftNumberNodeIndex]);
        }

        int32_t rightNumberNodeIndex = findRightNumberNodeIndex(nodes, nestedIndex);
        if (-1 != rightNumberNodeIndex)
        {
            addNumberToNode(numberPair.second, nodes[rightNumberNodeIndex]);
        }
        eraseNodes(nodes, nestedIndex);
        nodes[nestedIndex] = "0";
    }

private:
    int32_t findNestedIndex(const std::vector<Node>& nodes)
    {
        int32_t index = -1;
        std::stack<Node> nestStack;

        for (const Node& node : nodes)
        {
            index++;
            if (node == "[")
            {
                nestStack.push(node);
            }
            else if (node == "]")
            {
                nestStack.pop();
            }
            if (nestStack.size() == 5)
            {
                return index;
            }
        }

        return -1;
    }

    int32_t findLeftNumberNodeIndex(const std::vector<Node >& nodes, int32_t index)
    {
        for (int32_t leftNumberNodeIndex = index - 1; leftNumberNodeIndex > 0; leftNumberNodeIndex--)
        {
            if (isNumberNode(nodes[leftNumberNodeIndex]))
            {
                return leftNumberNodeIndex;
            }
        }
        return -1;
    }

    int32_t findRightNumberNodeIndex(const std::vector<Node >& nodes, int32_t index)
    {
        bool findCloseChar = false;
        for (int32_t rightNumberNodeIndex = index + 1; rightNumberNodeIndex < nodes.size(); rightNumberNodeIndex++)
        {
            if (nodes[rightNumberNodeIndex] == "]")
            {
                findCloseChar = true;
                continue;
            }
            if (findCloseChar && isNumberNode(nodes[rightNumberNodeIndex]))
            {
                return rightNumberNodeIndex;
            }
        }
        return -1;
    }

    bool isNumberNode(const Node& node)
    {
        return node != "]" && node != "[";
    }

    std::pair<uint16_t, uint16_t> converNodes2Pair(const std::vector<Node >& nodes, int32_t index)
    {
        uint16_t firstNumber = std::stoul(nodes[index + 1]);
        uint16_t secNumber = std::stoul(nodes[index + 2]);
        return { firstNumber, secNumber };
    }

    void addNumberToNode(uint16_t number, Node& node)
    {
        uint16_t nodeNumber = std::stoul(node);
        uint16_t sum = nodeNumber + number;
        node = std::to_string(sum);
    }

    void eraseNodes(std::vector<Node >& nodes, int32_t index)
    {
        for (int i = 0; i < 3; i++)
        {
            nodes.erase(nodes.begin() + index + 1);
        }
    }

};

class Spliter : public Reducable
{

public:
    bool isTarget(const std::vector<std::string>& nodes) override
    {
        return -1 != findGreaterNumberIndex(nodes);
    }

    void action(std::vector<std::string>& nodes) override
    {
        int32_t greaterNumberIndex = findGreaterNumberIndex(nodes);
        uint16_t number = std::stoul(nodes[greaterNumberIndex]);

        std::string firstSplitedNumber = std::to_string(number / 2);
        std::string secSplitedNumber = std::to_string((number + 1) / 2);

        nodes.insert(nodes.begin() + greaterNumberIndex + 1, "]");
        nodes.insert(nodes.begin() + greaterNumberIndex + 1, secSplitedNumber);
        nodes.insert(nodes.begin() + greaterNumberIndex + 1, firstSplitedNumber);
        nodes[greaterNumberIndex] = "[";
    }

private:
    bool isNumberNode(const Node& node)
    {
        return node != "]" && node != "[";
    }

    int32_t findGreaterNumberIndex(const std::vector<Node>& nodes)
    {
        for (int32_t i = 0; i < nodes.size(); i++)
        {
            if (isNumberNode(nodes[i]))
            {
                uint16_t number = std::stoul(nodes[i]);
                if (number >= 10)
                {
                    return i;
                }
            }
        }
        return -1;
    }
};

class NodeConverter
{

public:
    static std::vector<Node> convertStr2Nodes(const std::string& str)
    {
        std::vector<Node> nodes;
        std::string node;
        for (char c : str)
        {
            if (c >= '0' && c <= '9')
            {
                node += c;
            }
            else
            {
                if (!node.empty())
                {
                    nodes.push_back(node);
                    node.clear();
                }
                if (c != ',')
                {
                    std::string temp;
                    temp.push_back(c);
                    nodes.emplace_back(temp);
                }
            }
        }
        return nodes;
    }

    static std::string convertNodes2Str(const std::vector<Node>& nodes)
    {
        std::string str;
        for (auto it = nodes.begin(); it != nodes.end() - 1; )
        {
            str += *it;
            auto prev = it;
            it++;
            if (needComma(*prev, *it))
            {
                str += ",";
            }
        }
        str += "]";
        return str;
    }

private:
    static bool needComma(const Node& prev, const Node& next)
    {
        return (isNumber(prev) && (isNumber(next) || next == "[")) || (prev == "]" && isNumber(next));
    }

    static bool isNumber(const Node& node)
    {
        return (node[0] >= '0' && node[0] <= '9');
    }
};

class Reducer
{
public:
    std::string reduce(const std::vector<std::string>& inputs)
    {
        result.clear();
        result = inputs[0];
        for (int i = 0; i < inputs.size(); i++)
        {
            if (0 != i)
            {
                addNextNode(inputs[i]);
            }

            auto nodes = NodeConverter::convertStr2Nodes(result);
            for (int j = 0 ; j < reducables.size();)
            {
                if (reducables[j]->isTarget(nodes))
                {
                    reducables[j]->action(nodes);
                    //auto temp = NodeConverter::convertNodes2Str(nodes);
                    //std::cout << temp << std::endl;
                    j = 0;
                    continue;
                }
                j++;
            }
            result = NodeConverter::convertNodes2Str(nodes);
        }

        return result;
    }

    void addReducable(Reducable* reducable)
    {
        reducables.push_back(reducable);
    }

private:
    void addNextNode(const std::string& nextInput)
    {
        result = "[" + result;
        result = result + "," + nextInput + "]";
    }

private:
    std::string result;
    std::vector<Reducable*> reducables;
};

class FinalSummer
{
public:
    static size_t sum(const std::string snailfish)
    {
        size_t magnitude = 0;

        auto nodes = NodeConverter::convertStr2Nodes(snailfish);
        std::stack<uint16_t> numStack;

        for (const auto& node : nodes)
        {
            if (isNumberNode(node))
            {
                uint16_t num = std::stoul(node);
                numStack.push(num);
            }
            else if ("]" == node)
            {
                uint16_t rightNumber = numStack.top();
                numStack.pop();

                uint16_t leftNumber = numStack.top();
                numStack.pop();

                magnitude = leftNumber * 3 + rightNumber * 2;
                numStack.push(magnitude);
            }
        }

        return magnitude;
    }

private:
    static bool isNumberNode(const Node& node)
    {
        return node != "]" && node != "[";
    }

};

class TwoSnailfishSetMaker
{
public:
    static std::vector<std::vector<std::string>> make(const std::vector<std::string>& inputs)
    {
        std::vector<std::vector<std::string>> twoSnailfishes;

        for (int i = 0; i < inputs.size() - 1; i++)
        {
            auto oneInput = inputs[i];
            for (int j = i + 1; j < inputs.size(); j++)
            {
                auto otherInput = inputs[j];

                std::vector<std::string> twoSnailfish;
                twoSnailfish.emplace_back(oneInput);
                twoSnailfish.emplace_back(otherInput);
                twoSnailfishes.emplace_back(twoSnailfish);

                twoSnailfish.clear();
                twoSnailfish.emplace_back(otherInput);
                twoSnailfish.emplace_back(oneInput);
                twoSnailfishes.emplace_back(twoSnailfish);
            }
        }

        return twoSnailfishes;
    }
};


int main()
{
    auto inputs = InputReader::read();

    Exploder exploder;
    Spliter spliter;

    Reducer reducer;
    reducer.addReducable(&exploder);
    reducer.addReducable(&spliter);

    auto finalSnailFish = reducer.reduce(inputs);
    auto finalSum = FinalSummer::sum(finalSnailFish);

    std::cout << "18-1: " << finalSum << std::endl;

    auto twoSnailfishes = TwoSnailfishSetMaker::make(inputs);
    size_t maxFinalSum = 0;
    for (const auto& twoSnailfish : twoSnailfishes)
    {
        auto snailfish = reducer.reduce(twoSnailfish);
        auto sum = FinalSummer::sum(snailfish);

        if (sum > maxFinalSum)
        {
            maxFinalSum = sum;
        }
    }

    std::cout << "18-2: " << maxFinalSum << std::endl;
}