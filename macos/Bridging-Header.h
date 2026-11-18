#ifndef VNCHARKEY_BRIDGING_HEADER_H
#define VNCHARKEY_BRIDGING_HEADER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Fcitx5VnCharKeyContext Fcitx5VnCharKeyContext;

/// Initializes a new VnCharKey engine instance.
Fcitx5VnCharKeyContext* fcitx5_vncharkey_init(void);

/// Initializes a new VnCharKey engine instance with a specific method (0 = Auto, 1 = Telex, 2 = VNI).
Fcitx5VnCharKeyContext* fcitx5_vncharkey_init_with_method(uint32_t method);

/// Frees a VnCharKey engine instance.
void fcitx5_vncharkey_free(Fcitx5VnCharKeyContext* ctx);

/// Frees a string allocated by Rust FFI.
void fcitx5_vncharkey_free_string(char* s);

/// Resets context state (e.g. when changing focused app or clicking elsewhere).
void fcitx5_vncharkey_reset(Fcitx5VnCharKeyContext* ctx);

/// Handles physical Backspace pressed by the user.
void fcitx5_vncharkey_handle_backspace(Fcitx5VnCharKeyContext* ctx);

/// Checks whether the engine has an active composing word.
bool fcitx5_vncharkey_has_active_word(Fcitx5VnCharKeyContext* ctx);

/// Sets output character set (0: Unicode NFC, 1: Unicode Composite NFD).
void fcitx5_vncharkey_set_charset(Fcitx5VnCharKeyContext* ctx, uint32_t charset);

/// Processes a single keystroke.
/// Returns 1 if consumed/handled, 0 if pass-through.
/// Sets out_backspaces (number of backspaces to emit) and out_commit (new text to insert).
int fcitx5_vncharkey_process_key(
    Fcitx5VnCharKeyContext* ctx,
    uint32_t key_char,
    double timestamp_ms,
    size_t* out_backspaces,
    char** out_commit
);

void fcitx5_vncharkey_set_clipboard_text(Fcitx5VnCharKeyContext* ctx, const char* text);

/// Flips the current composing word between processed Vietnamese and raw input strokes (Ctrl/Shift-Shift).
int fcitx5_vncharkey_flip_word(
    Fcitx5VnCharKeyContext* ctx,
    size_t* out_backspaces,
    char** out_commit
);

/// Attempts to undo the last macro expansion on Backspace.
int fcitx5_vncharkey_undo_macro(
    Fcitx5VnCharKeyContext* ctx,
    size_t* out_backspaces,
    char** out_commit
);

/// Reverts the active composing word back to raw keystrokes on Escape.
int fcitx5_vncharkey_undo_escape(
    Fcitx5VnCharKeyContext* ctx,
    size_t* out_backspaces,
    char** out_commit
);

/// Sets active input method (0 = Auto, 1 = Telex, 2 = VNI, 3 = SimpleTelex).
void fcitx5_vncharkey_set_method(Fcitx5VnCharKeyContext* ctx, uint32_t method);
uint32_t fcitx5_vncharkey_get_method(Fcitx5VnCharKeyContext* ctx);

/// Configures Restore English (auto-restore).
void fcitx5_vncharkey_set_auto_restore(Fcitx5VnCharKeyContext* ctx, bool enabled);
bool fcitx5_vncharkey_get_auto_restore(Fcitx5VnCharKeyContext* ctx);

/// Configures Chánh Tả spell checking.
void fcitx5_vncharkey_set_spell_check(Fcitx5VnCharKeyContext* ctx, bool enabled);
bool fcitx5_vncharkey_get_spell_check(Fcitx5VnCharKeyContext* ctx);

/// Configures User Dictionary.
void fcitx5_vncharkey_set_dictionary_enabled(Fcitx5VnCharKeyContext* ctx, bool enabled);
bool fcitx5_vncharkey_get_dictionary_enabled(Fcitx5VnCharKeyContext* ctx);

/// Configures Adaptive Habit Learning.
void fcitx5_vncharkey_set_habit_learning_enabled(Fcitx5VnCharKeyContext* ctx, bool enabled);
bool fcitx5_vncharkey_get_habit_learning_enabled(Fcitx5VnCharKeyContext* ctx);
void fcitx5_vncharkey_set_habit_promote_threshold(Fcitx5VnCharKeyContext* ctx, uint32_t threshold);
uint32_t fcitx5_vncharkey_get_habit_promote_threshold(Fcitx5VnCharKeyContext* ctx);
void fcitx5_vncharkey_unlearn_word(Fcitx5VnCharKeyContext* ctx, const char* word);

/// Configures macro expansion.
void fcitx5_vncharkey_set_macro_enabled(Fcitx5VnCharKeyContext* ctx, bool enabled);
bool fcitx5_vncharkey_get_macro_enabled(Fcitx5VnCharKeyContext* ctx);

/// Configures bracket transformation ([ -> ơ, ] -> ư).
void fcitx5_vncharkey_set_bracket_transform(Fcitx5VnCharKeyContext* ctx, bool enabled);

/// Configures Initial Case Anchor (ĐƯợc -> Được, đƯỢc -> được).
void fcitx5_vncharkey_set_initial_case_anchor(Fcitx5VnCharKeyContext* ctx, bool enabled);
bool fcitx5_vncharkey_get_initial_case_anchor(Fcitx5VnCharKeyContext* ctx);

/// Configures type w to produce ư rule (0 = NonStart, 1 = Always, 2 = Disabled).
void fcitx5_vncharkey_set_w2u(Fcitx5VnCharKeyContext* ctx, uint32_t mode);

/// Configures tone style (0 = Modern: oà, uý; 1 = Classic: hòa, ủy).
void fcitx5_vncharkey_set_tone_style(Fcitx5VnCharKeyContext* ctx, uint32_t style);
uint32_t fcitx5_vncharkey_get_tone_style(Fcitx5VnCharKeyContext* ctx);

/// Sets keystroke idle timeout in milliseconds (0 = disabled).
void fcitx5_vncharkey_set_idle_timeout(Fcitx5VnCharKeyContext* ctx, uint64_t ms);
uint64_t fcitx5_vncharkey_get_idle_timeout(Fcitx5VnCharKeyContext* ctx);

/// Configures Quick Telex, Quick Initial, and Quick Final consonants.
void fcitx5_vncharkey_set_quick_telex(Fcitx5VnCharKeyContext* ctx, bool enabled);
void fcitx5_vncharkey_set_quick_start_consonant(Fcitx5VnCharKeyContext* ctx, bool enabled);
void fcitx5_vncharkey_set_quick_end_consonant(Fcitx5VnCharKeyContext* ctx, bool enabled);

/// Loads custom macros and dictionary from configuration files.
void fcitx5_vncharkey_load_macros(Fcitx5VnCharKeyContext* ctx, const char* path);
void fcitx5_vncharkey_load_dictionary(Fcitx5VnCharKeyContext* ctx, const char* path);

/// Rescues a raw sentence or phrase (Ngó Sen Word Rescue / Cứu Chữ).
int fcitx5_vncharkey_rescue_sentence(
    Fcitx5VnCharKeyContext* ctx,
    const char* text,
    char** out_rescued
);

/// Applies a tone or diacritic key to a selected text (Selection Accent Rescue / Cứu Dấu).
int fcitx5_vncharkey_apply_accent_to_selection(
    Fcitx5VnCharKeyContext* ctx,
    const char* selection,
    uint32_t ch,
    char** out_transformed
);

/// Cycles the casing of the active composing word (lower -> UPPER -> Title Case -> lower).
int fcitx5_vncharkey_cycle_word_case(
    Fcitx5VnCharKeyContext* ctx,
    size_t* out_backspaces,
    char** out_commit
);

/// Cycles the casing of arbitrary text or a selected string.
int fcitx5_vncharkey_cycle_text_case(
    const char* text,
    char** out_cased
);

/// Searches for matching math symbols and emojis by shortcode prefix.
/// Results are formatted as lines of `symbol\t:shortcode:\n`.
/// Must be freed with `fcitx5_vncharkey_free_string`.
char* fcitx5_vncharkey_search_symbols(const char* query, uint32_t limit);

#ifdef __cplusplus
}
#endif

#endif // VNCHARKEY_BRIDGING_HEADER_H

