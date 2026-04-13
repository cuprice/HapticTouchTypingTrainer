# キーボードの対応表を作る（文字 → (row, motor)）
key_map = {
    # row 0（上段）
    'q': (0, 0), 'w': (0, 1), 'e': (0, 2), 'r': (0, 3), 't': (0, 4),
    'y': (0, 5), 'u': (0, 6), 'i': (0, 7), 'o': (0, 8), 'p': (0, 9),

    # row 1（ホーム段）
    'a': (1, 0), 's': (1, 1), 'd': (1, 2), 'f': (1, 3), 'g': (1, 4),
    'h': (1, 5), 'j': (1, 6), 'k': (1, 7), 'l': (1, 8), ';': (1, 9),

    # row 2（下段）
    'z': (2, 0), 'x': (2, 1), 'c': (2, 2), 'v': (2, 3), 'b': (2, 4),
    'n': (2, 5), 'm': (2, 6), ',': (2, 7), '.': (2, 8), '/': (2, 9),
}

# ユーザーから文字列を入力してもらう（文字数制限なし）
text = input("文字を入力してください: ").lower()

# 結果を入れるリスト
motor_order = []
key_row_order = []

# 1文字ずつ処理
for ch in text:
    if ch in key_map:
        row, motor = key_map[ch]
        key_row_order.append(row)
        motor_order.append(motor)
    else:
        print(f"警告: '{ch}' は対応していません（無視します）")

# 結果を表示
print("\n--- 結果 ---")

print("\nint motorOrder[] = {")
print("    " + ",".join(map(str, motor_order)))
print("};")

print("\nint keyRowOrder[] = {")
print("    " + ",".join(map(str, key_row_order)))
print("};")