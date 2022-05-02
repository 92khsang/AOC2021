#include <utility>
#include <vector>
#include <array>
#include <iostream>
#include <string>
#include <unordered_set>

struct subtracted_hash
{
    std::size_t operator() (const std::array<int32_t, 3> &points) const {
        size_t sec_third =  (static_cast<size_t>(points[1]) << 32) +
                            (static_cast<size_t>(points[2]));
        return std::hash<size_t>()(points[0]) ^ std::hash<int32_t>()(sec_third);;
    }
};


using Scope = std::pair<int32_t, int32_t>;
using ScopeSet = std::unordered_set<std::array<int32_t, 3>, subtracted_hash>;

class Range
{
public:
    std::array<Scope, 3> scopes;

    bool operator==(const Range &rhs) const {
        return scopes == rhs.scopes;
    }

    bool operator!=(const Range &rhs) const {
        return !(rhs == *this);
    }

    bool isOverlappedScopes(const Range& other) const {
        for (int i = 0 ; i < scopes.size(); i ++) {
            if (!isOverlappedScope(this->scopes[i], other.scopes[i])) {
                return false;
            }
        }

        return true;
    }

    Range makeOverlappedRange(const Range& other) const {
        Range newRange;
        for (int i = 0 ; i < scopes.size(); i ++) {
            newRange.scopes[i] = makeOverlappedScope(this->scopes[i], other.scopes[i]);
        }
        return newRange;
    }

    bool isContain(const Range& other) const {
        for (int i = 0 ; i < scopes.size(); i ++) {
            if (!(this->scopes[i].first <= other.scopes[i].first && this->scopes[i].second >= other.scopes[i].second)) {
                return false;
            }
        }
        return true;
    }
private:
    bool isOverlappedScope(const Scope& thiz, const Scope& other) const {
        return (std::max(thiz.second, other.second) - std::min(thiz.first, other.first) <= ((thiz.second - thiz.first) + (other.second - other.first)));
    }

    Scope  makeOverlappedScope(const Scope& thiz, const Scope& other) const {
        Scope newScope;
        newScope.first = std::max(thiz.first, other.first);
        newScope.second = std::min(thiz.second, other.second);
        return newScope;
    }
};

class Step {
public:
    std::string operate;
    Range range;
};

class InputReader
{
public:
    static std::vector<Step> read()
    {
        std::ios_base::sync_with_stdio(false);
        freopen(INPUT_FILE_PATH, "rt", stdin);

        std::vector<Step> inputs;
        std::string input;

        while (!std::cin.eof())
        {
            getline(std::cin, input);
            if (input.empty())
            {
                continue;
            }

            Step step;
            char operate[4];
            sscanf(input.c_str(), "%s x=%d..%d,y=%d..%d,z=%d..%d",
                   operate,
                   &step.range.scopes[0].first, &step.range.scopes[0].second,
                   &step.range.scopes[1].first, &step.range.scopes[1].second,
                   &step.range.scopes[2].first, &step.range.scopes[2].second);

            step.operate = std::string(operate);
            inputs.push_back(step);
        }

        return inputs;
    }
};

class ScopeRule {
public:
    virtual bool isProcessable(const Range& scope) const = 0;
    virtual Range createNewScopeThatFitRule(const Range& scope) = 0;
};

class Counter {
public:
    size_t countAll() {
        total = 0;
        subtracted.clear();
        activeCubes.clear();

        for (const auto& step : steps) {
            if (!rule->isProcessable(step.range)) {
                continue;
            }

            auto newScope = rule->createNewScopeThatFitRule(step.range);
            if (step.operate == "on") {
                on(newScope);
            } else {
                off(newScope);
            }

            printf("%s, total : %lu\n", step.operate.c_str(), total);
        }

        return total;
    }

private:
    void on(const Range& range) {
        total += getTotalPossibilities(range);
        int index = activeCubes.size() - 1;

        ScopeSet overlappedSet;

        for (; index >= 0; index--) {
            const auto prevScope = activeCubes[index];
            if (!range.isOverlappedScopes(prevScope)) {
                continue;
            }

            auto overlappedRange = range.makeOverlappedRange(prevScope);

            addScopeSet(overlappedSet, overlappedRange.scopes);
        }

        subtractScopeSet(overlappedSet, subtracted);

        total -= overlappedSet.size();
        activeCubes.push_back(range);
    }

    void off(const Range& range) {
        auto prevSubtracted = subtracted.size();
        Range maxOverlappedSize;
        maxOverlappedSize.scopes[0] = {INT32_MAX, INT32_MAX};
        maxOverlappedSize.scopes[1] = {INT32_MAX, INT32_MAX};
        maxOverlappedSize.scopes[2] = {INT32_MAX, INT32_MAX};

        int index = activeCubes.size() - 1;
        for (; index >= 0; index--) {
            const auto prevScope = activeCubes[index];
            if (!range.isOverlappedScopes(prevScope)) {
                continue;
            }

            auto overlappedRange = range.makeOverlappedRange(prevScope);

            if (maxOverlappedSize.isContain(overlappedRange)) {
                continue;
            } else if (getTotalPossibilities(maxOverlappedSize) < getTotalPossibilities(overlappedRange)) {
                maxOverlappedSize = overlappedRange;
            }

            addScopeSet(subtracted, overlappedRange.scopes);
        }

        total -= ( subtracted.size() - prevSubtracted);
    }

    void subtractScopeSet(ScopeSet& left, ScopeSet& right) {

        for (auto it = right.begin(); it != right.end();) {
            if (left.find(*it) != left.end()) {
                auto found = left.find(*it);

                auto curIt = it;
                it++;

                left.erase(found);
                right.erase(curIt);
                continue;
            }
            it++;
        }
    }

    void addScopeSet(ScopeSet& scopeSet, std::array<Scope, 3>& scopes) {
        for (int32_t i = scopes[0].first; i <= scopes[0].second; i++) {
            for (int32_t j = scopes[1].first; j <= scopes[1].second; j++) {
                for (int32_t k = scopes[2].first; k <= scopes[2].second; k++) {
                    std::array<int32_t, 3> temp{i, j , k};
                    scopeSet.emplace(temp);
                }
            }
        }
    }

    size_t getTotalPossibilities(const Range& range) {
        return (getLength(range.scopes[0]) * getLength(range.scopes[1]) * getLength(range.scopes[2]));
    }

    size_t getLength(const Scope& scope) const {
        return (scope.second - scope.first + 1);
    }

public:
    void setScopeRule(ScopeRule *scopeRule) {
        this->rule = scopeRule;
    }

    void setInputSteps(const std::vector<Step> &inputSteps) {
        this->steps = inputSteps;
    }

private:
    size_t total;
    ScopeRule* rule;
    std::vector<Step> steps;
    std::vector<Range> activeCubes;

    ScopeSet subtracted;
};

class NoScopeRule : public ScopeRule {
public:
    bool isProcessable(const Range &scope) const override {
        return true;
    }

    Range createNewScopeThatFitRule(const Range &scope) override {
        return scope;
    }
};

class StartOneScopeRule : public ScopeRule {
public:
    explicit StartOneScopeRule() {
        fixedScope.scopes[0] = {-50, 50};
        fixedScope.scopes[1] = {-50, 50};
        fixedScope.scopes[2] = {-50, 50};
    }

    bool isProcessable(const Range &scope) const override {
        return fixedScope.isOverlappedScopes(scope);
    }

    Range createNewScopeThatFitRule(const Range &scope) override {
        return fixedScope.makeOverlappedRange(scope);
    }

private:
    Range fixedScope;
};

int main()
{

    auto inputs = InputReader::read();
    Counter counter;

    NoScopeRule noScopeRule;
    StartOneScopeRule startOneScopeRule;

    counter.setInputSteps(inputs);
//    counter.setScopeRule(&startOneScopeRule);
//    std::cout << "22-1: " << counter.countAll() << std::endl;

    counter.setScopeRule(&noScopeRule);
    std::cout << "22-2: " << counter.countAll() << std::endl;

}