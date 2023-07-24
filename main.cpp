/*
 *   Copyright (c) 2023 hieuphanuit
 *   All rights reserved.
 */
#include "abi_decode.hpp"
#include "abi_encode.hpp"

int main() {
  string testABI = R"([
			{
				"internalType": "string",
				"name": "name",
				"type": "string"
			},
			{
				"internalType": "address",
				"name": "owner",
				"type": "address"
			},
			{
				"internalType": "uint256",
				"name": "duration",
				"type": "uint256"
			},
			{
				"internalType": "bytes32",
				"name": "secret",
				"type": "bytes32"
			},
			{
				"internalType": "address",
				"name": "resolver",
				"type": "address"
			},
			{
				"internalType": "bytes[]",
				"name": "data",
				"type": "bytes[]"
			},
			{
				"internalType": "bool",
				"name": "reverseRecord",
				"type": "bool"
			},
			{
				"internalType": "uint16",
				"name": "ownerControlledFuses",
				"type": "uint16"
			}
		])";

  std::string strArguments = "[abcd,573a0c01345460e11a26c5d822b97864f079cea3,1e13380,0xbb67f296fe72923885ea40d6d557c8ee033d5bbfd58ab1130810d9dcb0682967,fd564d61e269f9d7f8E140Cd004019d490922e2b,[0x8b95dd715a8945770fa6038a085dac1514952060044ab30f69d18a7a199af44d3a9c437500000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000060000000000000000000000000000000000000000000000000000000000000001976a91462e907b15cbf27d5425399ebf6f0fb50ebb88f1888ac00000000000000,0x8b95dd715a8945770fa6038a085dac1514952060044ab30f69d18a7a199af44d3a9c437500000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000060000000000000000000000000000000000000000000000000000000000000001976a91462e907b15cbf27d5425399ebf6f0fb50ebb88f1888ac00000000000000],false,0]";

  vector<uint8_t> encoded = encode(testABI, strArguments);
  cout << "ENCODE: " << bytesToHexString(encoded.data(),  encoded.size()) << endl;

  json j = decode(encoded, testABI);
  std::cout << "DECODE: " << j.dump() << std::endl;
  
  return 0; 
}