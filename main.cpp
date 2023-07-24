/*
 *   Copyright (c) 2023 hieuphanuit
 *   All rights reserved.
 */
#include "abi_decode.hpp"
#include "abi_encode.hpp"

int main() {
  string testABI = R"([
			{
				"internalType": "int256[]",
				"name": "g",
				"type": "int256[]"
			},
			{
				"internalType": "int256[2]",
				"name": "c",
				"type": "int256[2]"
			},
			{
				"internalType": "string[]",
				"name": "a",
				"type": "string[]"
			},
			{
				"internalType": "int256",
				"name": "b",
				"type": "int256"
			}
		])";

  std::string strArguments = "[[1,2,3],[1,2],[a,b],4]";

  vector<uint8_t> encoded = encode(testABI, strArguments);
  cout << "ENCODE: " << bytesToHexString(encoded.data(),  encoded.size()) << endl;

  json j = decode(encoded, testABI);
  std::cout << "DECODE: " << j.dump() << std::endl;
  
  return 0; 
}