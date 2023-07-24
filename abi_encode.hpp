#pragma once
#include "abi_utilities.hpp"

vector<uint8_t> encodeArgument(json abi, string argument);
vector<uint8_t> encodeArguments(vector<json> abi, vector<string> strArguments);

vector<uint8_t> encodeInt(
    string n
) {
    cout << "encodeInt: " << n << endl;
    vector<uint8_t> result(32, 0);
    vector<uint8_t> originBytes = hexStringToUint8Array(n);
    int start = 32 - originBytes.size();
    for(int i =0; i< originBytes.size(); i ++) {
        result[start + i] = originBytes[i]; 
        if(originBytes[i] != 0) {
            cout << "originBytes[i]" << originBytes[i] << endl;
        }
    }
    return result;
}

vector<uint8_t> encodeString (
    string b
) {
    char const *c = b.c_str();
    uint64_t length = b.size();
    // Calculate the padding needed to make the length divisible by 32
    uint64_t padding = (32 - (length % 32)) % 32;
    uint64_t totalLength = length + padding;

    vector<uint8_t> result(32 + totalLength, 0);

    // Set the first 32 bytes as the length of originBytes
    for (int i = 0; i < 8; ++i) {
        result[31 - i] = static_cast<uint8_t>(length >> (i * 8));
    }

    // Set the next bytes as the content of originBytes
    for (size_t i = 0; i < length; ++i) {
        result[i + 32] = c[i];
    }

    return result;
}

vector<uint8_t> encodeBool(
    string b
) {
    cout << "encodeBool" << endl;
    vector<uint8_t> result(32, 0);
    if (b == "true" || b == "True") {
        result[31] = 1;
    }
    return result;
}

// vector<uint8_t> encodeTuple(
//     json abi,
//     string a
// ) {
//     uint64_t totalSize = 0;
//     for (const auto& elementAbi : abi) {
//         totalSize += getTypeSize(elementAbi);
//     }
//     vector<uint8_t> additionData(0,0);
//     vector<uint8_t> result(0,0);
//     uint64_t idx = 0;
//      for (const auto& elementAbi : abi) {
//         cout << "encoding " << elementAbi << endl;
//         cout << "argument " << strArguments[idx] << endl;
//         // dynamic
//         if (isDynamicType(elementAbi)) {
//             vector<uint8_t> offset(32, 0);
//             uint64_t offsetUint = totalSize + additionData.size();
//             // Set the offset
//             for (int j = 0; j < 8; ++j) {
//                 offset[31 - j] = static_cast<uint8_t>(offsetUint >> (j * 8));
//             }
//             result.insert(result.end(), offset.begin(), offset.end());
//             // Set the data
//             vector<uint8_t> elementData = encodeArgument(elementAbi, strArguments[idx]);
//             additionData.insert(additionData.end(), elementData.begin(), elementData.end());
//         } else {
//             // static    
//             vector<uint8_t> encodeRs = encodeArgument(elementAbi, strArguments[idx]);
//             cout << "encode RS" << bytesToHexString(encodeRs.data(),  encodeRs.size()) << endl;
//             result.insert(result.end(), encodeRs.begin(), encodeRs.end());
//         }
//         idx += 1;
//     }
//     result.insert(result.end(), additionData.begin(), additionData.end());
//     return result;
// }

vector<uint8_t> encodeBytesSlice(const string& b) {
    vector<uint8_t> originBytes = hexStringToUint8Array(b);
    uint64_t length = originBytes.size();

    // Calculate the padding needed to make the length divisible by 32
    uint64_t padding = (32 - (length % 32)) % 32;
    uint64_t totalLength = length + padding;

    vector<uint8_t> result(32 + totalLength, 0);

    // Set the first 32 bytes as the length of originBytes
    for (int j = 0; j < 8; ++j) {
        result[31 - j] = static_cast<uint8_t>(length >> (j * 8));
    }

    // Set the next bytes as the content of originBytes
    for (size_t i = 0; i < length; ++i) {
        result[i + 32] = originBytes[i];
    }

    return result;
}

vector<uint8_t> encodeAddress(
    string a
) {
    vector<uint8_t> result(32, 0);
    vector<uint8_t> originBytes = hexStringToUint8Array(a);
    for (int i = 0; i < 20; i++) {
        result[12+i] = originBytes[i];
    }
    return result;
}

vector<uint8_t> encodeArray(
    json abi,
    string a
) {
    uint32_t arrayLen = getArrayLength(abi["type"]);

    json newAbi;
    newAbi["name"] = abi["name"];
    newAbi["type"] = removeArraySizeFromType(abi["type"]);
    newAbi["components"] = abi["components"];
    uint32_t elemSize = getTypeSize(newAbi);
    vector<string> arguments = splitStringArgument(a);
    // uint32_t totalSize = elemSize * arrayLen;
    // vector<uint8_t> result(totalSize, 0);
    if (isDynamicType(newAbi)) {
        vector<uint8_t> offsets(0,0);
        vector<uint8_t> data(0,0);
        uint32_t totalSize = elemSize * arrayLen;
        for(int i = 0 ; i< arrayLen; i++) {
            vector<uint8_t> offset(32, 0);
            uint64_t offsetUint = totalSize + data.size();
            // Set the offset
            for (int j = 0; j < 8; ++j) {
                offset[31 - j] = static_cast<uint8_t>(offsetUint >> (j * 8));
            }
            offsets.insert(offsets.end(), offset.begin(), offset.end());
            // Set the data
            vector<uint8_t> elementData = encodeArgument(newAbi, arguments[i]);
            data.insert(data.end(), elementData.begin(), elementData.end());
        }
        offsets.insert(offsets.end(), data.begin(), data.end());
        return offsets;
    } else {    
        vector<uint8_t> data(0,0);
        for(int i = 0 ; i< arrayLen; i++) {
            vector<uint8_t> elementData = encodeArgument(newAbi, arguments[i]);
            data.insert(data.end(), elementData.begin(), elementData.end());
        } 
        return data;
    }
}

vector<uint8_t> encodeSlice(
    json abi,
    string a
) {
    vector<string> arguments = splitStringArgument(a);
    uint64_t length = arguments.size();
    vector<uint8_t> result(32, 0);
    // Set the first 32 bytes as the length of slice
    for (int j = 0; j < 8; ++j) {
        result[31 - j] = static_cast<uint8_t>(length >> (j * 8));
    }

    json newAbi;
    newAbi["name"] = abi["name"];
    newAbi["type"] = removeArraySizeFromType(abi["type"]);
    newAbi["components"] = abi["components"];
    uint32_t elemSize = getTypeSize(newAbi);
    if (isDynamicType(newAbi)) {
        vector<uint8_t> offsets(0,0);
        vector<uint8_t> data(0,0);
        uint32_t totalSize = elemSize * length;
        for(int i = 0 ; i< length; i++) {
            vector<uint8_t> offset(32, 0);
            uint64_t offsetUint = totalSize + data.size();
            // Set the offset
            for (int j = 0; j < 8; ++j) {
                offset[31 - j] = static_cast<uint8_t>(offsetUint >> (j * 8));
            }
            offsets.insert(offsets.end(), offset.begin(), offset.end());
            // Set the data
            vector<uint8_t> elementData = encodeArgument(newAbi, arguments[i]);
            data.insert(data.end(), elementData.begin(), elementData.end());
        }
        offsets.insert(offsets.end(), data.begin(), data.end());
        result.insert(result.end(), offsets.begin(),offsets.end() );
    } else {    
        vector<uint8_t> data(0,0);
        for(int i = 0 ; i< length; i++) {
            vector<uint8_t> elementData = encodeArgument(newAbi, arguments[i]);
            data.insert(data.end(), elementData.begin(), elementData.end());
        } 
        result.insert(result.end(), data.begin(), data.end());
    }

    return result;
}

vector<uint8_t> encodeArgument(json abi, string argument)  {
    SolidityType t = getType(abi);
    cout << "encodeArgument type: " << t << endl;
    // Dynamic,
    if (t == TupleTy) {
        vector<string> arguments = splitStringArgument(argument);
        vector<json> tupleAbis = abi["components"];
        return encodeArguments(tupleAbis, arguments);
    }
    if (t == SliceTy) {
        return encodeSlice(abi, argument);
    }
    if (t == ArrayTy) {
        return encodeArray(abi, argument);
    }
    if (t == StringTy) {
        return encodeString(argument);
    }
    // Static
    if (t == BytesTy || t == FixedBytesTy) {
        return encodeBytesSlice(argument);
    }

    if (t == IntTy || t== UintTy) {
        return encodeInt(argument);
    }

    if (t == AddressTy) {
        return encodeAddress(argument);
    }

    if (t == BoolTy) {
        return encodeBool(argument);
    }
   
    return vector<uint8_t>{};
}


vector<uint8_t> encodeArguments(vector<json> abi, vector<string> strArguments) {
    uint64_t totalSize = 0;
    for (const auto& elementAbi : abi) {
        totalSize += getTypeSize(elementAbi);
    }
    vector<uint8_t> additionData(0,0);
    vector<uint8_t> result(0,0);
    uint64_t idx = 0;

    for (const auto& elementAbi : abi) {
        cout << "encoding " << elementAbi << endl;
        cout << "argument " << strArguments[idx] << endl;
        // dynamic
        if (isDynamicType(elementAbi)) {
            vector<uint8_t> offset(32, 0);
            uint64_t offsetUint = totalSize + additionData.size();
            // Set the offset
            for (int j = 0; j < 8; ++j) {
                offset[31 - j] = static_cast<uint8_t>(offsetUint >> (j * 8));
            }
            result.insert(result.end(), offset.begin(), offset.end());
            // Set the data
            vector<uint8_t> elementData = encodeArgument(elementAbi, strArguments[idx]);
            additionData.insert(additionData.end(), elementData.begin(), elementData.end());
        } else {
            // static    
            vector<uint8_t> encodeRs = encodeArgument(elementAbi, strArguments[idx]);
            cout << "encode RS" << bytesToHexString(encodeRs.data(),  encodeRs.size()) << endl;
            result.insert(result.end(), encodeRs.begin(), encodeRs.end());
        }
        idx += 1;
    }
    result.insert(result.end(), additionData.begin(), additionData.end());
    return result;
} 

vector<uint8_t> encode(string strAbi, string strArguments) {
    vector<string> arguments = splitStringArgument(strArguments);
    vector<json> abi = json::parse(strAbi);
    return encodeArguments(abi, arguments);
}