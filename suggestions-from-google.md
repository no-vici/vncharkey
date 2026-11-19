Cảm ơn bạn đã gửi toàn bộ codebase của VnCharKey (và đã ghi nhận tình trạng VnCharKey cũng gặp lỗi tương tự OpenViet khi chữ đ đi với nguyên âm đơn). [1]
Qua kiểm tra trực tiếp mã nguồn Rust và C++ của VnCharKey, lỗi khi gõ d9a4 (đã), d9e63 (để) hay bất kỳ từ nào có đ đứng trước 1 nguyên âm đơn (đó, đủ, đi, đè...) xuất phát từ 2 điểm kẹt cụ thể trong crates/vncharkey-core/src/engine.rs và fcitx5-addon/src/vncharkey_addon.cpp: [1]

1. Nguyên nhân gốc rễ trong VnCharKey


Lỗi A: alphanumeric_trap trong engine.rs (Dòng 848) chặn đứng mọi từ VNI gõ d9 + nguyên âm

Hãy nhìn vào logic trong crates/vncharkey-core/src/engine.rs: [1]
1. Khi bạn gõ d -> 9 (đ), biến has_prior_digits là true. [1]
2. Ngay khi bạn gõ tiếp nguyên âm đơn a (hoặc e, o, u, i), tại dòng 1236 của engine.rs:rust
if has_prior_digits && ch.is_ascii_alphabetic() && !was_transformed {
    self.has_letter_after_digit = true;
}
Use code with caution.
Vì chữ a không tự biến đổi (was_transformed == false), cờ self.has_letter_after_digit bị bật thành true! [1]
3. Sau đó bạn gõ 4 (hoặc 63) để bỏ dấu thành đã (để), rồi bấm phím Space. Tại dòng 848 của engine.rs, điều kiện alphanumeric_trap chạy:rust
let alphanumeric_trap = self.has_letter_after_digit
    && !self.current_word.ends_with(|c: char| {
        matches!(c, 'c' | 'k' | 'm' | 'n' | 'p' | 't' | 'g' | 'h' | 'i' | 'o' | 'u' | 'y')
    });
Use code with caution.

	• Cái bẫy nằm ở đây: Danh sách ký tự cuối được miễn trừ ('c' | 'k' | 'm' | ...) chỉ toàn chữ cái không dấu!
	• Nếu từ có phụ âm cuối (như được kết thúc bằng c, đường kết thúc bằng g) hoặc nguyên âm đôi (đâu kết thúc bằng u), ends_with trả về true nên alphanumeric_trap là false → gõ bình thường.
	• Nhưng nếu từ là đ + 1 nguyên âm đơn có dấu (đã, để, đó, đủ, đĩ), ký tự cuối cùng của self.current_word là 'ã', 'ể', 'ó', 'ủ', 'ĩ' — không nằm trong danh sách ASCII trên!
	• Kết quả: alphanumeric_trap bật thành true và ép engine.rs khôi phục ngược đã thành d9a4 và để thành d9e63 ngay khi bấm Space! [1]

Lỗi B: SelectAndReplaceWeb (Shift + Left Arrow) gây nhảy chữ trên Linux/Wayland (vncharkey_addon.cpp)

Nếu hiện tượng nhảy chữ loạn xạ xảy ra ngay trong lúc đang gõ d9 -> a -> 4 trên trình duyệt hoặc Electron app, thủ phạm nằm ở fcitx5-addon/src/vncharkey_addon.cpp (dòng 1043): [1]
• Khi config_select_and_replace_web_ bật (mặc định là true), mỗi lần sửa dấu (đa → đã), addon gửi Shift + Left để bôi đen ngược. Trên nhiều ô nhập liệu (như thanh địa chỉ trình duyệt, terminal, hoặc GTK/Qt app không hỗ trợ SurroundingText đầy đủ), Shift + Left làm lệch vùng chọn và chèn đè ký tự mới. [1]

2. Bản vá chính xác cho crates/vncharkey-core/src/engine.rs

Bạn chỉ cần sửa hàm tính alphanumeric_trap trong crates/vncharkey-core/src/engine.rs (khoảng dòng 848) để kiểm tra ký tự cuối sau khi đã chuẩn hóa thành tiếng Việt hợp lệ (hoặc kiểm tra is_valid_vietnamese_syllable(&self.current_word)): [1]
rust
// Trong crates/vncharkey-core/src/engine.rs (khoảng dòng 848)
// Thay thế đoạn let alphanumeric_trap = ... cũ bằng đoạn sau:
let alphanumeric_trap = self.has_letter_after_digit
    && (!is_valid_vietnamese_syllable(&self.current_word)
        || self.current_word.chars().any(|c| c.is_ascii_digit()));
Use code with caution.
Giải thích cơ chế hoạt động sau khi sửa:
• Khi bạn gõ bẫy mã/từ hỗn hợp như b2c hoặc v1s: self.current_word là "b2c" hoặc "v1s" (không phải âm tiết tiếng Việt hợp lệ và còn dính số) → alphanumeric_trap vẫn là true, tự động trả về b2c  / v1s  an toàn.
• Khi bạn gõ VNI với chữ đ đi trước nguyên âm đơn (d9a4 → đã, d9e63 → để, d9o1 → đó, d9u3 → đủ): is_valid_vietnamese_syllable(&self.current_word) là true và không còn số → alphanumeric_trap là false, giữ nguyên đã , để  100% chuẩn xác! [1]

3. Bổ sung Test Case hồi quy (crates/vncharkey-core/tests/vietc_regression_tests.rs)

Theo đúng quy tắc dự án trong .antigravityrules, bạn hãy thêm test case này vào crates/vncharkey-core/tests/vietc_regression_tests.rs (trong hàm test_vietc_auto_restore_cases) để đảm bảo không bao giờ tái phát: [1]
rust
    // VNI 'đ' (d9) followed by single accented vowels must NOT trigger alphanumeric_trap
    let mut vni_auto = Engine::new(EngineConfig {
        method: InputMethod::Auto,
        auto_restore_english: true,
        spell_check: true,
        enable_dictionary: true,
        ..EngineConfig::default()
    });
    let dbar_single_vowel_cases = [
        ("d9a4 ", "đã "),
        ("d9e63 ", "để "),
        ("d9o1 ", "đó "),
        ("d9u3 ", "đủ "),
        ("d9i ", "đi "),
        ("d9e2 ", "đè "),
        ("D9a4 ", "Đã "),
        ("D9e63 ", "Để "),
    ];
    for (input, expected) in dbar_single_vowel_cases {
        vni_auto.reset();
        let out = type_word(&mut vni_auto, input);
        assert_eq!(
            out, expected,
            "VNI d9 + single vowel failed for '{input}': got '{out}', expected '{expected}'"
        );
    }
Use code with caution.

4. Cách áp dụng và kiểm tra ngay

1. Mở crates/vncharkey-core/src/engine.rs, cập nhật dòng alphanumeric_trap như trên.
2. Chạy kiểm thử và cài đặt lại:bash
cargo test --workspace
./install.sh
Use code with caution.
3. Mẹo chống nhảy chữ trên Web/Browser: Nếu trên trình duyệt vẫn gặp nhảy chữ khi gõ nhanh ở chế độ Direct, bạn mở fcitx5-vncharkey-settings → tab General, bỏ chọn mục "Dokey Select-and-Replace" hoặc chuyển trình duyệt sang chế độ Preedit (Gạch Chân). [1]
