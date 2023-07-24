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
#include "abi_utilities.hpp"

using namespace std;

json decodeElement(const uint8_t* bytes, uint32_t i, json abi);

json decodeString(const uint8_t* bytes, uint32_t i, json abi) {
    json result = json::object();
     cout << "DB String1:  " << bytesToHexString(bytes, 64)  << " i:" << i << endl;
    // Read the offset and length of the string

    uint32_t offset = 0;
    for (int j = 0; j < 32; j++) {
        offset <<= 8;
        offset |= bytes[i + j];
    }
    cout << "DB String Off set:  " << offset << endl;
    uint32_t length = 0;
    for (int j = 0; j < 32; j++) {
        length <<= 8;
        length |= bytes[offset + j];
    }
    cout << "DB String lengtth: " << bytesToHexString(bytes + offset, 32) << endl;
    const uint8_t* string_bytes = bytes + offset + 32; // 32 bytes for string length
    string element_str(string_bytes, string_bytes + length);
    cout << "DB String: " << offset << ":" << length << ":"<< bytesToHexString(string_bytes, length) << endl;
    cout << "DB String result1: " << element_str << endl;
   
    // Find the position of the NUL character
    size_t pos = element_str.find('\0');

    // Replace the NUL character with the escaped representation
    while (pos != string::npos) {
        element_str.replace(pos, 1, "");
        pos = element_str.find('\0', pos);
    }

    result = json::parse("\"" + element_str + "\"");
    cout << "DB String result2: " << result << endl;
    return result;
}

json decodeTuple(const uint8_t* bytes, uint32_t i, json abi) {
    SolidityType t = getType(abi);
    const uint8_t* tuple_bytes;
    cout << "DB decode tuple, type: " << t << endl;
    cout << "DB decode tuple, abi: " << abi << endl;
    if(isDynamicType(abi)){
        cout << "DB decode tuple 1" << endl;
        uint32_t offset = 0;
        for (int j = 0; j < 32; j++) {
            offset <<= 8;
            offset |= bytes[i + j];
        }
        tuple_bytes = bytes + offset;
    } else {
        cout << "DB decode tuple 2" << endl;
        tuple_bytes = bytes + i;
    }
    uint32_t aI = 0;
    cout << "DB decodeTuple: " <<  abi << endl;
    json result = json::object();
    for (const auto& tuple_element : abi["components"]) {
        result[string(tuple_element["name"])] = decodeElement(tuple_bytes, aI, tuple_element);
        aI += getTypeSize(tuple_element);
        cout << "DB decodeTuple pushing key: " <<  string(abi["name"]) << " Value: " << result[string(tuple_element["name"])] << endl;
    }
    return result;
}

vector<json> decodeArray(const uint8_t* bytes, uint32_t i, json abi) {
    cout << "DB array: " <<  abi << endl;
    vector<json> rs;
    uint32_t array_len = getArrayLength(abi["type"]);
    json newAbi;
    newAbi["name"] = abi["name"];
    newAbi["type"] = removeArraySizeFromType(abi["type"]);
    newAbi["components"] = abi["components"];
    uint32_t elemSize = getTypeSize(newAbi);
    const uint8_t* array_bytes;
    if (isDynamicType(newAbi)) {
        uint32_t offset = 0;
        for (int j = 0; j < 32; j++) {
            offset <<= 8;
            offset |= bytes[i + j];
        }
        array_bytes = bytes + offset;
    } else {
        array_bytes = bytes + i;
    }

    cout << "DB decodeArray array_len: " <<  array_len << endl;
    cout << "DB decodeArray abi: " <<  abi << endl;
    cout << "DB decodeArray newAbi: " <<  newAbi << endl;

    uint32_t aI = 0;
    for (uint32_t j = 0; j < array_len; j++) {
        json elem = decodeElement(array_bytes, aI, newAbi);
        rs.push_back(elem);
        cout << "DB decodeArray pushing key: " <<  string(abi["name"]) << " Value: " << elem << endl;
        cout << "DB decodeArray RS: " <<  rs << endl;
        cout << "DB decodeArray ABI: " <<  abi << endl;
        aI += getTypeSize(newAbi);
    }

    return rs;
}

vector<json> decodeSlice(const uint8_t* bytes, uint32_t i, json abi) {
    cout << "DB decodeSlice: " <<  abi << endl;
    vector<json> rs;
    uint32_t offset = 0;
    for (int j = 0; j < 32; j++) {
        offset <<= 8;
        offset |= bytes[i + j];
    }

    uint32_t array_len = 0;
    for (int j = 0; j < 32; j++) {
        array_len <<= 8;
        array_len |= bytes[offset + j];
    }
     cout << "DB decodeSlice: " <<  array_len << endl;

    json newAbi;
    newAbi["name"] = abi["name"];
    newAbi["type"] = removeArraySizeFromType(abi["type"]);
    newAbi["components"] = abi["components"];
    uint32_t aI = 0;

    const uint8_t* slice_bytes = bytes + offset + 32;
    for (uint32_t j = 0; j < array_len; j++) {
        json elem = decodeElement(slice_bytes, aI, newAbi);
        rs.push_back(elem);
        aI += getTypeSize(newAbi);
        cout << "DB decodeSlice pushing  Value: " << elem << endl;
    }
    cout << "aI: " << aI << endl;

    return rs;
}

json decodeInt(const uint8_t* bytes, uint32_t i, json abi) {
    const uint8_t* element_bytes = bytes + i;
    json result = json::object();
    result = bytesToHexString(element_bytes, 32);
    cout << "DB decodeInt:  " << bytesToHexString(element_bytes, 32) << endl;

    return result;
}

json decodeBool(const uint8_t* bytes, uint32_t i, json abi) {
    const uint8_t* element_bytes = bytes + i;
    json result = json::object();
    result = (element_bytes[31] == 1) ? "true" : "false";
    cout << "DB decodeBool:  " << result << endl;
    return result;
}

json decodeAddress(const uint8_t* bytes, uint32_t i, json abi) {
    const uint8_t* element_bytes = bytes + i;
    json result = json::object();
    result = bytesToHexString(element_bytes + 12, 20);
    cout << "DB decodeInt:  " << result << endl;
    return result;
}

json decodeBytes(const uint8_t* bytes, uint32_t i, json abi) {
    uint32_t length = 0;
    for (int j = 0; j < 32; j++) {
        length <<= 8;
        length |= bytes[i + j];
    }
    const uint8_t* element_bytes = bytes + i + 32; // 32 for length
    json result = bytesToHexString(element_bytes, length);
    return result;
}


json decodeElement(const uint8_t* bytes, uint32_t i, json abi) {
    cout << "I: " << i << endl;
    SolidityType t = getType(abi);
    cout << "Type: " << t << endl;
    json result = json::object();
    if (t == TupleTy) {
        result = decodeTuple(bytes, i, abi);
    }
    if (t == SliceTy) {
        result = decodeSlice(bytes, i, abi);
    }
    if (t == ArrayTy) {
        result = decodeArray(bytes, i, abi);
    }
    if (t == StringTy) {
        result = decodeString(bytes, i, abi);
    }
    if (t == IntTy) {
        result = decodeInt(bytes, i, abi);
    }
    if (t == BoolTy) {
        result = decodeBool(bytes, i, abi);
    }
    if (t == BytesTy) {
        result = decodeBytes(bytes, i, abi);
    }
    if (t == FixedBytesTy) {
        // same as int?
        result = decodeInt(bytes, i, abi);
    }
    if (t == AddressTy) {
        result = decodeAddress(bytes, i, abi);
    }
     cout << t << "result" << result.dump() << endl;
    return result;
}

json decode(vector<uint8_t> bytes, string strAbi) {
    vector<json> abi = json::parse(strAbi);
    json result = json::object();
    uint32_t i = 0;
    for (const auto& elementAbi : abi) {
        json element_value = decodeElement(bytes.data(), i, elementAbi);
        result[string(elementAbi["name"])] = element_value;
        i += getTypeSize(elementAbi);
        cout << "DB DONE i" << i << endl;
    }
    return result;
}
