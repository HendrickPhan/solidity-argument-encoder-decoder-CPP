/*
 *   Copyright (c) 2023 hieuphanuit
 *   All rights reserved.
 */
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "json.hpp"
#include <sstream>
using namespace std;


using json = nlohmann::json;

enum SolidityType {
    IntTy,
    UintTy,
    BoolTy,
    StringTy,
    SliceTy,
    ArrayTy,
    TupleTy,
    AddressTy,
    FixedBytesTy,
    BytesTy,
    HashTy,
    FixedPointTy,
    FunctionTy,
};

string bytesToHexString(const uint8_t* bytes, const uint32_t len) {
    stringstream ss;
    ss << hex << setfill('0');
    for (uint32_t i = 0; i < len; i++) {
        ss << setw(2) << static_cast<unsigned int>(bytes[i]);
    }
    return ss.str();
}

string removeArraySizeFromType(string type) {
    size_t pos = type.rfind("[");
    if (pos == string::npos) {
        // No array size found, return the original string
        return type;
    } else {
        // Remove the array size and any following dimensions
        return type.substr(0, pos) + type.substr(type.rfind("]")+1);
    }
}

json getArrayElementAbi(json abi) {
    json newAbi;
    newAbi["name"] = abi["name"];
    newAbi["type"] = removeArraySizeFromType(abi["type"]);
    newAbi["components"] = abi["components"];
    return newAbi;
}

SolidityType getType(json element) {
    string type = element["type"];
    size_t pos = type.rfind("[");
    if (pos != string::npos) {
        size_t end_pos = type.find("]", pos);
        string count_str = type.substr(pos+1, end_pos-pos-1);
        if(count_str == "") {
            return SliceTy;
        }
        return ArrayTy;
    }
    if (type == "tuple") {
        return TupleTy;
    }
    if (type == "string") {
        return StringTy;
    }
    if (type == "bool") {
        return BoolTy;
    }
    if (type == "address") {
        return AddressTy;
    }
    if (type == "bytes") {
        return BytesTy;
    }
    size_t posBytes = type.rfind("bytes");
    if (posBytes != string::npos) {
        return FixedBytesTy;
    }
    size_t posInt = type.rfind("int");
    if (posInt != string::npos) {
        return IntTy;
    }
    // default is IntTy, i dont check orther type cuz all of them is encode to 32 byte so just return IntTy
    return IntTy;
}

uint32_t getArrayLength(string type) {
    size_t pos = type.rfind("[");
    size_t end_pos = type.find("]", pos);
    string count_str = type.substr(pos+1, end_pos-pos-1);
    uint32_t count = 0;
    try {
        count = stoi(count_str);
    } catch (...) {
        throw invalid_argument("Invalid type string");
    }
    return count;
}


// isDynamicType returns true if the type is dynamic.
// The following types are called “dynamic”:
// * bytes
// * string
// * T[] for any T
// * T[k] for any dynamic T and any k >= 0
// * (T1,...,Tk) if Ti is dynamic for some 1 <= i <= k
bool isDynamicType(json abi)  {
    cout << "DB::: XX2" << abi << endl;
    SolidityType type = getType(abi);
    if (type == TupleTy) {
		for (const auto& tuple_element : abi["components"]) {
			if (isDynamicType(tuple_element)) {
				return true;
			}
		}
		return false;
	}
    if (type == ArrayTy) {
        json elemAbi = getArrayElementAbi(abi);
        return isDynamicType(elemAbi);
    }

	return (type == StringTy || type == BytesTy || type == SliceTy);
}

uint32_t getTypeSize(json abi) {

    SolidityType type = getType(abi);
    cout << "XX2" << abi << endl;
    cout << "XX3" << type << endl;

    if (type == ArrayTy){
        json elemAbi = getArrayElementAbi(abi);
        if (!isDynamicType(elemAbi)) {
            int size = getArrayLength(abi["type"]);
            SolidityType elemType = getType(elemAbi);
            // // Recursively calculate type size if it is a nested array
            if (elemType == ArrayTy || elemType == TupleTy) {
                return size * getTypeSize(elemAbi);
            }
            return size * 32;
        } else {
            return 32;
        }
    } 
    if (type == TupleTy){
        cout << "XX4" << abi << endl;
       if (!isDynamicType(abi)) {
            int total = 0;
            for (const auto& tuple_element : abi["components"]) {
                cout << "XX5.1" << tuple_element << endl;
                cout << "XX5" << getTypeSize(tuple_element) << endl;
                total += getTypeSize(tuple_element);
            }
            return total;
        } else {
            cout << "XX6"  << endl;

            return 32;
        }
	} 
    cout << "XX7"  << endl;
    return 32;
}


vector<uint8_t> hexStringToUint8Array(const string& hexStr) {
    string processedHexStr = hexStr;

    // Convert hex string to lowercase
    transform(processedHexStr.begin(), processedHexStr.end(), processedHexStr.begin(), ::tolower);

    // Remove "0x" prefix if present
    if (processedHexStr.substr(0, 2) == "0x") {
        processedHexStr = processedHexStr.substr(2);
    }
    cout << "hexStringToUint8Array: " << processedHexStr << endl;
    vector<uint8_t> result;
    result.reserve(processedHexStr.size() / 2);

    for (size_t i = 0; i < processedHexStr.size(); i += 2) {
        uint8_t byte = static_cast<uint8_t>(stoi(processedHexStr.substr(i, 2), nullptr, 16));
        result.push_back(byte);
    }
    return result;
}

std::vector<std::string> splitStringArgument(const std::string& input) {
    std::vector<std::string> result;
    std::string processedInput= input.substr(1, input.length() - 2);
    int trackVar = 0;
    int from = 0;

    for (std::size_t i = 0; i < processedInput.length(); ++i) {
        char c = processedInput[i];

        if (c == '[') {
            trackVar++;
        } else if (c == ']') {
            trackVar--;
        } else if (c == ',' && trackVar == 0) {
            result.push_back(processedInput.substr(from, i - from));
            from = i + 1;
        }
    }

    result.push_back(processedInput.substr(from));

    return result;
}
