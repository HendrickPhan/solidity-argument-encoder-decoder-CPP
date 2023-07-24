I. Giới thiệu
Encode tham số (Arguments) trong Solidity có nghĩa là chuyển đổi các tham số thành một chuỗi bytes để có thể gửi và xử lý trong hợp đồng thông minh. 
Điều này thường được thực hiện hàm "abi.encode" trong Solidity.
Hay như trong web3js chúng ta dùng "web3.eth.abi.encodeParameters".
Ví dụ:
```
pragma solidity ^0.8.0;

contract ExampleContract {
    function encodeParameters(uint256 number, string memory message) public pure returns (bytes memory) {
        return abi.encode(number, message);
    }
}
```

Ngược lại với encode ta có Decode. Decode là quá trình chúng ta biến đổi 1 chuỗi bytes trở về tham số với kiểu dữ liệu được định nghĩa.
Đê decode ta dùng hàm "abi.decode" trong Solidity. Và "web3.eth.abi.decodeParameters" với web3js

Ví dụ:
```
pragma solidity ^0.8.0;

contract ExampleContract {
    function decodeParameters(bytes memory encodedData) public pure returns (uint256, string memory) {
        (uint256 number, string memory message) = abi.decode(encodedData, (uint256, string));
        return (number, message);
    }
}
```

II. Đào sâu vào cơ chế
1. Các kiểu dữ liệu trong soldity
2. Cách encode hoạt động
3. Cách decode hoạt động
   
III. Tự xây dựng encode và decode
