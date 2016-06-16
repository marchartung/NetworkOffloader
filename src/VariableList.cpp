/*
 * VariableList.cpp
 *
 *  Created on: 18.04.2016
 *      Author: Marc Hartung
 */

#include "../include/VariableList.hpp"
#include "../include/network_impl/SimNetworkFunctions.hpp"
#include <ostream>
#include <string>

namespace NetOff
{
    void VariableList::addReal(const std::string& varName)
    {
        _vars[0].push_back(varName);
    }

    void VariableList::addInt(const std::string& varName)
    {
        _vars[1].push_back(varName);
    }

    void VariableList::addBool(const std::string& varName)
    {
        _vars[2].push_back(varName);
    }

    void VariableList::addReals(const std::vector<std::string>& varNames)
    {
        _vars[0].insert(_vars[0].end(), varNames.begin(), varNames.end());
    }

    void VariableList::addInts(const std::vector<std::string>& varNames)
    {
        _vars[1].insert(_vars[1].end(), varNames.begin(), varNames.end());
    }

    void VariableList::addBools(const std::vector<std::string>& varNames)
    {
        _vars[2].insert(_vars[2].end(), varNames.begin(), varNames.end());
    }

    const std::vector<std::string>& VariableList::getReals() const
    {
        return _vars[0];
    }

    const std::vector<std::string>& VariableList::getInts() const
    {
        return _vars[1];
    }

    const std::vector<std::string>& VariableList::getBools() const
    {
        return _vars[2];
    }

    size_t VariableList::dataSize() const
    {
        size_t res = 0;
        res += 3 * sizeof(size_t);  // safe data as: [numReal,numInt,numBool,[numChars,chars]]
        for (size_t i = 0; i < 3; ++i)
            for (size_t j = 0; j < _vars[i].size(); ++j)
                res += getStringDataSize(_vars[i][j]);
        return res;
    }

    VariableList::VariableList(const std::shared_ptr<char>& data)
    {
        _vars = getVariableListFromData(data.get())._vars;
    }

    VariableList::VariableList()
            : _vars(std::vector<std::vector<std::string>>(3))
    {
    }

    VariableList::VariableList(const std::vector<std::string>& realVars, const std::vector<std::string>& intVars, const std::vector<std::string>& boolVars)
            : _vars(std::vector<std::vector<std::string>>(3))
    {
        _vars[0] = realVars;
        _vars[1] = intVars;
        _vars[2] = boolVars;
    }

    bool VariableList::empty() const
    {
        return _vars[0].empty() && _vars[1].empty() && _vars[2].empty();
    }

    std::shared_ptr<const char> VariableList::data() const
    {
        std::shared_ptr<char> res(new char[dataSize()]);
        saveVariablesTo(res.get());
        return res;
    }

    std::shared_ptr<char> VariableList::data()
    {
        std::shared_ptr<char> res(new char[dataSize()]);
        saveVariablesTo(res.get());
        return res;
    }

    void VariableList::saveVariablesTo(char * data) const
    {
        // safe data as: [numReal,numInt,numBool,[numChars,chars]]
        char * curPos = data;
        for (size_t i = 0; i < 3; ++i)
        {
            curPos = saveShiftIntegralInData<size_t>(_vars[i].size(), curPos);
        }
        for (size_t i = 0; i < 3; ++i)
            for (size_t j = 0; j < _vars[i].size(); ++j)
            {
                saveStringInData(_vars[i][j], curPos);
                curPos += getStringDataSize(_vars[i][j]);
            }
    }

    VariableList VariableList::getVariableListFromData(const char * data)
    {
        VariableList res;
        std::vector<std::vector<std::string>> & vars = res._vars;

        vars = std::vector<std::vector<std::string>>(3);
        const char * curPos = data;
        for (size_t i = 0; i < 3; ++i)
        {
            vars[i] = std::vector<std::string>(getIntegralFromData<size_t>(curPos));
            curPos = shift<size_t>(curPos);
        }
        for (size_t i = 0; i < 3; ++i)
            for (size_t j = 0; j < vars[i].size(); ++j)
            {
                vars[i][j] = createStringFromData(curPos);
                curPos += getStringDataSize(vars[i][j]);
            }
        return res;
    }

    std::ostream & operator<<(std::ostream & out, const VariableList & in)
    {
        out << "Real:[";
        for (auto & str : in.getReals())
        {
            out << str << " ";
        }
        out << "]Ints:[";
        for (auto & str : in.getInts())
        {
            out << str << " ";
        }
        out << "]Bools:[";
        for (auto & str : in.getBools())
        {
            out << str << " ";
        }
        out << "]";
        return out;
    }

    bool VariableList::isSubsetOf(const VariableList& in) const
    {
        bool abort = true;
        for (size_t i = 0; i < _vars.size(); ++i)
        {
            for (const auto & str1 : _vars[i])
            {
                bool abort = true;
                for (const auto & str2 : in._vars[i])
                    if (str1 == str2)
                    {
                        abort = false;
                        break;
                    }

                if (abort)
                    return false;
            }
        }
        return true;
    }

}  // End namespace
