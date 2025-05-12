# Unstoppable Evaluation Tool (UET)


Một engine cờ mạnh mẽ và hiệu quả, được viết bằng C++.

## Tổng quan

Unstoppable Evaluation Tool (UET) là một engine cờ được thiết kế để đạt hiệu suất cao và phân tích chính xác các thế cờ. Nó kết hợp các thuật toán tìm kiếm hiện đại với một hàm đánh giá được điều chỉnh tỉ mỉ để chơi cờ ở cấp độ cao. Engine này tuân theo giao thức UCI (Universal Chess Interface) để tương thích với nhiều giao diện cờ khác nhau.

## Các tính năng chính

* **Giao thức UCI (Universal Chess Interface):**
    * UET giao tiếp với các GUI cờ (ví dụ: Cute Chess, Arena, En Croissant) bằng giao thức UCI.
    * UCI định nghĩa một tập hợp các lệnh chuẩn để trao đổi thông tin về ván cờ, các tùy chọn của engine và kết quả tìm kiếm.
    * `uci.cpp` xử lý việc phân tích cú pháp các lệnh UCI và định dạng đầu ra của engine.
* **Tìm kiếm Alpha-Beta:**
    * Thuật toán tìm kiếm cốt lõi của engine.
    * Đệ quy khám phá cây trò chơi, cắt tỉa các nhánh không hứa hẹn để giảm số lượng thế cờ cần đánh giá.
    * Việc cắt tỉa dựa trên các giá trị alpha (điểm số tối thiểu mà bên trắng đảm bảo) và beta (điểm số tối đa mà bên đen đảm bảo).
* **Tìm kiếm Quiescence:**
    * Đệ quy tìm kiếm tất cả các nước bắt quân liên tiếp.
    * Tránh hiện tượng đường chân trời (Horizon effect) khi đánh giá thế cờ vẫn còn nước bắt quân.
* **Null Move Pruning:**
    * Thử thực hiện nước đi rỗng (chuyển lượt cho đối thủ mà không di chuyển quân nào)
    * Nếu kết quả tìm kiếm của nước đi này cho thấy vị trí vẫn “quá tốt” cho bên thực hiện nước đi rỗng, khả năng cắt tỉa beta là rất cao.
    * Trả ngay về giá trị beta và bỏ qua việc tìm kiếm các nước đi hợp lệ còn lại từ vị trí gốc
* **Sắp xếp nước đi:**
    * Các heuristics để ưu tiên các nước đi có khả năng cao dẫn đến thế cờ tốt.
    * Ví dụ:
        * MVV-LVA (Most Valuable Victim - Least Valuable Attacker): Ưu tiên các nước đi bắt quân mạnh bằng quân yếu.
        * Nước đi sát thủ (Killer moves): Lưu trữ các nước đi gây ra cắt tỉa beta và khám phá chúng trước.
        * Lịch sử nước đi: Đếm tần suất một nước đi dẫn đến cải thiện điểm số.
* **Late Move Reduction:**
    * Các nước đi được tìm kiếm sẽ sắp xếp theo điểm giảm dần,
    * Giảm độ sâu tìm kiếm dành cho các nước đi không hứa hẹn (có điểm thấp hơn),
    * Nếu kết quả trả về lớn hơn alpha thì ta tìm kiếm lại nước này với độ sâu tối đa.
* **Mở rộng khi Chiếu Tướng (Check Extension):**
    * Khi tìm kiếm một nước đi mà vua bị chiếu tướng thì sẽ mở rộng độ sâu tìm kiếm.
    * Thoát khỏi nước chiếu là “bắt buộc” và việc mở rộng tìm kiếm có thể phát hiện các đòn chiến thuật bất ngờ hay tìm được nước chiếu hết sớm hơn.
* **Pawn Promotion Extension:**
    * Khi một quân tốt tiến đến hàng ngang số 7 (đối với Trắng) hoặc hàng ngang số 2 (đối với Đen) thì ta sẽ mở rộng độ sâu tìm kiếm.
    * Tốt ở hàng ngang số 7 là một mối đe doạ nguy cấp, đe doạ sẽ phong cấp ngay lập tức.
    * Việc tìm kiếm sâu hơn sẽ cho khả năng về các đòn chiến thuật sau khi phong cấp, hoặc cách đổi thủ có thể phản ứng lại.
* **Tìm kiếm sâu dần (Iterative Deepening):**
    * Tìm kiếm ở độ sâu 1, sau đó 2, 3, v.v., lặp lại cho đến khi hết thời gian.
    * Cho phép engine trả về nước đi tốt nhất tìm được trong thời gian giới hạn.
    * Cải thiện việc sắp xếp nước đi ở các độ sâu nông, dẫn đến cắt tỉa hiệu quả hơn ở các độ sâu sâu.
* **Bảng băm hoán vị (Transposition Table):**
    * Lưu trữ kết quả đánh giá của các thế cờ đã gặp trước đó để tránh tính toán lại.
    * Sử dụng băm Zobrist để tạo ra các khóa duy nhất cho các thế cờ.
    * Cải thiện đáng kể hiệu suất tìm kiếm.
* **Đa luồng:**
    * Sử dụng thư viện `tinycthread` để thực hiện tìm kiếm song song trên nhiều luồng CPU.
    * Chia cây trò chơi thành các phần và tìm kiếm chúng đồng thời.
    * Yêu cầu quản lý cẩn thận để tránh xung đột dữ liệu.
* **Đánh giá nâng cao:**
    * **Lực lượng:** Tính tổng giá trị của các quân cờ.
    * **Cấu trúc tốt:**
        * Tốt cô lập: `pawn_analysis.cpp` xác định các tốt không có tốt đồng minh trên các cột lân cận.
        * Tốt thông: `pawn_analysis.cpp` xác định các tốt không bị cản trở bởi tốt đối phương trên đường đi của chúng.
        * Tốt chồng: `pawn_analysis.cpp` phát hiện các tốt cùng màu trên cùng một cột.
    * **Vị trí quân:**
        * Các bảng giá trị ô (piece-square tables) trong `evaluate.cpp` gán điểm thưởng hoặc phạt cho quân đứng ở các ô cụ thể.
    * **An toàn của vua:**
        * `king_safety.cpp` đánh giá mức độ được bảo vệ của vua, xét đến các tốt đồng minh và sự hiện diện của tốt đối phương.
    * **Giai đoạn của ván cờ:**
        * Hàm `calculate_game_phase()` trong `evaluate.cpp` ước tính xem ván cờ đang ở giai đoạn khai cuộc, trung cuộc hay tàn cuộc.
        * Các yếu tố đánh giá khác nhau được kết hợp với trọng số khác nhau tùy thuộc vào giai đoạn.
* **Khai cuộc:**
    * `polybook.cpp` triển khai chức năng chơi khai cuộc bằng sách khai cuộc.
    * Sách khai cuộc chứa các nước đi đã được tính toán trước để chơi tốt trong giai đoạn đầu của ván cờ.

## Xây dựng

Để xây dựng UET từ mã nguồn, bạn cần có một trình biên dịch C++ (ví dụ: GCC, Clang, MSVC) và các công cụ phát triển cần thiết.

**Hướng dẫn chung Windows:**

1.  Mở terminal và điều hướng đến thư mục chứa mã nguồn.
2.  Sử dụng lệnh sau để biên dịch:

    ```bash
    g++ -std=c++17 *.cpp -pthread -O2 -o uet
    ```

    * `-o uet`: Đặt tên cho file thực thi là "uet".
    * `*.cpp`: Biên dịch tất cả các file .cpp trong thư mục hiện tại.
    * `-std=c++17`: Yêu cầu trình biên dịch hỗ trợ các tính năng của C++17.


**Lưu ý quan trọng:**

* Đảm bảo bạn đã cài đặt các công cụ phát triển cần thiết (ví dụ: build-essential trên Linux, Xcode Command Line Tools trên macOS, Visual Studio Build Tools trên Windows).
* Các tùy chọn biên dịch có thể cần được điều chỉnh tùy thuộc vào trình biên dịch và hệ thống của bạn.

## Sử dụng

1.  Biên dịch engine thành công.
2.  Chạy file thực thi từ terminal:

    ```bash
    ./uet  # Linux/macOS
    uet.exe  # Windows
    
    ```

3.  Mở một GUI cờ tương thích với UCI (ví dụ: Cute Chess, Arena, Fritz, En Croissant).
4.  Trong GUI, thêm engine UET. Bạn sẽ cần chỉ định đường dẫn đến file thực thi của UET.
5.  GUI sẽ gửi các lệnh UCI đến engine và hiển thị các nước đi của engine.

## Cấu trúc mã nguồn chi tiết

* `src/`: Thư mục chứa mã nguồn của engine.
    * `board.cpp/h`:
        * Định nghĩa cấu trúc `S_BOARD` để biểu diễn bàn cờ.
        * Triển khai các hàm để tạo, sao chép và thao tác bàn cờ.
        * Sử dụng mảng 120 ô và bitboard để tối ưu hóa hiệu suất.
    * `evaluate.cpp/h`:
        * Chứa hàm `EvalPosition()` để đánh giá một thế cờ.
        * Triển khai các hàm đánh giá các yếu tố khác nhau (lực lượng, cấu trúc tốt, vị trí quân, an toàn của vua).
        * Sử dụng các bảng giá trị ô và tính toán giai đoạn của ván cờ.
    * `movegen.cpp/h`:
        * Triển khai các hàm để sinh tất cả các nước đi hợp lệ (`GenerateAllMoves()`) và các nước đi bắt quân (`GenerateAllCaptures()`).
        * Sử dụng các bảng tính sẵn để tăng tốc độ sinh nước đi.
        * Sắp xếp nước đi để ưu tiên các nước đi tốt.
    * `search.cpp/h`:
        * Triển khai thuật toán tìm kiếm Alpha-Beta (`AlphaBeta()`).
        * Xử lý tìm kiếm sâu dần, cắt tỉa null-move và các kỹ thuật tìm kiếm khác.
        * Quản lý đa luồng để tìm kiếm song song.
    * `uci.cpp/h`:
        * Xử lý giao tiếp với GUI cờ thông qua giao thức UCI.
        * Phân tích cú pháp các lệnh UCI và gọi các hàm tương ứng của engine.
        * `Uci_Loop()` là hàm chính để nhận và xử lý các lệnh UCI.
    * `types.h`:
        * Định nghĩa các kiểu dữ liệu cơ bản được sử dụng trong toàn bộ engine (ví dụ: `Color`, `Square`, `Piece`).
    * `tinycthread.cpp/h`:
        * Cung cấp một API đa luồng nhỏ gọn và tương thích với nhiều nền tảng.
    * `polybook.cpp/h`:
        * Triển khai chức năng chơi khai cuộc bằng sách khai cuộc.
        * Đọc các nước đi khai cuộc từ một file.
    * `pawn_analysis.cpp/h`:
        * Phân tích cấu trúc tốt (tốt cô lập, tốt thông, v.v.).
        * Tính toán các điểm số liên quan đến cấu trúc tốt.
    * `king_safety.cpp/h`:
        * Đánh giá mức độ an toàn của vua.
        * Xem xét các tốt đồng minh xung quanh vua và sự hiện diện của tốt đối phương.
    * `init.cpp/h`:
        * Khởi tạo các bảng và biến toàn cục.
        * `AllInit()` là hàm chính để gọi tất cả các hàm khởi tạo khác.
    * `io.cpp/h`:
        * Các hàm tiện ích cho đầu vào/đầu ra, chẳng hạn như in bàn cờ và chuyển đổi giữa các định dạng nước đi.
    * `makemove.cpp/h`:
        * Triển khai các hàm để thực hiện và hoàn tác các nước đi trên bàn cờ.
        * Cập nhật các cấu trúc dữ liệu của bàn cờ sau khi thực hiện nước đi.
    * `misc.cpp/h`:
        * Các hàm tiện ích khác, chẳng hạn như lấy thời gian hiện tại.
    * `validate.cpp/h`:
        * Các hàm để kiểm tra tính hợp lệ của bàn cờ và các cấu trúc dữ liệu khác (dùng cho debug).
    * `zobrist.cpp/h`:
        * Triển khai băm Zobrist để tạo ra các khóa duy nhất cho các thế cờ.

## Đóng góp

Nếu bạn muốn đóng góp vào dự án UET, bạn có thể thực hiện theo những cách sau:

* **Báo cáo lỗi:** Nếu bạn tìm thấy bất kỳ lỗi nào, vui lòng tạo một issue trên GitHub với các bước tái tạo và thông tin chi tiết.
* **Gửi pull request:** Nếu bạn muốn sửa lỗi hoặc thêm một tính năng mới, bạn có thể tạo một nhánh (branch), thực hiện các thay đổi của mình và gửi một pull request. Vui lòng đảm bảo rằng mã của bạn tuân theo các quy ước mã hóa của dự án và có các bài kiểm tra thích hợp.
* **Đóng góp ý tưởng:** Chia sẻ ý tưởng của bạn về các tính năng mới, cải tiến hiệu suất hoặc bất kỳ khía cạnh nào khác của dự án.

## Link video demo

https://drive.google.com/drive/folders/1TFfjM-N0hLyvdCUQEcegxqEGekKAQfK7?usp=sharing
https://lichess.org/@/uet69/all

## Tài liệu tham khảo

   * `Stockfish11`:
        * https://github.com/official-stockfish/Stockfish.
   * `Vice engine`:
        * https://github.com/bluefeversoft/vice?tab=readme-ov-file.
   * `Chess programing wiki`:
        * https://www.chessprogramming.org/Main_Page.

## Lời cảm ơn

Dự án UET được xây dựng dựa trên những ý tưởng và kỹ thuật từ nhiều engine cờ mã nguồn mở khác. Chúng tôi xin bày tỏ lòng biết ơn đến các nhà phát triển và cộng đồng cờ vua đã đóng góp vào sự phát triển của lĩnh vực này.
