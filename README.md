# VietThang5605/ChessEngine

[![GitHub Stars](https://img.shields.io/github/stars/VietThang5605/ChessEngine?style=social)](https://github.com/VietThang5605/ChessEngine/stargazers)
[![GitHub Forks](https://img.shields.io/github/forks/VietThang5605/ChessEngine?style=social)](https://github.com/VietThang5605/ChessEngine/network/members)
[![GitHub Issues](https://img.shields.io/github/issues/VietThang5605/ChessEngine)](https://github.com/VietThang5605/ChessEngine/issues)
[![License](https://img.shields.io/github/license/VietThang5605/ChessEngine)](LICENSE) Một engine chơi cờ mạnh mẽ và linh hoạt được phát triển bởi VietThang5605.

## Mục lục

- [Giới thiệu](#giới-thiệu)
- [Tính năng](#tính-năng)
- [Cài đặt](#cài-đặt)
- [Cách sử dụng](#cách-sử-dụng)
- [Đóng góp](#đóng-góp)
- [Giấy phép](#giấy-phép)
- [Liên hệ](#liên-hệ)

## Giới thiệu

**ChessEngine** là một dự án mã nguồn mở nhằm mục đích xây dựng một engine chơi cờ có khả năng phân tích và đưa ra các nước đi tối ưu. Dự án này được phát triển với sự tập trung vào hiệu suất, khả năng mở rộng và dễ dàng tích hợp vào các ứng dụng cờ vua khác. Cho dù bạn là một nhà phát triển quan tâm đến trí tuệ nhân tạo và thuật toán tìm kiếm, hay chỉ đơn giản là một người yêu thích cờ vua muốn khám phá cách một engine hoạt động, ChessEngine đều mang đến những điều thú vị.

## Tính năng

ChessEngine hiện tại (hoặc sẽ có) các tính năng sau:

* **Đại diện bàn cờ hiệu quả:** Sử dụng cấu trúc dữ liệu tối ưu để biểu diễn trạng thái của ván cờ.
* **Tạo nước đi hợp lệ:** Có khả năng tạo ra tất cả các nước đi hợp lệ từ một vị trí nhất định.
* **Đánh giá vị trí:** Triển khai một hàm đánh giá để ước tính giá trị của một vị trí cờ.
* **Thuật toán tìm kiếm:**
    * [ ] Thuật toán Minimax cơ bản.
    * [ ] Cắt tỉa Alpha-Beta để tăng tốc độ tìm kiếm.
    * [ ] (Có thể có) Các thuật toán tìm kiếm nâng cao hơn như MCTS.
* **Hỗ trợ các quy tắc cờ vua tiêu chuẩn:** Bao gồm các nước đi đặc biệt như nhập thành, phong cấp tốt, và bắt tốt qua đường.
* **Giao diện dòng lệnh (CLI):** Cho phép người dùng tương tác trực tiếp với engine.
* **Khả năng tích hợp (API):** Thiết kế để dễ dàng tích hợp vào các ứng dụng cờ vua khác (ví dụ: giao diện người dùng đồ họa).
* **(Có thể có)** Hỗ trợ các biến thể cờ vua.
* **(Có thể có)** Sách khai cuộc và cơ sở dữ liệu end game.

## Cài đặt

Hướng dẫn cài đặt engine cờ vua của bạn. Điều này có thể bao gồm các bước tải xuống, các phụ thuộc cần thiết và cách biên dịch (nếu cần).

Ví dụ:

1.  Sao chép kho lưu trữ:
    ```bash
    git clone [https://github.com/VietThang5605/ChessEngine.git](https://github.com/VietThang5605/ChessEngine.git)
    cd ChessEngine
    ```
2.  (Nếu cần biên dịch) Cài đặt các phụ thuộc (ví dụ: trình biên dịch C++, CMake):
    ```bash
    # Ví dụ cho Ubuntu
    sudo apt-get update
    sudo apt-get install g++ cmake
    ```
3.  (Nếu cần biên dịch) Biên dịch dự án:
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

## Cách sử dụng

Hướng dẫn cách chạy engine và tương tác với nó. Nếu có giao diện dòng lệnh, hãy cung cấp ví dụ về các lệnh. Nếu có API, hãy cung cấp ví dụ về cách tích hợp nó.

Ví dụ (cho giao diện dòng lệnh):

1.  Chạy engine:
    ```bash
    ./chess_engine # hoặc đường dẫn đến file thực thi
    ```
2.  Nhập các lệnh (ví dụ: `uci`, `position startpos`, `go depth 5`):
    ```
    uci
    # Engine sẽ trả về thông tin của nó
    position startpos
    go depth 5
    # Engine sẽ tính toán và trả về nước đi tốt nhất
    ```

Ví dụ (cho API - tùy thuộc vào ngôn ngữ và thiết kế):

```python
# Ví dụ Python (chỉ mang tính minh họa)
from chess_engine import ChessEngine

engine = ChessEngine()
engine.set_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
best_move = engine.get_best_move(depth=5)
print(f"Nước đi tốt nhất: {best_move}")
